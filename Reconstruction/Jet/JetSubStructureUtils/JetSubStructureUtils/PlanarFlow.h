/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_planarflow_header
#define jetsubstructureutils_planarflow_header

#include "JetSubStructureUtils/SubstructureCalculator.h"

namespace JetSubStructureUtils {
  class PlanarFlow : public SubstructureCalculator<double> { 
    public:
      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const;
  };
}

#endif
