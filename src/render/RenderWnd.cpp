#include "RenderWnd.h"

CGraphicsView::CGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene());

    m_pPixmap = new QPixmap(size());
    m_pChart = new CChart(*m_pPixmap);
    
    scene()->addItem(m_pChart);
}

CGraphicsView::~CGraphicsView()
{
}
