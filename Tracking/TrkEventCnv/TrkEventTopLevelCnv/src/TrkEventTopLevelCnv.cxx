/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrkEventTopLevelCnv/SegmentCollectionCnv_tlp4.h"
#include "TrkEventTopLevelCnv/TrackCollectionCnv_tlp6.h"


DECLARE_TPCNV_FACTORY(SegmentCollectionCnv_tlp4,
                      Trk::SegmentCollection,
                      Trk::SegmentCollection_tlp4,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(TrackCollectionCnv_tlp6,
                      TrackCollection,
                      Trk::TrackCollection_tlp6,
                      Athena::TPCnvVers::Current)
