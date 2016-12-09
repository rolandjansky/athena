/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfo_p1
 *
 * @brief persistent partner for TrigTauTracksInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Olga Igonkina                                 Nikhef
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFO_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFO_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"


class TrigTauTracksInfo_p1 
{
  friend class TrigTauTracksInfoCnv_p1;

 public:
  
  TrigTauTracksInfo_p1() :
    m_roiID(0),    m_nCoreTracks(0),  m_nSlowTracks(0),  m_nIsoTracks(0),
    m_charge(0),   m_leadingTrackPt(0),   m_scalarPtSumCore(0),   
    m_scalarPtSumIso(0),   m_ptBalance(0)
    {}
  virtual ~TrigTauTracksInfo_p1(){}
  
  //private:

  int m_roiID;
  int m_nCoreTracks;
  int m_nSlowTracks;
  int m_nIsoTracks;

  float m_charge; 
  float m_leadingTrackPt;
  float m_scalarPtSumCore;
  float m_scalarPtSumIso;
  float m_ptBalance;


  TPObjRef   m_3fastest;

  TPObjRef  m_P4PtEtaPhiM;

};

#endif
