#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    map = new CustomCanvas(this);
    this->setCentralWidget(map);
    this->setMinimumSize(800, 600);

    toolBar_ = new QToolBar(this);
    QAction* activatePanAction = new QAction(QIcon("/root/MapExample/icons/grab.svg"), "Пан", toolBar_);
    activateSelectMode = new QAction(QIcon("/root/MapExample/icons/select.svg"), "Выбор начальной и конечной точек", toolBar_);
    playAction = new QAction(QIcon("/root/MapExample/icons/play.svg"), "Запуск", toolBar_);
    playAction->setEnabled(false);
    QAction* clearAction = new QAction(QIcon("/root/MapExample/icons/back2.svg"), "Очистить точки", toolBar_);

    toolBar_->addAction(activatePanAction);
    toolBar_->addSeparator();
    toolBar_->addAction(activateSelectMode);
    toolBar_->addSeparator();
    toolBar_->addAction(playAction);
    toolBar_->addSeparator();
    toolBar_->addAction(clearAction);

    connect(activatePanAction, &QAction::triggered, map, &CustomCanvas::activatePan);
    connect(activateSelectMode, &QAction::triggered, map, &CustomCanvas::activateSelecting);
    connect(playAction, &QAction::triggered, this, &MainWindow::onPlayPressed);
    connect(clearAction, &QAction::triggered, map, &CustomCanvas::clearAllPoints);
    connect(clearAction, &QAction::triggered, [=]{twoPoints.clear();});
    connect(clearAction, &QAction::triggered, [=]{activateSelectMode->setEnabled(true);});

    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar_);

    statusBar_ = new QStatusBar(this);
    connect(map, &CustomCanvas::coordinatesChanged, this, &MainWindow::updateStatusBar);
    this->setStatusBar(statusBar_);

    connect(map, &CustomCanvas::enablePlayButton, this, &MainWindow::setPlayButtonEnabled);
    connect(map, &CustomCanvas::controlPointAdded, this, &MainWindow::addPoint);

    map->activatePan();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setPlayButtonEnabled(bool flag)
{
    playAction->setEnabled(flag);
}

void MainWindow::addPoint(const QgsPointXY &point)
{
    twoPoints.push_back(point);

    if (twoPoints.size() == 2)
    {
        map->activatePan();
        activateSelectMode->setEnabled(false);
    }
}

void MainWindow::updateStatusBar(double x, double y)
{
    statusBar_->showMessage("LAT: " + QString().setNum(x) + " LON: " + QString().setNum(y));
}

void MainWindow::onPlayPressed()
{
    this->setEnabled(false);

    int stepsCount = 10;

    double xdiff = (twoPoints[1].x() - twoPoints[0].x()) / stepsCount;
    double ydiff = (twoPoints[1].y() - twoPoints[0].y()) / stepsCount;

    for (auto i = 1; i < stepsCount + 1; i++)
    {
        map->addTrajectoryPoint({twoPoints[0].x() + i * xdiff,  twoPoints[0].y() + i * ydiff});
        QTest::qWait(1000);
    }

    this->setEnabled(true);
    playAction->setEnabled(false);
    map->activatePan();
}
