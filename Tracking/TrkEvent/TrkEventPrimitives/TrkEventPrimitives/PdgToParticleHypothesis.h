/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PdgToParticleHypothesis.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_PDGTOPARTICLEHYPOTHESIS_H
#define TRKEVENTPRIMITIVES_PDGTOPARTICLEHYPOTHESIS_H

// Fatras
#include "TrkEventPrimitives/ParticleHypothesis.h"
// STD
#include <cmath>

namespace Trk {

/** @class PdgToParticleHypothesis

small converter from the (abs) PDG code
to the particle hypothsis used in Tracking

@author Andreas.Salzburger@cern.ch

**/

class PdgToParticleHypothesis
{

public:
  /** Constructor */
  PdgToParticleHypothesis() = default;

  /** Converter method : PDG -> Particle Hyptothesis */
  Trk::ParticleHypothesis convert(int pdg,
                                  bool& stable,
                                  bool& exiting,
                                  double charge = 1.) const;

  /** Converter method : PDG -> Particle Hyptothesis , w/o stable exiting*/
  Trk::ParticleHypothesis convert(int pdg, double charge = 1.) const;

  /** Converter method : Particle Hyptothesis -> PDG*/
  int convert(Trk::ParticleHypothesis particleHypo,
              double charge,
              bool dist = true) const;
};
}

#include "TrkEventPrimitives/PdgToParticleHypothesis.icc"
#endif
