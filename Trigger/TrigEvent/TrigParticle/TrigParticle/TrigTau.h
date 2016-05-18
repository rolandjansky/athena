/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAU_H_
#define _TRIGTAU_H_
/**

File: TrigTau.h

Description: To be created by the last FEX algorithm.
  
Design: Must be light and easy to serialize AND to store in ESD; should 
             avoid ElementLinks and try to persistify pointers instead; 
             should avoid inheritance to be easy to serialize.

Authour: M.P. Casado - Tu Mar 01 2006

Modified:

*/


#include <vector>
#include <iostream>
#include <string>

#include "FourMom/P4PtEtaPhiM.h"
#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h"

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

class TrigTau: public P4PtEtaPhiM,  public NavigableTerminalNode ,
      virtual public INavigable4Momentum   {

 public:
  
  TrigTau();
  
  TrigTau(int roi, 
	  float eta, float phi, float zvtx, float pt, 
	  float err_zvtx,  
	  float etCalib, float etFlow, int ntracks,
	  const TrigTauCluster* cluster, const TrigInDetTrackCollection* tracks,
	  const TrigTauTracksInfo* tracksInfo);
  
  
  TrigTau(int roi, 
	  float eta =0, float phi=0, float zvtx=0, float pt=0, float etCalib=0,
	  float etFlow = 0, int ntracks=0,
	  const TrigTauCluster* cluster=NULL, const TrigInDetTrackCollection* tracks=NULL,
	  const TrigTauTracksInfo* tracksInfo=NULL);
  
  ~TrigTau();
  
  /** accessor methods */
  int   roiId()    const;
  float Zvtx()     const;
  float err_Zvtx() const;
  float etCalibCluster()  const;
  float simpleEtFlow() const;
  int    nMatchedTracks()  const;
  float  trkAvgDist() const;
  float  etOverPtLeadTrk() const;
  const  TrigTauCluster* tauCluster() const;
  const  TrigInDetTrackCollection*  trackCollection()   const;
  const  TrigTauTracksInfo*  tracksInfo()   const;

  /** set methods */
  void setRoiId(int roiId);
  void setZvtx(float zvtx);
  void setErr_Zvtx(float err_zvtx);
  void setEtCalibCluster(float etCalibCluster);
  void setSimpleEtFlow(float etFlow);
  void setNMatchedTracks(int nmatchedTracks);
  void setTauCluster(const TrigTauCluster* tauCluster);
  void setTrackCollection(const TrigInDetTrackCollection* trackColl);
  void setTracksInfo(const TrigTauTracksInfo* trackInfo);
  void setTrkAvgDist( float trkAvgDist);
  void setEtOverPtLeadTrk( float etOverPtLeadTrk);


 private:
  
  // private data members
  /** Identifier of the RoI. */
  int   m_roiID;
  /** Z coordinate of the tau vertex. (Not filled) */
  float m_Zvtx; 
  /** Error on z coordinate. (Not filled) */
  float m_err_Zvtx;

  /** Transverse energy of the calibrated tau cluster. */
  float m_etCalibCluster;
  /** EtFlow as calculated from 3 fastest tracks and cluster with energy of first 3 EM samplings */
  float m_simpleEtFlow;
  /** Number of tracks matched to the calorimeter cluster. */
  int    m_nMatchedTracks;
  /** Calorimeter tau cluster. */
  const TrigTauCluster* m_cluster;
  /** Collection of inner detector tracks. */
  const TrigTauTracksInfo*  m_tracksInfo;
  /** Collection of inner detector tracks. */
  const TrigInDetTrackCollection*  m_tracks;


  /** average distance of tracks to tau direction */
  float m_trkAvgDist;
  /** ration of cluster energy to leading pt track */
  float m_etOverPtLeadTrk;

};

/// Helper function for printing the object
std::string str( const TrigTau& tau );

/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const TrigTau& tau );

/// Operator comparing two TrigTau objects for equality
bool operator== ( const TrigTau& left, const TrigTau& right );
/// Operator comparing two TrigTau objects for inequality
inline bool operator!= ( const TrigTau& left, const TrigTau& right ) {
   return !( left == right );
}
	
/// Comparison with feedback
void diff( const TrigTau& left, const TrigTau& right,
           std::map< std::string, double >& varChange );

CLASS_DEF( TrigTau , 45894787 , 3 )

#include "TrigParticle/TrigTauContainer.h"

#endif // TRIG_TAU
