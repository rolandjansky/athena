/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetContainerCnv_p1
 *
 * @brief transient persistent converter for TrigL2BjetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BjetContainerCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_P1_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p1.h"
#include "TrigParticleTPCnv/TrigL2BjetCnv_p1.h"
 
class TrigL2BjetContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigL2BjetContainer, TrigL2BjetContainer_p1, TrigL2BjetCnv_p1 >
{
 
 public:
  TrigL2BjetContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
