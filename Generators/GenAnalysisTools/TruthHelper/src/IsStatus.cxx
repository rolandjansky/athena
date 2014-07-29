/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsStatus.h"
#include "HepMC/GenParticle.h"

namespace TruthHelper {


  bool IsStatus::operator()(const HepMC::GenParticle* const p ) const {
    return p->status() == m_stat;
  }


  GenIMCselector* IsStatus::create() const {
    return new IsStatus(*this);
  }


}
