/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"


#include "LArSimEventTPCnv/LArHit_p1.h"

#include "LArSimEventTPCnv/LArHitContainer_p1.h"
#include "LArSimEventTPCnv/LArHitContainer_p2.h"

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitFloat.h"
#include "LArSimEventTPCnv/LArHitContainerCnv_p1.h"
#include "LArSimEventTPCnv/LArHitFloatContainerCnv_p1.h"
#include "LArSimEventTPCnv/LArHitContainerCnv_p2.h"
#include "LArSimEventTPCnv/LArHitFloatContainerCnv_p2.h"


DECLARE_TPCNV_FACTORY(LArHitContainerCnv_p1,
                      LArHitContainer,
                      LArHitContainer_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(LArHitContainerCnv_p2,
                      LArHitContainer,
                      LArHitContainer_p2,
                      Athena::TPCnvVers::Current)
  
// We can't really handle multiple transient classes using the same p class.
// Just drop this for now --- i don't think this is relevant for either
// ARA or trigger.
//DECLARE_TPCNV_FACTORY(LArHitFloatContainerCnv_p2,
//                      LArHitFloatContainer,
//                      LArHitContainer_p2,
//                      Athena::TPCnvVers::Current)
