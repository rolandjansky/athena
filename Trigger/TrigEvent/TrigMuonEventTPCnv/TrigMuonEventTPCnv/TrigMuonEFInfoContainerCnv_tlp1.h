/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigMuonEFInfoContainer_tlp1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_CNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_tlp1.h"

#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p5.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p5.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p3.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainerCnv_p1.h"

class TrigMuonEFInfoContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigMuonEFInfoContainerCnv_p1, TrigMuonEFInfoContainer_tlp1 >
{

 public:

  TrigMuonEFInfoContainerCnv_tlp1();
  virtual ~TrigMuonEFInfoContainerCnv_tlp1() {}

  //default methods
  virtual void  setPStorage( TrigMuonEFInfoContainer_tlp1 *storage );

 protected:
  TrigMuonEFInfoContainerCnv_p1         m_MuonEFInfoContainerCnv;
  TrigMuonEFInfoCnv_p1                  m_MuonEFInfoCnv;
  TrigMuonEFInfoCnv_p2                  m_MuonEFInfoCnv_p2;
  TrigMuonEFInfoCnv_p3                  m_MuonEFInfoCnv_p3;
  TrigMuonEFInfoCnv_p4                  m_MuonEFInfoCnv_p4;
  TrigMuonEFTrackCnv_p1                 m_MuonEFTrackCnv;
  TrigMuonEFTrackCnv_p2                 m_MuonEFTrackCnv_p2;
  TrigMuonEFTrackCnv_p3                 m_MuonEFTrackCnv_p3;
  TrigMuonEFTrackCnv_p4                 m_MuonEFTrackCnv_p4;
  TrigMuonEFTrackCnv_p5                 m_MuonEFTrackCnv_p5;
  TrigMuonEFCbTrackCnv_p1               m_MuonEFCbTrackCnv;
  TrigMuonEFCbTrackCnv_p2               m_MuonEFCbTrackCnv_p2;
  TrigMuonEFCbTrackCnv_p3               m_MuonEFCbTrackCnv_p3;
  TrigMuonEFCbTrackCnv_p4               m_MuonEFCbTrackCnv_p4;
  TrigMuonEFCbTrackCnv_p5               m_MuonEFCbTrackCnv_p5;
  P4IPtCotThPhiMCnv_p1                  m_P4IPtCotThPhiMCnv;
  TrigMuonEFInfoTrackContainerCnv_p1    m_MuonEFInfoTrackContainerCnv;
  TrigMuonEFInfoTrackCnv_p1             m_MuonEFInfoTrackCnv;

}; //end of class definitions

#endif

