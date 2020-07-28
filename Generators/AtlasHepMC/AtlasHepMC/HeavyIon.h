/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_HEAVYION_H
#define ATLASHEPMC_HEAVYION_H
#ifdef HEPMC3
#include "HepMC3/GenHeavyIon.h"
#include "HepMC3/PrintStreams.h"
namespace HepMC {
typedef HepMC3::GenHeavyIonPtr GenHeavyIonPtr;
typedef HepMC3::GenHeavyIon GenHeavyIon;
}
#else
#include "HepMC/HeavyIon.h"
#endif
#endif
