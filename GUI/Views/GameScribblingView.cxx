#include "GameScribblingView.hxx"

#include <QPainter>
#include <QMouseEvent>

GameScribblingView::GameScribblingView(QWidget* p_parent):
  AbstractScribblingView(p_parent),
  m_scribbling(false) {
}

GameScribblingView::~GameScribblingView() = default;

void GameScribblingView::mousePressEvent(QMouseEvent* p_event) {
  if (p_event->buttons() == Qt::LeftButton && !m_scribbling) {
    m_scribbling = true;
    Q_EMIT(Scribbling(p_event->pos()));
  }
}

void GameScribblingView::mouseMoveEvent(QMouseEvent* p_event) {
  if (m_scribbling) {
    Q_EMIT(Moving(p_event->pos()));
  }
}

void GameScribblingView::mouseReleaseEvent(QMouseEvent* p_event) {
  if (m_scribbling) {
    Q_EMIT(Slicing(p_event->pos()));
    m_scribbling = false;
  }
}

void GameScribblingView::paintEvent(QPaintEvent* p_event) {
  QPainter painter(this);
  QRect dirtyRect = p_event->rect();
  painter.drawImage(dirtyRect, GetImage(), dirtyRect);
}