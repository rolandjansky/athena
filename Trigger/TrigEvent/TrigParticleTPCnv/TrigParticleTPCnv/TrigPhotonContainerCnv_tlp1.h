/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigPhotonContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonContainerCnv_tlp1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_CNV_TLP1_H
#define  TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigPhotonContainer_tlp1.h"
 
#include "TrigParticleTPCnv/TrigPhotonContainerCnv_p1.h"   
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h" 
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h" 
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h" 

 
class TrigPhotonContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigPhotonContainerCnv_p1, TrigPhotonContainer_tlp1 >
{
 
 public:
 
  TrigPhotonContainerCnv_tlp1();
  virtual ~TrigPhotonContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigPhotonContainer_tlp1 *storage );
  
 protected: 
  TrigPhotonContainerCnv_p1    m_trigPhotonContCnv;
  TrigPhotonCnv_p1             m_PhotonCnv;
  TrigCaloClusterCnv_p1          m_CaloClusterCnv;
  TrigEMClusterCnv_p1          m_EMclusterCnv;
  P4PtEtaPhiMCnv_p1            m_P4PtEtaPhiMCnv;
 
}; //end of class definitions
 


#endif
