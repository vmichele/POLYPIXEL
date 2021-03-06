#include "GraphicsObjectItem.hxx"

#include "Core/Objects/Object.hxx"
#include "Core/Objects/Obstacles/Tape.hxx"
#include "Core/Objects/Deviations/Mirror.hxx"
#include "Core/Objects/Obstacles/OneWay.hxx"
#include "Core/Objects/Deviations/Portal.hxx"
#include "CreateLevelObjectsListModel.hxx"

#include <random>

#include <QPainter>
#include <QStandardItem>

GraphicsObjectItem::GraphicsObjectItem(QGraphicsItem* p_parent):
  QGraphicsItem(p_parent),
  m_controlPoints(),
  m_boundingPolygon(),
  m_currentControlPointRow(-1){

  SetState(eEnabledState);

  setFlag(QGraphicsItem::ItemIsSelectable, true);
}

GraphicsObjectItem::~GraphicsObjectItem() = default;

QList<QColor> GraphicsObjectItem::GetColorAccordingToItemState() const {
  switch (data(eStateRole).value<State>()) {
  case eEnabledState: {
    return GetEnabledColors();
  }
  case eDisabledState: {
    return GetDisabledColors();
  }
  case eSelectedState: {
    return GetSelectedColors();
  }
  case eHighlightUpState: {
    return GetHighlightedUpColors();
  }
  case eHighlightDownState: {
    return GetHighlightedDownColors();
  }
  default:
    return {};
  }
}

void GraphicsObjectItem::DrawControlPoints(QPainter* p_painter)
{
  p_painter->save();
  p_painter->setPen(Qt::NoPen);
  p_painter->setBrush(QColor("#000000"));

  m_controlPoints.clear();
  m_controlPoints = ComputeControlPoints();

  int k = 0;
  for (auto const& controlPointAndType: m_controlPoints) {
    auto controlPoint = controlPointAndType.first;
    QRectF pointBoundingRect(controlPoint.x()-6, controlPoint.y()-6, 12, 12);
    if (controlPointAndType.second == Object::eCenter) {
      p_painter->save();
      p_painter->setBrush(Qt::white);
      p_painter->setPen(QPen(QBrush(QColor("#000000")), 2));
      QRectF pointBoundingRect(controlPoint.x()-5, controlPoint.y()-5, 10, 10);
      p_painter->drawEllipse(pointBoundingRect);
      p_painter->restore();
    } else if (m_currentControlPointRow == k) {
      p_painter->save();
      p_painter->setBrush(Qt::white);
      p_painter->setPen(QPen(QBrush(QColor("#38ACEC")), 3));
      QRectF pointBoundingRect(controlPoint.x()-6, controlPoint.y()-6, 12, 12);
      p_painter->drawEllipse(pointBoundingRect);
      p_painter->restore();
    } else {
      p_painter->drawEllipse(pointBoundingRect);
    }
    ++k;
  }
  p_painter->restore();
}

void GraphicsObjectItem::UpdateControlPoints() {
  m_controlPoints = ComputeControlPoints();
}

void GraphicsObjectItem::SetState(GraphicsObjectItem::State p_state) {
  setData(eStateRole, p_state);

  Q_EMIT StateChanged();
}

GraphicsObjectItem::State GraphicsObjectItem::GetState() const {
  return data(eStateRole).value<State>();
}

bool GraphicsObjectItem::Intersect(const ppxl::Point& p_point) const {
  return m_boundingPolygon.IsPointInside(p_point);
}

QPolygonF GraphicsObjectItem::ComputeQuad(ppxl::Segment const& p_line, int p_factor) const {
  auto normal = p_line.GetNormal();
  auto lineCopy = p_line;
  lineCopy.Translate(-p_factor*normal.GetX(), -p_factor*normal.GetY());
  auto A = p_line.GetA();
  auto Ax = A.GetX();
  auto Ay = A.GetY();
  auto B = p_line.GetB();
  auto Bx = B.GetX();
  auto By = B.GetY();
  auto C = lineCopy.GetB();
  auto Cx = C.GetX();
  auto Cy = C.GetY();
  auto D = lineCopy.GetA();
  auto Dx = D.GetX();
  auto Dy = D.GetY();

  return QPolygonF({QPoint(Ax, Ay), QPoint(Bx, By), QPoint(Cx, Cy), QPoint(Dx, Dy)});
}

void GraphicsObjectItem::paint(QPainter* p_painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  DrawObject(p_painter);
  if (data(eStateRole).value<State>() == eSelectedState) {
    DrawControlPoints(p_painter);
  }
}

QList<QColor> GraphicsObjectItem::GetHighlightedDownColors() const {
  QList<QColor> highlightedColors;
  for (auto color: GetEnabledColors()) {
    color.setAlpha(63);
    highlightedColors << color;
  }
  return highlightedColors;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// POLYGON
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsPolygonItem::GraphicsPolygonItem(ppxl::Polygon* p_polygon, QGraphicsItem* p_parent):
  GraphicsObjectItem(p_parent),
  m_polygon(p_polygon),
  m_enabledColor(GetRandomColor()) {

}

QColor GraphicsPolygonItem::GetRandomColor() {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<> distribution(0, 255);

  auto r = distribution(rng);
  auto g = distribution(rng);
  auto b = distribution(rng);

  return QColor(r, g, b);
}

void GraphicsPolygonItem::SetColor(QColor const& p_color) {
  m_enabledColor = p_color;
}

QColor const& GraphicsPolygonItem::GetColor() const {
  return m_enabledColor;
}

void GraphicsPolygonItem::SetCurrentVertexRow(int p_currentVertexRow) {
    m_currentControlPointRow = p_currentVertexRow;
}

bool GraphicsPolygonItem::Intersect(const ppxl::Point &p_point) const {
  return m_polygon->IsPointNearOneEdge(p_point, 10);
}

QRectF GraphicsPolygonItem::boundingRect() const {
  auto xmin = std::numeric_limits<double>::infinity();
  auto xmax = -std::numeric_limits<double>::infinity();
  auto ymin = std::numeric_limits<double>::infinity();
  auto ymax = -std::numeric_limits<double>::infinity();

  for (auto const& vertex: m_polygon->GetVertices()) {
    xmin = std::min(vertex.GetX(), xmin);
    xmax = std::max(vertex.GetX(), xmax);
    ymin = std::min(vertex.GetY(), ymin);
    ymax = std::max(vertex.GetY(), ymax);
  }

  return QRectF(QPointF(xmin-30, ymin-30), QPointF(xmax+30, ymax+30));
}

void GraphicsPolygonItem::DrawObject(QPainter* p_painter) {
  p_painter->save();
  p_painter->setBrush(Qt::NoBrush);
  p_painter->setPen(QPen(QBrush(GetColorAccordingToItemState().first()), 7));
  auto vertices = m_polygon->GetVertices();
  if (vertices.size() > 1) {
    for (unsigned int vertexRow = 0; vertexRow < vertices.size(); ++vertexRow) {
      ppxl::Point A = vertices.at(vertexRow);
      unsigned int indexB = (vertexRow+1)%vertices.size();
      ppxl::Point B = vertices.at(indexB);
      p_painter->drawLine(A.GetX(), A.GetY(), B.GetX(), B.GetY());

      if (data(eStateRole).value<State>() == eSelectedState) {
        auto vertexPos = A;
        ppxl::Vector fontShift(-5, 5);
        vertexPos.Translated(fontShift);
        if (m_polygon->HasEnoughVertices()) {
          unsigned int indexZ = (vertexRow+vertices.size()-1)%vertices.size();
          ppxl::Point Z = vertices.at(indexZ);
          ppxl::Polygon tempPolygon({Z, A, B});
          auto tempBarycenter = tempPolygon.Barycenter();
          ppxl::Vector shiftVector(A, tempBarycenter);
          shiftVector.Normalize();
          shiftVector *= 20;
          m_polygon->IsPointInside(vertexPos.Translate(shiftVector-fontShift)) ?
            vertexPos.Translated(-shiftVector):
            vertexPos.Translated(shiftVector);
        }
        p_painter->save();
        if (static_cast<int>(vertexRow) == m_currentControlPointRow) {
          p_painter->setPen(QPen(QBrush(QColor("#38ACEC")), 7));
          auto font = p_painter->font();
          font.setBold(true);
          font.setPointSizeF(1.2*font.pointSizeF());
          p_painter->setFont(font);
        } else {
          p_painter->setPen(QPen(QBrush(Qt::black), 7));
        }
        p_painter->drawText(QPointF(vertexPos.GetX(), vertexPos.GetY()), QString(QChar(static_cast<char>('A'+vertexRow))));
        p_painter->restore();
      }
    }
  }
  p_painter->restore();
}

QList<QPair<QPoint, Object::ControlPointType>> GraphicsPolygonItem::ComputeControlPoints() const {
  QList<QPair<QPoint, Object::ControlPointType>> controlPoints;
  for (auto const& vertex: m_polygon->GetVertices()) {
    controlPoints << QPair(QPoint(vertex.GetX(), vertex.GetY()), Object::eVertex);
  }
  if (m_polygon->HasEnoughVertices()) {
    auto barycenter = m_polygon->Barycenter();
    controlPoints << QPair(QPoint(barycenter.GetX(), barycenter.GetY()), Object::eCenter);
  }
  return controlPoints;
}

void GraphicsPolygonItem::ComputeBoundingPolygon() {
  m_boundingPolygon = *m_polygon;
}

GraphicsPolygonItem::~GraphicsPolygonItem() = default;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TAPE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsTapeItem::GraphicsTapeItem(Tape* p_tape, QGraphicsItem* p_parent):
  GraphicsObjectItem(p_parent),
  m_tape(p_tape) {
}

GraphicsTapeItem::~GraphicsTapeItem() = default;

QRectF GraphicsTapeItem::boundingRect() const {
  return QRectF(m_tape->GetXmin()-5, m_tape->GetYmin()-5, m_tape->GetW()+10, m_tape->GetH()+10);
}

void GraphicsTapeItem::DrawObject(QPainter* p_painter) {
  auto color = GetColorAccordingToItemState().first();
  auto fadeColor = color;
  fadeColor.setAlpha(63);

  p_painter->save();
  p_painter->setBrush(QBrush(fadeColor));
  p_painter->setPen(QPen(QBrush(color), 7));
  p_painter->drawRect(m_tape->GetXmin(), m_tape->GetYmin(), m_tape->GetW(), m_tape->GetH());
  p_painter->restore();
}

void GraphicsTapeItem::ComputeBoundingPolygon() {
  auto l = m_tape->GetX1();
  auto t = m_tape->GetY1();
  auto r = m_tape->GetX2();
  auto b = m_tape->GetY2();

  ppxl::Point lt(l, t);
  ppxl::Point rt(r, t);
  ppxl::Point rb(r, b);
  ppxl::Point lb(l, b);

  m_boundingPolygon = ppxl::Polygon({lt, rt, rb, lb});
}

QList<QPair<QPoint, Object::ControlPointType>> GraphicsTapeItem::ComputeControlPoints() const {
  auto l = m_tape->GetX1();
  auto t = m_tape->GetY1();
  auto r = m_tape->GetX2();
  auto b = m_tape->GetY2();

  QPoint lt(l, t);
  QPoint tt((l+r)/2, t);
  QPoint rt(r, t);
  QPoint rr(r, (t+b)/2);
  QPoint rb(r, b);
  QPoint bb((l+r)/2, b);
  QPoint lb(l, b);
  QPoint ll(l, (t+b)/2);
  QPoint c((l+r)/2, (t+b)/2);

  return {
    {lt, Object::eTopLeft},
    {tt, Object::eTop},
    {rt, Object::eTopRight},
    {rr, Object::eRight},
    {rb, Object::eBottomRight},
    {bb, Object::eBottom},
    {lb, Object::eBottomLeft},
    {ll, Object::eLeft},
    {c, Object::eCenter}
  };
}

QList<QColor> GraphicsTapeItem::GetEnabledColors() const {
  return {QColor("#f44336")};
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MIRROR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsMirrorItem::GraphicsMirrorItem(Mirror* p_mirror, QGraphicsItem* p_parent):
  GraphicsObjectItem(p_parent),
  m_mirror(p_mirror) {
}

GraphicsMirrorItem::~GraphicsMirrorItem() = default;

QRectF GraphicsMirrorItem::boundingRect() const {
  double left;
  double top;
  double right;
  double bottom;
  m_boundingPolygon.ComputeBoundingRect(left, top, right, bottom);

  return QRectF(QPointF(left-5, top-5), QPointF(right+5, bottom+5));
}

void GraphicsMirrorItem::DrawObject(QPainter* p_painter) {
  auto color = GetColorAccordingToItemState().first();
  auto factor = 40;
  auto mirrorLine = m_mirror->GetLine();
  auto mirrorNormal = mirrorLine.GetNormal();
  auto mirrorDirection = mirrorLine.GetDirection();
  mirrorLine.Translate(factor/2 * mirrorNormal);

  auto ComputeCrossHatching = [](ppxl::Segment const& p_line, int p_factor, int p_spacing) {
    auto ComputeCrossHatchingBounds = [](ppxl::Segment const& p_line, int p_spacing, int p_shift) {
      auto lineDirection = p_line.GetDirection();
      std::vector<ppxl::Point> crossHatchingBounds;

      if (lineDirection.IsNull()) {
        return crossHatchingBounds;
      }

      auto firstPoint = p_line.GetA();
      ppxl::Point nextPoint = firstPoint.Translated(p_shift*lineDirection).Translated(p_spacing*lineDirection);

      while (p_line.PointIsInBoundingBox(nextPoint)) {
        crossHatchingBounds.push_back(nextPoint);
        nextPoint.Translated(p_spacing*lineDirection);
      }
      return crossHatchingBounds;
    };

    std::vector<QLineF> crossHatchingList;
    auto copyLine = p_line;
    auto normal = p_line.GetNormal();
    copyLine.Translate(-p_factor*normal);

    auto firstCrossHatchingBounds = ComputeCrossHatchingBounds(p_line, p_spacing, -p_spacing);
    auto secondCrossHatchingBounds = ComputeCrossHatchingBounds(copyLine, p_spacing, 50-p_spacing);

    for (auto k = 0ul; k < std::min(firstCrossHatchingBounds.size(), secondCrossHatchingBounds.size()); ++k) {
      auto A = firstCrossHatchingBounds.at(k);
      auto B = secondCrossHatchingBounds.at(k);
      crossHatchingList.push_back(QLineF(QPointF(A.GetX(), A.GetY()), QPointF(B.GetX(), B.GetY())));
    }

    return crossHatchingList;
  };

  p_painter->save();
  p_painter->setBrush(QBrush(color));
  p_painter->setPen(Qt::NoPen);
  auto polygon = ComputeQuad(mirrorLine, factor);
  p_painter->drawPolygon(polygon);

  int k = 0;
  for (auto line: ComputeCrossHatching(mirrorLine, factor, 20)) {
    if (k%4 == 0) {
      p_painter->setPen(QPen(QBrush(Qt::white), 12));
      p_painter->drawLine(line);
    } else if (k%4 == 1) {
      p_painter->setPen(QPen(QBrush(Qt::white), 4));
      p_painter->drawLine(line);
    }
    ++k;
  }

  p_painter->restore();
}

void GraphicsMirrorItem::ComputeBoundingPolygon() {
  m_boundingPolygon.Clear();
  auto graphicsPolygon = ComputeQuad(m_mirror->GetLine(), 50);
  for (auto const& vertex: graphicsPolygon) {
    m_boundingPolygon.AppendVertex(ppxl::Point(vertex.x(), vertex.y()));
  }
  m_boundingPolygon.Translate(25*m_mirror->GetLine().GetNormal().Normalize());
}

QList<QPair<QPoint, Object::ControlPointType>> GraphicsMirrorItem::ComputeControlPoints() const {
  auto line = m_mirror->GetLine();
  auto A = line.GetA();
  auto Ax = A.GetX();
  auto Ay = A.GetY();
  auto B = line.GetB();
  auto Bx = B.GetX();
  auto By = B.GetY();
  QPoint a(Ax, Ay);
  QPoint b(Bx, By);
  QPoint c((Ax+Bx)/2, (Ay+By)/2);

  return {{a, Object::eTopLeft}, {b, Object::eBottomRight}, {c, Object::eCenter}};
}

QList<QColor> GraphicsMirrorItem::GetEnabledColors() const {
  return {QColor("#aba5d9")};
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ONE WAY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsOneWayItem::GraphicsOneWayItem(OneWay* p_oneWay, QGraphicsItem* p_parent):
  GraphicsObjectItem(p_parent),
  m_oneWay(p_oneWay) {
}

GraphicsOneWayItem::~GraphicsOneWayItem() = default;

QRectF GraphicsOneWayItem::boundingRect() const {
  double left;
  double top;
  double right;
  double bottom;
  m_boundingPolygon.ComputeBoundingRect(left, top, right, bottom);

  return QRectF(QPointF(left-5, top-5), QPointF(right+5, bottom+5));
}

void GraphicsOneWayItem::DrawObject(QPainter* p_painter) {
  auto ComputeArrow = [](ppxl::Segment const& p_line, ppxl::Vector const& p_normal) {
    auto shift = 80;
    auto lineCopy = p_line;
    lineCopy.Translate(-shift*p_normal.GetX(), -shift*p_normal.GetY());
    auto direction = lineCopy.GetDirection();
    auto lineCopyCenter = lineCopy.GetCenter();
    auto minDemiLength = std::min(30., p_line.Length()/2.);
    auto A = lineCopyCenter.Translate(minDemiLength*direction);
    auto Ax = A.GetX();
    auto Ay = A.GetY();
    auto B = lineCopyCenter.Translate(-minDemiLength*direction);
    auto Bx = B.GetX();
    auto By = B.GetY();
    auto C = lineCopyCenter;
    C.Translated(50*p_normal);
    auto Cx = C.GetX();
    auto Cy = C.GetY();

    return QPolygonF({QPoint(Ax, Ay), QPoint(Bx, By), QPoint(Cx, Cy)});
  };

  auto color = GetColorAccordingToItemState().first();

  p_painter->save();
  p_painter->setBrush(Qt::NoBrush);
  p_painter->setPen(QPen(QBrush(color), 7));
  p_painter->drawPolygon(ComputeQuad(m_oneWay->GetLine(), 100));
  p_painter->setBrush(QBrush(color));
  p_painter->drawPolygon(ComputeQuad(m_oneWay->GetLine(), 20));
  p_painter->drawPolygon(ComputeArrow(m_oneWay->GetLine(), m_oneWay->GetNormal()));
  p_painter->restore();
}

void GraphicsOneWayItem::ComputeBoundingPolygon() {
  m_boundingPolygon.Clear();
  auto graphicsPolygon = ComputeQuad(m_oneWay->GetLine(), 100);
  for (auto const& vertex: graphicsPolygon) {
    m_boundingPolygon.AppendVertex(ppxl::Point(vertex.x(), vertex.y()));
  }
}

QList<QPair<QPoint, Object::ControlPointType>> GraphicsOneWayItem::ComputeControlPoints() const {
  auto line = m_oneWay->GetLine();
  auto A = line.GetA();
  auto Ax = A.GetX();
  auto Ay = A.GetY();
  auto B = line.GetB();
  auto Bx = B.GetX();
  auto By = B.GetY();
  QPoint a(Ax, Ay);
  QPoint b(Bx, By);
  QPoint c((Ax+Bx)/2, (Ay+By)/2);

  return {{a, Object::eTopLeft}, {b, Object::eBottomRight}, {c, Object::eCenter}};
}

QList<QColor> GraphicsOneWayItem::GetEnabledColors() const {
  return {QColor("#9b0080")};
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PORTAL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsPortalItem::GraphicsPortalItem(Portal* p_portal, QGraphicsItem* p_parent):
  GraphicsObjectItem(p_parent),
  m_portal(p_portal) {
}

GraphicsPortalItem::~GraphicsPortalItem() = default;

QRectF GraphicsPortalItem::boundingRect() const {
  auto lineIn = m_portal->GetIn();
  auto A = lineIn.GetA();
  auto Ax = A.GetX();
  auto Ay = A.GetY();
  auto B = lineIn.GetB();
  auto Bx = B.GetX();
  auto By = B.GetY();
  auto lineOut = m_portal->GetOut();
  auto C = lineOut.GetA();
  auto Cx = C.GetX();
  auto Cy = C.GetY();
  auto D = lineOut.GetB();
  auto Dx = D.GetX();
  auto Dy = D.GetY();
  auto minX = qMin(qMin(Ax, Bx), qMin(Cx, Dx));
  auto maxX = qMax(qMax(Ax, Bx), qMax(Cx, Dx));
  auto minY = qMin(qMin(Ay, By), qMin(Cy, Dy));
  auto maxY = qMax(qMax(Ay, By), qMax(Cy, Dy));
  return QRectF(minX, minY, qAbs(maxX-minX), qAbs(maxY-minY));
}

void GraphicsPortalItem::DrawObject(QPainter* p_painter) {
  auto colors = GetColorAccordingToItemState();

  auto portalIn = m_portal->GetIn();
  auto portalInA = portalIn.GetA();
  auto portalInB = portalIn.GetB();
  auto colorIn = colors.at(0);

  auto portalOut = m_portal->GetOut();
  auto portalOutA = portalOut.GetA();
  auto portalOutB = portalOut.GetB();
  auto colorOut = colors.at(1);

  p_painter->save();
  p_painter->setBrush(Qt::NoBrush);
  p_painter->setPen(QPen(QBrush(colorIn), 7));
  p_painter->drawLine(portalInA.GetX(), portalInA.GetY(), portalInB.GetX(), portalInB.GetY());
  p_painter->setPen(QPen(QBrush(colorOut), 7));
  p_painter->drawLine(portalOutA.GetX(), portalOutA.GetY(), portalOutB.GetX(), portalOutB.GetY());
  p_painter->restore();
}

void GraphicsPortalItem::ComputeBoundingPolygon() {
  auto portalIn = m_portal->GetIn();
  m_boundingPolygon.Clear();
  auto graphicsPolygonIn = ComputeQuad(portalIn, 7);
  for (auto const& vertex: graphicsPolygonIn) {
    m_boundingPolygon.AppendVertex(ppxl::Point(vertex.x(), vertex.y()));
  }

  auto portalOut = m_portal->GetOut();
  m_boundingPolygonOut.Clear();
  auto graphicsPolygonOut = ComputeQuad(portalIn, 7);
  for (auto const& vertex: graphicsPolygonOut) {
    m_boundingPolygonOut.AppendVertex(ppxl::Point(vertex.x(), vertex.y()));
  }
}

bool GraphicsPortalItem::Intersect(const ppxl::Point& p_point) const {
  return GraphicsObjectItem::Intersect(p_point) || m_boundingPolygonOut.IsPointInside(p_point);
}

QList<QPair<QPoint, Object::ControlPointType>> GraphicsPortalItem::ComputeControlPoints() const {
  auto lineIn = m_portal->GetIn();
  auto A = lineIn.GetA();
  auto Ax = A.GetX();
  auto Ay = A.GetY();
  auto B = lineIn.GetB();
  auto Bx = B.GetX();
  auto By = B.GetY();
  QPoint a(Ax, Ay);
  QPoint b(Bx, By);
  QPoint c((Ax+Bx)/2, (Ay+By)/2);

  auto lineOut = m_portal->GetOut();
  auto D = lineOut.GetA();
  auto Dx = D.GetX();
  auto Dy = D.GetY();
  auto E = lineOut.GetB();
  auto Ex = E.GetX();
  auto Ey = E.GetY();
  QPoint d(Dx, Dy);
  QPoint e(Ex, Ey);
  QPoint f((Dx+Ex)/2, (Dy+Ey)/2);

  if (m_portal->IsCreating()) {
    return {{a, Object::eTopLeft}, {b, Object::eBottomRight}, {c, Object::eCenter},
      {d, Object::eTopLeft}, {e, Object::eBottomRight}, {f, Object::eCenter}};
  } else {
    return {{a, Object::eTopLeftYellow}, {b, Object::eBottomRightYellow}, {c, Object::eCenterYellow},
      {d, Object::eTopLeftBlue}, {e, Object::eBottomRightBlue}, {f, Object::eCenterBlue}};
  }
}

QList<QColor> GraphicsPortalItem::GetEnabledColors() const {
  return {QColor("#dfb069"), QColor("#8087bc")};
}

QList<QColor> GraphicsPortalItem::GetDisabledColors() const {
  return {QColor("#bdbdbd"), QColor("#9e9e9e")};
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RECTANGLE SELECTION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsRectangleSelectionItem::GraphicsRectangleSelectionItem(QGraphicsItem* p_parent):
  QGraphicsRectItem(p_parent) {
}

GraphicsRectangleSelectionItem::~GraphicsRectangleSelectionItem() = default;

void GraphicsRectangleSelectionItem::paint(QPainter* p_painter, const QStyleOptionGraphicsItem*, QWidget*) {
  p_painter->save();

  QColor selectionColorBorder("#38ACEC");
  QColor selectionColorInside = selectionColorBorder;
  selectionColorInside.setAlpha(64);
  p_painter->setPen(QPen(selectionColorBorder, 2));
  p_painter->setBrush(QBrush(selectionColorInside));

  p_painter->drawRoundedRect(rect(), 3, 3);

  p_painter->restore();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CUTTING LINE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CuttingLineGraphicsItem::CuttingLineGraphicsItem(QGraphicsItem* p_parent):
  QGraphicsItem(p_parent) {

  SetNoCut();
}

void CuttingLineGraphicsItem::SetNoCut() {
  SetCutState(eNoCut);
}

void CuttingLineGraphicsItem::SetGoodCut() {
  SetCutState(eGoodCut);
}

void CuttingLineGraphicsItem::SetBadCut() {
  SetCutState(eBadCut);
}

void CuttingLineGraphicsItem::SetLinesList(std::vector<ppxl::Segment> const& p_linesList) {
  m_linesList = p_linesList;
  update();
}

void CuttingLineGraphicsItem::SetCutState(CuttingLineGraphicsItem::CutState p_cutState)
{
  setData(eCutStateRole, p_cutState);
}

CuttingLineGraphicsItem::CutState CuttingLineGraphicsItem::GetCutState() const {
  return data(eCutStateRole).value<CutState>();
}

CuttingLineGraphicsItem::~CuttingLineGraphicsItem() = default;

void CuttingLineGraphicsItem::paint(QPainter* p_painter, const QStyleOptionGraphicsItem*, QWidget*) {
  p_painter->save();

  auto penWidth = 7;

  switch (GetCutState()) {
  case eGoodCut: {
    p_painter->setPen(QPen(QColor("#28a745"), penWidth));
    break;
  } case eBadCut: {
    p_painter->setPen(QPen(QColor("#fd7e14"), penWidth));
    break;
  } case eNoCut: {
    p_painter->setPen(QPen(QColor("#6c757d"), penWidth));
    break;
  } default:
    break;
  }

  for (auto const& segment: m_linesList) {
    auto A = segment.GetA();
    auto B = segment.GetB();
    p_painter->drawLine(QLineF(QPointF(A.GetX(), A.GetY()), QPointF(B.GetX(), B.GetY())));
  }

  p_painter->restore();
}

QRectF CuttingLineGraphicsItem::boundingRect() const {
  if (m_linesList.empty()) {
    return QRectF();
  }

  auto firstLine = m_linesList.at(0);
  auto A = firstLine.GetA();
  auto xmin = A.GetX();
  auto xmax = A.GetX();
  auto ymin = A.GetY();
  auto ymax = A.GetY();

  for (auto const& segment: m_linesList) {
    auto B = segment.GetB();
    xmin = std::min(B.GetX(), xmin);
    xmax = std::max(B.GetX(), xmax);
    ymin = std::min(B.GetY(), ymin);
  }

  return QRectF(QPointF(xmin-5, ymin-5), QPointF(xmax+5, ymax+5));
}
