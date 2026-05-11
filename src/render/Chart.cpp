#include "Chart.h"


CChart::CChart(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

CChart::~CChart()
{
}

QRectF CChart::boundingRect() const
{
    return QRectF(100,100,300,300);
}

void CChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);

    if (auto* glWidget = qobject_cast<QOpenGLWidget*>(widget))
    {
        painter->drawText(QPoint(100, 100), u8"100");
    }

}


