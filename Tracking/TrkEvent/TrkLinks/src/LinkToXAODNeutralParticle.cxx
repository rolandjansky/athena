/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkLinks/LinkToXAODNeutralParticle.h"
#include "xAODTracking/NeutralParticleContainer.h"
#include "xAODTracking/NeutralParticle.h"

namespace Trk {
LinkToXAODNeutralParticle::LinkToXAODNeutralParticle()
  : ElementLink<xAOD::NeutralParticleContainer>()
{}

LinkToXAODNeutralParticle::LinkToXAODNeutralParticle(
  ElementLink<xAOD::NeutralParticleContainer>& link)
  : ElementLink<xAOD::NeutralParticleContainer>(link)
{}

const NeutralParameters*
LinkToXAODNeutralParticle::neutralParameters() const
{
  if (isValid()) {
    const xAOD::NeutralParticle* neut = this->cachedElement();
    if (nullptr != neut) {
      return &(neut->perigeeParameters());
    }
    return nullptr;
  }
  return nullptr;
} // end of parameters method

Trk::LinkToXAODNeutralParticle*
Trk::LinkToXAODNeutralParticle::clone() const
{
  return new LinkToXAODNeutralParticle(*this);
}

} // end of namespace definitions
