/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeatureContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for IsoMuonFeatureContainer_tlp1
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureContainerCnv_tlp1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_p1.h"   
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p1.h"   

 
class IsoMuonFeatureContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< IsoMuonFeatureContainerCnv_p1, IsoMuonFeatureContainer_tlp1 >
{
 
 public:
 
  IsoMuonFeatureContainerCnv_tlp1();
  virtual ~IsoMuonFeatureContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( IsoMuonFeatureContainer_tlp1 *storage );
 
 protected: 
  IsoMuonFeatureContainerCnv_p1    m_isoMuonFeatureContainerCnv;
  IsoMuonFeatureCnv_p1             m_isoMuonFeatureCnv;
 
}; //end of class definitions

 
#endif
