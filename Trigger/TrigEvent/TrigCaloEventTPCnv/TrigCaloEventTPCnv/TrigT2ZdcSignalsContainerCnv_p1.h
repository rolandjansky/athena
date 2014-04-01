/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENTTPCNV_TRIGT2ZDCSIGNALSCONTAINER_CNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2ZDCSIGNALSCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigT2ZdcSignalsContainer.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignalsContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignalsCnv_p1.h"

typedef T_AthenaPoolTPCnvVector<TrigT2ZdcSignalsContainer,TrigT2ZdcSignalsContainer_p1, TrigT2ZdcSignalsCnv_p1> TrigT2ZdcSignalsContainerCnv_p1;

#endif
