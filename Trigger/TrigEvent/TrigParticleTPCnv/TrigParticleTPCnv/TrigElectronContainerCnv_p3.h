/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectronContainerCnv_p3
 *
 * @brief "top level" transient-persistent converter for TrigElectronContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Ricardo Goncalo  <r.goncalo@rhul.ac.uk>     - Royal Holloway 
 *
 * File and Version Information:
 * $Id: TrigElectronContainerCnv_p3.h 563360 2013-09-27 13:59:30Z masik $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_P3_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_P3_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p3.h"
#include "TrigParticleTPCnv/TrigElectronCnv_p3.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< TrigElectronContainer, TrigElectronContainer_p3, TrigElectronCnv_p3 > TrigElectronContainerCnv_p3;

#endif
