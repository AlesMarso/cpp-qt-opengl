#pragma once

#include <QGraphicsPixmapItem>
#include <QPainter>

class CChart : public QGraphicsPixmapItem
{
public:
    CChart(const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    ~CChart();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
