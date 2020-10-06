/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGL2VERTEX_H
#define TRIGINDETEVENT_TRIGL2VERTEX_H

#include <list>
#include <vector>
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrkTrack/Track.h"
#include "GaudiKernel/MsgStream.h"

#define MAX_SIZE_VERT_COVM  30

/** This is the implementation of classes used by TrigL2VertexFitter: 

 - TrigL2Vertex - an extended version of TrigVertex class which incapsulates vertex fit parameters 
   vector and associated covariance matrix. 

 - TrigVertexFittingNode: base class for 

 - TrigVertexFitInputTrack which incapsulates input track parameters and is responsible 
   for updating vertex fit parameters
 - TrigVertexFitConstraint which incapsulates mass constraint information

   The algorithm used in m_updateVertex method of the TrigVertexFitInputTrack class is described in 
   ATL-COM-DAQ-2007-036 note available at http://cdsweb.cern.ch/record/1062118

   NB: a user should not modify TrigL2Vertex directly but rather use methods provided by TrigVertexingTool

   Vertex fitting loop in the TrigL2VertexFitter basically consists of alternate calls of getChi2Distance 
and m_updateVertex methods of TrigVertexFittingNodes (i.e. tracks and constraints) 

*/

class TrigVertexFittingNode
{
 public:
  TrigVertexFittingNode(){};
  virtual ~TrigVertexFittingNode(){};
  virtual double getChi2Distance(class TrigL2Vertex*) = 0;//!< abstract method
  virtual void updateVertex(class TrigL2Vertex*) = 0;//!< abstract method
  virtual MsgStream& report( MsgStream& ) const = 0;   
 protected:
  double m_resid[2];
  double m_V[2][2];
  double m_D[2][MAX_SIZE_VERT_COVM];
};

inline MsgStream& operator << ( MsgStream& msg, const TrigVertexFittingNode& node)
{ 
  return node.report(msg); 
}

inline MsgStream& operator << ( MsgStream& msg, const TrigVertexFittingNode* node)
{ 
  return node->report(msg); 
}

class TrigVertexFitInputTrack : public TrigVertexFittingNode
{
 public:
  TrigVertexFitInputTrack(const TrigInDetTrack*, double);//!< constructor for L2 tracks 
  TrigVertexFitInputTrack(const Trk::Track*, double);//!< constructor for EF (offline) tracks
  ~TrigVertexFitInputTrack();
  const TrigInDetTrack* getTrigTrack();//!< getter for L2 tracks 
  const Trk::Track* getTrkTrack();//!< getter for EF (offline) tracks
  void initializeVertex(class TrigL2Vertex*);//!< resets and fills its part of the covariance and parameter vector
  bool linearize(class TrigL2Vertex*);//!< re-calculates linearization of the measurement model in the vicinity of parameters provided by input TrigL2Vertex
  virtual double getChi2Distance(class TrigL2Vertex*);//!< implementation of abstract method from the base class
  virtual void updateVertex(class TrigL2Vertex*);//!< implementation of abstract method from the base class
  virtual MsgStream& report( MsgStream& ) const;
  void setIndex(int);//!< to be used by TrigVertexingTool
  int getIndex() const;//!< to be used by TrigVertexingTool
  int getTrackType();//!< 0: L2 track, 1: EF(offline) track
  bool isActive();//!< if true this track will be used in the vertex fit otherwise it will be masked
  void activate();//!< sets isActive to true
  void mask();//!< sets isActive to false
  void setMass(double);//!< sets a mass of this particle
  double getMass() const {return m_mass;}//!< gets a mass of this particle
  const double* Perigee() const;//!< track parameters at the perigee
  double PerigeeCovariance(int,int) const;//!< covariance of track parameters at the perigee
 private:
  const TrigInDetTrack* m_pTrigTrack;
  const Trk::Track* m_pTrkTrack;
  int m_nTrackType;
  int m_index;
  double m_mass;
  double m_Vqq[3][3];
  double m_Vuq[2][3];
  double m_Vuu[2][2];
  double m_u[2];
  double m_q[3];
  double m_Perigee[5];
  double m_PerigeeCovariance[5][5];
  double m_A[2][3];
  double m_B[2][3];
  double m_h[2];

  bool m_active;
};

class TrigVertexFitConstraint : public TrigVertexFittingNode
{
 public:
  TrigVertexFitConstraint(double,const TrigVertexFitInputTrack*, const TrigVertexFitInputTrack*);//!< two-track mass constraint
  TrigVertexFitConstraint(double,const TrigVertexFitInputTrack*, const TrigVertexFitInputTrack*,
			  const TrigVertexFitInputTrack*);//!< three-track mass constraint
  ~TrigVertexFitConstraint();
  virtual double getChi2Distance(class TrigL2Vertex*);//!< implementation of abstract method from the base class
  virtual void updateVertex(class TrigL2Vertex*);//!< implementation of abstract method from the base class
  virtual MsgStream& report( MsgStream& ) const;
  double getValue();//!< returns a mass of the constraint
 private:
  double calculateInvariantMass(TrigL2Vertex* pV);
  std::list<const TrigVertexFitInputTrack*> m_trackList;
  double m_value;
};

/*
class TrigVertexCovarianceRow
{
 public:
  TrigVertexCovarianceRow(int);
  ~TrigVertexCovarianceRow();
  double& operator[] (int);
 private:
  int m_size;
  double* m_data;
};

class TrigVertexCovariance
{
 public:
  TrigVertexCovariance(int);
  ~TrigVertexCovariance();
  TrigVertexCovarianceRow& operator[] (int);
 private:
  int m_size;
  std::vector<TrigVertexCovarianceRow*>* m_data;
};
*/

class TrigL2Vertex
{
 public:
  TrigL2Vertex();
  ~TrigL2Vertex();
  TrigL2Vertex(const TrigL2Vertex&) = delete;
  TrigL2Vertex& operator=(const TrigL2Vertex&) = delete;
  bool prepareForFit();//!< resets all internal structures + initialization of the covariance
  void reset();//!< resets all internal structures
  double chi2();//!< returns accumulated \f$\chi^2\f$ of the fit
  int    ndof();//!< returns accumulated number-of-degree-of-freedom of the fit
  double mass();//!< returns calculated mass of the vertex
  double massVariance();//!< returns variance of the calculated mass of the vertex

  void addChi2(double);//!< increments accumulated \f$\chi^2\f$ of the fit
  void addNdof(int);//!< increments accumulated number-of-degree-of-freedom of the fit
  void setMass(double);//!< sets calculated mass of the vertex
  void setMassVariance(double);//!< sets variance of the calculated mass of the vertex
  int getNumberOfTracks();
  double* getParametersVector();//!< returns vector of vertex fit parameters: vertex position + refitted track momenta at-perigee (sic !)
  //TrigVertexCovariance* m_getCovariance();
  //void m_setCovariance(TrigVertexCovariance*);

  bool isVertexFitted();//!< vertex status
  bool isMassEstimated();//!< vertex status
  bool isReadyForFit();//!< vertex status

  const TrigVertexFitInputTrack* contains(const TrigInDetTrack*);//!< checks whether L2 track is in the vertex
  const TrigVertexFitInputTrack* contains(const Trk::Track*);//!< checks whether ofline track is in the vertex

  std::list<TrigVertexFitConstraint*>* getConstraints();//!< lists of all constraints imposed on the vertex
  std::list<TrigVertexFitInputTrack*>* getTracks();//!< lists of tracks in the vertex
  void setStatus(int);//!< sets vertex status flag
  int getStatus();//!< returns vertex status flag
  void setMotherTrack(TrigInDetTrackFitPar*);//!< sets mother particle parameters after kinematical fitting
  const TrigInDetTrackFitPar* getMotherTrack();//!< returns mother particle parameters if  m_isMassEstimated() is true
  MsgStream& report( MsgStream& ) const;
  double m_Gk[MAX_SIZE_VERT_COVM][MAX_SIZE_VERT_COVM];

 private:
  int m_nTracks,m_nDOF;
  //int m_nTrackType;
  //TrigVertexCovariance* m_Gk;
  
  std::list<TrigVertexFitInputTrack*>* m_pvTracks;
  std::list<TrigVertexFitConstraint*>* m_pvConstraints;
  double m_mass,m_chiSquared;
  double m_massVar;
  int m_nStatus;
  //double* m_Rk;
  double m_Rk[MAX_SIZE_VERT_COVM];
  TrigInDetTrackFitPar* m_P;
  bool m_ready;
};

inline MsgStream& operator << ( MsgStream& msg, const TrigL2Vertex& vrt)
{ 
  return vrt.report(msg); 
}

inline MsgStream& operator << ( MsgStream& msg, const TrigL2Vertex* vrt)
{ 
  return vrt->report(msg); 
}

#endif
