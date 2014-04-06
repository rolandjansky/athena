/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONACCESSOR_H
#define TRACKPARTICLETRUTHCOLLECTIONACCESSOR_H

#include "ParticleTruth/TrackParticleTruthCollection.h"

// A class to access private data members of TrackParticleTruthCollection from the convertors
class TrackParticleTruthCollectionAccessor {
public:
  static DataLink<Rec::TrackParticleContainer>& trackParticleContainerLink(TrackParticleTruthCollection* truthColl) {
    return truthColl->m_trackParticleContainer;
  }
  static const DataLink<Rec::TrackParticleContainer>& trackParticleContainerLink(const TrackParticleTruthCollection* truthColl) {
    return truthColl->m_trackParticleContainer;
  }
};

#endif/*TRACKPARTICLETRUTHCOLLECTIONACCESSOR_H*/

