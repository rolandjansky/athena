/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTRACKFITTERSTATSCNV_H
#define FTKTRACKFITTERSTATSCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "FTKTrackFitterStatsCnv_p1.h" // last persistency version
#include "TrigFTKPool/FTKTrackFitterStats_p1.h"
#include "TrigFTKPool/FTKTrackFitterStats.h"

#include <memory>

typedef T_AthenaPoolTPCnvCnv< FTKTrackFitterStats,
                              FTKTrackFitterStatsCnv_p1 >
  FTKTrackFitterStatsCnv;


#endif // FTKTRACKFITTERSTATSCNV_H
