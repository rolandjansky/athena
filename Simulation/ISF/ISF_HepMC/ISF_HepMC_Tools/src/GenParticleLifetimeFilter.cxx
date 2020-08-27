/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleLifetimeFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenParticleLifetimeFilter.h"

// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"

/** Constructor **/
ISF::GenParticleLifetimeFilter::GenParticleLifetimeFilter( const std::string& t,
                                                           const std::string& n,
                                                           const IInterface* p )
  : base_class(t,n,p)
{
    declareProperty("MinimumLifetime", m_minLifetime, "");
}


/** does the given particle pass the filter? */
bool ISF::GenParticleLifetimeFilter::pass(const HepMC::GenParticle& particle) const
{
  // the GenParticle end vertex
  auto endVtx = particle.end_vertex();
  // no production vertex?
  if (!endVtx) {
    ATH_MSG_DEBUG("GenParticle does not have an end vertex, this is fine");
    return true;
  }
  // (x,y,z) end position
  const auto& end4Vec = endVtx->position();

  // the GenParticle production vertex
  auto  prodVtx = particle.production_vertex();
  // no production vertex?
  if (!prodVtx) {
    ATH_MSG_DEBUG("GenParticle does not have a production vertex, filtering it out");
    return false;
  }
  // (x,y,z) production position
  const auto& prod4Vec = prodVtx->position();
  const CLHEP::HepLorentzVector lv0 ( prod4Vec.x(), prod4Vec.y(), prod4Vec.z(), prod4Vec.t() );
  const CLHEP::HepLorentzVector lv1 ( end4Vec.x(), end4Vec.y(), end4Vec.z(), end4Vec.t() );
  return ((lv1-lv0).mag()>m_minLifetime);
}
