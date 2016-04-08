/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HepMC_TruthBinding.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENTBASE_HEPMC_TRUTHBINDING_H
#define ISF_EVENTBASE_HEPMC_TRUTHBINDING_H

// ISF framework includes
#include "ISF_Event/ITruthBinding.h"
// HepMC includes
#include "HepMC/GenParticle.h"

namespace ISF {

  typedef TruthBindingT<HepMC::GenParticle> HepMC_TruthBinding;

} // end of namespace

#endif // ISF_EVENTBASE_HEPMC_TRUTHBINDING_H

