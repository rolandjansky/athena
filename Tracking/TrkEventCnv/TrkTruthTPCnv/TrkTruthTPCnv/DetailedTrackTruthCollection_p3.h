/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2008
// Olivier.Arnaez@cern.ch, 2015

#ifndef DETAILEDTRACKTRUTHCOLLECTION_P3_H
#define DETAILEDTRACKTRUTHCOLLECTION_P3_H

#include "TrkTruthTPCnv/TrackTruthKey_p0.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p3.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include "TrkTrack/TrackCollection.h"

#include "CLIDSvc/CLASS_DEF.h"

#include <vector>

namespace Trk {

  class DetailedTrackTruthCollection_p3 {
  public:

    DataLink_p1 m_trackCollectionLink;
    
    // Perhaps can use here a 32 bit unsigned instead of the 64 bit one?
    typedef TrackCollection::size_type size_type;
    
    struct Entry {
      TrackTruthKey_p0 key;
      DetailedTrackTruth_p3 detailedTruth;
    };

    typedef std::vector<Entry> CollectionType;
    CollectionType m_entries;
  };
  
}

CLASS_DEF( Trk::DetailedTrackTruthCollection_p3 , 1322426762 , 1 )

#endif/*DETAILEDTRACKTRUTHCOLLECTION_P3_H*/
