#pragma once
#include <vector>
#include <QDateTime>

struct Candle {
    int       pointNum;
    double    open;
    double    high;
    double    low;
    double    close;
};

using CandleVector = std::vector<Candle>;