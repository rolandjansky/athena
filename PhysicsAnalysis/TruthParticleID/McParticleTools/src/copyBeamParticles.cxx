/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file McParticleTools/src/copyBeamParticles.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2022
 * @brief Helper to copy beam particle information.
 */


#include "copyBeamParticles.h"


namespace TruthHelper {


// Copy beam particle information from inEvt to outEvt (HepMC2 only).
// Can't be done in copyemptyGenEvent because the beam particles
// must have already been added to outEvt.
void copyBeamParticles (const HepMC::GenEvent& inEvt [[maybe_unused]],
                        HepMC::GenEvent& outEvt [[maybe_unused]])
{
#ifndef HEPMC3
  if (inEvt.valid_beam_particles()) {
    std::pair<HepMC::GenParticle*,HepMC::GenParticle*> bp = inEvt.beam_particles();
    outEvt.set_beam_particles (outEvt.barcode_to_particle (bp.first->barcode()),
                               outEvt.barcode_to_particle (bp.second->barcode()));
  }
#endif
}


} // namespace TruthHelper
