#pragma once

#include <QGraphicsObject>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QWidget>
#include <QObject>

#include <memory>

#include "data/Candle.h"
#include "data/CandleStick.h"

class CCandlesStickGraphicsItem : public QGraphicsObject, protected QOpenGLFunctions_4_5_Core, public CCandleStick
{
public:
	CCandlesStickGraphicsItem(QGraphicsItem* parent = nullptr);
	~CCandlesStickGraphicsItem();

	void SetCandles(const CandleVector& candles);

	QRectF boundingRect() const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	void FillCandles(const CandleVector& candles) override;

	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

	void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
	void initializeGL();
	void cleanupGL();
	void setupVertexData();

private:
	QMatrix4x4 computeMVP() const;

private:
	bool m_glInited{ false };

	QOpenGLShaderProgram* m_pShaderProgram{ nullptr };
	
	QOpenGLVertexArrayObject* m_pVAOBody{ nullptr };
	QOpenGLBuffer* m_pVBOBody{ nullptr };

	QOpenGLVertexArrayObject* m_pVAOWick{ nullptr };
	QOpenGLBuffer* m_pVBOWick{ nullptr };

	float rectWidth{ 50 };
	float rectHeight{ 50 };
	QColor rectColor{ Qt::blue };

	int m_firstVisible = 0;
	int m_visibleCount = 100;

	int m_startFirstCandle = 0;
	int m_startVisibleCandles = 0;
	bool m_isDragging{ false };
	QPointF m_lastMousePos;

	QColor m_bearColor;
	QColor m_bullColor;
};
