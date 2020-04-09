/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007
// Olivier.Arnaez@cern.ch, 2015


#ifndef PRD_MULTITRUTHCOLLECTION_P4_H
#define PRD_MULTITRUTHCOLLECTION_P4_H

#include "GeneratorObjectsTPCnv/HepMcParticleLink_p3.h"

#include <vector>

namespace Trk {
  class PRD_MultiTruthCollection_p4 {
  public:
    // the lore is that Root does not split std::pair, so make our own class
    struct Entry {
      unsigned long long id; // the Identifier
      HepMcParticleLink_p3 particle;
      Entry(unsigned long long i, const HepMcParticleLink_p3& p) : id(i), particle(p) {}

      Entry() : id(0) {} // stupid call required by reflex dicts 
    };
    
    typedef std::vector<Entry> CollectionType;
    CollectionType m_entries;
  };
}

#endif/*PRD_MULTITRUTHCOLLECTION_P4_H*/
