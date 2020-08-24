/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsGenInteracting.h"
#include "TruthUtils/HepMCHelpers.h"

namespace TruthHelper {


  bool IsGenInteracting::operator()(HepMC::ConstGenParticlePtr p ) const {
    return MC::isSimInteracting(p);
  }


  GenIMCselector* IsGenInteracting::create() const {
    return new IsGenInteracting(*this);
  }


}
