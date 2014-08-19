/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SITRACK_TRIPLET_MAKER_H__
#define __TRIG_L2_SITRACK_TRIPLET_MAKER_H__

#include<cmath>
#include<exception>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"

class TrigL2SpacePointStorage;
class TrigL2SiTrackTripletCuts;
class TrigL2TimeoutException;

class TrigL2SiTrackTripletMaker {

 public:

  TrigL2SiTrackTripletMaker(const TrigL2SiTrackTripletMaker& t) : m_spLay(t.m_spLay), 
    m_cuts(t.m_cuts), m_trks_num_active(t.m_trks_num_active) {
    tripletMakingCount = t. tripletMakingCount;
    cut_eta = t.cut_eta;
    cut_phi = t.cut_phi;
    cut_d0  = t.cut_d0;
    cut_pt  = t.cut_pt; 
    cut_rphi = t.cut_rphi;
    for(unsigned int lay = 0; lay<MAX_SILICON_LAYER_NUM; lay++)
      cut_rz[lay] = t.cut_rz[lay];
  }

  TrigL2SiTrackTripletMaker(const TrigL2SpacePointStorage& sps, 
			    const TrigL2SiTrackTripletCuts& c,
			    unsigned int& trackNum) :  m_spLay(sps), m_cuts(c), m_trks_num_active(trackNum) {

    m_trks_num_active=0;

    tripletMakingCount=0;

    float ptMin = m_cuts.m_recoPtMin+m_cuts.m_recoPtMinTripletDiff; 
    if(ptMin<1.0) 
      ptMin=1.0;
    cut_eta = (5./5.) * (0.0325 + 490/ptMin);
    cut_phi = (5./5.) * (0.003  + 970/ptMin);
    if(cut_eta<m_cuts.m_etaHalfWidth) 
      cut_eta = m_cuts.m_etaHalfWidth;
    if(cut_phi<m_cuts.m_phiHalfWidth) 
      cut_phi = m_cuts.m_phiHalfWidth;
    cut_d0  = m_cuts.m_recoD0Max + m_cuts.m_recoD0MaxTripletDiff;
    cut_pt  = m_cuts.m_recoPtMin + m_cuts.m_recoPtMinTripletDiff;
    cut_rphi= m_cuts.m_dynamicalExtensionCutRPhi*m_cuts.m_dynamicalExtensionCutRPhi;

    for(unsigned int lay = 0; lay<MAX_SILICON_LAYER_NUM; lay++) {

      if((lay<OffsetBarrelSCT) || ((lay>=OffsetEndcapPixels) && (lay<OffsetEndcapSCT))) {
	cut_rz[lay]=m_cuts.m_dynamicalExtensionCutRZPixel*m_cuts.m_dynamicalExtensionCutRZPixel;
      }
      if(((lay>=OffsetBarrelSCT) && ( lay<OffsetEndcapPixels)) || (lay>=OffsetEndcapSCT)) {
	cut_rz[lay]=m_cuts.m_dynamicalExtensionCutRZ*m_cuts.m_dynamicalExtensionCutRZ;
      }
    }

  }

  ~TrigL2SiTrackTripletMaker(){};

  bool operator()(TrigL2TrackCandidate&) throw(TrigL2TimeoutException);

  class TripletLimitException {
  public:
    TripletLimitException(){};
    ~TripletLimitException(){};
  };

protected:

  const TrigL2SpacePointStorage& m_spLay;
  const TrigL2SiTrackTripletCuts& m_cuts;
  unsigned int& m_trks_num_active;

  int tripletMakingCount;
  float cut_eta;
  float cut_phi;
  float cut_d0;
  float cut_pt; 
  float cut_rphi;
  float cut_rz[MAX_SILICON_LAYER_NUM];
  
};

#endif
