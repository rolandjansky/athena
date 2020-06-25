/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsGenNonInteracting.h"
#include "TruthUtils/HepMCHelpers.h"

namespace TruthHelper {


  bool IsGenNonInteracting::operator()(const HepMC::ConstGenParticlePtr p ) const {
    return ! MC::isSimInteracting(p);
  }


  GenIMCselector* IsGenNonInteracting::create() const {
    return new IsGenNonInteracting(*this);
  }


}
