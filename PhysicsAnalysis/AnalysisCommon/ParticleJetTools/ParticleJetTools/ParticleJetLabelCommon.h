/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PARTICLE_JET_LABEL_COMMON_H
#define PARTICLE_JET_LABEL_COMMON_H

#include "xAODTruth/TruthParticle.h"
#include "xAODJet/Jet.h"

#include <vector>

namespace ParticleJetTools {

  struct LabelNames {
    std::string singleint;
    std::string doubleint;
    std::string pt;
  };

  struct Particles {
    std::vector<const xAOD::TruthParticle*> b;
    std::vector<const xAOD::TruthParticle*> c;
    std::vector<const xAOD::TruthParticle*> tau;
  };

  void setJetLabels(xAOD::Jet& jet,
                    const Particles& particles,
                    const LabelNames& names);

  void childrenRemoved
  ( const std::vector<const xAOD::TruthParticle*>& parents
    , std::vector<const xAOD::TruthParticle*>& children
    );
}

#endif
