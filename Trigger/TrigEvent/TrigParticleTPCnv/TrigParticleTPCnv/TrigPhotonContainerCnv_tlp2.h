/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonContainerCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigPhotonContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonContainerCnv_tlp2.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_CNV_TLP2_H
#define  TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_CNV_TLP2_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigPhotonContainer_tlp2.h"
 
#include "TrigParticleTPCnv/TrigPhotonContainerCnv_p2.h"   
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h" 
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h" 
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h" 


 
class TrigPhotonContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter< TrigPhotonContainerCnv_p2, TrigPhotonContainer_tlp2 >
{
 
 public:
 
  TrigPhotonContainerCnv_tlp2();
  virtual ~TrigPhotonContainerCnv_tlp2() {}
 
  //default methods  
  virtual void  setPStorage( TrigPhotonContainer_tlp2 *storage );
  
 protected: 
  TrigPhotonContainerCnv_p2    m_trigPhotonContCnv;
  TrigPhotonCnv_p2             m_PhotonCnv;
  P4PtEtaPhiMCnv_p1            m_P4PtEtaPhiMCnv;
 
}; //end of class definitions
 


#endif
