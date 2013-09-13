/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : MuonFeatureContainerCnv_tlp2
 *
 * @brief "top-level" transient-persistent converter for MuonFeatureContainer_tlp2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: MuonFeatureContainerCnv_tlp2.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_CNV_TLP2_H
#define  TRIGMUONEVENTTPCNV_MUONFEATURECONTAINER_CNV_TLP2_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_tlp2.h"
 
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_p2.h"   
 
class MuonFeatureContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter< MuonFeatureContainerCnv_p2, MuonFeatureContainer_tlp2 >
{
 
 public:
 
  MuonFeatureContainerCnv_tlp2();
  virtual ~MuonFeatureContainerCnv_tlp2() {}
 
  //default methods  
  virtual void  setPStorage( MuonFeatureContainer_tlp2 *storage );
 
 protected: 
  MuonFeatureContainerCnv_p2    m_muonFeatureContainerCnv;
  MuonFeatureCnv_p2             m_muonFeatureCnv;
 
}; //end of class definitions
 

#endif
