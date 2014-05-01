/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLE_PARTICLEDICT_H
#define PARTICLE_PARTICLEDICT_H

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "Navigation/Navigable.h"

//#include <utility>
#include <vector>

namespace ParticleDict{

  struct temp {
    std::vector<Rec::TrackParticle*> m_1;        // dict warning otherwise ... god knows why!
    std::vector<const Rec::TrackParticle*> m_1c;
    NavigableIterator<Rec::TrackParticleContainer,double,std::vector<std::pair<ElementLink<Rec::TrackParticleContainer>,double> > > m_ni;
  };

}

#endif
