/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopParticleLevel/ParticleLevelTauObjectSelector.h"

#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "TopParticleLevel/TruthTools.h"

namespace top {
  ParticleLevelTauObjectSelector::ParticleLevelTauObjectSelector(Options opt /* = Options() */)
    : m_opt(opt) { /* Deliberately Empty */}

  bool ParticleLevelTauObjectSelector::apply(const xAOD::TruthParticle& truthParticle) {
    // Obtain the particle type and particle origin from the decoration
    // created by the MCTruthClassifier. For legacy reasons, try both
    // decoration names.
    unsigned int type;

    if (truthParticle.isAvailable<unsigned int>("particleType")) {
      type = truthParticle.auxdata<unsigned int>("particleType");
    } else if (truthParticle.isAvailable<unsigned int>("classifierParticleType")) {
      type = truthParticle.auxdata<unsigned int>("classifierParticleType");
    } else {
      throw std::runtime_error {
              "Could not obtain MCTruthClassifier result decoration."
      };
    }

    // --------------------------------------------------
    // Accept muons iff they do not come from a hadron
    using namespace MCTruthPartClassifier;
    if (type != ParticleType::IsoTau) {
      return false;
    }

    // --------------------------------------------------
    // Apply kinematic cut on the dressed pT:
    //     must exceed 25 GeV
    if (truthParticle.pt() < m_opt.pt_min) {
      return false;
    }

    // --------------------------------------------------
    // Apply kinematic cut on the dressed eta:
    //     must be less than or equal to 2.5
    if (std::fabs(truthParticle.eta()) > m_opt.eta_max) {
      return false;
    }

    // --------------------------------------------------
    // Everything that reaches this point has passed the selection
    return true;
  }
}
