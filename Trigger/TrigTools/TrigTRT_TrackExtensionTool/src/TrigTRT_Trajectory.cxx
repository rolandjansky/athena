/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_TrackExtensionGeometry.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoad.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_Trajectory.h"

TrigTRT_Trajectory::TrigTRT_Trajectory(TrigInDetTrack* pT)
{
  m_vpTrkSurfaces.clear();
  m_vpTrackStates.clear();
  m_vpNodes.clear();
  m_status=true;
  m_pTrack=pT;
  m_pRoad=NULL;
  m_pTI=NULL;
  m_startingTS=NULL;
  m_highPt=true;
}

TrigTRT_Trajectory::~TrigTRT_Trajectory()
{
  for(std::vector<Trk::TrkTrackState*>::iterator ptIt=m_vpTrackStates.begin();
      ptIt!=m_vpTrackStates.end();++ptIt)
    {
      delete (*ptIt);
    }
  m_vpTrackStates.clear();
  for(std::vector<Trk::TrkPlanarSurface*>::iterator psIt=m_vpTrkSurfaces.begin();
      psIt!=m_vpTrkSurfaces.end();++psIt)
    {
      delete (*psIt);
    }
  m_vpTrkSurfaces.clear(); 
  for(std::vector<Trk::TrkBaseNode*>::iterator pnIt=m_vpNodes.begin();
      pnIt!=m_vpNodes.end();++pnIt)
  {
    delete (*pnIt);
  }
  m_vpNodes.clear();
  if(m_startingTS!=NULL) 
    {
      delete m_startingTS;
    }
  if(m_pRoad!=NULL) delete m_pRoad;
  if(m_pTI!=NULL) delete m_pTI; 
}

void TrigTRT_Trajectory::m_setStartingTrackState(Trk::TrkTrackState* pTS)
{
  m_startingTS=pTS;
}

Trk::TrkTrackState* TrigTRT_Trajectory::m_getStartingTrackState()
{
  return m_startingTS;
}

void TrigTRT_Trajectory::m_addSurface(Trk::TrkPlanarSurface* pS)
{
  m_vpTrkSurfaces.push_back(pS);
}

void TrigTRT_Trajectory::m_addTrackState(Trk::TrkTrackState* pTS)
{
  m_vpTrackStates.push_back(pTS);
}

void TrigTRT_Trajectory::m_addFilteringNode(Trk::TrkBaseNode* pN)
{
  m_vpNodes.push_back(pN);
}

void TrigTRT_Trajectory::m_addTRT_SummaryInfo(TrigTRT_Info* pTI)
{
  m_pTI=pTI;
}

TrigTRT_Info* TrigTRT_Trajectory::m_getTRT_SummaryInfo()
{
  return m_pTI;
}

void TrigTRT_Trajectory::m_smoothTrajectory()
{
  std::vector<Trk::TrkTrackState*>::reverse_iterator ptsIt(m_vpTrackStates.rbegin()),
    ptsEnd(m_vpTrackStates.rend());
  for(;ptsIt!=ptsEnd;++ptsIt)
    {
      // printf("smoothing...");(*ptsIt)->m_report();
      (*ptsIt)->m_runSmoother();
      // printf("done\n");
    }
}

std::vector<Trk::TrkTrackState*>* TrigTRT_Trajectory::m_getTrackStates()
{
  return (&m_vpTrackStates);
}

std::vector<Trk::TrkBaseNode*>* TrigTRT_Trajectory::m_getFilteringNodes()
{
  return (&m_vpNodes);
}

int TrigTRT_Trajectory::m_getNumberOfTRT_Hits()
{
  if(m_pTI==NULL) return 0;
  else
    {
      return (int)(m_pTI->m_getTRT_DriftTime_Sum()+m_pTI->m_getTRT_NoDriftTime_Sum()+0.5);
    }
}

int TrigTRT_Trajectory::m_getNumberOfHighThresholdTRT_Hits()
{
  if(m_pTI==NULL) return 0;
  else
    {
      return (int)(m_pTI->m_getTR_DriftTime_Sum()+m_pTI->m_getTR_NoDriftTime_Sum()+0.5);
    }
}

int TrigTRT_Trajectory::m_getNumberOfCrossedTRT_Straws()
{
  if(m_pTI==NULL) return 0;
  else
    {
      return (int)(m_pTI->m_getCrossedStraws()+0.5);
    }
}

int TrigTRT_Trajectory::m_getNumberOfDriftTimeTRT_Hits()
{
  if(m_pTI==NULL) return 0;
  else
    {
      return (int)(m_pTI->m_getTRT_DriftTime_Sum()+0.5);
    }
}

int TrigTRT_Trajectory::m_getNumberOfMissedDetection()
{
  if(m_pTI==NULL) return 0;
  else
    {
      return (int)(m_pTI->m_getCrossedLayers()-m_pTI->m_getDetectionWeight()+0.5);
    }
}

void TrigTRT_Trajectory::m_setStatus(bool s)
{
  m_status=s;
}

bool TrigTRT_Trajectory::m_isValid()
{
  return m_status;
}

bool TrigTRT_Trajectory::m_isFast()
{
  return m_highPt;
}

void TrigTRT_Trajectory::m_setFast(bool f)
{
  m_highPt=f;
}

void TrigTRT_Trajectory::m_addRoad(TrigTRT_DetElementRoad* pR)
{
  m_pRoad=pR;
}

TrigTRT_DetElementRoad* TrigTRT_Trajectory::m_getRoad()
{
  return m_pRoad;
}

TrigInDetTrack* TrigTRT_Trajectory::m_getTrigInDetTrack()
{
  return m_pTrack;
}


