/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrkTruthTPCnv/DetailedTrackTruthCollectionCnv_p3.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p3.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"

DECLARE_TPCNV_FACTORY (DetailedTrackTruthCollectionCnv_p3,
                       DetailedTrackTruthCollection,
                       Trk::DetailedTrackTruthCollection_p3,
                       Athena::TPCnvVers::Current)
