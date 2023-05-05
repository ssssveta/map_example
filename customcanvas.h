#ifndef CUSTOMCANVAS_H
#define CUSTOMCANVAS_H
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <QObject>
#include <qgssymbol.h>
#include <qgsgeometry.h>
#include <qgsmaptoolpan.h>
#include <qgsmaptoolemitpoint.h>
#include <qgslabeling.h>
#include <qgsfield.h>
#include <qgsvectorlayerlabeling.h>
#include <qgspallabeling.h>
#include <qgsrulebasedrenderer.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsgeometry.h>
#include <qgssinglesymbolrenderer.h>
#include <qgsmarkersymbollayer.h>

class CustomCanvas : public QgsMapCanvas
{

    Q_OBJECT

public:
    CustomCanvas(QWidget* parent = nullptr);
    ~CustomCanvas();

public slots:
    void addTrajectoryPoint(QgsPointXY point);
    void addControlPoint(const QgsPointXY &point, Qt::MouseButton button);
    void activatePan();
    void clearAllPoints();
    void activateSelecting();
signals:
    void coordinatesChanged(double lat, double lon);
    void enablePlayButton(bool flag);
    void controlPointAdded(const QgsPointXY &point);

private:
    void mouseMoveEvent(QMouseEvent *event);
    void setupControlPointsLayer();
    void setupTrajectoryPointsLayer();
    double calculateAngle(QgsPointXY p, QgsPointXY p0);

    QgsMapToolEmitPoint* emitPointTool = nullptr;
    QgsMapToolPan* panTool = nullptr;

    QgsVectorLayer* controlPointsLayer = new QgsVectorLayer("Point", "Points", "memory");
    QgsVectorLayer* trajectoryPointsLayer = new QgsVectorLayer("Point", "Points", "memory");

    QgsVectorLayer* earthLayer = new QgsVectorLayer("/root/MapExample/kx-world-land-areas-110-million-SHP/world-land-areas-110-million.shp", "earth", "ogr");
    QgsVectorLayer* oceanLayer = new QgsVectorLayer("/root/MapExample/kx-world-bathymetry-110-million-SHP/world-bathymetry-110-million.shp", "water", "ogr");

    QVector<QgsPointXY> twoPoints;
};

#endif // CUSTOMCANVAS_H
