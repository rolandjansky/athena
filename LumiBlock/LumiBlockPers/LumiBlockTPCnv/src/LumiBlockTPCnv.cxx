/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include <vector>
#include "LumiBlockTPCnv/LumiBlockRange_p1.h"
#include "LumiBlockTPCnv/LumiBlockCollection_p1.h"
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p1.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p1.h"
#include "LumiBlockTPCnv/LumiBlockRange_p2.h"
#include "LumiBlockTPCnv/LumiBlockCollection_p2.h"
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p2.h"
#include "LumiBlockTPCnv/LumiBlockCollectionCnv_p2.h"

DECLARE_TPCNV_FACTORY(LumiBlockRangeCnv_p1,
                      LB_IOVRange,
                      LumiBlockRange_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_NAMED_TPCNV_FACTORY(LumiBlockCollectionCnv_p1,
                            LumiBlockCollectionCnv_p1,
                            LumiBlockCollection,
                            LumiBlockCollection_p1,
                            Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(LumiBlockRangeCnv_p2,
                      LB_IOVRange,
                      LumiBlockRange_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(LumiBlockCollectionCnv_p2,
                            LumiBlockCollectionCnv_p2,
                            LumiBlockCollection,
                            LumiBlockCollection_p2,
                            Athena::TPCnvVers::Current)
                      
