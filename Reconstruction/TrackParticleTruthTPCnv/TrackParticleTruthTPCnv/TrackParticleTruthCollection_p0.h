/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTION_P0_H
#define TRACKPARTICLETRUTHCOLLECTION_P0_H

#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "ParticleTruth/TrackParticleTruth.h"

#include <vector>
#include <utility>

class TrackParticleTruthVector : 
  public std::vector<std::pair< ElementLink< Rec::TrackParticleContainer >, TrackParticleTruth> > 
{};

#endif/*TRACKPARTICLETRUTHCOLLECTION_P0_H*/
