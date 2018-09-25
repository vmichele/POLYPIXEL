#include "PolygonItemDelegate.hxx"

#include "GUI/Models/PolygonModel.hxx"

#include <QSpinBox>

#include <limits>

PolygonItemDelegate::PolygonItemDelegate(QObject* p_parent):
  QStyledItemDelegate(p_parent) {
}

QWidget* PolygonItemDelegate::createEditor(QWidget* p_parent, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const {
  Q_UNUSED(p_option);

  QSpinBox* editor = nullptr;

  if (p_index.data(PolygonModel::eItemTypeRole).value<PolygonModel::ItemType>() == PolygonModel::eX) {
    editor = new QSpinBox(p_parent);
    connect(editor, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, p_index](int p_value) {
      Q_EMIT(ValueXChanged(p_value, p_index));
    });
  } else if (p_index.data(PolygonModel::eItemTypeRole).value<PolygonModel::ItemType>() == PolygonModel::eY) {
    editor = new QSpinBox(p_parent);
    connect(editor, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, p_index](int p_value) {
      Q_EMIT(ValueYChanged(p_value, p_index));
    });
  }

  if (editor) {
    editor->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
  }

  return editor;
}

void PolygonItemDelegate::setEditorData(QWidget* p_editor, const QModelIndex& p_index) const {
  if (p_index.data(PolygonModel::eItemTypeRole).value<PolygonModel::ItemType>() == PolygonModel::eX
   || p_index.data(PolygonModel::eItemTypeRole).value<PolygonModel::ItemType>() == PolygonModel::eY) {
    auto editor = qobject_cast<QSpinBox*>(p_editor);
    editor->setValue(p_index.data().toInt());
  }
}

void PolygonItemDelegate::setModelData(QWidget* p_editor, QAbstractItemModel* p_model, const QModelIndex& p_index) const {
  if (p_index.data(PolygonModel::eItemTypeRole).value<PolygonModel::ItemType>() == PolygonModel::eX
   || p_index.data(PolygonModel::eItemTypeRole).value<PolygonModel::ItemType>() == PolygonModel::eY) {



    auto editor = qobject_cast<QSpinBox*>(p_editor);
    p_model->setData(p_index, editor->value());
  }
}