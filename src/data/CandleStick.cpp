#include "CandleStick.h"

CCandleStick::CCandleStick()
{
}

CCandleStick::~CCandleStick()
{
}

void CCandleStick::FillCandles(const CandleVector& candles)
{
	m_candles = candles;
}
