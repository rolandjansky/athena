/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1LegoSystems/VP1CaloHitLegoSystem.h"
#include "ui_calohitlegocontrollerform.h"

#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1JobConfigInfo.h"

#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloEvent/CaloCellContainer.h"

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCube.h>

#include <QMap>

class VP1CaloHitLegoSystem::Clockwork
{
public:

  QMap<QString,SoSwitch*>    switchMap;
  QMap<QString,QCheckBox*>   checkBoxMap;
  QMap<QCheckBox*,QString>   checkBoxNamesMap;

  SoSwitch          *fcalSwitch[3];     // FCAL
  SoSwitch          *hecSwitch[4];      // HEC
  SoSwitch          *emecSwitch[4];     // EMEC
  SoSwitch          *embSwitch[4];      // EMB

  double lowThresholdEnergy;
  double highThresholdEnergy;
  double larEnergyScale;
};

VP1CaloHitLegoSystem::VP1CaloHitLegoSystem()
  :IVP13DSystemSimple("CaloHitLego","Display the calorimeter hits in an eta-phi view","boudreau@pitt.edu"),
   m_clockwork(new Clockwork())
{
  m_clockwork->lowThresholdEnergy=500;
  m_clockwork->highThresholdEnergy=1000000;
  m_clockwork->larEnergyScale=0.0002;
}

VP1CaloHitLegoSystem::~VP1CaloHitLegoSystem()
{
  delete m_clockwork;
  m_clockwork = 0;
}

QWidget* VP1CaloHitLegoSystem::buildController()
{
  QWidget* controller = new QWidget(0);
  Ui::frmCaloHitLegoController ui;
  ui.setupUi(controller);

  VP1JobConfigInfo::ensureInit(this);
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry()) {
    controller->setEnabled(false);
  }

  // Populate Check Box Names Map
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMB0, "EMB0");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMB1, "EMB1");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMB2, "EMB2");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMB3, "EMB3");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMEC0,"EMEC0");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMEC1,"EMEC1");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMEC2,"EMEC2");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEMEC3,"EMEC3");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxHEC0, "HEC0");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxHEC1, "HEC1");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxHEC2, "HEC2");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxHEC3, "HEC3");

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,m_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    m_clockwork->checkBoxMap.insert(m_clockwork->checkBoxNamesMap[cb],cb);
  }

  return controller;
}

void VP1CaloHitLegoSystem::systemuncreate()
{
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;

  for (int i=0;i<3;i++) {
    m_clockwork->fcalSwitch[i]->removeAllChildren();
    m_clockwork->fcalSwitch[i]->unref();
    m_clockwork->fcalSwitch[i]=0;
  }

  for (int i=0;i<4;i++) {
    m_clockwork->embSwitch[i]->removeAllChildren();
    m_clockwork->embSwitch[i]->unref();
    m_clockwork->embSwitch[i]=0;
    m_clockwork->emecSwitch[i]->removeAllChildren();
    m_clockwork->emecSwitch[i]->unref();
    m_clockwork->emecSwitch[i]=0;
    m_clockwork->hecSwitch[i]->removeAllChildren();
    m_clockwork->hecSwitch[i]->unref();
    m_clockwork->hecSwitch[i]=0;
  }

}

void VP1CaloHitLegoSystem::systemcreate(StoreGateSvc* /*detstore*/)
{
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;

  for (int i=0;i<3;i++) {
    m_clockwork->fcalSwitch[i] = new SoSwitch();
    m_clockwork->fcalSwitch[i]->ref();
  }

  for (int i=0;i<4;i++) {
    m_clockwork->embSwitch[i] = new SoSwitch();
    m_clockwork->embSwitch[i]->ref();
    m_clockwork->emecSwitch[i] = new SoSwitch();
    m_clockwork->emecSwitch[i]->ref();
    m_clockwork->hecSwitch[i] = new SoSwitch();
    m_clockwork->hecSwitch[i]->ref();
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
}

void VP1CaloHitLegoSystem::buildPermanentSceneGraph(StoreGateSvc* /*detStore*/, SoSeparator */*root*/)
{
}


void VP1CaloHitLegoSystem::systemerase()
{
  VP1JobConfigInfo::ensureInit(this);
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;

  for (int i=0; i<3; i++)
    m_clockwork->fcalSwitch[i]->removeAllChildren();
  for (int i=0; i<4; i++) {
    m_clockwork->embSwitch[i]->removeAllChildren();
    m_clockwork->emecSwitch[i]->removeAllChildren();
    m_clockwork->hecSwitch[i]->removeAllChildren();
  }
}

void VP1CaloHitLegoSystem::buildEventSceneGraph(StoreGateSvc* /*sg*/, SoSeparator *root)
{
  VP1JobConfigInfo::ensureInit(this);
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;
  const CaloCellContainer * p_calocells;
  QString key = "AllCalo";
  if(VP1SGContentsHelper(this).contains<CaloCellContainer>(key)) {
    if(VP1SGAccessHelper(this).retrieve(p_calocells,key)) {
      for(CaloCellContainer::const_iterator i_cell=p_calocells->begin(); i_cell!=p_calocells->end(); ++i_cell) {
	double energy = (*i_cell)->energy();
	// Cannot draw object if it has no Calo DDE
	const EMBDetectorElement *embElement = dynamic_cast<const EMBDetectorElement *> ((*i_cell)->caloDDE());
	if (embElement) {
	  bool overThresh=(energy>=m_clockwork->lowThresholdEnergy) &&  (energy<=m_clockwork->highThresholdEnergy);
	  if (overThresh) {

	    EMBCellConstLink cellPtr = (const_cast<EMBDetectorElement *> (embElement))->getEMBCell();
	    double phiMin =  cellPtr->getPhiLocalLower();
	    double phiMax =  cellPtr->getPhiLocalUpper();
	    double etaMin =  cellPtr->getEtaMin();
	    double etaMax =  cellPtr->getEtaMax();

	    if (cellPtr->getEndcapIndex()==0) {
	      phiMin = M_PI-phiMin;
	      phiMax = M_PI-phiMax;
	      etaMin = -etaMin;
	      etaMax = -etaMax;
	    }
	    double height = energy*fabs(1.0/cosh((etaMin+etaMax)/2.0))*m_clockwork->larEnergyScale;
	    SoCube *cube = new SoCube();
	    SoTranslation *translation = new SoTranslation();
	    translation->translation.setValue((etaMax+etaMin)/2.0, (phiMax+phiMin)/2.0, height/2.0);
	    cube->width=(fabs(etaMax-etaMin));
	    cube->height=(fabs(phiMax-phiMin));
	    cube->depth=height;
	    SoSeparator *sep = new SoSeparator();
	    sep->addChild(translation);
	    sep->addChild(cube);
	    m_clockwork->embSwitch[cellPtr->getSamplingIndex()]->addChild(sep);

	  }
	}
	const EMECDetectorElement *emecElement = dynamic_cast<const EMECDetectorElement *> ((*i_cell)->caloDDE());
	if (emecElement) {
	  bool overThresh=(energy>=m_clockwork->lowThresholdEnergy) &&  (energy<=m_clockwork->highThresholdEnergy);
	  if (overThresh) {

	    EMECCellConstLink cellPtr = (const_cast<EMECDetectorElement *> (emecElement))->getEMECCell();
	    double phiMin =  cellPtr->getPhiLocalLower();
	    double phiMax =  cellPtr->getPhiLocalUpper();
	    double etaMin =  cellPtr->getEtaMin();
	    double etaMax =  cellPtr->getEtaMax();

	    if (cellPtr->getEndcapIndex()==0) {
	      phiMin = M_PI-phiMin;
	      phiMax = M_PI-phiMax;
	      etaMin = -etaMin;
	      etaMax = -etaMax;
	    }
	    double height = energy*fabs(1.0/cosh((etaMin+etaMax)/2.0))*m_clockwork->larEnergyScale;
	    SoCube *cube = new SoCube();
	    SoTranslation *translation = new SoTranslation();
	    translation->translation.setValue((etaMax+etaMin)/2.0, (phiMax+phiMin)/2.0, height/2);
	    cube->width=(fabs(etaMax-etaMin));
	    cube->height=(fabs(phiMax-phiMin));
	    cube->depth=height;
	    SoSeparator *sep = new SoSeparator();
	    sep->addChild(translation);
	    sep->addChild(cube);
	    m_clockwork->emecSwitch[cellPtr->getSamplingIndex()]->addChild(sep);

	  }
	}
	const HECDetectorElement *hecElement = dynamic_cast<const HECDetectorElement *> ((*i_cell)->caloDDE());
	if (hecElement) {
	  bool overThresh=(energy>=m_clockwork->lowThresholdEnergy) &&  (energy<=m_clockwork->highThresholdEnergy);
	  if (overThresh) {

	    HECCellConstLink cellPtr = (const_cast<HECDetectorElement *> (hecElement))->getHECCell();
	    double phiMin =  cellPtr->getPhiLocalLower();
	    double phiMax =  cellPtr->getPhiLocalUpper();
	    double etaMin =  cellPtr->getEtaMinNominal();
	    double etaMax =  cellPtr->getEtaMaxNominal();

	    if (cellPtr->getEndcapIndex()==0) {
	      phiMin = M_PI-phiMin;
	      phiMax = M_PI-phiMax;
	      etaMin = -etaMin;
	      etaMax = -etaMax;
	    }
	    double height = energy*fabs(1.0/cosh((etaMin+etaMax)/2.0))*m_clockwork->larEnergyScale;
	    SoCube *cube = new SoCube();
	    SoTranslation *translation = new SoTranslation();
	    translation->translation.setValue((etaMax+etaMin)/2.0, (phiMax+phiMin)/2.0, height/2);
	    cube->width=(fabs(etaMax-etaMin));
	    cube->height=(fabs(phiMax-phiMin));
	    cube->depth=height;
	    SoSeparator *sep = new SoSeparator();
	    sep->addChild(translation);
	    sep->addChild(cube);
	    m_clockwork->hecSwitch[cellPtr->getSamplingIndex()]->addChild(sep);

	  }
	}
      }
    }
  }

  for (int i=0;i<3;i++)
    root->addChild(m_clockwork->fcalSwitch[i]);
  for (int i=0;i<4;i++) {
    root->addChild(m_clockwork->embSwitch[i]);
    root->addChild(m_clockwork->emecSwitch[i]);
    root->addChild(m_clockwork->hecSwitch[i]);
  }
}

void VP1CaloHitLegoSystem::checkboxChanged()
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
      // Get swtich
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


void VP1CaloHitLegoSystem::userPickedNode(SoNode* /*pickedNode*/, SoPath * /*pickedPath*/)
{
}

QByteArray VP1CaloHitLegoSystem::saveState()
{
  ensureBuildController();
  VP1Serialise serialise(0/*version*/,this);
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

void VP1CaloHitLegoSystem::restoreFromState(QByteArray ba)
{
  VP1Deserialise state(ba,this);
  if (state.version()!=0) {
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




