#pragma once
#include <vector>
#include <QDateTime>

struct Candle {
    QDateTime time;
    double    open;
    double    high;
    double    low;
    double    close;
};

using CandleVector = std::vector<Candle>;