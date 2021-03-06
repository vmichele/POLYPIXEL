#ifndef MAINMENUWIDGET_HXX
#define MAINMENUWIDGET_HXX

#include <QWidget>

class QLabel;
class QtMaterialContainedButton;

class MainMenuWidget: public QWidget {
  Q_OBJECT

public:
  explicit MainMenuWidget(QWidget* p_parent = nullptr);

Q_SIGNALS:
  void PlayRequested();
  void CreateLevelRequested();
  void OptionsRequested();
  void ChooseLevelRequested();

protected:
  void paintEvent(QPaintEvent* p_event) override;

private:
  QtMaterialContainedButton* m_worldsButton;
  QtMaterialContainedButton* m_createLevelButton;
  QtMaterialContainedButton* m_optionsButton;
};

#endif
