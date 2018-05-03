/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTION_P2_H
#define TRACKPARTICLETRUTHCOLLECTION_P2_H

#include "DataModelAthenaPool/DataLink_p1.h"
#include "Particle/TrackParticleContainer.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

#include <vector>


namespace Rec {
  class TrackParticleTruthCollection_p2 {
  public:

    DataLink_p1 m_trackCollectionLink;

    // Perhaps can use here a 32 bit unsigned instead of the 64 bit one?
    typedef TrackParticleContainer::size_type size_type;
    
    struct Entry {
      size_type index;

      // Do TrackParticleTruth here instead of introducing a separate converer for it.
      // TrackParticleTruth::m_flag is not used, don't store it.
      float probability;
      HepMcParticleLink_p2 particle;
    };

    typedef std::vector<Entry> CollectionType;
    CollectionType m_entries;
  };
  
}

#endif/*TRACKPARTICLETRUTHCOLLECTION_P2_H*/

