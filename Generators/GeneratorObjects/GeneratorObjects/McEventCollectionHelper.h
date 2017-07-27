/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTRUTHSIMALGS_MCEVENTCOLLECTIONHELPER_H
#define MCTRUTHSIMALGS_MCEVENTCOLLECTIONHELPER_H

#include "GeneratorObjects/HepMcParticleLink.h"

#include <string>

/** @namespace McEventCollectionHelper McEventCollectionHelper.h "McEventCollectionHelper.h"
 *
 *  Helper functions for mapping pileUpTypes onto McEventCollection names
 */

namespace McEventCollectionHelper {
  EBC_EVCOLL getMcEventCollectionHMPLEnumFromPileUpType(const int pileupType) ;
  std::string getMcEventCollectionNameFromPileUpType(const int pileupType) ;
}

#endif //MCTRUTHSIMALGS_MCEVENTCOLLECTIONHELPER_H

