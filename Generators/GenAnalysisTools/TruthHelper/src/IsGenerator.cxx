/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsGenerator.h"
#include "HepMC/GenParticle.h"

namespace TruthHelper {


  bool IsGenerator::operator()(const HepMC::GenParticle* const p ) const {
    return p->barcode() < 200000 &&
      (p->status() < 200 ||
       p->status() % 1000 == 1 || p->status() % 1000 == 2 ||
       p->status() == 10902);
  }


  GenIMCselector* IsGenerator::create() const {
    return new IsGenerator(*this);
  }


}
