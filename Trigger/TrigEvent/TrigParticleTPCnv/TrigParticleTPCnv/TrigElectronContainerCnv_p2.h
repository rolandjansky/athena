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
 * @author Ricardo Goncalo  <r.goncalo@rhul.ac.uk>     - Royal Holloway 
 *
 * File and Version Information:
 * $Id: TrigElectronContainerCnv_p2.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_P2_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p2.h"
#include "TrigParticleTPCnv/TrigElectronCnv_p2.h"



class TrigElectronContainerCnv_p2 
 : public T_AthenaPoolTPPtrVectorCnv< TrigElectronContainer, TrigElectronContainer_p2, TrigElectronCnv_p2 >
{

 public:
  
  TrigElectronContainerCnv_p2() {}
  
};

#endif
