/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_TRT_TRAJECTORY_H__
#define __TRIG_TRT_TRAJECTORY_H__

namespace Trk {	
	class TrkBaseNode;
	class TrkTrackState;      
	class TrkPlanarSurface;
}
class TrigTRT_DetElementRoad;
class TrigTRT_Info;
class TrigInDetTrack;

class TrigTRT_Trajectory
{
 public:
  TrigTRT_Trajectory(TrigInDetTrack*);
  ~TrigTRT_Trajectory();
  void m_addSurface(Trk::TrkPlanarSurface*);
  void m_addTrackState(Trk::TrkTrackState*);
  void m_addFilteringNode(Trk::TrkBaseNode*);
  void m_addTRT_SummaryInfo(TrigTRT_Info*);
  TrigTRT_Info* m_getTRT_SummaryInfo();
  void m_smoothTrajectory();
  std::vector<Trk::TrkTrackState*>* m_getTrackStates();
  std::vector<Trk::TrkBaseNode*>* m_getFilteringNodes();
  void m_setStartingTrackState(Trk::TrkTrackState*);
  Trk::TrkTrackState* m_getStartingTrackState();
  int m_getNumberOfTRT_Hits();
  int m_getNumberOfHighThresholdTRT_Hits();
  int m_getNumberOfCrossedTRT_Straws();
  int m_getNumberOfDriftTimeTRT_Hits();
  int m_getNumberOfMissedDetection();
  void m_setStatus(bool);
  void m_addRoad(TrigTRT_DetElementRoad*);
  TrigTRT_DetElementRoad* m_getRoad();
  TrigInDetTrack* m_getTrigInDetTrack();
  bool m_isValid();
  bool m_isFast(); 
  void m_setFast(bool);
  std::vector<Trk::TrkPlanarSurface*> m_vpTrkSurfaces;
 private:
  std::vector<Trk::TrkTrackState*> m_vpTrackStates;
  std::vector<Trk::TrkBaseNode*> m_vpNodes;
  Trk::TrkTrackState* m_startingTS;
  bool m_status;
  bool m_highPt;
  TrigInDetTrack* m_pTrack;
  TrigTRT_DetElementRoad* m_pRoad;
  TrigTRT_Info* m_pTI;
};
#endif
