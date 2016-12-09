/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfo_tlp1
 *
 * @brief "top level" persistent partner for TrigTauTracksInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Olga Igonkina                                 Nikhef
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFO_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFO_TLP1_H

#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"
#include <vector>

class TrigTauTracksInfo_tlp1
{
 public:
  
  TrigTauTracksInfo_tlp1() {}
  friend class TrigTauTracksInfoCnv_tlp1;
  
 private:
  
  std::vector< TrigTauTracksInfo_p1 >            m_trigTauTracksInfo;
  std::vector< P4PtEtaPhiM_p1       >            m_p4PtEtaPhiM;

};

#endif
