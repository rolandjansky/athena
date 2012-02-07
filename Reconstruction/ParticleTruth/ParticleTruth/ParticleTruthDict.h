/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLETRUTH_PARTICLETRUTHDICT_H
#define PARTICLETRUTH_PARTICLETRUTHDICT_H

#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "ParticleTruth/TrackParticleTruthCollectionContainer.h"
#include "ParticleTruth/TrackParticleTruthKey.h"

#include <utility>
#include <vector>

namespace ParticleTruthDict {

  struct temp {
    std::pair<Rec::TrackParticleTruthKey, TrackParticleTruth> m_3;
    TrackParticleTruthCollectionContainer aaa;
  };

}

#endif
