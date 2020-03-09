/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTATHENAPOOL_JETMOMENTMAPCNV_H
#define JETEVENTATHENAPOOL_JETMOMENTMAPCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

#include "JetEventTPCnv/JetMomentMapCnv_p1.h"
#include "JetEventTPCnv/JetMomentMapCnv_p6.h"

#include "JetEvent/JetMomentMap.h"

#include <string>
#include <vector>

typedef T_AthenaPoolTPCnvCnv<JetMomentMap,
                             JetMomentMapCnv_p6,
                             JetMomentMapCnv_p1>
  JetMomentMapCnv;
                             

#endif
