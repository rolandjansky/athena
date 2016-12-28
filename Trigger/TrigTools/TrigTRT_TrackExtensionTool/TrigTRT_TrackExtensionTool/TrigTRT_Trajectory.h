/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTRTTRACKEXTENSIONTOOL_TRIGTRT_TRAJECTORY_H
#define TRIGTRTTRACKEXTENSIONTOOL_TRIGTRT_TRAJECTORY_H

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
  void addSurface(Trk::TrkPlanarSurface*);
  void addTrackState(Trk::TrkTrackState*);
  void addFilteringNode(Trk::TrkBaseNode*);
  void addTRT_SummaryInfo(TrigTRT_Info*);
  TrigTRT_Info* getTRT_SummaryInfo();
  void smoothTrajectory();
  std::vector<Trk::TrkTrackState*>* getTrackStates();
  std::vector<Trk::TrkBaseNode*>* getFilteringNodes();
  void setStartingTrackState(Trk::TrkTrackState*);
  Trk::TrkTrackState* getStartingTrackState();
  int getNumberOfTRT_Hits();
  int getNumberOfHighThresholdTRT_Hits();
  int getNumberOfCrossedTRT_Straws();
  int getNumberOfDriftTimeTRT_Hits();
  int getNumberOfMissedDetection();
  void setStatus(bool);
  void addRoad(TrigTRT_DetElementRoad*);
  TrigTRT_DetElementRoad* getRoad();
  TrigInDetTrack* getTrigInDetTrack();
  bool isValid();
  bool isFast(); 
  void setFast(bool);
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
