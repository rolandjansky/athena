/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigFTK_RawDataTPCnv/FTK_RawTrack_p1.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainer_p1.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainerCnv_tlp1.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainer_tlp1.h"

DECLARE_TPCNV_FACTORY(FTK_RawTrackContainerCnv_tlp1,
                      FTK_RawTrackContainer,
                      FTK_RawTrackContainer_tlp1,
                      Athena::TPCnvVers::Current) 

