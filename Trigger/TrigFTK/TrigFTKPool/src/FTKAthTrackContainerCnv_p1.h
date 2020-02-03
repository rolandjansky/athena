/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKATHTRACKCONTAINERCNV_P1_H
#define FTKATHTRACKCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigFTKPool/FTKAthTrackContainer.h"
#include "TrigFTKPool/FTKAthTrackContainer_p1.h"
#include "FTKAthTrackCnv_p1.h"

typedef T_AthenaPoolTPCnvVectorConst<FTKAthTrackContainer, FTKAthTrackContainer_p1, FTKAthTrackCnv_p1> FTKAthTrackContainerCnv_p1;

template<>
class T_TPCnv<FTKAthTrackContainer, FTKAthTrackContainer_p1> : public FTKAthTrackContainerCnv_p1 {
public:
};

#endif // FTKATHTRACKCONTAINERCNV_P1_H
