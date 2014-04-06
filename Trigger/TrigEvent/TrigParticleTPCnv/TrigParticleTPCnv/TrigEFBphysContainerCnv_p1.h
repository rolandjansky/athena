/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphysContainerCnv_p1
 *
 * @brief transient persistent converter for TrigEFBphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysContainerCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_CNV_P1_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYSCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticleTPCnv/TrigEFBphysCnv_p2.h"
#include "TrigParticle/TrigEFBphysContainer.h"
#include "TrigParticleTPCnv/TrigEFBphysContainer_p1.h"
 
class TrigEFBphysContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigEFBphysContainer, TrigEFBphysContainer_p1,  ITPConverterFor<TrigEFBphys> >
{
 
 public:
  TrigEFBphysContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
