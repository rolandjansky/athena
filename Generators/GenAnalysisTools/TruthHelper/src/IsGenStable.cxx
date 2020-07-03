/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsGenStable.h"
#include "TruthUtils/HepMCHelpers.h"

namespace TruthHelper {


  bool IsGenStable::operator()(const HepMC::ConstGenParticlePtr p ) const {
    return MC::isGenStable(p);
  }


  GenIMCselector* IsGenStable::create() const {
    return new IsGenStable(*this);
  }


}
