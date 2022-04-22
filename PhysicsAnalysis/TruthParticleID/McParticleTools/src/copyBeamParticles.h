// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file McParticleTools/src/copyBeamParticles.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2022
 * @brief Helper to copy beam particle information.
 */


#ifndef MCPARTICLETOOLS_COPYBEAMPARTICLES_H
#define MCPARTICLETOOLS_COPYBEAMPARTICLES_H


#include "AtlasHepMC/GenEvent.h"


namespace TruthHelper {


// Copy beam particle information from inEvt to outEvt (HepMC2 only).
// Can't be done in copyemptyGenEvent because the beam particles
// must have already been added to outEvt.
void copyBeamParticles (const HepMC::GenEvent& inEvt,
                        HepMC::GenEvent& outEvt);


} // namespace TruthHelper


#endif // not MCPARTICLETOOLS_COPYBEAMPARTICLES_H
