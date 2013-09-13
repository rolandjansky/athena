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
 * $Id: MuonFeatureContainerCnv_tlp1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_tlp1.h"
 
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_p1.h"   
 
class MuonFeatureContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< MuonFeatureContainerCnv_p1, MuonFeatureContainer_tlp1 >
{
 
 public:
 
  MuonFeatureContainerCnv_tlp1();
  virtual ~MuonFeatureContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( MuonFeatureContainer_tlp1 *storage );
 
 protected: 
  MuonFeatureContainerCnv_p1    m_muonFeatureContainerCnv;
  MuonFeatureCnv_p1             m_muonFeatureCnv;
  MuonFeatureCnv_p2             m_muonFeatureCnv_p2;
 
}; //end of class definitions

#endif
