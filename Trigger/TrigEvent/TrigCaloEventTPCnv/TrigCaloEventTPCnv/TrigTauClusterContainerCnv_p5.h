/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterContainerCnv_p5
 *
 * @brief transient persistent converter for TrigTauClusterContainer
 *
  * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P5_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p5.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p5.h"

typedef T_AthenaPoolTPCnvVector< TrigTauClusterContainer, TrigTauClusterContainer_p5, TrigTauClusterCnv_p5 > TrigTauClusterContainerCnv_p5;

#endif
