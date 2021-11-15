/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETAILEDMUONPATTERNTRUTHCOLLECTION_H
#define DETAILEDMUONPATTERNTRUTHCOLLECTION_H

#include <map>

#include "AthLinks/DataLink.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTruthData/TrackTruthKey.h"

class TrackTruthCollectionAccessor;

class DetailedMuonPatternTruthCollection
    : public std::multimap<ElementLink<DataVector<Muon::MuonPatternCombination> >, DetailedTrackTruth> {
    friend class TrackTruthCollectionAccessor;  // in TrackTruthTPCnv
    DataLink<MuonPatternCombinationCollection> m_trackCollection;

public:
    DetailedMuonPatternTruthCollection(const DataLink<MuonPatternCombinationCollection> tracks) : m_trackCollection(tracks) {}
    DataLink<MuonPatternCombinationCollection> trackCollectionLink() const { return m_trackCollection; }

    // for POOL
    DetailedMuonPatternTruthCollection() {}
};

CLASS_DEF(DetailedMuonPatternTruthCollection, 1289199398, 1)

#endif /*DETAILEDMUONPATTERNTRUTHCOLLECTION_H*/
