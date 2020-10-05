/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_GENPARTICLEFWD_H
#define ATLASHEPMC_GENPARTICLEFWD_H
#ifdef HEPMC3
#include "HepMC3/GenParticle_fwd.h"
namespace HepMC {
typedef HepMC3::GenParticlePtr GenParticlePtr;
typedef HepMC3::ConstGenParticlePtr ConstGenParticlePtr;
}
#else
namespace HepMC {
class GenParticle;
typedef GenParticle* GenParticlePtr;
typedef const GenParticle* ConstGenParticlePtr;
}
#endif
#endif
