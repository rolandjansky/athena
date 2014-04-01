/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCONTAINER_CNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGCALOCLUSTERCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigCaloClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p3.h"

typedef T_AthenaPoolTPCnvVector<TrigCaloClusterContainer,TrigCaloClusterContainer_p3, TrigCaloClusterCnv_p3> TrigCaloClusterContainerCnv_p3;

#endif
