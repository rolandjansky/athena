/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVERTEXFITTER_TRIGPRIMARYVERTEXTRACK_H
#define TRIGVERTEXFITTER_TRIGPRIMARYVERTEXTRACK_H

class TrigL2Vertex;
class TrigInDetTrack;
namespace Trk {
  class Track;
}

class TrigPrimaryVertexTrack : public TrigVertexFittingNode
{
  public:
  TrigPrimaryVertexTrack(const TrigInDetTrack*);//!< constructor for L2 tracks 
  TrigPrimaryVertexTrack(const Trk::Track*);//!< constructor for EF (offline) tracks
  ~TrigPrimaryVertexTrack();
  const TrigInDetTrack* getTrigTrack();//!< getter for L2 tracks 
  const Trk::Track* getTrkTrack();//!< getter for EF (offline) tracks

  virtual double getChi2Distance(TrigL2Vertex*);//!< implementation of abstract method from the base class
  virtual void updateVertex(TrigL2Vertex*);//!< implementation of abstract method from the base class
  virtual MsgStream& report( MsgStream& ) const;
  void setIndex(int);//!< to be used by TrigVertexingTool
  int getIndex() const;//!< to be used by TrigVertexingTool
  int getTrackType();//!< 0: L2 track, 1: EF(offline) track
  bool isActive();//!< if true this track will be used in the vertex fit otherwise it will be masked
  void activate();//!< sets m_isActive to true
  void mask();//!< sets m_isActive to false
  const double* Perigee() const;//!< track parameters at the perigee
  double PerigeeCovariance(int,int) const;//!< covariance of track parameters at the perigee
  double getChi2Contribution();//!< chi2-contribution to the vertex fit
 private:
  const TrigInDetTrack* m_pTrigTrack;
  const Trk::Track* m_pTrkTrack;
  int m_nTrackType;
  int m_index;
  double m_Vqq[3][3];
  double m_Vuq[2][3];
  double m_Vuu[2][2];
  double m_u[2];
  double m_q[3];
  double m_Perigee[5];
  double m_PerigeeCovariance[5][5];
  double m_A[2][3];
  double m_B[2][3];
  double m_dChi2;
  bool m_active;
};


#endif // not TRIGVERTEXFITTER_TRIGPRIMARYVERTEXTRACK_H
