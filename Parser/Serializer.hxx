#ifndef SERIALIZER_HXX
#define SERIALIZER_HXX

#include <QtXml/QDomElement>

#include "Core/Geometry/Point.hxx"
#include "Core/Geometry/Segment.hxx"
#include "Core/Geometry/Polygon.hxx"
#include "Core/Objects/Obstacles/Tape.hxx"
#include "Core/Objects/Obstacles/OneWay.hxx"
#include "Core/Objects/Deviations/Mirror.hxx"
#include "Core/Objects/Deviations/Portal.hxx"

class Serializer {
public:
//  Serializer();
  Serializer(QString const& p_xmlFileName);

  virtual ~Serializer();

  // Serialize
  void WriteXML(int p_indent = 2);

  // GameInfos
  void SetPartsGoal(int p_partsGoal = 1);
  void SetLinesGoal(int p_linesGoal = 0);
  void SetMaxGapToWin(int p_maxGapToWin = 1);
  void SetTolerance(int p_tolerance = 1);
  void SetStarsCount(int p_starscount = 0);

  // Polygon
  QDomElement PointToNode(ppxl::Point const& p_point, QString const& p_nodeName);
  QDomElement SegmentToNode(ppxl::Segment const& p_segment);
  void AppendPolygon(ppxl::Polygon const& p_polygon, int p_id);
  void SetPolygonsList(const QList<ppxl::Polygon>& p_polygons);

  // Tape
  QDomElement TapeToNode(Tape const& p_tape, int p_id);
  void AppendTape(Tape const& p_tape, int p_id);
  void SetTapeList(QList<Tape*> const& p_tapes);

  // OneWay
  QDomElement OneWayToNode(OneWay const& p_oneWay, int p_id);
  void AppendOneWay(OneWay const& p_oneWay, int p_id);
  void SetOneWaysList(QList<OneWay*> const& p_oneWays);

  // Mirror
  QDomElement MirrorToNode(Mirror const& p_mirror, int p_id);
  void AppendMirror(Mirror const& p_mirror, int p_id);
  void SetMirrorsList(QList<Mirror*> const& p_mirrors);

  // Portal
  QDomElement PortalToNode(Portal const& p_portal, int p_id);
  void AppendPortal(Portal const& p_portal, int p_id);
  void SetPortalsList(QList<Portal*> const& p_portals);

private:
  QString m_xmlFileName;
  QDomDocument m_doc;

  QDomElement m_polygons;
  QDomElement m_tapes;
  QDomElement m_oneWays;
  QDomElement m_mirrors;
  QDomElement m_portals;
  QDomElement m_linesGoal;
  QDomElement m_partsGoal;
  QDomElement m_maxGapToWin;
  QDomElement m_tolerance;
  QDomElement m_starsCount;
};

#endif
