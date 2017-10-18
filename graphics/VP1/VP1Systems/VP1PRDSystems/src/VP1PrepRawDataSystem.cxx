/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Fixme: cleanup includes.
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1PRDSystems/VP1PrepRawDataSystem.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "VP1PRDSystems/PRDSysCommonData.h"
#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/PRDTrackSegmentHelper.h"
#include "VP1PRDSystems/PRDCollHandle_CSC.h"
#include "VP1PRDSystems/PRDCollHandle_CSC_Strip.h"
#include "VP1PRDSystems/PRDCollHandle_MDT.h"
#include "VP1PRDSystems/PRDCollHandle_MM.h"
#include "VP1PRDSystems/PRDCollHandle_Pixel.h"
#include "VP1PRDSystems/PRDCollHandle_RPC.h"
#include "VP1PRDSystems/PRDCollHandle_SCT.h"
#include "VP1PRDSystems/PRDCollHandle_TGC.h"
#include "VP1PRDSystems/PRDCollHandle_sTGC.h"
#include "VP1PRDSystems/PRDCollHandle_TRT.h"
#include "VP1PRDSystems/PRDCollHandle_SpacePoints.h"
#include "VP1PRDSystems/TouchedMuonChamberHelper.h"

#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1CollectionWidget.h"
#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1CameraHelper.h"

#include <QBuffer>
#include <QByteArray>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/SoPath.h>

#include <map>

//Fixme: Update visuals if any detail level changes!

//____________________________________________________________________
class VP1PrepRawDataSystem::Imp {
public:
  VP1PrepRawDataSystem * theclass;
  PRDSysCommonData * common;
  PRDSystemController * controller;
  SoCooperativeSelection * selNode_click;
  SoCooperativeSelection * selNode_highlight;
  
  InDetProjFlags::InDetProjPartsFlags idprojflags_pixel;
  InDetProjFlags::InDetProjPartsFlags idprojflags_sct;
  InDetProjFlags::InDetProjPartsFlags idprojflags_trt;
  int appropriatemdtprojection;
  
  SoSeparator * multisel_sep;
  void clearMultiSelLine() {
    while (multisel_sep&&multisel_sep->getNumChildren()>2)
      multisel_sep->removeChild(2);
  }
  
  QList<PRDHandleBase*> currentlySelectedHandles;
  QList<const Trk::PrepRawData*> lastEmittedPRDList;
  void selectionChanged() {
    currentlySelectedHandles.removeAll(0);//just to be sure
    clearMultiSelLine();
    
    //Proper highlighting:
    if (selNode_highlight) {
      selNode_highlight->deselectAll();
      foreach(PRDHandleBase*handle,currentlySelectedHandles) {
        // 	theclass->messageDebug("Highlighting handle...");
        SoSeparator* target = handle->collHandle()->simpleDetailLevel() ? handle->sepSimple() : handle->sepDetailed();
        SoPath * path = new SoPath(selNode_highlight);
        path->ref();
        // 	theclass->messageDebug("Attempting to highlight sep="+str(target));
        if (!VP1QtInventorUtils::changePathTail(path,selNode_highlight,target)) {
          theclass->message("ERROR: Failed to relocate picked node.");
          path->unref();
          continue;
        }
        selNode_highlight->select(path);
        path->unref();
      }
      selNode_highlight->touch();
      if (multisel_sep&&currentlySelectedHandles.count()>1&&controller->showSelectionLine()) {
        SoLineSet * line = new SoLineSet;
        SoVertexProperty * vertices = new SoVertexProperty;
        line->vertexProperty = vertices;
        line->numVertices.set1Value(0,currentlySelectedHandles.count());
        int i(0);
        foreach(PRDHandleBase*handle,currentlySelectedHandles) {
          Amg::Vector3D pos(handle->center());
          vertices->vertex.set1Value(i++,pos.x(),pos.y(),pos.z());
        }
        multisel_sep->addChild(line);
        //Fixme: make this unpickable!!
        
      }
    }
    
    //emit signal if appropriate:
    QList<const Trk::PrepRawData*> currentPRDs;
    for(int i = 0; i < currentlySelectedHandles.count(); ++i) {
      PRDHandleBase*handle = currentlySelectedHandles.at(i);
      const Trk::PrepRawData* prd1 = handle->getPRD();
      const Trk::PrepRawData* prd2 = handle->getSecondPRD();
      if (prd1&&currentPRDs.contains(prd1)) prd1 = 0;
      if (prd2&&currentPRDs.contains(prd2)) prd2 = 0;
      if (!prd2) {
        if (!prd1)
          continue;
        currentPRDs << prd1;
      } else {
        //both prd1 and prd2 are non-zero and not in the list
        //already. Which should be first? We order them by distance to
        //the preceding or following prd.
        
        Amg::Vector3D p1(handle->positionPRD());
        Amg::Vector3D p2(handle->positionSecondPRD());
        
        if (i>0) {
          Amg::Vector3D prevpos = currentlySelectedHandles.at(i-1)->center();
          if ((p1-prevpos).mag2()>(p2-prevpos).mag2()) {
            //prd2 before prd1
            currentPRDs << prd2;
            currentPRDs << prd1;
          } else {
            //prd1 before prd2
            currentPRDs << prd1;
            currentPRDs << prd2;
          }
        } else {
          if (i+1<currentlySelectedHandles.count()) {
            Amg::Vector3D nextpos = currentlySelectedHandles.at(i+1)->center();
            if ((p1-nextpos).mag2()>(p2-nextpos).mag2()) {
              //prd1 before prd2
              currentPRDs << prd1;
              currentPRDs << prd2;
            } else {
              //prd2 before prd1
              currentPRDs << prd2;
              currentPRDs << prd1;
            }
          } else {
            //Only this one. Just add them:
            currentPRDs << prd1;
            currentPRDs << prd2;
          }
        }
      }
    }
    if (currentPRDs!=lastEmittedPRDList) {
      lastEmittedPRDList = currentPRDs;
      theclass->messageVerbose("Emitting list of "+str(lastEmittedPRDList.count())+" selected PRDs");
      emit theclass->selectedPRDsChanged(lastEmittedPRDList);
    }
  }
  
  template <class T>
  QList<PRDCollHandleBase*> createSpecificCollections() {
    QList<PRDCollHandleBase*> l;
    foreach (QString name, T::availableCollections(theclass)) {
      T * col = new T(common,name);
      col->init();
      l << col;
    }
    return l;
  }
  QList<PRDCollHandleBase*> createCollections() {
    QList<PRDCollHandleBase*> l;
    l << createSpecificCollections<PRDCollHandle_Pixel>();
    l << createSpecificCollections<PRDCollHandle_SCT>();
    l << createSpecificCollections<PRDCollHandle_TRT>();
    l << createSpecificCollections<PRDCollHandle_SpacePoints>();
    l << createSpecificCollections<PRDCollHandle_CSC>();
    l << createSpecificCollections<PRDCollHandle_CSC_Strip>();
    l << createSpecificCollections<PRDCollHandle_MDT>();
    l << createSpecificCollections<PRDCollHandle_MM>();
    l << createSpecificCollections<PRDCollHandle_RPC>();
    l << createSpecificCollections<PRDCollHandle_TGC>();
    l << createSpecificCollections<PRDCollHandle_sTGC>();
    return l;
  }
  
  void ensureInitCommonData() {
    if (!common) {
      theclass->ensureBuildController();
      common = new PRDSysCommonData(theclass,controller);
    }
  }
};

//_____________________________________________________________________________________
VP1PrepRawDataSystem::VP1PrepRawDataSystem()
: IVP13DSystemSimple("Hits",
                     "System showing tracking hits (PRD's)",
                     "Edward.Moyse@cern.ch, Thomas.Kittelmann@cern.ch"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->common = 0;
  m_d->controller = 0;
  m_d->idprojflags_pixel = InDetProjFlags::NoProjections;
  m_d->idprojflags_sct = InDetProjFlags::NoProjections;
  m_d->idprojflags_trt = InDetProjFlags::NoProjections;
  m_d->appropriatemdtprojection = 0;
  m_d->selNode_click = 0;
  m_d->selNode_highlight = 0;
  m_d->multisel_sep = 0;
  
}

//_____________________________________________________________________________________
VP1PrepRawDataSystem::~VP1PrepRawDataSystem()
{
  delete m_d;
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::systemcreate(StoreGateSvc* /*detstore*/)
{
  messageVerbose("systemcreate");
  if (VP1JobConfigInfo::hasMuonGeometry()&&!VP1DetInfo::muonDetMgr())
    message("Error: Can't retrieve MuonDetectorManager. Expect reduced performance and functionality.");//Fixme: Only if any muon collections are present!
  m_d->ensureInitCommonData();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::systemuncreate()
{
  if (m_d->multisel_sep) {
    m_d->multisel_sep->unref();
    m_d->multisel_sep=0;
  }
  
  delete m_d->common; m_d->common = 0;
  m_d->controller = 0;
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::systemerase()
{
  messageVerbose("systemerase");
  
  deselectAll();
  
  m_d->common->controller()->collWidget()->clear();
  
  m_d->common->clearEventData();
  
  if (m_d->selNode_click) {
    unregisterSelectionNode(m_d->selNode_click);
    unregisterSelectionNode(m_d->selNode_highlight);
    m_d->selNode_click->unref();
    m_d->selNode_click=0;
    m_d->selNode_highlight->unref();
    m_d->selNode_highlight=0;
  }
  
  m_d->clearMultiSelLine();
  
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::buildPermanentSceneGraph(StoreGateSvc* /*detstore*/, SoSeparator * /*root*/)
{
  messageVerbose("buildPermanentSceneGraph (does not do anything)");
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::buildEventSceneGraph(StoreGateSvc*, SoSeparator *root)
{
  messageVerbose("buildEventSceneGraph");
  
  // set complexity to a lower value, so that e.g. the straws are manageable
  SoComplexity * complexity = new SoComplexity;
  complexity->value.setValue(0.3f);//Fixme: Hardcoded here and elsewhere
  root->addChild(complexity);
  
  //   //Point sizes and line widths:
  //   root->addChild(m_d->controller->prdDrawStyle());
  
  //Create collection list based on contents of event store, inform
  //about projections, populate gui and apply states:
  QList<PRDCollHandleBase*> cols = m_d->createCollections();
  
  //Inform about appropriate projects:
  foreach (PRDCollHandleBase* col,cols) {
    
    connect(col,SIGNAL(detailLevelChanged()),this,SLOT(updateSelectionVisualsAndPossiblyEmitPRDList()));
    
    PRDCollHandle_MDT* mdtcol = dynamic_cast<PRDCollHandle_MDT*>(col);
    if (mdtcol) {
      mdtcol->setAppropriateProjection( m_d->appropriatemdtprojection );
      continue;
    }
    
    PRDCollHandle_TRT* trtcol = dynamic_cast<PRDCollHandle_TRT*>(col);
    if (trtcol) {
      trtcol->setAppropriateProjection(m_d->idprojflags_trt);//NB: Add for pixel/sct as well once supported!
      continue;
    }
    
  }
  
  m_d->controller->collWidget()->setCollections(cols);
  
  
  //Add collections to event scenegraph:
  m_d->selNode_click = new SoCooperativeSelection;
  m_d->selNode_click->ref();
  m_d->selNode_highlight = new SoCooperativeSelection;
  m_d->selNode_highlight->ref();
  m_d->selNode_click->activePolicy = SoCooperativeSelection::ACTIVE;
  m_d->selNode_highlight->activePolicy = SoCooperativeSelection::INERT;
  m_d->selNode_click->policy = SoCooperativeSelection::SINGLE;
  m_d->selNode_highlight->policy = SoCooperativeSelection::SINGLE;
  
  foreach (VP1StdCollection* col,m_d->controller->collWidget()->collections<VP1StdCollection>())
  m_d->selNode_click->addChild(col->collSwitch());
  m_d->selNode_highlight->addChild(m_d->selNode_click);
  root->addChild(m_d->selNode_highlight);
  
  registerSelectionNode(m_d->selNode_click);
  registerSelectionNode(m_d->selNode_highlight);
  
  if (!m_d->multisel_sep) {
    m_d->multisel_sep = new SoSeparator;
    m_d->multisel_sep->ref();
    SoPickStyle * ps = new SoPickStyle;
    ps->style= SoPickStyle::UNPICKABLE;
    m_d->multisel_sep->addChild(ps);
    m_d->multisel_sep->addChild(m_d->controller->getMultiSelectionLineMaterial());
  }
  root->addChild(m_d->multisel_sep);
  
  
  messageVerbose("buildEventSceneGraph done");
  
}


//_____________________________________________________________________________________
void VP1PrepRawDataSystem::deselectAll(SoCooperativeSelection* exception_sel)
{
  if (exception_sel)
    message("WARNING: The PRD system always deselects all registered nodes/");
  IVP13DSystemSimple::deselectAll(0);
  m_d->currentlySelectedHandles.clear();
  m_d->selectionChanged();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::selectionVisualsChanged()
{
  m_d->selectionChanged();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::clearSelection()
{
  messageVerbose("clearSelection");
  deselectAll();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::selectionModeChanged()
{
  messageVerbose("selectionModeChanged");
  deselectAll();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::updateSelectionVisualsAndPossiblyEmitPRDList()
{
  messageVerbose("updateSelectionVisualsAndPossiblyEmitPRDList");
  m_d->selectionChanged();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::userPickedNode(SoNode*, SoPath *)
{
  messageVerbose("userPickedNode");
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::userSelectedSingleNode(SoCooperativeSelection* sel, SoNode*, SoPath*pickedPath)
{
  if (m_d->selNode_highlight==sel)
    return;
  messageVerbose("userSelectedSingleNode");
  PRDHandleBase * handle = m_d->common->pickedPathToHandle(pickedPath);//This also pops the path, so that all shape nodes
  //making up the whole shape will be highlighted
  sel->deselectAll();
  
  if (!handle) {
    deselectAll();
    message("Error: Could not identify picked node");
    return;
  }
  
  if (!m_d->controller->selectionModeMultiple()) {
    if (m_d->currentlySelectedHandles.count()==1&&*(m_d->currentlySelectedHandles.begin())==handle)
      return;
    m_d->currentlySelectedHandles.clear();
    m_d->currentlySelectedHandles<<handle;
    if (handle->inMuonChamber()&&m_d->controller->muonOrientToChambersOnClick()) {
      messageVerbose("emits prdInMuonChamberSelected");
      emit prdInMuonChamberSelected(handle->parentMuonChamberPV());
    } else if (m_d->controller->zoomOnClick()) {
      std::set<SoCamera*> cameras(getCameraList());
      std::set<SoCamera*>::iterator it,itE = cameras.end();
      for (it=cameras.begin();it!=itE;++it)
        VP1CameraHelper::animatedZoomToPath(*it,handle->collHandle()->collSep(),pickedPath,2.0,1.0);
    }
    if (m_d->controller->printInfoOnClick()) {
      foreach (QString line, handle->clicked())
      message(line);
    }
  } else {
    if (m_d->currentlySelectedHandles.contains(handle)) {
      m_d->currentlySelectedHandles.removeAll(handle);
    } else {
      m_d->currentlySelectedHandles << handle;
    }
  }
  
  m_d->selectionChanged();
  
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::userDeselectedSingleNode(SoCooperativeSelection* sel, SoNode*, SoPath*)
{
  if (m_d->selNode_highlight==sel)
    return;
  //  messageVerbose("userDeselectedSingleNode");
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::userChangedSelection(SoCooperativeSelection*sel , QSet<SoNode*>, QSet<SoPath*>)
{
  if (m_d->selNode_highlight==sel)
    return;
  messageVerbose("userChangedSelection");
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::userClickedOnBgd()
{
  messageVerbose("userClickedOnBgd");
  if (!m_d->controller->selectionModeMultiple()) {
    m_d->currentlySelectedHandles.clear();
    m_d->selectionChanged();
  }
}


//_____________________________________________________________________________________
QWidget * VP1PrepRawDataSystem::buildController()
{
  messageVerbose("buildController");
  m_d->controller = new PRDSystemController(this);
  
  messageVerbose("Passing ID projection settings on to collWidget");
  
  connect(m_d->controller,SIGNAL(selectionModeMultipleChanged(bool)),this,SLOT(selectionModeChanged()));
  connect(m_d->controller,SIGNAL(showSelectionLineChanged(bool)),this,SLOT(selectionVisualsChanged()));
  connect(m_d->controller,SIGNAL(clearSelection()),this,SLOT(clearSelection()));
  
  connect(m_d->controller,SIGNAL(inDetPartsUsingProjectionsChanged(InDetProjFlags::DetTypeFlags)),
          this,SLOT(emitUsedIDProjectionsChanged(InDetProjFlags::DetTypeFlags)));
  InDetProjFlags::DetTypeFlags f = m_d->controller->inDetPartsUsingProjections();
  if ( f != InDetProjFlags::NoDet)
    emitUsedIDProjectionsChanged(f);//Fixme: Check that this is actually sufficiently late for the guideline sys!!!
  
  return m_d->controller;
}

//_____________________________________________________________________________________
QByteArray VP1PrepRawDataSystem::saveState() {
  
  VP1Serialise serialise(0/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());
  
  ensureBuildController();
  
  serialise.save(m_d->controller->saveSettings());
  serialise.save(m_d->controller->collWidget());
  
  serialise.disableUnsavedChecks();//We do the testing in the controller
  return serialise.result();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::restoreFromState(QByteArray ba) {
  //Version & base state:
  VP1Deserialise state(ba,this);
  if (state.version()!=0) {
    message("Warning: State data in .vp1 file is in wrong format - ignoring!");
    return;
  }
  ensureBuildController();
  
  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());
  
  m_d->controller->restoreSettings(state.restoreByteArray());
  state.restore(m_d->controller->collWidget());//We do the testing in the controller
  
  state.disableUnrestoredChecks();//We do the testing in the controller
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::visibleTracksChanged(const std::vector< std::pair<const Trk::Track*, const SoMaterial*> >& tracks)
{
  m_d->ensureInitCommonData();
  if (!m_d->common)
    return;//for signals received after uncreate
  m_d->common->trackAndSegmentHelper()->visibleTracksChanged(tracks);
  selectionVisualsChanged();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::visibleSegmentsChanged(const std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >& segments)
{
  m_d->ensureInitCommonData();
  if (!m_d->common)
    return;//for signals received after uncreate
  m_d->common->trackAndSegmentHelper()->visibleSegmentsChanged(segments);
  selectionVisualsChanged();
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::emitTouchedMuonChambersChanged(const std::set<GeoPVConstLink>& s)
{
  messageVerbose("emits touchedMuonChambersChanged (ntouchedchambers="+QString::number(s.size())+")");
  emit touchedMuonChambersChanged(s);
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::emitUsedIDProjectionsChanged(InDetProjFlags::DetTypeFlags f)
{
  messageVerbose("emits usedIDProjectionsChanged");
  emit usedIDProjectionsChanged(f);
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::appropriateMDTProjectionsChanged(int iproj)
{
  if (m_d->appropriatemdtprojection==iproj)
    return;
  m_d->appropriatemdtprojection = iproj;
  if (!m_d->controller)
    return;//applied upon creation of collections instead
  foreach (PRDCollHandle_MDT* mdtcol,m_d->controller->collWidget()->collections<PRDCollHandle_MDT>())
  mdtcol->setAppropriateProjection( m_d->appropriatemdtprojection );
  
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::setApplicableIDProjections( InDetProjFlags::InDetProjPartsFlags pixel,
                                                      InDetProjFlags::InDetProjPartsFlags sct,
                                                      InDetProjFlags::InDetProjPartsFlags trt )
{
  messageVerbose("Signal received in setApplicableProjections (from "
                 +QString(sender()?sender()->objectName():"NULL sender")+")");
  if (m_d->idprojflags_pixel==pixel&&m_d->idprojflags_sct==sct&&m_d->idprojflags_trt==trt) {
    return;
  }
  
  m_d->idprojflags_pixel = pixel;
  m_d->idprojflags_trt = trt;
  m_d->idprojflags_sct = sct;
  
  if (!m_d->controller)
    return;//applied upon creation of collections instead
  
  foreach (PRDCollHandle_TRT* trtcol,m_d->controller->collWidget()->collections<PRDCollHandle_TRT>())
  trtcol->setAppropriateProjection(m_d->idprojflags_trt);
  
  //NB: Add for pixel/sct as well once supported!
  
}

//_____________________________________________________________________________________
void VP1PrepRawDataSystem::muonChambersWithTracksChanged(const std::set<GeoPVConstLink>& chambers)
{
  messageVerbose("muonChambersWithTracksChanged : received "+str(chambers.size())+" chambers.");
  
  // //Update map of touched chamber lists:
  // bool listChanged(m_d->sender2ChamberList.find(sender())!=m_d->sender2ChamberList.end()?
  //                  (chambers != m_d->sender2ChamberList[sender()]):true);
  // m_d->sender2ChamberList[sender()] = chambers;
  
  //Trigger update if list changed and in auto mode:
  // if ( listChanged && m_d->controller->limitToActiveChambers() )
  //   message("Limittoactivechambers - not yet implemented");
  m_d->ensureInitCommonData();
  if (m_d->common && m_d->common->touchedMuonChamberHelper())
    m_d->common->touchedMuonChamberHelper()->updateTouchedByTracks(chambers);
  else 
    message("muonChambersWithTracksChanged - helpers not yet created!");
}
