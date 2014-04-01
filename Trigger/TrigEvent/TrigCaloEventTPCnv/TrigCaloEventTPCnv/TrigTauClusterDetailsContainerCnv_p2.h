/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetailsContainerCnv_p2
 *
 * @brief transient persistent converter for TrigTauClusterDetailsContainer
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_CNV_P2_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p2.h"
 
typedef T_AthenaPoolTPCnvVector< TrigTauClusterDetailsContainer, TrigTauClusterDetailsContainer_p2, TrigTauClusterDetailsCnv_p2 > TrigTauClusterDetailsContainerCnv_p2;
 
#endif
