/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_ktmassdrop_header
#define jetsubstructureutils_ktmassdrop_header

#include "JetSubStructureUtils/SubstructureCalculator.h"

namespace JetSubStructureUtils {
  class KtMassDrop : public SubstructureCalculator<double> { 
    public:
      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const;
  };
}

#endif
