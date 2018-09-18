#include "OptionsView.hxx"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

OptionsView::OptionsView(QWidget* parent):
  QWidget(parent),
  m_optionsLabel(new QLabel("Options")),
  m_menuButton(new QPushButton("Menu"))
{
  auto mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_optionsLabel);
  mainLayout->addWidget(m_menuButton);
  mainLayout->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
  setLayout(mainLayout);

  connect(m_menuButton, &QPushButton::clicked, this, &OptionsView::Done);
}
