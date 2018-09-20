#include "CreateLevelController.hxx"

#include "GUI/Models/CreateLevelModel.hxx"
#include "GUI/Views/CreateLevelView.hxx"
#include "GUI/Commands/Commands.hxx"

#include <QUndoStack>

CreateLevelController::CreateLevelController(CreateLevelModel* p_model, CreateLevelView* p_view, QObject* p_parent):
  QObject(p_parent),
  m_model(p_model),
  m_view(p_view),
  m_undoStack(new QUndoStack(this)) {

  m_view->SetModel(m_model);

  connect(m_view, &CreateLevelView::PolygonInserted, this, &CreateLevelController::InsertPolygon);
  connect(m_view, &CreateLevelView::PolygonRemoved, this, &CreateLevelController::RemovePolygon);
  connect(m_view, &CreateLevelView::PolygonMoved, this, &CreateLevelController::MovePolygon);
  connect(m_view, &CreateLevelView::VertexInserted, this, &CreateLevelController::InsertVertex);
  connect(m_view, &CreateLevelView::VertexRemoved, this, &CreateLevelController::RemoveVertex);
  connect(m_view, &CreateLevelView::VertexMoved, this, &CreateLevelController::MoveVertex);

  connect(m_undoStack, &QUndoStack::indexChanged, m_view, &CreateLevelView::Redraw);
}

CreateLevelController::~CreateLevelController() {
  // When QUndoStack is deleted, it emits a last indexChanged, calling Redraw
  // from the scribbling view, whose model is already deleted, so disconnect.
  disconnect(m_undoStack, &QUndoStack::indexChanged, m_view, &CreateLevelView::Redraw);
}

void CreateLevelController::Redraw() {
  m_view->Redraw();
}

void CreateLevelController::InsertPolygon(int p_polygonRow, ppxl::Polygon const& p_polygon) {
  QUndoCommand* addPolygonCommand = new AddPolygonCommand(m_model, p_polygonRow, p_polygon, p_polygonRow, 0);
  m_undoStack->push(addPolygonCommand);
}

void CreateLevelController::AppendPolygon(ppxl::Polygon const& p_polygon) {
  InsertPolygon(m_model->rowCount(), p_polygon);
}

void CreateLevelController::RemovePolygon(int p_polygonRow, ppxl::Polygon const& p_polygon) {
  int newPolygonRow = p_polygonRow;
  if (m_model->rowCount() == 0) {
    newPolygonRow = -1;
  } else {
    if (p_polygonRow == m_model->rowCount()-1) {
      newPolygonRow = p_polygonRow-1;
    }
  }

  QUndoCommand* removePolygonCommand = new RemovePolygonCommand(m_model, p_polygonRow, p_polygon, newPolygonRow, 0);
  m_undoStack->push(removePolygonCommand);
}

void CreateLevelController::MovePolygon(int p_polygonRow, ppxl::Vector const& p_direction, bool p_pushToStack) {
  if (p_pushToStack) {
    QUndoCommand* movePolygonCommand = new MovePolygonCommand(m_model, p_polygonRow, p_direction, p_polygonRow, -1);
    m_undoStack->push(movePolygonCommand);
  } else {
    ppxl::Polygon polygon;
    m_model->TranslatePolygon(p_polygonRow, p_direction);
  }
}

void CreateLevelController::InsertVertex(int p_polygonRow, int p_vertexRow, ppxl::Point const& p_vertex) {
  QUndoCommand* addVertexCommand = new AddVertexCommand(m_model, p_polygonRow, p_vertexRow, p_vertex, p_polygonRow, p_vertexRow);
  m_undoStack->push(addVertexCommand);
}

void CreateLevelController::AppendVertex(int p_polygonRow, const ppxl::Point& p_vertex) {
  InsertVertex(p_polygonRow, m_model->GetPolygonItemsList().at(p_polygonRow)->rowCount(), p_vertex);
}

void CreateLevelController::RemoveVertex(int p_polygonRow, int p_vertexRow, ppxl::Point const& p_vertex) {
  QUndoCommand* removeVertexCommand = new RemoveVertexCommand(m_model, p_polygonRow, p_vertexRow, p_vertex, p_polygonRow, 0);
  m_undoStack->push(removeVertexCommand);
}

void CreateLevelController::MoveVertex(int p_polygonRow, int p_vertexRow, const ppxl::Vector& p_direction, bool p_pushToStack) {
  if (p_pushToStack) {
    QUndoCommand* moverVertexCommand = new MoveVertexCommand(m_model, p_polygonRow, p_vertexRow, p_direction, p_polygonRow, p_vertexRow);
    m_undoStack->push(moverVertexCommand);
  } else {
    m_model->TranslateVertex(p_polygonRow, p_vertexRow, p_direction);
  }
}