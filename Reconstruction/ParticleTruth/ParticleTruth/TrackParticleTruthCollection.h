/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLETRUTH_TRKPARTICLETRUTHCOLLECTION_H
#define PARTICLETRUTH_TRKPARTICLETRUTHCOLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "Particle/TrackParticleContainer.h"
#include "ParticleTruth/TrackParticleTruthKey.h"
#include "DataModel/DataLink.h"
#include <map>

class TrackParticleTruthCollectionAccessor;

class TrackParticleTruthCollection : public std::map< Rec::TrackParticleTruthKey, TrackParticleTruth>
{
  friend class TrackParticleTruthCollectionAccessor; // in TrackParticleTruthTPCnv
  DataLink<Rec::TrackParticleContainer> m_trackParticleContainer;
 
 public:
  TrackParticleTruthCollection() {}; // pool
  virtual ~TrackParticleTruthCollection() {};

  DataLink<Rec::TrackParticleContainer> trackParticleContainerLink() const { return m_trackParticleContainer; }
  TrackParticleTruthCollection(const DataLink<Rec::TrackParticleContainer>& trackparticles) : m_trackParticleContainer(trackparticles) {}
};

// AE: newly generated clid
CLASS_DEF( TrackParticleTruthCollection, 1314707306 , 1 )

#endif // PARTICLE_TRKPARTICLETRUTHCOLLECTION_H







