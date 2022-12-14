/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @class  : TrigTauTracksInfo_p2
 *
 * @brief persistent partner for TrigTauTracksInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Olga Igonkina                                 Nikhef
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFO_P2_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFO_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"

#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigTauTracksInfo_p2
{
  friend class TrigTauTracksInfoCnv_p2;

 public:
  
  TrigTauTracksInfo_p2() {}
  virtual ~TrigTauTracksInfo_p2() = default;
  
  //private:

  int m_allTheInts[4];
/*
  int m_roiID;
  int m_nCoreTracks;
  int m_nSlowTracks;
  int m_nIsoTracks;
*/

  float m_allTheFloats[5];
/*
  float m_charge; 
  float m_leadingTrackPt;
  float m_scalarPtSumCore;
  float m_scalarPtSumIso;
  float m_ptBalance;
*/

//  TPObjRef   m_3fastest;
  P4PtEtaPhiM_p1 m_3fastest;
  P4PtEtaPhiM_p1 m_P4PtEtaPhiM;

};

#endif
