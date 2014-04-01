/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterContainerCnv_p4
 *
 * @brief transient persistent converter for TrigTauClusterContainer
 *
  * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P4_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p4.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p4.h"

typedef T_AthenaPoolTPCnvVector< TrigTauClusterContainer, TrigTauClusterContainer_p4, TrigTauClusterCnv_p4 > TrigTauClusterContainerCnv_p4;

#endif
