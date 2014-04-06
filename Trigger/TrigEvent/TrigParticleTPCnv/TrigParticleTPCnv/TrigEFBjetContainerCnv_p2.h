/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetContainerCnv_p2
 *
 * @brief transient persistent converter for TrigEFBjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_CNV_P2_H
#define TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigEFBjetContainer.h"
#include "TrigParticleTPCnv/TrigEFBjetContainer_p2.h"
#include "TrigParticleTPCnv/TrigEFBjetCnv_p2.h"
 
class TrigEFBjetContainerCnv_p2 : public T_AthenaPoolTPPtrVectorCnv< TrigEFBjetContainer, TrigEFBjetContainer_p2, TrigEFBjetCnv_p2 > {
 
 public:

  TrigEFBjetContainerCnv_p2() {}
 
};
 
#endif
