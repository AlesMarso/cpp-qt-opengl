#pragma once

#include <QGraphicsView>
#include <QPixmap>

#include "Chart.h"

class CGraphicsView : public QGraphicsView
{
public:
    CGraphicsView(QWidget* parent = nullptr);
    ~CGraphicsView();

private:
    QPixmap* m_pPixmap{nullptr};
    CChart* m_pChart{nullptr};
};
