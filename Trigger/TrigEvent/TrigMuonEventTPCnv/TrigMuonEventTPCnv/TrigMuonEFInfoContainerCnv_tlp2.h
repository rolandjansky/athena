/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoContainerCnv_tlp2
 *
 * @brief "top-level" transient-persistent converter for TrigMuonEFInfoContainer_tlp2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 * @author Alexander Oh  <alexander.oh@cern.ch>        - U. Manchester
 *
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_CNV_TLP2_H
#define  TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_CNV_TLP2_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainer_tlp2.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_p2.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p5.h"
 
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainerCnv_p2.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackCnv_p1.h"

#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p6.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p7.h"

#include "TrigMuonEventTPCnv/TrigMuonEFTrackCnv_p6.h"

#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"

class TrigMuonEFInfoContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter< TrigMuonEFInfoContainerCnv_p2, TrigMuonEFInfoContainer_tlp2 >{

 public:

  TrigMuonEFInfoContainerCnv_tlp2();
  virtual ~TrigMuonEFInfoContainerCnv_tlp2() {}

  virtual void  setPStorage( TrigMuonEFInfoContainer_tlp2 *storage );

 protected:
  TrigMuonEFInfoContainerCnv_p2         m_MuonEFInfoContainerCnv;
  TrigMuonEFInfoCnv_p5                  m_MuonEFInfoCnv;
  TrigMuonEFTrackCnv_p6                 m_MuonEFTrackCnv;
  
  TrigMuonEFCbTrackCnv_p6               m_MuonEFCbTrackCnv;
  TrigMuonEFCbTrackCnv_p7               m_MuonEFCbTrackCnv_p7;
  
  P4IPtCotThPhiMCnv_p1                  m_P4IPtCotThPhiMCnv;
  TrigMuonEFInfoTrackContainerCnv_p2    m_MuonEFInfoTrackContainerCnv;
  TrigMuonEFInfoTrackCnv_p1             m_MuonEFInfoTrackCnv;

}; 

#endif

