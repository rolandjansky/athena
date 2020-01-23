/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKATHTRACKCONTAINERCNV_H
#define FTKATHTRACKCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "FTKAthTrackContainerCnv_p1.h" // last persistency version

#include "TrigFTKPool/FTKAthTrackContainer_p1.h"
#include "TrigFTKPool/FTKAthTrackContainer.h"

#include <memory>

typedef T_AthenaPoolTPCnvCnv< FTKAthTrackContainer,
                              FTKAthTrackContainerCnv_p1 >
  FTKAthTrackContainerCnv;


#endif // FTKATHTRACKCONTAINERCNV_H

