#include "CandleStickGraphicsView.h"

#include <QGraphicsScene>
#include <QOpenGLWidget>

CCandleStickGraphicsView::CCandleStickGraphicsView(QWidget* parent)
	: QGraphicsView(parent)
{
	setScene(new QGraphicsScene());
	setViewport(new QOpenGLWidget());
	scene()->addItem(m_candlesStick = new CCandlesStickGraphicsItem());
	setSceneRect(rect());
}

CCandleStickGraphicsView::~CCandleStickGraphicsView()
{
}

void CCandleStickGraphicsView::SetCandlesData(const CandleVector& candles)
{
}

void CCandleStickGraphicsView::resizeEvent(QResizeEvent* event)
{
	QGraphicsView::resizeEvent(event);
	setSceneRect(0,0, event->size().width(), event->size().height());
}

void CCandleStickGraphicsView::AppendCandles(const CandleVector& candles)
{
	if (m_candlesStick)
	{
		m_candlesStick->FillCandles(candles);
		m_candlesStick->update();
	}
}
