/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTau/TauxAODHelpers.h"
#include "xAODTau/TauJet.h"

typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;

// ==================================================================
const xAOD::TruthParticle* xAOD::TauHelpers::getTruthParticle(const xAOD::IParticle* particle, bool debug)
{
  return getLink<xAOD::TruthParticle>(particle, "truthParticleLink", debug);
}
