/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/HepMC/Flow.h"
#include "CLHEP/HepMC/GenEvent.h"
#include "CLHEP/HepMC/GenEventConvert.h"
#include "CLHEP/HepMC/GenParticle.h"
#include "CLHEP/HepMC/GenVertex.h"
#include "CLHEP/HepMC/Polarization.h"
#include "CLHEP/HepMC/WeightContainer.h"

// functions declared but not defined in 1.8.1.0
inline void HepMC::GenParticle::setParticleID(HepPDT::ParticleID pid) { int i = pid.pid(); ++i; }
