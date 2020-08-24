/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_HEPEVTWRAPPER_H
#define ATLASHEPMC_HEPEVTWRAPPER_H
#ifdef HEPMC3
#include "HepMC3/HEPEVT_Wrapper.h"
namespace HepMC
{
typedef HepMC3::HEPEVT_Wrapper HEPEVT_Wrapper;
}
#else
#include "HepMC/HEPEVT_Wrapper.h"
#endif
#endif
