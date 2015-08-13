/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "RecEventTPCnv/RecoTimingObjCnv_p1.h"
                      
DECLARE_TPCNV_FACTORY(RecoTimingObjCnv_p1,
                      RecoTimingObj,
                      RecoTimingObj_p1,
                      Athena::TPCnvVers::Current)
