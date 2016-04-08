/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef PRD_MULTITRUTHCOLLECTION_P2_H
#define PRD_MULTITRUTHCOLLECTION_P2_H

#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

#include <vector>

namespace Trk {
  class PRD_MultiTruthCollection_p2 {
  public:
    // the lore is that Root does not split std::pair, so make our own class
    struct Entry {
      unsigned long long id; // the Identifier
      HepMcParticleLink_p1 particle;
      Entry(unsigned long long i, const HepMcParticleLink_p1& p) : id(i), particle(p) {}

      Entry() : id(0) {} // stupid call required by reflex dicts 
    };
    
    typedef std::vector<Entry> CollectionType;
    CollectionType m_entries;
  };
}

#endif/*PRD_MULTITRUTHCOLLECTION_P2_H*/
