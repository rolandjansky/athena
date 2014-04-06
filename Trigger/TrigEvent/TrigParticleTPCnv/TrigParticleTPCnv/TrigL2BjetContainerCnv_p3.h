/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetContainerCnv_p3
 *
 * @brief transient persistent converter for TrigL2BjetContainer
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_P3_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p3.h"
#include "TrigParticleTPCnv/TrigL2BjetCnv_p3.h"
 
typedef T_AthenaPoolTPCnvVector< TrigL2BjetContainer, TrigL2BjetContainer_p3, TrigL2BjetCnv_p3 > TrigL2BjetContainerCnv_p3;

#endif
