/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef DETAILEDTRACKTRUTHCOLLECTION_H
#define DETAILEDTRACKTRUTHCOLLECTION_H

#include "TrkTruthData/TrackTruthKey.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTrack/TrackCollection.h"
#include "AthLinks/DataLink.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <map>

class TrackTruthCollectionAccessor;

class DetailedTrackTruthCollection : public std::multimap<Trk::TrackTruthKey, DetailedTrackTruth> 
{
  friend class TrackTruthCollectionAccessor; // in TrackTruthTPCnv
  DataLink<TrackCollection> m_trackCollection;
 public:
  DetailedTrackTruthCollection(const DataLink<TrackCollection> tracks) : m_trackCollection(tracks) {}
  DataLink<TrackCollection> trackCollectionLink() const { return m_trackCollection; }
  void setTrackCollection(const DataLink<TrackCollection> tracks){m_trackCollection=tracks;}

  // for POOL
  DetailedTrackTruthCollection() {}
};


CLASS_DEF(DetailedTrackTruthCollection, 1289199392, 1)

#endif/*DETAILEDTRACKTRUTHCOLLECTION_H*/
