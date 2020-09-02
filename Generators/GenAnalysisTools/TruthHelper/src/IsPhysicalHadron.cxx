/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsPhysicalHadron.h"
#include "TruthUtils/HepMCHelpers.h"

namespace TruthHelper {


  bool IsPhysicalHadron::operator()(HepMC::ConstGenParticlePtr p ) const {
    return MC::isPhysicalHadron(p);
  }


  GenIMCselector* IsPhysicalHadron::create() const {
    return new IsPhysicalHadron(*this);
  }


}
