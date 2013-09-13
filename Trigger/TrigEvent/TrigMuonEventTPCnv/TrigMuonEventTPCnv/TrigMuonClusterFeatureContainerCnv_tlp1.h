/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonClusterFeatureContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TrigMuonClusterFeatureContainer_tlp1
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 * @author Antonio Policicchio <Antonio.Policicchio@cern.ch>  - U. of Washington
 *
 * File and Version Information:
 * $Id: TrigMuonClusterFeatureContainerCnv_tlp1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainerCnv_p1.h"   
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_p1.h"   

 
class TrigMuonClusterFeatureContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigMuonClusterFeatureContainerCnv_p1, TrigMuonClusterFeatureContainer_tlp1 >
{
 
 public:
 
  TrigMuonClusterFeatureContainerCnv_tlp1();
  virtual ~TrigMuonClusterFeatureContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigMuonClusterFeatureContainer_tlp1 *storage );
 
 protected: 
  TrigMuonClusterFeatureContainerCnv_p1    m_trigMuonClusterFeatureContainerCnv;
  TrigMuonClusterFeatureCnv_p1             m_trigMuonClusterFeatureCnv;
 
}; //end of class definitions
 
#endif
