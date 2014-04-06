/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauContainerCnv_p3
 *
 * @brief transient persistent converter for TrigTauContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauContainerCnv_p3.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAUCONTAINER_CNV_P3_H
#define TRIGPARTICLETPCNV_TRIGTAUCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigParticle/TrigTauContainer.h"
#include "TrigParticleTPCnv/TrigTauContainer_p3.h"
#include "TrigParticleTPCnv/TrigTauCnv_p3.h"

typedef T_AthenaPoolTPCnvVector<TrigTauContainer,TrigTauContainer_p3, TrigTauCnv_p3> TrigTauContainerCnv_p3;

#endif
