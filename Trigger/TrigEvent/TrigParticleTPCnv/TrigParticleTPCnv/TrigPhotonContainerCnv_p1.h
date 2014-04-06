/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonContainerCnv_p1
 *
 * @brief transient persistent converter for TrigPhotonContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonContainerCnv_p1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_CNV_P1_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p1.h"
#include "TrigParticleTPCnv/TrigPhotonCnv_p1.h"
 
class TrigPhotonContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigPhotonContainer, TrigPhotonContainer_p1, TrigPhotonCnv_p1 >
{
 
 public:
  TrigPhotonContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
