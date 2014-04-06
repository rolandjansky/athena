/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetContainerCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigL2BjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef  TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_TLP2_H
#define  TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_TLP2_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigL2BjetContainer_tlp2.h"
#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_p2.h"   

#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
 
class TrigL2BjetContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter< TrigL2BjetContainerCnv_p2, TrigL2BjetContainer_tlp2 > {
 
 public:
 
  TrigL2BjetContainerCnv_tlp2();
  virtual ~TrigL2BjetContainerCnv_tlp2() {}
 
  virtual void  setPStorage( TrigL2BjetContainer_tlp2 *storage );
  
 protected:

  TrigL2BjetContainerCnv_p2 m_trigL2BjetContCnv;
  TrigL2BjetCnv_p2          m_L2BjetCnv;
  P4PtEtaPhiMCnv_p1         m_p4PtEtaPhiMCnv;
   
};
 

#endif
