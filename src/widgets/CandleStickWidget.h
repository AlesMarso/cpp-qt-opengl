#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include "data/Candle.h"

class CandlestickWidget : public QOpenGLWidget,
    protected QOpenGLFunctions_4_5_Core
{
public:
    explicit CandlestickWidget(QWidget* parent = nullptr);
    ~CandlestickWidget() override;

    /* API */
    void appendCandles(const CandleVector& candles);
    void setVisibleCount(int count);        // сколько свечей показывать

protected:
    /* OpenGL life‑cycle */
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    /* Input – зум + панорамирование */
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    /* ----- OpenGL resources ----- */
    QOpenGLShaderProgram* m_program = nullptr;
    GLuint m_idVAOWick{ 0 };          // Vertex Array Object
    GLuint m_idVBOWick{ 0 };          // Vertex Buffer Object (dynamic)

    GLuint m_idVBOBody{ 0 };
    GLuint m_idVAOBody{ 0 };

    /* ----- данные ----- */
    CandleVector m_allCandles; // полный набор свечей
    int m_firstVisible = 0;    // индекс первой свечи в окне
    int m_visibleCount = 120; // сколько свечей отрисовать

    /* ----- взаимодействие ----- */
    float m_zoomX = 1.0f;      // зум по оси X (wheel)
    QPoint m_lastMousePos;     // для панорамирования мышью

    /* ----- вспомогательные функции ----- */
    void updateVBO();                // построить VBO только из видимой части
    QMatrix4x4 computeMVP() const;   // матрица Model‑View‑Projection

    float m_scaleX = 1.f;
    float m_scaleY = 1.f;

    bool m_isDragging{ false };

    int m_startVisibleCount = 120;
    int m_startFirstVisible = 0;
};