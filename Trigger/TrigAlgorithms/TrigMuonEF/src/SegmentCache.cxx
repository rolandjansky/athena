/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SegmentCache.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternCombination.h"

SegmentCache::SegmentCache(){
  m_segments=0;
  m_pattcoll=0;
  m_msTracks = 0;
  m_saTracks = 0;
  m_nroi=0;
 }


/**
 * Destructor.
 * We do not delete the pointers, since these are not owned by this class.
 */
SegmentCache::~SegmentCache(){
  
}

void SegmentCache::AddPatternCollection(const MuonPatternCombinationCollection* pat) {
  m_pattcoll = pat;
}

void SegmentCache::AddSegments(const Trk::SegmentCollection* trk) {
  m_segments = trk;
}

void SegmentCache::SetNROI(int n){
  m_nroi= n;
}

void SegmentCache::SetSpectrometerTrackCollection( const TrackCollection* msTracks ) {
  m_msTracks = msTracks;
}

void SegmentCache::SetExtrapolatedTrackCollection( TrackCollection* saTracks ) {
  m_saTracks = saTracks;
}
