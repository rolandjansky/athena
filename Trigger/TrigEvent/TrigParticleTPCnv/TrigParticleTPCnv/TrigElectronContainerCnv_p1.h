/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @class  : TrigElectronContainerCnv_p1
 *
 * @brief transient persistent converter for TrigElectronContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigElectronContainerCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_P1_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p1.h"
#include "TrigParticleTPCnv/TrigElectronCnv_p1.h"



class TrigElectronContainerCnv_p1 
 : public T_AthenaPoolTPPtrVectorCnv< TrigElectronContainer, TrigElectronContainer_p1, TrigElectronCnv_p1 >
{

 public:
  
  TrigElectronContainerCnv_p1() {}
  
};

#endif
