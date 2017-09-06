/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1LegoSystems/VP1HVLegoSystem.h"
#include "ui_hvlegocontrollerform.h"

#include <QMap>

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>

//#include "LArReadoutGeometry/FCALTile.h"

#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1JobConfigInfo.h"

#include "LArIdentifier/LArElectrodeID.h"


#define MAX_OBJECTS_PER_TECHNOLOGY 3000

class VP1HVLegoSystem::Clockwork
{
public:
  //    Switches:
  //    1. EMB0-3
  //    2. EMEC0-3
  //    3. HEC0-3
  QMap<QString,SoSwitch*>    switchMap;

  // Two maps for controller check boxes
  QMap<QCheckBox*,QString>   checkBoxNamesMap;
  QMap<QString,QCheckBox*>   checkBoxMap;

  SoSwitch          *fcalSwitch[3];     // FCAL
  SoSwitch          *hecSwitch[4];      // HEC
  SoSwitch          *emecSwitch[4];     // EMEC
  SoSwitch          *embSwitch[4];      // EMB


  //  std::map < SoNode *, const FCALTile *>   TileMap;
  //  std::map < SoNode *, HECCellConstLink>   HECMap;
  //  std::map < SoNode *, EMECCellConstLink>  EMECMap;
  //  std::map < SoNode *, EMBCellConstLink>   EMBMap;


};

VP1HVLegoSystem::VP1HVLegoSystem()
  :IVP13DSystemSimple("HVLego","Display the readout in an eta-phi view","boudreau@pitt.edu"),
   _clockwork(new Clockwork())
{
}

VP1HVLegoSystem::~VP1HVLegoSystem()
{
  delete _clockwork;
  _clockwork = 0;
}

QWidget* VP1HVLegoSystem::buildController()
{
  QWidget* controller = new QWidget(0);
  Ui::frmHVLegoController ui;
  ui.setupUi(controller);

  VP1JobConfigInfo::ensureInit(this);
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry()) {
    controller->setEnabled(false);
  }

  // Populate Check Box Names Map
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMB, "EMB");
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMEC, "EMEC");

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    _clockwork->checkBoxMap.insert(_clockwork->checkBoxNamesMap[cb],cb);
  }

  return controller;
}

void VP1HVLegoSystem::systemcreate(StoreGateSvc* )
{
}

void VP1HVLegoSystem::buildPermanentSceneGraph(StoreGateSvc */*detStore*/, SoSeparator *root) {

  VP1JobConfigInfo::ensureInit(this);
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;

  for (int i=0;i<3;i++) {
    _clockwork->fcalSwitch[i] = new SoSwitch();
    root->addChild(_clockwork->fcalSwitch[i]);
  }

  for (int i=0;i<4;i++) {
    _clockwork->embSwitch[i] = new SoSwitch();
    root->addChild(_clockwork->embSwitch[i]);

    _clockwork->emecSwitch[i] = new SoSwitch();
    root->addChild(_clockwork->emecSwitch[i]);

    _clockwork->hecSwitch[i] = new SoSwitch();
    root->addChild(_clockwork->hecSwitch[i]);
  }
  _clockwork->switchMap["EMB0"] = _clockwork->embSwitch[0];
  _clockwork->switchMap["EMB1"] = _clockwork->embSwitch[1];
  _clockwork->switchMap["EMB2"] = _clockwork->embSwitch[2];
  _clockwork->switchMap["EMB3"] = _clockwork->embSwitch[3];

  _clockwork->switchMap["EMEC0"] = _clockwork->emecSwitch[0];
  _clockwork->switchMap["EMEC1"] = _clockwork->emecSwitch[1];
  _clockwork->switchMap["EMEC2"] = _clockwork->emecSwitch[2];
  _clockwork->switchMap["EMEC3"] = _clockwork->emecSwitch[3];

  _clockwork->switchMap["HEC0"] = _clockwork->hecSwitch[0];
  _clockwork->switchMap["HEC1"] = _clockwork->hecSwitch[1];
  _clockwork->switchMap["HEC2"] = _clockwork->hecSwitch[2];
  _clockwork->switchMap["HEC3"] = _clockwork->hecSwitch[3];


  const LArElectrodeID* elecId(0);
  if (!VP1SGAccessHelper(this,true/*detectorstore*/).retrieve(elecId, "LArElectrodeID")) {
    message("ERROR: Cannot access LArElectrodeID");
  }
  else {
    if (verbose()) {
      std::vector<HWIdentifier>::const_iterator begin=elecId->electrode_begin(), end=elecId->electrode_end(), e;
      for (e=begin;e!=end;e++) {
	if (elecId->detector(*e) ==0) {
	  std::cout << elecId->detector(*e) << ' '
		    << elecId->zside(*e) << ' '
		    << elecId->module(*e) << ' '
		    << elecId->hv_phi(*e) << ' '
		    << elecId->hv_eta(*e) << ' '
		    << elecId->gap(*e) << ' '
		    << elecId->electrode(*e) << std::endl;
	}
      }
    }
  }

}


void VP1HVLegoSystem::buildEventSceneGraph(StoreGateSvc* , SoSeparator *)
{
}

void VP1HVLegoSystem::checkboxChanged()
{

  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;
  // Get ChB pointer
  QCheckBox* cb = dynamic_cast<QCheckBox*>(sender());
  if(cb && _clockwork->checkBoxNamesMap.contains(cb))
  {
    // Get technology name
    QString swName = _clockwork->checkBoxNamesMap[cb];

    if(_clockwork->switchMap.contains(swName))
    {
      // Get swtich
      SoSwitch* _switch = _clockwork->switchMap[swName];
      if(cb->isChecked())
      {
	_switch->whichChild = SO_SWITCH_ALL;
      }
      else
	_switch->whichChild = SO_SWITCH_NONE;
    }
  }
}


void VP1HVLegoSystem::userPickedNode(SoNode* /*pickedNode*/, SoPath */*pickedPath*/)
{
}

QByteArray VP1HVLegoSystem::saveState()
{
  ensureBuildController();
  VP1Serialise serialise(1/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());

  //Checkboxes (by name for greater stability in case we change content of map):
  QMapIterator<QString,QCheckBox*> it(_clockwork->checkBoxMap);
  QMap<QString,bool> checkboxstate;
  while (it.hasNext()) {
    it.next();
    checkboxstate.insert(it.key(),it.value()->isChecked());
    serialise.widgetHandled(it.value());
  }
  serialise.save(checkboxstate);

  serialise.warnUnsaved(controllerWidget());
  return serialise.result();
}

void VP1HVLegoSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);
  if (state.version()==0) {
    message("Warning: State data in .vp1 file has obsolete format - ignoring!");
    return;
  }
  if (state.version()!=1) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  ensureBuildController();
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  //Checkboxes (by name for greater stability in case we change content of map):
  QMap<QString,bool> checkboxstate(state.restore<QMap<QString,bool> >());
  QMapIterator<QString,QCheckBox*> it(_clockwork->checkBoxMap);
  while (it.hasNext()) {
    it.next();
    state.widgetHandled(it.value());
    if (checkboxstate.contains(it.key())) {
      bool checked = checkboxstate.value(it.key());
      if (it.value()->isChecked()!=checked)
	it.value()->setChecked(checked);
    }
  }

  state.warnUnrestored(controllerWidget());
}
