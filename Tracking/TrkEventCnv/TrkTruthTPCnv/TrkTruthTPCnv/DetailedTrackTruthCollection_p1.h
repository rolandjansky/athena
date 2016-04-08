/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef DETAILEDTRACKTRUTHCOLLECTION_P1_H
#define DETAILEDTRACKTRUTHCOLLECTION_P1_H

#include "TrkTruthTPCnv/TrackTruthKey_p0.h"
#include "TrkTruthTPCnv/DetailedTrackTruth_p1.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include "TrkTrack/TrackCollection.h"

#include "CLIDSvc/CLASS_DEF.h"

#include <vector>

namespace Trk {

  class DetailedTrackTruthCollection_p1 {
  public:

    DataLink_p1 m_trackCollectionLink;
    
    // Perhaps can use here a 32 bit unsigned instead of the 64 bit one?
    typedef TrackCollection::size_type size_type;
    
    struct Entry {
      TrackTruthKey_p0 key;
      DetailedTrackTruth_p1 detailedTruth;
    };

    typedef std::vector<Entry> CollectionType;
    CollectionType m_entries;
  };
  
}

CLASS_DEF( Trk::DetailedTrackTruthCollection_p1 , 1321426761 , 1 )

#endif/*DETAILEDTRACKTRUTHCOLLECTION_P1_H*/
