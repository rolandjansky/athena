/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_GENIMCSELECTOR_H
#define TRUTHHELPER_GENIMCSELECTOR_H

#include "AtlasHepMC/GenParticle.h"
#include <functional>

namespace TruthHelper {


  // Interface/base class for HepMC::GenParticle selectors
  /// @deprecated Use the functions in TruthUtils instead
  class GenIMCselector {
  public:

    // Virtual destructor to allow inheritance
    virtual ~GenIMCselector() = default;

    // Allow selector objects to be copied (to be implemented).
    virtual GenIMCselector* create() const = 0;

    // Selection function on GenParticle pointer (to be implemented).
    virtual bool operator()(HepMC::ConstGenParticlePtr p) const = 0;

#ifdef HEPMC3
    //So far this function is not needed for HepMC3.
    //This form of ifdef is kept for convenience.
#else
    // Selection function on GenParticle reference.
    bool operator()(HepMC::GenParticle& p) const {
      return this->operator()(&p);
    }
#endif

  };


}


#endif
