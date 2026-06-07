#include <iostream>

#include <QCoreApplication>
#include <QRandomGenerator>
#include <QApplication>
#include "window/MainWindow.h"

#include "widgets/CandleStickWidget.h"
#include "widgets/CandleStickGraphicsView.h"
#include "data/Data.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QMainWindow win;
    win.resize(1280, 720);
    win.setWindowTitle(QStringLiteral("Candlestick (OpenGL 4.5)"));

    auto* chart = new CCandleStickGraphicsView();
    win.setCentralWidget(chart);
    win.show();

    CandleVector data;
    QDateTime now = QDateTime::currentDateTimeUtc();
    QRandomGenerator* rng = QRandomGenerator::global();

    double open = 100.0 + rng->bounded(30);

    std::shared_ptr<data::CDataSource> dataSource = std::make_shared<data::CDataSource>(data::TDataItemType::Candle, 0);
    dataSource->CreateChild(data::TDataType::Price);

    for (int i = 0; i < 65536; ++i) {
        double close = open + (rng->bounded(13) - 6);
        double high = std::max(open, close) + rng->bounded(5);
        double low = std::min(open, close) - rng->bounded(5);
        data.push_back({ i, open, high, low, close });
        
        dataSource->AddDataItem(
            data::CDataItem(
                data::TCandle(open, high, low, close)));

        open = close;
    }

    chart->SetVisibleData(65536);
    chart->AppendCandles(data);

    return app.exec();
}
