/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauContainerCnv_p1
 *
 * @brief transient persistent converter for TrigTauContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauContainerCnv_p1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAUCONTAINER_CNV_P1_H
#define TRIGPARTICLETPCNV_TRIGTAUCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigTauContainer.h"
#include "TrigParticleTPCnv/TrigTauContainer_p1.h"
#include "TrigParticleTPCnv/TrigTauCnv_p1.h"
 
class TrigTauContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigTauContainer, TrigTauContainer_p1, ITPConverterFor<TrigTau> >
{
 
 public:
  TrigTauContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
