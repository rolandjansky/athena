/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1BanksSystems/VP1BanksSystem.h"
#include "ui_vp1bankscontroller.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "GaudiKernel/ISvcLocator.h"

//____________________________________________________________________
class VP1BanksSystem::Imp {
public:
  Imp(VP1BanksSystem*tc) : theclass(tc), allowRefresh(false), clidSvc(0) {}
  VP1BanksSystem * theclass;
  bool useDetStore();
  bool allowRefresh;
  Ui::VP1BanksSystemControllerForm ui;

  QString idToName(const CLID&) const;
  IClassIDSvc *clidSvc;

  QStringList lastEmitted_keys;
  QStringList lastEmitted_types;
  QStringList lastEmitted_ids;

  //  void emitIfChanged(const QStringList& keys, const QStringList& types, const QStringList& ids) {
  void emitIfChanged(QStringList keys, QStringList types, QStringList ids) {
    if (lastEmitted_keys==keys&&lastEmitted_types==types&&lastEmitted_ids==ids)
      return;
    lastEmitted_keys=keys;
    lastEmitted_types=types;
    lastEmitted_ids=ids;
    theclass->messageVerbose("emitting entriesChanged(..) with "+str(keys.count())+" entries");
    theclass->entriesChanged(keys,types,ids);
  }

};


//____________________________________________________________________
VP1BanksSystem::VP1BanksSystem()
  : IVP1System("Banks",
	       "System for browsing the contents of StoreGate",
	       "Joe Boudreau <boudreau@pitt.edu> (original), Thomas.Kittelmann@cern.ch (VP1 implementation)"),
    d(new Imp(this))
{
  d->clidSvc=0;
  d->ui.radioButton_store_event = 0;
  d->ui.radioButton_store_detector = 0;
  d->ui.pushButton_refresh = 0;
}


//____________________________________________________________________
VP1BanksSystem::~VP1BanksSystem()
{
  delete d; d=0;
}

//____________________________________________________________________
QString VP1BanksSystem::Imp::idToName( const CLID& id ) const
{
  if (!clidSvc)
    return "";
  std::string n;
  if (clidSvc->getTypeNameOfID(id, n).isFailure())
    return "<unknown>";
  return n.c_str();
}

//____________________________________________________________________
bool VP1BanksSystem::Imp::useDetStore()
{
  return ui.radioButton_store_event && !ui.radioButton_store_event->isChecked();
}


//____________________________________________________________________
void VP1BanksSystem::create( StoreGateSvc* /*detstore*/ )
{
  QWidget * controller = new QWidget;
  d->ui.setupUi(controller);
  connect(d->ui.radioButton_store_event,SIGNAL(toggled(bool)),this,SLOT(refreshList()));
  connect(d->ui.pushButton_refresh,SIGNAL(clicked()),this,SLOT(refreshList()));
  registerController(controller);

  ISvcLocator* svcLoc = serviceLocator();
  if (!svcLoc) {
    message("Error: Got NULL pointer to the service locator!");
    return;
  }
  StatusCode status = svcLoc->service( "ClassIDSvc",d->clidSvc );
  if(!status.isSuccess()) {
    d->clidSvc=0;
    message("Error: Could not retrieve ClassIDSvc!!");
    return;
  }


}

//____________________________________________________________________
void VP1BanksSystem::refresh( StoreGateSvc* )
{
  d->allowRefresh = true;
  refreshList();
}

//____________________________________________________________________
void VP1BanksSystem::refreshList()
{
  if (!d->allowRefresh)
    return;
  messageVerbose("Refreshing list for "+QString(d->useDetStore()?"detector":"event")+" store");

  QStringList entry_key;
  QStringList entry_type;
  QStringList entry_id;

  VP1SGContentsHelper sgcont(this,d->useDetStore());

  int i(0);
  foreach (CLID id, sgcont.getPossibleCLIDs()) {
    QStringList keys = sgcont.getKeys(id);
    if (!keys.isEmpty()) {
      QString typeName = d->idToName(id);
      QString idstr = QString::number(id);
      foreach (QString key,keys) {
	entry_key << key;
	entry_type << typeName;
	entry_id << idstr;
      }
    }
    if (!i++%20)
      updateGUI();
  }
  d->emitIfChanged(entry_key,entry_type,entry_id);
}

//____________________________________________________________________
void VP1BanksSystem::erase()
{
  d->allowRefresh = false;
  d->emitIfChanged(QStringList(),QStringList(),QStringList());
}


//____________________________________________________________________
QByteArray VP1BanksSystem::saveState()
{
  VP1Serialise serialise(0/*version*/,this);
  serialise.save(IVP1System::saveState());//Info from base class

  serialise.save(d->ui.radioButton_store_event,d->ui.radioButton_store_detector);

  serialise.warnUnsaved(controllerWidget());
  return serialise.result();

}

//____________________________________________________________________
void VP1BanksSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);
  if (state.version()!=0) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  IVP1System::restoreFromState(state.restoreByteArray());

  state.restore(d->ui.radioButton_store_event,d->ui.radioButton_store_detector);

  state.warnUnrestored(controllerWidget());
}
