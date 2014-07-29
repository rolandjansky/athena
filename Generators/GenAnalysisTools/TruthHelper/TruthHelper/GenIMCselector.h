/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_GENIMCSELECTOR_H
#define TRUTHHELPER_GENIMCSELECTOR_H

#include "HepMC/GenParticle.h"
#include <functional>

namespace TruthHelper {


  // Interface/base class for HepMC::GenParticle selectors
  /// @deprecated Use the functions in TruthUtils instead
  class GenIMCselector : public std::unary_function<HepMC::GenParticle*, bool> {
  public:

    // Virtual destructor to allow inheritance
    virtual ~GenIMCselector() { }

    // Allow selector objects to be copied (to be implemented).
    virtual GenIMCselector* create() const = 0;

    // Selection function on GenParticle pointer (to be implemented).
    virtual bool operator()(const HepMC::GenParticle* p) const = 0;

    // Selection function on GenParticle reference.
    bool operator()(const HepMC::GenParticle& p) const {
      return this->operator()(&p);
    }

  };


}


#endif
