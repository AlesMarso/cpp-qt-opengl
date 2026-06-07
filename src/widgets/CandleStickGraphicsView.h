#pragma once

#include <QGraphicsView>
#include <QResizeEvent>

#include "CandlesStickGraphicsItem.h"
#include "data/Candle.h"

class CCandleStickGraphicsView : public QGraphicsView
{
public:
	CCandleStickGraphicsView(QWidget* parent = nullptr);
	~CCandleStickGraphicsView();

	void SetCandlesData(const CandleVector& candles);

	void resizeEvent(QResizeEvent* event);

	void AppendCandles(const CandleVector& candles);

	void SetVisibleData(int count);

private:
	CCandlesStickGraphicsItem* m_candlesStick{ nullptr };
};