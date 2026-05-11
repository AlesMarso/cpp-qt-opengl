#include <iostream>

#include <QCoreApplication>
#include <QRandomGenerator>
#include <QApplication>
#include "window/MainWindow.h"

#include "widgets/CandleStickWidget.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QMainWindow win;
    win.resize(1280, 720);
    win.setWindowTitle(QStringLiteral("Candlestick (OpenGL 4.5)"));

    auto* chart = new CandlestickWidget;
    win.setCentralWidget(chart);
    win.show();

    /* ---------- Тестовые данные ---------- */
    CandleVector data;
    QDateTime now = QDateTime::currentDateTimeUtc();
    QRandomGenerator* rng = QRandomGenerator::global();

    double open = 100.0 + rng->bounded(30);            // 100‑129

    for (int i = 0; i < 65536; ++i) {
        double close = open + (rng->bounded(13) - 6);      // отклонение -6 … +6
        double high = std::max(open, close) + rng->bounded(5);
        double low = std::min(open, close) - rng->bounded(5);
        data.push_back({ now.addSecs(i * 60), open, high, low, close });
        open = close;
    }
    // -------------------------------------

    chart->appendCandles(data);
    chart->setVisibleCount(500);      // сколько свечей показывать сразу
    return app.exec();
}
