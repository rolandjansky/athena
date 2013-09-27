/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef TRKTRUTHDATA_TRACKTRUTHCOLLECTION_H
#define TRKTRUTHDATA_TRACKTRUTHCOLLECTION_H

#include "TrkTruthData/TrackTruthKey.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkTrack/TrackCollection.h"
#include "DataModel/DataLink.h"
#include "CLIDSvc/CLASS_DEF.h"

#include <map>

//class TrackCollection; // forward-declaring a typedef is not elegant
class TrackTruthCollectionAccessor;

class TrackTruthCollection : public std::map<Trk::TrackTruthKey, TrackTruth> 
{
  friend class TrackTruthCollectionAccessor; // in TrackTruthTPCnv
  DataLink<TrackCollection> m_trackCollection;

public:

  DataLink<TrackCollection> trackCollectionLink() const { return m_trackCollection; }
  TrackTruthCollection(const DataLink<TrackCollection>& tracks) : m_trackCollection(tracks) {}
  
  TrackTruthCollection() {} // For POOL
};

CLASS_DEF(TrackTruthCollection,1236535072 , 1)

#endif // TRKTRUTHDATA_TRACKTRUTHCOLLECTION_H
