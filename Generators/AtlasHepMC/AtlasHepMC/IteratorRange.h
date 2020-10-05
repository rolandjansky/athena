/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_ITERATORRANGE_H
#define ATLASHEPMC_ITERATORRANGE_H
#ifdef HEPMC3
namespace HepMC {
enum IteratorRange { parents, children, family,
                     ancestors, descendants, relatives
                   };
}
#else
#include "HepMC/IteratorRange.h"
#endif
#endif
