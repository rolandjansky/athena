/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTracking/ParticleCaloExtension.h"

namespace xAOD {

  ParticleCaloExtension::ParticleCaloExtension( float charge, std::vector<std::vector<float> >&& parameters, 
                                                std::vector<std::vector<float> >&& parametersCovariance, 
                                                std::vector<int>&& identifiers ) :
    m_charge(charge), m_parameters(parameters),m_parametersCovariance(parametersCovariance),m_identifiers(identifiers), m_cellsAreSet(false) 
  {
  }

  ParticleCaloExtension::~ParticleCaloExtension() {
    // disable unsused field warning with clang.  ideally we'd remove
    // the member, but I'm not sure if that breaks xAOD streaming
    (void) m_charge;
  }
}
