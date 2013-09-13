/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for MuonFeatureContainer_tlp1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureContainerCnv_tlp1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p1.h"   
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p1.h"   
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p2.h"   

 
class CombinedMuonFeatureContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CombinedMuonFeatureContainerCnv_p1, CombinedMuonFeatureContainer_tlp1 >
{
 
 public:
 
  CombinedMuonFeatureContainerCnv_tlp1();
  virtual ~CombinedMuonFeatureContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( CombinedMuonFeatureContainer_tlp1 *storage );
 
 protected: 
  CombinedMuonFeatureContainerCnv_p1    m_combinedMuonFeatureContainerCnv;
  CombinedMuonFeatureCnv_p1             m_combinedMuonFeatureCnv;
  CombinedMuonFeatureCnv_p2             m_combinedMuonFeatureCnv_p2;
 
}; //end of class definitions
 

#endif
