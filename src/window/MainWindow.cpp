#include "MainWindow.h"

#include <QVBoxLayout>

#include "render/RenderWnd.h"

CMainWindow::CMainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setMinimumSize(1200, 800);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    layout->addWidget(new CGraphicsView(this));

    setCentralWidget(centralWidget);
}

CMainWindow::~CMainWindow()
{

}
