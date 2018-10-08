#ifndef MAPVIEW_HXX
#define MAPVIEW_HXX

#include "GUI/GraphicsItem/PlayerItem.hxx"

#include <QGraphicsView>
#include <QMap>
#include <memory>

class PlayerItem;
class GameStartItem;

struct Level {
  Level(int p_levelNumber, QString const& p_levelName, QString const& p_levelPath):
    m_levelNumber(p_levelNumber),
    m_levelName(p_levelName),
    m_levelPath(p_levelPath) {}

int m_levelNumber;
QString m_levelName;
QString m_levelPath;
};

struct Path {
  enum Directions {
    eRight,
    eUp,
    eLeft,
    eDown,
    eNone
  };

  Path(Level* p_levelStart, QList<Directions> const& p_stepsList, Level* p_levelEnd):
    m_startLevel(p_levelStart),
    m_directionsList(p_stepsList),
    m_endLevel(p_levelEnd) {}

  Level* FindNextLevel(Level* p_level) const {
    if (p_level == m_startLevel) {
      return m_endLevel;
    } else {
      return nullptr;
    }
  }

  Level* FindPreviousLevel(Level* p_level) const {
    if (p_level == m_endLevel) {
      return m_startLevel;
    } else {
      return nullptr;
    }
  }

Level* m_startLevel;
QList<Directions> m_directionsList;
Level* m_endLevel;
};

struct World {
  World(int p_worldNumber, QString const& p_worldName);
  virtual ~World();

  void SetFirstLevelPosition(QPointF const& p_position);
  void AppendPath(Path const& p_path);
  QList<Path> GetPathsFromStartLevel(Level* p_startLevel) const;
  Path::Directions FindDirectionToNextLevel(Level* p_level);

int m_worldNumber;
QString m_worldName;
QMap<Level*, QPointF> m_positionByLevel;
QList<Level*> m_levelsList;
QList<Path> m_pathsList;
QPointF m_firstLevelPosition;
};

class MapView: public QGraphicsView {
  Q_OBJECT

public:
  explicit MapView(QWidget* p_parent = nullptr);
  ~MapView() override;

  void InitView();
  void MoveToNextLevel();

signals:
  void PlayerReachedDestination();
  void PlayLevelRequested(QString const& p_levelPath);
  void CancelLevelRequested();

protected:
  void OpenMessageBoxTest();
  void InitWorld1();
  void MovePlayer(PlayerItem::Direction p_direction);
  void AnimatePlayer();
  void PlayLevel();
  void CleanGameStart();

  void DisplayGameStart();

private:
  QGraphicsScene* m_scene;
  PlayerItem* m_player;
  GameStartItem* m_gameStartItem;

  QList<World*> m_worlds;
  Level* m_currentLevel;
  World* m_currentWorld;
  QList<Path::Directions> m_currentDirections;

  bool m_playerIsMoving;
  bool m_viewInitialized;
};

#endif
