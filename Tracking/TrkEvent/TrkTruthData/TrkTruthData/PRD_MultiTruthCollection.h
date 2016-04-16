/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef PRD_MULTITRUTHCOLLECTION_H
#define PRD_MULTITRUTHCOLLECTION_H

#include <map>

#include "Identifier/Identifier.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include "CLIDSvc/CLASS_DEF.h"

/**
 * A PRD is mapped onto all contributing particles.
 */
class PRD_MultiTruthCollection : public std::multimap<Identifier, HepMcParticleLink> {};

CLASS_DEF(PRD_MultiTruthCollection, 1162521747, 1)

#endif/*PRD_MULTITRUTHCOLLECTION_H*/
