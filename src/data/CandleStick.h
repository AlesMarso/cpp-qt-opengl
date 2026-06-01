#pragma once

#include "Candle.h"

class CCandleStick
{
public:
	CCandleStick();
	~CCandleStick();

	virtual void FillCandles(const CandleVector& candles);

protected:
	CandleVector m_candles;
};
