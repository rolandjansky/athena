/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackSysCommonData                //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackSysCommonData.h"

#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1TrackSystems/AscObjSelectionManager.h"
#include "VP1TrackSystems/TrackPropagationHelper.h"
#include "VP1TrackSystems/TrackLODManager.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1TrackSystems/VP1TrackSanity.h"
#include "VP1TrackSystems/TrackHandle_TrkTrack.h"
#include "VP1TrackSystems/TrackHandleBase.h"

#include "VP1PRDSystems/MuonChamberProjectionHelper.h"
#include "VP1PRDSystems/TouchedMuonChamberHelper.h"

#include "VP1GuideLineSystems/InDetProjHelper.h"

#include "VP1Base/VisibleObjectToMaterialHelper.h"
#include "VP1Base/VP1Msg.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>

#include <QTreeWidgetItem>

#include<map>

//____________________________________________________________________
class TrackSysCommonData::Imp {
public:
  std::map<SoNode*,TrackHandleBase*> nodeToTrackHandle;
};

//____________________________________________________________________
TrackSysCommonData::TrackSysCommonData(VP1TrackSystem * sys,TrackSystemController * controller)
  : VP1HelperClassBase(sys,"TrackSysCommonData"), m_textSep(nullptr), m_d(new Imp),
    m_ascObjSelectionManager(nullptr), m_controller(controller), m_lastSelectedTrack(nullptr)
{
  m_3dsystem = sys;
  m_muonChamberProjectionHelper = new MuonChamberProjectionHelper(sys);
  m_indetProjHelper_Pixel = InDetProjHelper::createPixelHelper(sys);
  m_indetProjHelper_SCT = InDetProjHelper::createSCTHelper(sys);
  m_indetProjHelper_TRT = InDetProjHelper::createTRTHelper(sys);
  m_touchedMuonChamberHelper = new TouchedMuonChamberHelper;
  m_trackPropagationHelper = new TrackPropagationHelper(sys);
  m_trackSanity = new VP1TrackSanity(sys);

  m_singlePoint = new SoPointSet;
  m_singlePoint->ref();
  SoVertexProperty * vertices = new SoVertexProperty;
  vertices->vertex.set1Value(0,0.0f,0.0f,0.0f);
  m_singlePoint->numPoints=1;
  m_singlePoint->vertexProperty.setValue(vertices);

  m_trackLODManager = new TrackLODManager(nullptr,sys);
  QObject::connect( m_controller,SIGNAL(assocObjDetailLevelChanged(TrackCommonFlags::DETAILLEVEL)),
		    m_trackLODManager,SLOT(setDetailLevel(TrackCommonFlags::DETAILLEVEL)) );
  m_trackLODManager->setDetailLevel(m_controller->assocObjDetailLevel());

  QObject::connect(m_touchedMuonChamberHelper,SIGNAL(touchedMuonChambersChanged(const std::set<GeoPVConstLink>&)),
		   sys,SLOT(emitTouchedMuonChambersChanged(const std::set<GeoPVConstLink>&)));//Fixme: need track sys!!

  m_visTrkTracksToMaterialHelper = new VisibleObjectToMaterialHelper<Trk::Track>(nullptr,sys);
  m_visTrkSegmentsToMaterialHelper = new VisibleObjectToMaterialHelper<Trk::Segment>(nullptr,sys);
  QObject::connect(m_visTrkTracksToMaterialHelper,SIGNAL(visibleObjectsChanged()),sys,SLOT(visibleObjectsChanged()));
  QObject::connect(m_visTrkSegmentsToMaterialHelper,SIGNAL(visibleObjectsChanged()),sys,SLOT(visibleObjectsChanged()));
}

//____________________________________________________________________
TrackSysCommonData::~TrackSysCommonData()
{
  delete m_muonChamberProjectionHelper;
  delete m_indetProjHelper_Pixel;
  delete m_indetProjHelper_SCT;
  delete m_indetProjHelper_TRT;
  delete m_touchedMuonChamberHelper;
  delete m_trackPropagationHelper;
  delete m_trackLODManager;
  delete m_visTrkTracksToMaterialHelper;
  delete m_visTrkSegmentsToMaterialHelper;
  delete m_trackSanity;
  m_singlePoint->unref();
  if (m_textSep) {
    m_textSep->unref();
    m_textSep = nullptr;
  }
  delete m_d;
}

//____________________________________________________________________
void TrackSysCommonData::setEventData(AscObjSelectionManager* aomgr)
{
  m_ascObjSelectionManager = aomgr;
}

//____________________________________________________________________
void TrackSysCommonData::clearEventData()
{
  m_ascObjSelectionManager->aboutTodelete();//To allow to emit signals.
  delete m_ascObjSelectionManager;
  m_ascObjSelectionManager = nullptr;
  m_touchedMuonChamberHelper->eraseEventData();
  m_trackLODManager->eraseEventData();
  m_visTrkTracksToMaterialHelper->setNoVisibleObjects();
  m_visTrkSegmentsToMaterialHelper->setNoVisibleObjects();
  //  m_d->nodeToTrackHandle.clear(); TrackHandle destructor already ensures that unregister... below is called.
}

//____________________________________________________________________
void TrackSysCommonData::registerTrack(SoNode*node,TrackHandleBase*handle)
{
  if (VP1Msg::verbose()) {
    if (!node||!handle) {
      message("registerTrack ERROR: Received null pointer!");
      return;
    }
    std::map<SoNode*,TrackHandleBase*>::iterator it = m_d->nodeToTrackHandle.find(node);
    if (it!=m_d->nodeToTrackHandle.end())
      message("registerTrack ERROR: Node already registered!");
  }
  m_d->nodeToTrackHandle[node]=handle;
}

//____________________________________________________________________
void TrackSysCommonData::unregisterTrack(SoNode*node)
{
  if (VP1Msg::verbose()) {
    if (!node) {
      message("unregisterTrack ERROR: Received null pointer!");
      return;
    }
  }
  std::map<SoNode*,TrackHandleBase*>::iterator it = m_d->nodeToTrackHandle.find(node);
  if (it==m_d->nodeToTrackHandle.end()) {
    message("unregisterTrack ERROR: Not previously registered!");
    return;
  }
  m_d->nodeToTrackHandle.erase(it);
}

//____________________________________________________________________
TrackHandleBase* TrackSysCommonData::trackHandle(SoNode*n)
{
  if (!n)
    return nullptr;
  std::map<SoNode*,TrackHandleBase*>::iterator it = m_d->nodeToTrackHandle.find(n);
  if (it!=m_d->nodeToTrackHandle.end())
    return it->second;
  return nullptr;
}

//____________________________________________________________________
SoNode* TrackSysCommonData::node(TrackHandleBase* h)
{
  if (!h)
    return nullptr;
  std::map<SoNode*,TrackHandleBase*>::iterator it = m_d->nodeToTrackHandle.begin(), itEnd=m_d->nodeToTrackHandle.end();
  for (; it!=itEnd;++it)
    if (it->second==h) return it->first;
  return nullptr;
}

//____________________________________________________________________
SoNode* TrackSysCommonData::node(QTreeWidgetItem* item)
{
  if (!item)
    return nullptr;
  std::map<SoNode*,TrackHandleBase*>::iterator it = m_d->nodeToTrackHandle.begin(), itEnd=m_d->nodeToTrackHandle.end();
  for (; it!=itEnd;++it)
    if (it->second->browserTreeItem()==item) return it->first;
  return nullptr;
}

const TrackHandleBase* TrackSysCommonData::getHandle(const Trk::Track* trk)
{
  if (!trk) {
    messageVerbose("TrackSysCommonData::getHandle(): Received null pointer!");
    return nullptr;
  }
//  messageVerbose("TrackSysCommonData::getHandle(): about to loop over this many elements:"+QString::number(m_d->nodeToTrackHandle.size()));

  std::map<SoNode*,TrackHandleBase*>::iterator it = m_d->nodeToTrackHandle.begin(), itEnd=m_d->nodeToTrackHandle.end();
  for (; it!=itEnd;++it){
    const TrackHandle_TrkTrack* trkHandle = dynamic_cast<const TrackHandle_TrkTrack*>(it->second);
    if (trkHandle && trkHandle->trkTrackPointer()==trk) return trkHandle;
  }
  return nullptr;  
}
