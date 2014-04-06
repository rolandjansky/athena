/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonContainerCnv_p3
 *
 * @brief transient persistent converter for TrigPhotonContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonContainerCnv_p3.h 563360 2013-09-27 13:59:30Z masik $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_CNV_P3_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p3.h"
#include "TrigParticleTPCnv/TrigPhotonCnv_p3.h"
 
typedef T_AthenaPoolTPCnvVector< TrigPhotonContainer, TrigPhotonContainer_p3, TrigPhotonCnv_p3 > TrigPhotonContainerCnv_p3;

#endif
