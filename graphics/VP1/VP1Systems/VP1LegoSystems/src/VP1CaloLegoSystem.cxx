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

#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "LArReadoutGeometry/FCALModule.h"

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
   m_clockwork(new Clockwork())
{
  m_clockwork->dummyCubeSep = 0;
  m_clockwork->root = 0;
}

VP1CaloLegoSystem::~VP1CaloLegoSystem()
{
  delete m_clockwork;
  m_clockwork = 0;
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

void VP1CaloLegoSystem::systemcreate(StoreGateSvc* )
{
}

void VP1CaloLegoSystem::buildPermanentSceneGraph(StoreGateSvc*, SoSeparator *root) {

  VP1JobConfigInfo::ensureInit(this);
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;

  m_clockwork->root = root;

  for (int i=0;i<3;i++) {
    m_clockwork->fcalSwitch[i] = new SoSwitch();
    root->addChild(m_clockwork->fcalSwitch[i]);
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

  createEtaPhi();

}

void VP1CaloLegoSystem::systemerase()
{
  if (m_clockwork->dummyCubeSep) {
    if (m_clockwork->root->findChild(m_clockwork->dummyCubeSep)>=0)
      m_clockwork->root->removeChild(m_clockwork->dummyCubeSep);
    m_clockwork->dummyCubeSep->unref();
    m_clockwork->dummyCubeSep = 0;
  }

}

void VP1CaloLegoSystem::systemuncreate()
{
  if (!VP1JobConfigInfo::hasLArGeometry()||!VP1JobConfigInfo::hasTileGeometry())
    return;

  m_clockwork->root->enableNotify(false);
  for (int i=0;i<4;i++) {
    if (i<3) {
      m_clockwork->fcalSwitch[i]->enableNotify(false);
      m_clockwork->fcalSwitch[i]->removeAllChildren();
    }
    m_clockwork->embSwitch[i]->enableNotify(false);
    m_clockwork->emecSwitch[i]->enableNotify(false);
    m_clockwork->hecSwitch[i]->enableNotify(false);
    m_clockwork->embSwitch[i]->removeAllChildren();
    m_clockwork->emecSwitch[i]->removeAllChildren();
    m_clockwork->hecSwitch[i]->removeAllChildren();
  }

  m_clockwork->TileMap.clear();
  m_clockwork->HECMap.clear();
  m_clockwork->EMECMap.clear();
  m_clockwork->EMBMap.clear();
  m_clockwork->root->removeAllChildren();
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
      m_clockwork->fcalSwitch[i]->addChild(drawStyle);
      m_clockwork->fcalSwitch[i]->addChild(lm);
      m_clockwork->fcalSwitch[i]->addChild(green);
    }
    m_clockwork->embSwitch[i]->addChild(drawStyle);
    m_clockwork->emecSwitch[i]->addChild(drawStyle);
    m_clockwork->hecSwitch[i]->addChild(drawStyle);
    m_clockwork->embSwitch[i]->addChild(lm);
    m_clockwork->emecSwitch[i]->addChild(lm);
    m_clockwork->hecSwitch[i]->addChild(lm);
    m_clockwork->embSwitch[i]->addChild(blue);
    m_clockwork->emecSwitch[i]->addChild(magenta);
    m_clockwork->hecSwitch[i]->addChild(yellow);
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

	  m_clockwork->TileMap[ls]=&(*t);
	}
	int sp = fcalMod->getModuleIndex()-1;
	m_clockwork->fcalSwitch[sp]->addChild(sep1);

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

	      m_clockwork->HECMap[ls]=cellPtr;
	    }
	  }
	}
	m_clockwork->hecSwitch[region->getSamplingIndex()]->addChild(sep1);
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
	    m_clockwork->EMECMap[ls]=cellPtr;
	  }
	}
	m_clockwork->emecSwitch[region->getSamplingIndex()]->addChild(sep1);
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
	    m_clockwork->EMBMap[ls]=cellPtr;
	  }
	}
	m_clockwork->embSwitch[region->getSamplingIndex()]->addChild(sep1);
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


void VP1CaloLegoSystem::userPickedNode(SoNode* /*pickedNode*/, SoPath */*pickedPath*/)
{
}

QByteArray VP1CaloLegoSystem::saveState()
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
