/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetContainerCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigEFBjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef  TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_CNV_TLP2_H
#define  TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_CNV_TLP2_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigEFBjetContainer_tlp2.h" 
#include "TrigParticleTPCnv/TrigEFBjetContainerCnv_p2.h"   
 
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

class TrigEFBjetContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter< TrigEFBjetContainerCnv_p2, TrigEFBjetContainer_tlp2 > {
 
 public:
 
  TrigEFBjetContainerCnv_tlp2();
  virtual ~TrigEFBjetContainerCnv_tlp2() {}
 
  virtual void  setPStorage( TrigEFBjetContainer_tlp2 *storage );
  
 protected:

  TrigEFBjetContainerCnv_p2 m_trigEFBjetContCnv;
  TrigEFBjetCnv_p2          m_EFBjetCnv;
  P4PtEtaPhiMCnv_p1         m_p4PtEtaPhiMCnv;
   
};
 



#endif
