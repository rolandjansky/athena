/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef PRD_MULTITRUTHCOLLECTION_P1_H
#define PRD_MULTITRUTHCOLLECTION_P1_H

#ifdef __IDENTIFIER_PERS_VARSIZE__
#include "Identifier/Identifier.h"
#endif

#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

#include <vector>

namespace Trk {
  class PRD_MultiTruthCollection_p1 {
  public:
    // the lore is that Root does not split std::pair, so make our own class
    struct Entry {
// #ifdef __IDENTIFIER_PERS_VARSIZE__
//       Identifier::value_type id; // the Identifier
//       HepMcParticleLink_p1 particle;
//       Entry(Identifier::value_type i, const HepMcParticleLink_p1& p) : id(i), particle(p) {}
// #else
//       unsigned int id; // the Identifier
//       HepMcParticleLink_p1 particle;
//       Entry(unsigned int i, const HepMcParticleLink_p1& p) : id(i), particle(p) {}
// #endif
      unsigned int id; // the Identifier
      HepMcParticleLink_p1 particle;
      Entry(unsigned int i, const HepMcParticleLink_p1& p) : id(i), particle(p) {}

      Entry() : id(0) {} // stupid call required by reflex dicts 
    };
    
    typedef std::vector<Entry> CollectionType;
    CollectionType m_entries;
  };
}

#endif/*PRD_MULTITRUTHCOLLECTION_P1_H*/
