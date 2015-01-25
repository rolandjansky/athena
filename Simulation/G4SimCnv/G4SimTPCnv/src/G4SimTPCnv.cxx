/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "G4SimTPCnv/TrackRecordCollectionCnv_p1.h"
#include "G4SimTPCnv/TrackRecordCollectionCnv_p2.h"
#include "G4SimTPCnv/TrackRecord_p1.h"
#include "G4SimTPCnv/TrackRecordCollection_p1.h"
#include "G4SimTPCnv/TrackRecordCollection_p2.h"

// For Root streamer:
// for ROOT streamer
#include "G4SimTPCnv/TrackRecord_p0.h"
#include "G4SimTPCnv/TrackRecordStreamer_p0.h"

DECLARE_NAMED_TPCNV_FACTORY(TrackRecordCollectionCnv_p1,
                            TrackRecordCollectionCnv_p1,
                            AtlasHitsVector<TrackRecord>,
                            TrackRecordCollection_p1,
                            Athena::TPCnvVers::Old)
                      
DECLARE_NAMED_TPCNV_FACTORY(TrackRecordCollectionCnv_p2,
                            TrackRecordCollectionCnv_p2,
                            AtlasHitsVector<TrackRecord>,
                            TrackRecordCollection_p2,
                            Athena::TPCnvVers::Current)
                      
