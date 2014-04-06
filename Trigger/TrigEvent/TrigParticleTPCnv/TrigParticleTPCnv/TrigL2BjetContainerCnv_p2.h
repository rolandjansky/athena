/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetContainerCnv_p2
 *
 * @brief transient persistent converter for TrigL2BjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_P2_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p2.h"
#include "TrigParticleTPCnv/TrigL2BjetCnv_p2.h"
 
class TrigL2BjetContainerCnv_p2 : public T_AthenaPoolTPPtrVectorCnv< TrigL2BjetContainer, TrigL2BjetContainer_p2, TrigL2BjetCnv_p2 > {
 
 public:

  TrigL2BjetContainerCnv_p2() {}
 
};

#endif
