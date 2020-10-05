/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsGenType.h"
#include "AtlasHepMC/GenParticle.h"
#include <vector>

namespace TruthHelper {


  IsGenType::IsGenType(int ptype) {
    m_TypeList.push_back(ptype);
  }


  IsGenType::IsGenType(std::vector<int> ptypes) {
    for (std::vector<int>::const_iterator itype = ptypes.begin(); itype != ptypes.end(); ++itype) {
      m_TypeList.push_back((*itype));
    }
  }


  IsGenType::IsGenType(const IsGenType& rhs)
    : GenIMCselector(), m_TypeList(rhs.m_TypeList)
  { }

 IsGenType& IsGenType::operator=( const IsGenType& rhs){
      m_TypeList = rhs.m_TypeList;
      return *this;
}


  bool IsGenType::operator()(const HepMC::ConstGenParticlePtr p ) const {
    for (std::vector<int>::const_iterator itype = m_TypeList.begin(); itype != m_TypeList.end(); ++itype) {
      if (std::abs(p->pdg_id()) == (*itype)) return true;
    }
    return false;
  }


  GenIMCselector* IsGenType::create() const {
    return new IsGenType(*this);
  }


}
