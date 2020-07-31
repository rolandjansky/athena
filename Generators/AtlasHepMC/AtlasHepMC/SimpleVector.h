/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_SIMPLEVECTOR_H
#define ATLASHEPMC_SIMPLEVECTOR_H
#ifdef HEPMC3
#include "HepMC3/FourVector.h"
#include "HepMC3/PrintStreams.h"
namespace HepMC
{
typedef HepMC3::FourVector FourVector;
}
#else
#include "HepMC/SimpleVector.h"
#endif
#endif
