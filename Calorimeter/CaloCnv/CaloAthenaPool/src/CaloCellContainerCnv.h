/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloCellContainerCnv_H
#define CaloCellContainerCnv_H


#include "CaloTPCnv/CaloCellContainerCnv_p1.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCompactCellContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"


typedef T_AthenaPoolTPCnvCnv<CaloCellContainer, CaloCellContainerCnv_p1>
  CaloCellContainerCnv;


#endif
