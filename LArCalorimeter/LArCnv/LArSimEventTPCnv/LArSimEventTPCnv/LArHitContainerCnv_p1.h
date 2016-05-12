/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARHITCONTAINERCNV_P1_H
#define LARTPCNV_LARHITCONTAINERCNV_P1_H

#include "LArSimEventTPCnv/LArHitContainer_p1.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArHitCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

typedef T_AthenaHitsVectorCnv< LArHitContainer, LArHitContainer_p1, LArHitCnv_p1 >  LArHitContainerCnv_p1;


#endif //not LARTPCNV_LARHITCONTAINERCNV_P1_H
