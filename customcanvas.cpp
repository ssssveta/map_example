#include "customcanvas.h"

CustomCanvas::CustomCanvas(QWidget* parent) : QgsMapCanvas(parent)
{
    this->enableAntiAliasing(true);
    this->setMapSettingsFlags(this->mapSettings().flags() | QgsMapSettings::RenderPreviewJob);
    this->setParallelRenderingEnabled(true);
    this->setCachingEnabled(true);
    this->setPreviewJobsEnabled(true);
    this->setMapUpdateInterval(100);

    emitPointTool = new QgsMapToolEmitPoint(this);
    panTool = new QgsMapToolPan(this);

    setupControlPointsLayer();
    setupTrajectoryPointsLayer();

    this->setLayers({controlPointsLayer, trajectoryPointsLayer, earthLayer, oceanLayer});
    this->setExtent(earthLayer->extent());

    connect(emitPointTool, &QgsMapToolEmitPoint::canvasClicked, this, &CustomCanvas::addControlPoint);


}

CustomCanvas::~CustomCanvas()
{

}

void CustomCanvas::addTrajectoryPoint(QgsPointXY point)
{
    QgsFeature f;
    f.setFields(trajectoryPointsLayer->fields(), true);
    f.setGeometry(QgsGeometry::fromPointXY(point));
    f.setAttribute("isLast", true);

    double angle = 0;
    angle = calculateAngle({twoPoints[1].x(), twoPoints[1].y()}, {twoPoints[0].x(), twoPoints[0].y()});
    f.setAttribute("angle", angle);

    trajectoryPointsLayer->startEditing();
    QgsFeatureRequest rqs;
    QString expr = "isLast = true";
    rqs.setFilterExpression(expr);
    QgsFeature f1;
    QgsFeatureIterator it;
    it = trajectoryPointsLayer->getFeatures(rqs);
    while (it.nextFeature(f1))
    {
        trajectoryPointsLayer->changeAttributeValue(f1.id(), 0, false);
    }
    trajectoryPointsLayer->addFeature(f);
    trajectoryPointsLayer->commitChanges();
}

void CustomCanvas::addControlPoint(const QgsPointXY &point, Qt::MouseButton button)
{
    if (twoPoints.size() < 2)
    {
        twoPoints.push_back(point);

        controlPointsLayer->startEditing();

        QgsFeature feat;
        feat.setFields(controlPointsLayer->fields(), true);
        feat.setAttribute("fid", twoPoints.size() - 1);
        feat.setGeometry(QgsGeometry::fromPointXY(point));
        controlPointsLayer->addFeature(feat);
        controlPointsLayer->commitChanges();

        emit controlPointAdded(point);
    }

    if (twoPoints.size() == 2)
        emit enablePlayButton(true);
}

void CustomCanvas::activatePan()
{
    this->setMapTool(panTool);
}

void CustomCanvas::clearAllPoints()
{
    twoPoints.clear();

    controlPointsLayer->startEditing();
    QgsFeature f;
    QgsFeatureIterator iter = controlPointsLayer->getFeatures();
    while(iter.nextFeature(f))
        controlPointsLayer->deleteFeature(f.id());
    controlPointsLayer->commitChanges();

    trajectoryPointsLayer->startEditing();
    QgsFeature f1;
    QgsFeatureIterator iter1 = trajectoryPointsLayer->getFeatures();
    while(iter1.nextFeature(f1))
        trajectoryPointsLayer->deleteFeature(f1.id());
    trajectoryPointsLayer->commitChanges();

    emit enablePlayButton(false);
}

void CustomCanvas::activateSelecting()
{
    this->setMapTool(emitPointTool);
}

void CustomCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QgsMapCanvas::mouseMoveEvent(event);
    QPoint screenPoint = this->mouseLastXY();
    QgsPointXY mapPoint = this->getCoordinateTransform()->toMapCoordinates(screenPoint);
    emit coordinatesChanged(mapPoint.x(), mapPoint.y());
}

void CustomCanvas::setupControlPointsLayer()
{
    controlPointsLayer->startEditing();
    controlPointsLayer->dataProvider()->addAttributes({QgsField("fid", QVariant::Int)});
    controlPointsLayer->updateFields();

    controlPointsLayer->setLabelsEnabled(true);
    QgsPalLayerSettings pls;
    pls.fieldName = "fid";
    pls.placement = QgsPalLayerSettings::Placement::Line;
    QgsVectorLayerSimpleLabeling* simple_label = new QgsVectorLayerSimpleLabeling(pls);
    controlPointsLayer->setLabeling(simple_label);
    controlPointsLayer->commitChanges();
}

void CustomCanvas::setupTrajectoryPointsLayer()
{
    trajectoryPointsLayer->dataProvider()->addAttributes({{"isLast", QVariant::Bool}, {"angle", QVariant::Double}});
    trajectoryPointsLayer->updateFields();

    QgsRuleBasedRenderer* render = new QgsRuleBasedRenderer(QgsSymbol::defaultSymbol(trajectoryPointsLayer->geometryType()));
    QgsSymbol* ss = QgsSymbol::defaultSymbol(trajectoryPointsLayer->geometryType());
    QgsSymbol* symbol = QgsSymbol::defaultSymbol(trajectoryPointsLayer->geometryType());
    QgsSvgMarkerSymbolLayer* svgL = new QgsSvgMarkerSymbolLayer("/root/MapExample/icons/airplane.svg", 10);
    symbol->setColor(Qt::gray);
    ss->changeSymbolLayer(0, svgL);
    QgsMarkerSymbol *m = dynamic_cast<QgsMarkerSymbol *>(ss);
    QgsProperty p;
    p.setField("angle");
    m->setDataDefinedAngle(p);

    QgsRuleBasedRenderer::Rule *rule = new QgsRuleBasedRenderer::Rule(ss, 0, 0, "isLast = true");
    QgsRuleBasedRenderer::Rule *rule3 = new QgsRuleBasedRenderer::Rule(symbol, 0, 0, "isLast = false");
    render->rootRule()->appendChild(rule);

    render->rootRule()->appendChild(rule3);
    render->rootRule()->removeChildAt(0);
    trajectoryPointsLayer->setRenderer(render);
}

double CustomCanvas::calculateAngle(QgsPointXY p, QgsPointXY p0)
{
    double delX = p.x() - p0.x();
    double delY = p.y() - p0.y();
    double angle = 90;
    if (delY > 0 || delY < 0)
        angle = atan(delX/delY) * 180 / M_PI;
    if (delY < 0)
        angle += 180;
    return angle;
}
