/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_GENEVENTFWD_H
#define ATLASHEPMC_GENEVENTFWD_H
#ifdef HEPMC3
namespace HepMC3 { class GenEvent; }
namespace HepMC {
using GenEvent=HepMC3::GenEvent;
}
#else
namespace HepMC {
class GenEvent;
}
#endif
#endif
