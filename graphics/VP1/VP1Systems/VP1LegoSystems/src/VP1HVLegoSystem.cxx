/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1LegoSystems/VP1HVLegoSystem.h"
#include "ui_hvlegocontrollerform.h"

#include <QMap>

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>

#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Msg.h"
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
   m_clockwork(new Clockwork())
{
}

VP1HVLegoSystem::~VP1HVLegoSystem()
{
  delete m_clockwork;
  m_clockwork = 0;
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
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMB, "EMB");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMEC, "EMEC");

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,m_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    m_clockwork->checkBoxMap.insert(m_clockwork->checkBoxNamesMap[cb],cb);
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
    m_clockwork->fcalSwitch[i] = new SoSwitch();
    root->addChild(m_clockwork->fcalSwitch[i]);
  }

  for (int i=0;i<4;i++) {
    m_clockwork->embSwitch[i] = new SoSwitch();
    root->addChild(m_clockwork->embSwitch[i]);

    m_clockwork->emecSwitch[i] = new SoSwitch();
    root->addChild(m_clockwork->emecSwitch[i]);

    m_clockwork->hecSwitch[i] = new SoSwitch();
    root->addChild(m_clockwork->hecSwitch[i]);
  }
  m_clockwork->switchMap["EMB0"] = m_clockwork->embSwitch[0];
  m_clockwork->switchMap["EMB1"] = m_clockwork->embSwitch[1];
  m_clockwork->switchMap["EMB2"] = m_clockwork->embSwitch[2];
  m_clockwork->switchMap["EMB3"] = m_clockwork->embSwitch[3];

  m_clockwork->switchMap["EMEC0"] = m_clockwork->emecSwitch[0];
  m_clockwork->switchMap["EMEC1"] = m_clockwork->emecSwitch[1];
  m_clockwork->switchMap["EMEC2"] = m_clockwork->emecSwitch[2];
  m_clockwork->switchMap["EMEC3"] = m_clockwork->emecSwitch[3];

  m_clockwork->switchMap["HEC0"] = m_clockwork->hecSwitch[0];
  m_clockwork->switchMap["HEC1"] = m_clockwork->hecSwitch[1];
  m_clockwork->switchMap["HEC2"] = m_clockwork->hecSwitch[2];
  m_clockwork->switchMap["HEC3"] = m_clockwork->hecSwitch[3];


  const LArElectrodeID* elecId(0);
  if (!VP1SGAccessHelper(this,true/*detectorstore*/).retrieve(elecId, "LArElectrodeID")) {
    message("ERROR: Cannot access LArElectrodeID");
  }
  else {
    if (VP1Msg::verbose()) {
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
  if(cb && m_clockwork->checkBoxNamesMap.contains(cb))
  {
    // Get technology name
    QString swName = m_clockwork->checkBoxNamesMap[cb];

    if(m_clockwork->switchMap.contains(swName))
    {
      // Get switch
      SoSwitch* sw = m_clockwork->switchMap[swName];
      if(cb->isChecked())
      {
	sw->whichChild = SO_SWITCH_ALL;
      }
      else
	sw->whichChild = SO_SWITCH_NONE;
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
  QMapIterator<QString,QCheckBox*> it(m_clockwork->checkBoxMap);
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
  QMapIterator<QString,QCheckBox*> it(m_clockwork->checkBoxMap);
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
