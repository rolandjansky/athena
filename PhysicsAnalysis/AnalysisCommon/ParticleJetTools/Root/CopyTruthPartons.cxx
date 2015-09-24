/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/CopyTruthPartons.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TruthUtils/PIDHelpers.h"

using namespace std;


bool CopyTruthPartons::classify(const xAOD::TruthParticle* tp) const {
    return MC::PID::isParton(tp->pdgId());
}
