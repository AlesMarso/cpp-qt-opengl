#include "CandlestickWidget.h"
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>

CandlestickWidget::CandlestickWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    /* Запрашиваем контекст OpenGL 4.5 Core (нужен для VAO/VBO) */
    QSurfaceFormat fmt;
    fmt.setVersion(4, 5);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(fmt);

    /* Обновляем сцену 60 FPS – удобно для демо‑версий.
       В реальном приложении достаточно вызывать update() только
       когда меняются данные. */
}

CandlestickWidget::~CandlestickWidget()
{
    makeCurrent();
    glDeleteBuffers(1, &m_idVBOWick);
    glDeleteBuffers(1, &m_idVBOBody);
    glDeleteVertexArrays(1, &m_idVAOWick);
    delete m_program;
    doneCurrent();
}

/* --------------------------------------------------------------
   1️⃣  Инициализация OpenGL (создание шейдеров, VAO/VBO)
   -------------------------------------------------------------- */
void CandlestickWidget::initializeGL()
{
    initializeOpenGLFunctions();

    /* ---------- Шейдеры ----------
       Обратите внимание: у uniform‑переменной имя uMVP,
       а не push‑constant. */
    const char* vertSrc = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;   // (x, price)
        layout(location = 1) in vec3 aCol;   // (r,g,b)
        uniform mat4 uMVP;
        out vec3 vCol;
        void main()
        {
            gl_Position = uMVP * vec4(aPos, 0.0, 1.0);
            vCol = aCol;
        }
    )";

    const char* fragSrc = R"(
        #version 450 core
        in vec3 vCol;
        out vec4 fragColor;
        void main()
        {
            fragColor = vec4(vCol, 1.0);
        }
    )";

    m_program = new QOpenGLShaderProgram(this);
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertSrc))
        qWarning() << "Vertex shader compile error:\n" << m_program->log();
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSrc))
        qWarning() << "Fragment shader compile error:\n" << m_program->log();
    if (!m_program->link())
        qWarning() << "Shader program link error:\n" << m_program->log();

    /* ---------- VAO + VBO ----------
       Выделяем сразу небольшую буферную область; в updateVBO()
       будет перезаписываться только нужный кусок. */
    glGenVertexArrays(1, &m_idVAOBody);
    glGenVertexArrays(1, &m_idVAOWick);
    glGenBuffers(1, &m_idVBOWick);
    glGenBuffers(1, &m_idVBOBody);

    glBindVertexArray(m_idVAOWick);
    glBindBuffer(GL_ARRAY_BUFFER, m_idVBOWick);
    glBufferData(GL_ARRAY_BUFFER, 64 * 1024, nullptr, GL_DYNAMIC_DRAW); // 64 KiB «запас»

    // layout(location = 0) → vec2 aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), reinterpret_cast<void*>(0));

    // layout(location = 1) → vec3 aCol
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float),
        reinterpret_cast<void*>(2 * sizeof(float)));

    glBindVertexArray(m_idVAOBody);
    glBindBuffer(GL_ARRAY_BUFFER, m_idVBOBody);
    glBufferData(GL_ARRAY_BUFFER, 256 * 1024, nullptr, GL_DYNAMIC_DRAW);

    // layout(location = 0) → vec2 aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), reinterpret_cast<void*>(0));

    // layout(location = 1) → vec3 aCol
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float),
        reinterpret_cast<void*>(2 * sizeof(float)));

    glBindVertexArray(0);

}
void CandlestickWidget::resizeGL(int w, int h)
{
   glViewport(0, 0, w, h);
}

/* --------------------------------------------------------------
   3️⃣  Добавление новых свечей
   -------------------------------------------------------------- */
void CandlestickWidget::appendCandles(const CandleVector& candles)
{
    m_allCandles = candles;
    updateVBO();          // построить буфер из видимой части
    update();             // запрос перерисовки
}

/* --------------------------------------------------------------
   4️⃣  Установка количества свечей, которое должно быть видно
   -------------------------------------------------------------- */
void CandlestickWidget::setVisibleCount(int cnt)
{
    m_visibleCount = std::max(1, cnt);

    // Если сместились границы – подгоняем первый индекс
    if (m_firstVisible + m_visibleCount > m_allCandles.size())
        m_firstVisible = std::max(0, int(m_allCandles.size()) - m_visibleCount);

    updateVBO();
    update();
}

/* --------------------------------------------------------------
   5️⃣  Пересборка VBO только из видимой части
   -------------------------------------------------------------- */
void CandlestickWidget::updateVBO()
{
    if (m_allCandles.empty())
        return;

    int start = m_firstVisible;
    int end = std::min(start + m_visibleCount,
        int(m_allCandles.size()));

    /* Одна свеча → 8 вершин:
       2 – тень (wick)
       6 – тело (body, 2 треугольника) */
    constexpr int vertsPerCandle = 8;
    std::vector<float> verts;
    verts.reserve((end - start) * 2 * 5); // 5 float per vertex

    std::vector<float> bodyVerts;
    bodyVerts.reserve((end - start) * 6 * 5); // 5 float per vertex

    /* Ширина свечи в NDC‑единицах.
       Мы хотим, чтобы тело заняло ~80 % от «ячейки» между соседними свечами,
       а пустая часть — 20 % (отступы слева/справа). */
    const float cellWidth = 1;// 2 / float(m_visibleCount) * m_zoomX; // ширина одной "ячейки" в NDC
    const float candleW = cellWidth * 0.8f;   // 80 % от ячейки – реальная ширина тела
    const float halfW = candleW * 0.5f;

    std::thread th(
        [this, &bodyVerts, start, end, cellWidth, candleW, halfW]()
        {
            for (int i = start; i < end; ++i) {
                const Candle& c = m_allCandles[i];

                /* X‑координата центра свечи в NDC:
                   левая граница окна = -1,
                   шаг = cellWidth,
                   центр i‑й свечи = -1 + cellWidth/2 + (i‑start)*cellWidth   */
                float xCenter = cellWidth / 2 + float(i);

                bool bullish = c.close >= c.open;
                float r = bullish ? 0.0f : 1.0f;
                float g = bullish ? 1.0f : 0.0f;
                float b = 0.0f;

                float yOpen = float(c.open);
                float yClose = float(c.close);
                if (yOpen == yClose)
                    yClose += 1;

                bodyVerts.insert(bodyVerts.end(),
                    { xCenter - halfW, yOpen,  r, g, b,
                      xCenter + halfW, yOpen,  r, g, b,
                      xCenter + halfW, yClose, r, g, b,
                      xCenter - halfW, yOpen,  r, g, b,
                      xCenter + halfW, yClose, r, g, b,
                      xCenter - halfW, yClose, r, g, b });
            }
        }
    );

    std::thread th1(
        [this, &verts, start, end, cellWidth, candleW, halfW]()
        {
            for (int i = start; i < end; ++i) {
                const Candle& c = m_allCandles[i];

                /* X‑координата центра свечи в NDC:
                   левая граница окна = -1,
                   шаг = cellWidth,
                   центр i‑й свечи = -1 + cellWidth/2 + (i‑start)*cellWidth   */
                float xCenter = cellWidth / 2 + float(i);

                bool bullish = c.close >= c.open;
                float r = bullish ? 0.0f : 1.0f;
                float g = bullish ? 1.0f : 0.0f;
                float b = 0.0f;

                /* ---------- Тень (wick) — 2 вершины (только Y меняется) ---------- */
                verts.insert(verts.end(),
                    { xCenter, float(c.high), r, g, b,
                      xCenter, float(c.low),  r, g, b });
            }
        }
    );

    th.join();
    th1.join();

    auto neededBytesBody = bodyVerts.size() * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, m_idVBOBody);
    auto curSizeBody = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE,
        reinterpret_cast<GLint*>(&curSizeBody));
    if (size_t(curSizeBody) < neededBytesBody) {
        glBufferData(GL_ARRAY_BUFFER, neededBytesBody, bodyVerts.data(),
            GL_DYNAMIC_DRAW);
    }
    else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, neededBytesBody, bodyVerts.data());
    }

    auto neededBytesWick = verts.size() * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, m_idVBOWick);
    GLsizei curSizeWick = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE,
        reinterpret_cast<GLint*>(&curSizeWick));
    if (size_t(curSizeWick) < neededBytesWick) {
        glBufferData(GL_ARRAY_BUFFER, neededBytesWick, verts.data(),
            GL_DYNAMIC_DRAW);
    }
    else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, neededBytesWick, verts.data());
    }
}

/* --------------------------------------------------------------
   6️⃣  Вычисление MVP (ортографическая проекция)
   -------------------------------------------------------------- */
QMatrix4x4 CandlestickWidget::computeMVP() const
{
    if (m_allCandles.empty())
        return QMatrix4x4();

    float left = static_cast<float>(m_firstVisible);
    float right = static_cast<float>(m_firstVisible + m_visibleCount);

    int start = m_firstVisible;
    int end = std::min(start + m_visibleCount,
        int(m_allCandles.size()));

    /* Y‑границы в текущем окне */
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest(); 
    for (int i = start; i < end; ++i) {
        const Candle& c = m_allCandles[i];
        minY = std::min(minY, c.low);
        maxY = std::max(maxY, c.high);
    }

    double margin = (maxY - minY) * 0.05;
    minY -= margin;
    maxY += margin;

    double midY = (minY + maxY) * 0.5;
    double rangeY = std::max(1e-6, maxY - minY);

    /* Ортографическая проекция:
         X‑диапазон:   -1 … +1  →  каждая "единица" X‑координаты будет
                        растягиваться на 2 / visibleCount.
         Y‑диапазон:   -1 … +1  →  (price‑midY) * 2 / rangeY.
       Затем масштабируем по X‑оси на zoom (m_zoomX).              */
    QMatrix4x4 proj;
    proj.ortho(left, right, minY, maxY, -1.0f, 1.0f);

    const auto sz = size();

    QMatrix4x4 model;
    // сначала переводим X‑координату в [-1, +1] (по количеству свечей)
    //model.scale(2 / float(m_visibleCount) * m_zoomX, 1.0f, 1.0f);
    // потом перемещаем так, чтобы X‑0 оказалось в левой границе окна
    // теперь Y‑масштаб и смещение
    //model.scale(1.0f, float(2.0 / rangeY), 1.0f);
    //model.translate(0.0f, float(-midY), 0.0f);

    //glOrtho((double)m_firstVisible, (double)(m_firstVisible + m_visibleCount), minY, maxY, -1.0f, 1.0f);

    return proj * model;
}

/* --------------------------------------------------------------
   7️⃣  Рендер
   -------------------------------------------------------------- */
void CandlestickWidget::paintGL()
{
    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_allCandles.empty())
        return;

    QMatrix4x4 mvp = computeMVP();

    m_program->bind();
    m_program->setUniformValue("uMVP", mvp);   // ← обычный uniform

    /* Сколько свечей действительно находится в буфере? */
    int visibleCandles = std::min(m_visibleCount,
        int(m_allCandles.size()) - m_firstVisible);

    /* 1) Тело (body) – 6 вершин на свечу */
    glBindVertexArray(m_idVAOBody);
    glBindBuffer(GL_ARRAY_BUFFER, m_idVBOBody);
    glDrawArrays(GL_TRIANGLES, 0, visibleCandles * 6);

    /* 2) Тени (wick) – 2 вершины на свечу */
    glBindVertexArray(m_idVAOWick);
    glBindBuffer(GL_ARRAY_BUFFER, m_idVBOWick);
    glDrawArrays(GL_LINES, 0, visibleCandles * 2);

    glBindVertexArray(0);
    m_program->release();
}

/* --------------------------------------------------------------
   8️⃣  Обработка ввода – зум + панорамирование
   -------------------------------------------------------------- */
void CandlestickWidget::wheelEvent(QWheelEvent* event)
{
    // каждый «шаг» колеса = 120 → 15 % зума
    int step = (event->angleDelta().y() > 0) ? -10 : +10;
    m_firstVisible = std::clamp(m_firstVisible + step, 0, static_cast<int>(m_allCandles.size()) - m_visibleCount);
    event->accept();

    updateVBO();
    update();
}

void CandlestickWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        m_startFirstVisible = m_firstVisible;
        m_startVisibleCount = m_visibleCount;
    }
}

void CandlestickWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_isDragging)
        return;

    QPoint delta = event->pos() - m_lastMousePos;

    int deltaX = delta.x();
    int sign = (deltaX > 0) ? -1 : +1;
    int step = std::abs(deltaX) / 15;

    int newCount = m_startVisibleCount + sign * step;
    newCount = std::max(2, newCount);
    newCount = std::min(newCount, static_cast<int>(m_allCandles.size()));

    m_visibleCount = newCount;

    updateVBO();
    update();
}

void CandlestickWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        m_isDragging = false;
}
