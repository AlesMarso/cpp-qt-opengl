#pragma once
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QDebug>
#include <QTimer>
#include <QMatrix4x4>
#include <QRandomGenerator>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QGraphicsPixmapItem>
#include <QPainter>

class CChart : public QGraphicsItem, public QOpenGLFunctions
{
public:
    CChart(QGraphicsItem* parent = nullptr);
    ~CChart();

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
