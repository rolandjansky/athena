/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetContainerCnv_p1
 *
 * @brief transient persistent converter for TrigEFBjetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBjetContainerCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_CNV_P1_H
#define TRIGPARTICLETPCNV_TRIGEFBJETCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigEFBjetContainer.h"
#include "TrigParticleTPCnv/TrigEFBjetContainer_p1.h"
#include "TrigParticleTPCnv/TrigEFBjetCnv_p1.h"
 
class TrigEFBjetContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigEFBjetContainer, TrigEFBjetContainer_p1, TrigEFBjetCnv_p1 >
{
 
 public:
  TrigEFBjetContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
