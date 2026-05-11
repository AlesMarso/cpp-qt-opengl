#include "RenderWnd.h"

#include <QOpenGLWidget>

CGraphicsView::CGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene());

    m_pPixmap = new QPixmap(size());
    m_pChart = new CChart();
    
    scene()->addItem(m_pChart);

    QOpenGLWidget* pGLWidget = new QOpenGLWidget();
    QSurfaceFormat format;
    format.setSamples(8);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    pGLWidget->setFormat(format);
    setViewport(pGLWidget);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

CGraphicsView::~CGraphicsView()
{
}
