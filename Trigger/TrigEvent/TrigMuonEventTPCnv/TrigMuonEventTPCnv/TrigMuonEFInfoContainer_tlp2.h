/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoContainer_tlp2
 *
 * @brief "top-level" persistent partner for TrigMuonEFInfoContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_TLP2_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_TLP2_H

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p5.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p6.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p6.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p7.h"
#include "EventCommonTPCnv/P4IPtCotThPhiM_p1.h"


class TrigMuonEFInfoContainer_tlp2 {

 public:

  TrigMuonEFInfoContainer_tlp2(){}

  // DO NOT CHANGE VARIABLE NAMES, BREAKS BACKWARD COMPATIBILITY IN BS READING

  std::vector<TrigMuonEFInfoContainer_p2>       m_TrigMuonEFInfoContainer_p2;
  
  std::vector<TrigMuonEFInfo_p5>                m_MuonEFInfo_p5;
  std::vector<TrigMuonEFTrack_p6>               m_MuonEFTrack_p6;
  std::vector<TrigMuonEFCbTrack_p6>             m_MuonEFCbTrack_p6;
  std::vector<TrigMuonEFCbTrack_p7>             m_MuonEFCbTrack_p7;
  
  std::vector<P4IPtCotThPhiM_p1>                m_P4IPtCotThPhiM;
  std::vector<TrigMuonEFInfoTrackContainer_p2>  m_MuonEFInfoTrackContainer;
  std::vector<TrigMuonEFInfoTrack_p1>           m_MuonEFInfoTrack;

};

#endif
