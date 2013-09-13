/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigMuonEFContainer_tlp1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEFContainerCnv_tlp1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainer_tlp1.h"
 
#include "TrigMuonEventTPCnv/TrigMuonEFContainerCnv_p1.h"   
#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h" 
 
class TrigMuonEFContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigMuonEFContainerCnv_p1, TrigMuonEFContainer_tlp1 >
{
 
 public:
 
  TrigMuonEFContainerCnv_tlp1();
  virtual ~TrigMuonEFContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigMuonEFContainer_tlp1 *storage );
 
 protected: 
  TrigMuonEFContainerCnv_p1    m_trigMuonEFContainerCnv;
  TrigMuonEFCnv_p1             m_MuonEFCnv;
  P4IPtCotThPhiMCnv_p1         m_P4IPtCotThPhiMCnv;
 
}; //end of class definitions
 

#endif
