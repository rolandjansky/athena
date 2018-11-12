/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLETRUTH_TRACKPARTICLETRUTHVECTOR_H
#define PARTICLETRUTH_TRACKPARTICLETRUTHVECTOR_H

#include "ParticleTruth/TrackParticleTruth.h"
#include "Particle/TrackParticleContainer.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <map>

class TrackParticleTruthVector : public std::vector<std::pair< ElementLink< Rec::TrackParticleContainer >, TrackParticleTruth> > 
{
 public:
  TrackParticleTruthVector();
  virtual ~TrackParticleTruthVector();
};

CLASS_DEF(TrackParticleTruthVector, 220964299, 1)

#endif // PARTICLETRUTH_TRACKPARTICLETRUTHVECTOR_H
