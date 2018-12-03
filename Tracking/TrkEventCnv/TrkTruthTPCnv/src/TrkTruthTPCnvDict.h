/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRUTHTPCNV_H
#define TRKTRUTHTPCNV_H

//-----------------------------------------------------------------------------
// MC truth
//-----------------------------------------------------------------------------
#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p1.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p2.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p3.h"

#include "TrkTruthTPCnv/TrackTruthCollection_p0.h"
#include "TrkTruthTPCnv/TrackTruthCollection_p1.h"
#include "TrkTruthTPCnv/TrackTruthCollection_p2.h"

#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p0.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p0.h"
#include "TrkTruthTPCnv/TrackTruthKey_p0.h"
#include "TrkTruthTPCnv/SubDetHitStatistics_p0.h"
#include "TrkTruthTPCnv/TruthTrajectory_p0.h"

#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p1.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p1.h"
#include "TrkTruthTPCnv/TruthTrajectory_p1.h"

#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p2.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p2.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p3.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p3.h"
#include "TrkTruthTPCnv/TruthTrajectory_p2.h"

namespace {
  // --->> genreflex: WARNING: Class std::pair<ElementLink<DataVector<Trk::Track>, DataProxyStorage<DataVector<Trk::Track> >,ForwardIndexingPolicy<DataVector<Trk::Track> > >,TrackTruth> in selection file ../i686-slc4-gcc34-opt/dict/TrkTruthTPCnv_selection.xml not generated.  Fixing that:
  std::pair<ElementLink<TrackCollection>,TrackTruth>  dummy1;

  // --->> genreflex: WARNING: Class std::pair<Trk::TrackTruthKey_p0, Trk::DetailedTrackTruth_p0 > in selection file ../i686-slc4-gcc34-opt/dict/TrkTruthTPCnv_selection.xml not generated.
  std::pair<Trk::TrackTruthKey_p0, Trk::DetailedTrackTruth_p0 > dummy2;
}

#endif/*TRKTRUTHTPCNV_H*/
