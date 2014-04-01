/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterContainerCnv_p3
 *
 * @author M. Pilar Casado <casado@ifae.es>  - IFAE/UAB - Barcelona
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p3.h"

typedef T_AthenaPoolTPCnvVector< TrigTauClusterContainer, TrigTauClusterContainer_p3, TrigTauClusterCnv_p3 > TrigTauClusterContainerCnv_p3;

#endif
