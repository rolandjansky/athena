/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleHelper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "ISF_Event/ParticleHelper.h"

// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/Flow.h"
#include "AtlasHepMC/SimpleVector.h" // HepMC::FourVector

// ISF includes
#include "ISF_Event/ISFParticle.h"

HepMC::GenParticlePtr ISF::ParticleHelper::convert( const ISF::ISFParticle &particle) {

  const Amg::Vector3D &mom = particle.momentum();
  double mass = particle.mass();
  double energy = sqrt( mom.mag2() + mass*mass);
  HepMC::FourVector fourMomentum( mom.x(), mom.y(), mom.z(), energy);
  int status = 1; // stable particle not decayed by EventGenerator

  auto hepParticle = HepMC::newGenParticlePtr( fourMomentum, particle.pdgCode(), status );
  HepMC::suggest_barcode(hepParticle, particle.barcode() );

  // return a newly created GenParticle
  return hepParticle;
}

