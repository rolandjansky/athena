/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P1_H
#define CALOTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P1_H

#define private public
#include "CaloTPCnv/CaloCalibrationHitContainer_p1.h"
#undef private 
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloCalibrationHitCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

typedef T_AthenaHitsVectorCnv< CaloCalibrationHitContainer, CaloCalibrationHitContainer_p1, CaloCalibrationHitCnv_p1 >  CaloCalibrationHitContainerCnv_p1;



#endif // not CALOTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P1_H
