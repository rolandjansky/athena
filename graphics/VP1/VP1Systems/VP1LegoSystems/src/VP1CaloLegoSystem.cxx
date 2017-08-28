/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1LegoSystems/VP1CaloLegoSystem.h"
#include "ui_calolegocontrollerform.h"

#include <QMap>

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>

#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"
//#include "LArReadoutGeometry/FCALTile.h"

#include "LArReadoutGeometry/HECDetectorManager.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"

#include "LArReadoutGeometry/EMECDetectorManager.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"

#include "LArReadoutGeometry/EMBDetectorManager.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"

#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1JobConfigInfo.h"

#define MAX_OBJECTS_PER_TECHNOLOGY 3000

class VP1CaloLegoSystem::Clockwork
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


  std::map < SoNode *, const FCALTile *>   TileMap;
  std::map < SoNode *, HECCellConstLink>   HECMap;
  std::map < SoNode *, EMECCellConstLink>  EMECMap;
  std::map < SoNode *, EMBCellConstLink>   EMBMap;

  SoSeparator * dummyCubeSep;
  SoSeparator * root;

};

VP1CaloLegoSystem::VP1CaloLegoSystem()
  :IVP13DSystemSimple("CaloLego","Display the readout in an eta-phi view","boudreau@pitt.edu"),
   _clockwork(new Clockwork())
{
  _clockwork->dummyCubeSep = 0;
  _clockwork->root = 0;
}

VP1CaloLegoSystem::~VP1CaloLegoSystem()
{
  delete _clockwork;
  _clockwork = 0;
}

QWidget* VP1CaloLegoSystem::buildController()
{
  QWidget* controller = new QWidget(0);
  Ui::frmCaloLegoController ui;
  ui.setupUi(controller);

  VP1JobConfigInfo::ensureInit(this);
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry()) {
    controller->setEnabled(false);
  }

  // Populate Check Box Names Map
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMB0, "EMB0");
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMB1, "EMB1");
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMB2, "EMB2");
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMB3, "EMB3");
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMEC0,"EMEC0");
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMEC1,"EMEC1");
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMEC2,"EMEC2");
  _clockwork->checkBoxNamesMap.insert(ui.chbxEMEC3,"EMEC3");
  _clockwork->checkBoxNamesMap.insert(ui.chbxHEC0, "HEC0");
  _clockwork->checkBoxNamesMap.insert(ui.chbxHEC1, "HEC1");
  _clockwork->checkBoxNamesMap.insert(ui.chbxHEC2, "HEC2");
  _clockwork->checkBoxNamesMap.insert(ui.chbxHEC3, "HEC3");

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    _clockwork->checkBoxMap.insert(_clockwork->checkBoxNamesMap[cb],cb);
  }

  return controller;
}

void VP1CaloLegoSystem::systemcreate(StoreGateSvc* )
{
}

void VP1CaloLegoSystem::buildPermanentSceneGraph(StoreGateSvc*, SoSeparator *root) {

  VP1JobConfigInfo::ensureInit(this);
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;

  _clockwork->root = root;

  for (int i=0;i<3;i++) {
    _clockwork->fcalSwitch[i] = new SoSwitch();
    root->addChild(_clockwork->fcalSwitch[i]);
  }

  //We add a fully transparent dummy cube in order to get proper
  //initial view (after first event we remove it again):
  SoSeparator * dummyCubeSep = new SoSeparator;
  dummyCubeSep->ref();
  SoMaterial * dummyMat = new SoMaterial;
  dummyMat->transparency = 1.0f;
  SoTranslation * dummyTransl = new SoTranslation;
  dummyTransl->translation.setValue(SbVec3f(0,3.15,0));
  SoCube * dummyCube = new SoCube;
  dummyCube->width = 6.7;
  dummyCube->height = 6.3;
  dummyCube->depth = 0.1;
  dummyCubeSep->addChild(dummyMat);
  dummyCubeSep->addChild(dummyTransl);
  dummyCubeSep->addChild(dummyCube);
  root->addChild(dummyCubeSep);

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

  createEtaPhi();

}

void VP1CaloLegoSystem::systemerase()
{
  if (_clockwork->dummyCubeSep) {
    if (_clockwork->root->findChild(_clockwork->dummyCubeSep)>=0)
      _clockwork->root->removeChild(_clockwork->dummyCubeSep);
    _clockwork->dummyCubeSep->unref();
    _clockwork->dummyCubeSep = 0;
  }

}

void VP1CaloLegoSystem::systemuncreate()
{
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;

  _clockwork->root->enableNotify(false);
  for (int i=0;i<4;i++) {
    if (i<3) {
      _clockwork->fcalSwitch[i]->enableNotify(false);
      _clockwork->fcalSwitch[i]->removeAllChildren();
    }
    _clockwork->embSwitch[i]->enableNotify(false);
    _clockwork->emecSwitch[i]->enableNotify(false);
    _clockwork->hecSwitch[i]->enableNotify(false);
    _clockwork->embSwitch[i]->removeAllChildren();
    _clockwork->emecSwitch[i]->removeAllChildren();
    _clockwork->hecSwitch[i]->removeAllChildren();
  }

  _clockwork->TileMap.clear();
  _clockwork->HECMap.clear();
  _clockwork->EMECMap.clear();
  _clockwork->EMBMap.clear();
  _clockwork->root->removeAllChildren();
}

void VP1CaloLegoSystem::createEtaPhi() {

  //  Styles & cet:
  SoDrawStyle *drawStyle = new SoDrawStyle();
  drawStyle->lineWidth=1;
  SoLightModel *lm = new SoLightModel();
  lm->model=SoLightModel::BASE_COLOR;

  SoMaterial *blue= new SoMaterial();
  blue->diffuseColor.setValue(0,0,1);

  SoMaterial *green= new SoMaterial();
  green->diffuseColor.setValue(0 , 1, 0);

  SoMaterial *yellow= new SoMaterial();
  yellow->diffuseColor.setValue(0, 1.00, 1.00);

  SoMaterial *magenta = new SoMaterial();
  magenta->diffuseColor.setValue(1.00,0.00, 1.00);

  for (int i=0;i<4;i++) {
    if (i<3) {
      _clockwork->fcalSwitch[i]->addChild(drawStyle);
      _clockwork->fcalSwitch[i]->addChild(lm);
      _clockwork->fcalSwitch[i]->addChild(green);
    }
    _clockwork->embSwitch[i]->addChild(drawStyle);
    _clockwork->emecSwitch[i]->addChild(drawStyle);
    _clockwork->hecSwitch[i]->addChild(drawStyle);
    _clockwork->embSwitch[i]->addChild(lm);
    _clockwork->emecSwitch[i]->addChild(lm);
    _clockwork->hecSwitch[i]->addChild(lm);
    _clockwork->embSwitch[i]->addChild(blue);
    _clockwork->emecSwitch[i]->addChild(magenta);
    _clockwork->hecSwitch[i]->addChild(yellow);
  }

  VP1DetInfo::ensureInit(this);

  {
    const FCALDetectorManager * manager=VP1DetInfo::fcalDetMgr();

    if (manager) {
      FCALDetectorManager::ConstIterator e;
      for (e=manager->beginFCAL();e!=manager->endFCAL();  e++) {

	const FCALModule *fcalMod = *e;
	SoSeparator *sep1 = new SoSeparator();
	VP1ExtraSepLayerHelper *sep = new VP1ExtraSepLayerHelper(sep1);

	FCALModule::ConstIterator   t;
	for (t=fcalMod->beginTiles();t!=fcalMod->endTiles();t++) {
	  double x = t->getX();
	  double y = t->getY();
	  double dx = fcalMod->getFullWidthX(*t)/2.0;
	  double dy = fcalMod->getFullWidthY(*t)/2.0;
// 	  double zf = fcalMod->getEndcapIndex()== 0 ?  +fcalMod->getFullDepthZ(*t)/2.0 : -fcalMod->getFullDepthZ(*t)/2.0;
// 	  double zc = 0;
// 	  double zb = fcalMod->getEndcapIndex()== 0 ?  -fcalMod->getFullDepthZ(*t)/2.0 : +fcalMod->getFullDepthZ(*t)/2.0;

	  int cc=0;
	  SoVertexProperty *vtxProperty = new SoVertexProperty();
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(x-dx+3,y-dy+3   ,0));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(x+dx-3,y-dy+3,   0));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(x+dx-3,y+dy-3   ,0));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(x-dx+3,y+dy-3   ,0));
	  vtxProperty->vertex.set1Value(cc++,  SbVec3f(x-dx+3,y-dy+3   ,0));

	  SoLineSet *ls = new SoLineSet();
	  ls->numVertices=5;
	  ls->vertexProperty=vtxProperty;
	  sep->addNode(ls);

	  _clockwork->TileMap[ls]=&(*t);
	}
	int sp = fcalMod->getModuleIndex()-1;
	_clockwork->fcalSwitch[sp]->addChild(sep1);

      }
    }
  }

  {
    const HECDetectorManager * manager=VP1DetInfo::hecDetMgr();

    if (manager) {

      HECDetectorManager::DetectorRegionConstIterator e;
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  e++) {

	const HECDetectorRegion *region=*e;
	SoSeparator *sep1 = new SoSeparator();
	VP1ExtraSepLayerHelper *sep = new VP1ExtraSepLayerHelper(sep1);

	for (unsigned int iPhi=region->beginPhiIndex();iPhi<region->endPhiIndex();iPhi++) {
	  for (unsigned int iEta=region->beginEtaIndex();iEta<region->endEtaIndex();iEta++) {
	    HECCellConstLink cellPtr = region->getHECCell(iEta,iPhi);
	    if (cellPtr) {

	      double phiMin = cellPtr->getPhiLocalLower();
	      double phiMax = cellPtr->getPhiLocalUpper();
	      double etaMin = cellPtr->getEtaMinNominal();
	      double etaMax = cellPtr->getEtaMaxNominal();

	      if (region->getEndcapIndex()==0) {
		phiMin = M_PI-phiMin;
		phiMax = M_PI-phiMax;
		etaMin = -etaMin;
		etaMax = -etaMax;
	      }

	      int cc=0;
	      SoVertexProperty *vtxProperty = new SoVertexProperty();
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMin  ,0));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMax  ,0));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMax,phiMax  ,0));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMax,phiMin  ,0));
	      vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMin  ,0));

	      SoLineSet *ls = new SoLineSet();
	      ls->numVertices=5;
	      ls->vertexProperty=vtxProperty;
	      sep->addNode(ls);

	      _clockwork->HECMap[ls]=cellPtr;
	    }
	  }
	}
	_clockwork->hecSwitch[region->getSamplingIndex()]->addChild(sep1);
      }
    }
  }


  {
    const EMECDetectorManager * manager=VP1DetInfo::emecDetMgr();

    if (manager) {

      EMECDetectorManager::DetectorRegionConstIterator e;
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  e++) {
	const EMECDetectorRegion *region = *e;

	// Then grid:


	SoSeparator *sep1 = new SoSeparator();
	VP1ExtraSepLayerHelper *sep = new VP1ExtraSepLayerHelper(sep1);


	for (unsigned int iPhi=region->beginPhiIndex();iPhi<region->endPhiIndex();iPhi++) {
	  for (unsigned int iEta=region->beginEtaIndex();iEta<region->endEtaIndex();iEta++) {



	    EMECCellConstLink cellPtr = ((EMECDetectorRegion *) region)->getEMECCell(iEta,iPhi);
	    double phiMin = cellPtr->getPhiLocalLower();
	    double phiMax = cellPtr->getPhiLocalUpper();
	    double etaMin = cellPtr->getEtaMin();
	    double etaMax = cellPtr->getEtaMax();

	    if (region->getEndcapIndex()==0) {
	      phiMin = M_PI-phiMin;
	      phiMax = M_PI-phiMax;
	      etaMin = -etaMin;
	      etaMax = -etaMax;
	    }

	    int cc=0;
	    SoVertexProperty *vtxProperty = new SoVertexProperty();
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMin  ,0));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMax  ,0));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMax,phiMax  ,0));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMax,phiMin  ,0));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMin  ,0));

	    SoLineSet *ls = new SoLineSet();
	    ls->numVertices=5;
	    ls->vertexProperty=vtxProperty;
	    sep->addNode(ls);
	    _clockwork->EMECMap[ls]=cellPtr;
	  }
	}
	_clockwork->emecSwitch[region->getSamplingIndex()]->addChild(sep1);
      }
    }
  }
  {
    const EMBDetectorManager * manager=VP1DetInfo::embDetMgr();

    if (manager) {

      EMBDetectorManager::DetectorRegionConstIterator e;
      for (e=manager->beginDetectorRegion();e!=manager->endDetectorRegion();  e++) {
	const EMBDetectorRegion *region = *e;
	SoSeparator *sep1 = new SoSeparator();
	VP1ExtraSepLayerHelper *sep = new VP1ExtraSepLayerHelper(sep1);

	for (unsigned int iPhi=region->beginPhiIndex();iPhi<region->endPhiIndex();iPhi++) {
	  for (unsigned int iEta=region->beginEtaIndex();iEta<region->endEtaIndex();iEta++) {


	    EMBCellConstLink cellPtr = ((EMBDetectorRegion *) region)->getEMBCell(iEta,iPhi);
	    double phiMin = cellPtr->getPhiLocalLower();
	    double phiMax = cellPtr->getPhiLocalUpper();
	    double etaMin = cellPtr->getEtaMin();
	    double etaMax = cellPtr->getEtaMax();

	    if (region->getEndcapIndex()==0) {
	      phiMin = M_PI-phiMin;
	      phiMax = M_PI-phiMax;
	      etaMin = -etaMin;
	      etaMax = -etaMax;
	    }

	    int cc=0;
	    SoVertexProperty *vtxProperty = new SoVertexProperty();
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMin  ,0));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMax  ,0));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMax,phiMax  ,0));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMax,phiMin  ,0));
	    vtxProperty->vertex.set1Value(cc++,  SbVec3f(etaMin,phiMin  ,0));

	    SoLineSet *ls = new SoLineSet();
	    ls->numVertices=5;
	    ls->vertexProperty=vtxProperty;
	    sep->addNode(ls);
	    _clockwork->EMBMap[ls]=cellPtr;
	  }
	}
	_clockwork->embSwitch[region->getSamplingIndex()]->addChild(sep1);
      }
    }
  }
}

void VP1CaloLegoSystem::buildEventSceneGraph(StoreGateSvc* , SoSeparator *)
{
}

void VP1CaloLegoSystem::checkboxChanged()
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


void VP1CaloLegoSystem::userPickedNode(SoNode* /*pickedNode*/, SoPath */*pickedPath*/)
{
}

QByteArray VP1CaloLegoSystem::saveState()
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

void VP1CaloLegoSystem::restoreFromState(QByteArray ba)
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
