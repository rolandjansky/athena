/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETAILEDMUONPATTERNTRUTHCOLLECTION_H
#define DETAILEDMUONPATTERNTRUTHCOLLECTION_H

#include "TrkTruthData/TrackTruthKey.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTrack/TrackCollection.h"
#include "DataModel/DataLink.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternCombination.h"

#include <map>

class TrackTruthCollectionAccessor;

class DetailedMuonPatternTruthCollection : public std::multimap<ElementLink<DataVector<Muon::MuonPatternCombination> >, DetailedTrackTruth> 
{
  friend class TrackTruthCollectionAccessor; // in TrackTruthTPCnv
  DataLink<MuonPatternCombinationCollection> m_trackCollection;
 public:
  DetailedMuonPatternTruthCollection(const DataLink<MuonPatternCombinationCollection> tracks) : m_trackCollection(tracks) {}
  DataLink<MuonPatternCombinationCollection> trackCollectionLink() const { return m_trackCollection; }

  // for POOL
  DetailedMuonPatternTruthCollection() {}
};


CLASS_DEF(DetailedMuonPatternTruthCollection, 1289199398, 1)

#endif/*DETAILEDMUONPATTERNTRUTHCOLLECTION_H*/
