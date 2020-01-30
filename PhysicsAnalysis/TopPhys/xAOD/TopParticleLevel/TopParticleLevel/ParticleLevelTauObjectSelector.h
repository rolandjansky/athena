/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef _TOP_PARTICLE_LEVEL_TAUOBJECTSELECTOR_H_
#define _TOP_PARTICLE_LEVEL_TAUOBJECTSELECTOR_H_

#include "TopParticleLevel/ObjectSelectorBase.h"

#include "xAODTruth/TruthParticleContainer.h"

namespace top {
  class ParticleLevelTauObjectSelector
    : public ObjectSelectorBase<xAOD::TruthParticle> {
  public:
    // Struct used to store the cut values used by the object selector. It
    // is important that the object has a sensible default config because it
    // will be default constructed for the trivial constructor of the object
    // selector.
    struct Options {
      Options(double ptMin = 25.e3,
              double etaMax = 2.5) :
        pt_min(ptMin),
        eta_max(etaMax) {}
      double pt_min;
      double eta_max;
    };
  public:
    ParticleLevelTauObjectSelector(Options opt = Options());
    virtual bool apply(const xAOD::TruthParticle& truthParticle);
  private:
    Options m_opt;
  };
}

#endif /* _TOP_PARTICLE_LEVEL_TAUOBJECTSELECTOR_H_ */
