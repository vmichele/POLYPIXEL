#ifndef OBJECTMODEL_HXX
#define OBJECTMODEL_HXX

#include <QStandardItemModel>

#include "Objects/Deviations/Mirror.hxx"
#include "Objects/Deviations/Portal.hxx"
#include "Objects/Mutables/Countdown.hxx"
#include "Objects/Mutables/Disposable.hxx"
#include "Objects/Mutables/Switch.hxx"
#include "Objects/Mutables/Transfer.hxx"
#include "Objects/Obstacles/OneWay.hxx"
#include "Objects/Obstacles/Tape.hxx"

#include <QStandardItemModel>

class MirrorModel: public QStandardItemModel {

public:
  MirrorModel(QObject* p_parent = nullptr);
  ~MirrorModel() override;

  void AddMirror(Mirror const& p_mirror);
  inline QList<Mirror*> GetMirrorsList() const { return m_mirrorsList; }

private:
  QStandardItem* m_mirrorsItem;
  QList<Mirror*> m_mirrorsList;
};



class PortalModel: public QStandardItemModel {

public:
  PortalModel(QObject* p_parent = nullptr);
  ~PortalModel() override;

  void AddPortal(Portal const& p_portal);
  inline QList<Portal*> GetPortalsList() const { return m_portalsList; }

private:
  QStandardItem* m_portalsItem;
  QList<Portal*> m_portalsList;
};



class CountdownModel: public QStandardItemModel {

public:
  CountdownModel(QObject* p_parent = nullptr);
  ~CountdownModel() override;

  void AddCountdown(Countdown const& p_countdown);
  inline QList<Countdown*> GetCountdownsList() const { return m_countdownsList; }

private:
  QStandardItem* m_countdownsItem;
  QList<Countdown*> m_countdownsList;
};



class DisposableModel: public QStandardItemModel {

public:
  DisposableModel(QObject* p_parent = nullptr);
  ~DisposableModel() override;

  void AddDisposable(Disposable const& p_disposable);
  inline QList<Disposable*> GetDisposablesList() const { return m_disposablesList; }

private:
  QStandardItem* m_disposablesItem;
  QList<Disposable*> m_disposablesList;
};



class SwitchModel: public QStandardItemModel {

public:
  SwitchModel(QObject* p_parent = nullptr);
  ~SwitchModel() override;

  void AddSwitch(Switch const& p_switch);
  inline QList<Switch*> GetSwitchesList() const { return m_switchesList; }

private:
  QStandardItem* m_switchesItem;
  QList<Switch*> m_switchesList;
};



class TransferModel: public QStandardItemModel {

public:
  TransferModel(QObject* p_parent = nullptr);
  ~TransferModel() override;

  void AddTransfer(Transfer const& p_transfer);
  inline QList<Transfer*> GetTransfersList() const { return m_transfersList; }

private:
  QStandardItem* m_transfersItem;
  QList<Transfer*> m_transfersList;
};



class OneWayModel: public QStandardItemModel {

public:
  OneWayModel(QObject* p_parent = nullptr);
  ~OneWayModel() override;

  void AddOneWay(OneWay const& p_oneWay);
  inline QList<OneWay*> GetOneWaysList() const { return m_oneWaysList; }

private:
  QStandardItem* m_oneWaysItem;
  QList<OneWay*> m_oneWaysList;
};



class TapeModel: public QStandardItemModel {

public:
  TapeModel(QObject* p_parent = nullptr);
  ~TapeModel() override;

  void AddTape(Tape const& p_tape);
  inline QList<Tape*> GetTapesList() const { return m_tapesList; }

private:
  QStandardItem* m_tapesItem;
  QList<Tape*> m_tapesList;
};


#endif
