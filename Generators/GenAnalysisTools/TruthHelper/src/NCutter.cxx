/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/NCutter.h"
#include "AtlasHepMC/GenParticle.h"
#include <vector>

namespace TruthHelper {


  bool NCutter::operator()(HepMC::ConstGenParticlePtr p ) const {
    for (std::vector<GenIMCselector*>::const_iterator i = m_selectors.begin(); i != m_selectors.end(); i++) {
      if ( !(*i)->operator()(p) ) return false;
    }
    return true;
  }
    NCutter& NCutter::operator=(const NCutter& rhs) {
      NCutter tmp(rhs);
      std::swap(m_selectors, tmp.m_selectors);
      return *this;
}


  GenIMCselector* NCutter::create() const {
    return new NCutter(*this);
  }


}
