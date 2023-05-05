#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QTest>
#include "customcanvas.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setPlayButtonEnabled(bool flag);
    void addPoint(const QgsPointXY& point);
    void updateStatusBar(double x, double y);

private slots:
    void onPlayPressed();

private:
    CustomCanvas* map;
    QToolBar* toolBar_;
    QStatusBar* statusBar_;
    QVector<QgsPointXY> twoPoints;
    QAction* playAction;
    QAction* activateSelectMode;
};
#endif // MAINWINDOW_H
