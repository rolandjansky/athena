/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsLastPhysicalHadron.h"
#include "AtlasHepMC/GenParticle.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsPhysicalHadron.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"

namespace TruthHelper {


  bool IsLastPhysicalHadron::operator()(HepMC::ConstGenParticlePtr  p ) const {
    return IsGenStable()(p) && IsPhysicalHadron()(p);
  }


  GenIMCselector* IsLastPhysicalHadron::create() const {
    return new IsLastPhysicalHadron(*this);
  }


}
