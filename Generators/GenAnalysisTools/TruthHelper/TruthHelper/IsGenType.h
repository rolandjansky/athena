/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISGENTYPE_H
#define TRUTHHELPER_ISGENTYPE_H

#include "TruthHelper/GenIMCselector.h"
#include <vector>

namespace TruthHelper {


  /// @brief Identify particles of specified PDG ID
  /// @deprecated Use the functions in TruthUtils instead
  class IsGenType: public GenIMCselector {
  public:

    IsGenType(int ParticleType);

    IsGenType(std::vector<int> ParticleTypeList);

    IsGenType(const IsGenType& src);

    IsGenType& operator= (const IsGenType& rhs);

    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr p ) const;


  private:

    std::vector<int> m_TypeList;

  };


}

#endif
