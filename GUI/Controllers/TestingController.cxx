#include "TestingController.hxx"

#include "GUI/Views/TestingWidget.hxx"
#include "GUI/Models/PolygonModel.hxx"
#include "GUI/Models/ObjectModel.hxx"
#include "Parser/Serializer.hxx"

#include <QFileDialog>
#include <QVector>

TestingController::TestingController(TestingWidget* p_view, QObject* p_parent):
  PlayingController(p_view, p_parent) {

  m_view = static_cast<TestingWidget*>(p_view);

  connect(m_view, &TestingWidget::RestartRequested, this, &TestingController::Restart);
  connect(m_view, &TestingWidget::SaveLevelRequested, this, &TestingController::SaveLevel);
}

void TestingController::SetPolygonsItem(PolygonModel* p_model) {
  m_polygonsList.clear();
  m_polygonModel->InitColor();

  for (auto* polygon: p_model->GetPolygonsList()) {
    m_polygonsList << ppxl::Polygon(*polygon);
  }

  m_polygonModel->SetPolygonsList(m_polygonsList);
  m_view->SetModel(m_polygonModel);
}

void TestingController::ResetPolygonList() {
  m_polygonModel->SetPolygonsList(m_polygonsList);
}

void TestingController::SetLinesGoal(int p_linesGoal) {
  m_gameInfo.m_linesCount = 0;
  m_gameInfo.m_linesGoal = p_linesGoal;
}

void TestingController::SetPartsGoal(int p_partsGoal) {
  m_gameInfo.m_partsCount = 0;
  m_gameInfo.m_partsGoal = p_partsGoal;
}

void TestingController::SetMaxGapToWin(int p_maxGapToWin) {
  m_gameInfo.m_maxGapToWin = p_maxGapToWin;
}

void TestingController::SetTolerance(int p_tolerance) {
  m_gameInfo.m_tolerance = p_tolerance;
}

void TestingController::Restart() {
  ResetPolygonList();
  PlayLevel();
}

void TestingController::PlayLevel() {
  m_levelPath = "";
  m_gameInfo.m_linesCount = 0;
  m_gameInfo.m_partsCount = m_polygonModel->GetPolygonsCount();
  m_gameInfo.m_stars = 0;
  m_slicer.InitTotalOrientedArea();
  m_view->SetSaveButtonEnable(false);

  UpdateViewFromGameInfo();
  m_view->StartLevel();
  Redraw();

  connect(m_polygonModel, &PolygonModel::PolygonListChanged, this, &TestingController::Redraw);
  connect(m_view, &TestingWidget::Moving, this, &TestingController::DisplayAreas);
}

void TestingController::Redraw() {
  m_view->ClearImage();
  m_view->DrawFromModel();
}

void TestingController::DisplayAreas(QPoint const& p_endPoint) {
  auto lines = MoveLine(p_endPoint);

  if (m_slicer.ComputeLinesType(std::vector<ppxl::Segment>(lines.begin(), lines.end())) == Slicer::eGoodCrossing) {
    QList<ppxl::Polygon> newPolygonList;

    double orientedAreaTotal = 0.;
    for (auto polygon: newPolygonList) {
      orientedAreaTotal += polygon.OrientedArea();
    }

    for (auto polygon: newPolygonList) {
      double currArea = qRound(10.*polygon.OrientedArea() * 100. / orientedAreaTotal) / 10.;
      m_view->DrawText(polygon.Barycenter(), QString::number(currArea), 50);
    }
  }
}

void TestingController::CheckWinning() {
  if (m_gameInfo.m_linesCount >= m_gameInfo.m_linesGoal || m_gameInfo.m_partsCount >= m_gameInfo.m_partsGoal) {
    double minArea;
    double maxArea;
    auto areasList = m_slicer.ComputeAreas(minArea, maxArea);
    double gap = std::abs(maxArea - minArea);

    auto starsCount = ComputeStarsCount(gap);
    m_gameInfo.m_stars = starsCount;
    m_view->SetSaveButtonEnable(starsCount == 4);

    UpdateViewFromGameInfo();

    //m_view->DrawAreas(areasList);
    m_view->EndLevel();

    disconnect(m_polygonModel, &PolygonModel::PolygonListChanged, this, &TestingController::Redraw);
    disconnect(m_view, &TestingWidget::Moving, this, &TestingController::DisplayAreas);
  }
}

void TestingController::UpdateViewFromGameInfo() {
  PlayingController::UpdateViewFromGameInfo();
  m_view->UpdateStarsCount(std::min(m_gameInfo.m_stars, 3));
  m_view->UpdatePerfect(m_gameInfo.m_stars == 4);
}

void TestingController::SaveLevel() {
  auto fileName = QFileDialog::getSaveFileName(m_view, tr("Save your level"), ":/world1", "POLYPIXEL Files (*.ppxl)");

  if (fileName.isEmpty()) {
    return;
  }

  Serializer serializer(fileName);
  serializer.SetPolygonsList(m_polygonsList);
  serializer.SetLinesGoal(m_gameInfo.m_linesGoal);
  serializer.SetPartsGoal(m_gameInfo.m_partsGoal);
  serializer.SetMaxGapToWin(m_gameInfo.m_maxGapToWin);
  serializer.SetTolerance(m_gameInfo.m_tolerance);
  serializer.SetStarsCount(0);

  auto tapeModel = static_cast<TapeModel*>(m_objectModelsList.at(ObjectModel::eTapeModel));
  serializer.SetTapeList(tapeModel->GetTapesList());

  auto mirrorModel = static_cast<MirrorModel*>(m_objectModelsList.at(ObjectModel::eMirrorModel));
  serializer.SetMirrorsList(mirrorModel->GetMirrorsList());

  auto oneWayModel = static_cast<OneWayModel*>(m_objectModelsList.at(ObjectModel::eOneWayModel));
  serializer.SetOneWaysList(oneWayModel->GetOneWaysList());

  auto portalModel = static_cast<PortalModel*>(m_objectModelsList.at(ObjectModel::ePortalModel));
  serializer.SetPortalsList(portalModel->GetPortalsList());

  serializer.WriteXML();
}
