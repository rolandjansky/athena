/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "EventTagTPCnv/RawInfoSummaryForTag_p1.h"
#include "EventTagTPCnv/RawInfoSummaryForTagCnv_p1.h"

DECLARE_TPCNV_FACTORY(RawInfoSummaryForTagCnv_p1,
                      RawInfoSummaryForTag,
                      RawInfoSummaryForTag_p1,
                      Athena::TPCnvVers::Current)
