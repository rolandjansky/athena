/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoContainer_tlp1
 *
 * @brief "top-level" persistent partner for TrigMuonEFInfoContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfo_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrack_p5.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrack_p4.h"
#include "EventCommonTPCnv/P4IPtCotThPhiM_p1.h"


class TrigMuonEFInfoContainer_tlp1
{

 public:

  TrigMuonEFInfoContainer_tlp1(){}
  // aoh:
  // DO NOT CHANGE VARIABLE NAMES, BREAKS BACKWARD COMPATIBILITY IN BS READING
  //
  std::vector<TrigMuonEFInfoContainer_p1>       m_TrigMuonEFInfoContainers;
  std::vector<TrigMuonEFInfo_p1>                m_MuonEFInfo;
  std::vector<TrigMuonEFInfo_p2>                m_MuonEFInfo_p2;
  std::vector<TrigMuonEFInfo_p3>                m_MuonEFInfo_p3;
  std::vector<TrigMuonEFInfo_p4>                m_MuonEFInfo_p4;
  std::vector<TrigMuonEFTrack_p1>               m_MuonEFTrack;
  std::vector<TrigMuonEFTrack_p2>               m_MuonEFTrack_p2;
  std::vector<TrigMuonEFTrack_p3>               m_MuonEFTrack_p3;
  std::vector<TrigMuonEFTrack_p4>               m_MuonEFTrack_p4;
  std::vector<TrigMuonEFTrack_p5>               m_MuonEFTrack_p5;
  std::vector<TrigMuonEFCbTrack_p1>             m_MuonEFCbTrack;
  std::vector<TrigMuonEFCbTrack_p2>             m_MuonEFCbTrack_p2;
  std::vector<TrigMuonEFCbTrack_p3>             m_MuonEFCbTrack_p3;
  std::vector<TrigMuonEFCbTrack_p4>             m_MuonEFCbTrack_p4;
  std::vector<P4IPtCotThPhiM_p1>                m_P4IPtCotThPhiM;
  std::vector<TrigMuonEFInfoTrackContainer_p1>  m_MuonEFInfoTrackContainer;
  std::vector<TrigMuonEFInfoTrack_p1>           m_MuonEFInfoTrack;

};//end of class definitions

#endif
