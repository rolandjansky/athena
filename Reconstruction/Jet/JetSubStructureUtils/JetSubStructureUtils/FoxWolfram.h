/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremoments_foxwolfram_header
#define jetsubstructuremoments_foxwolfram_header

#include "JetSubStructureUtils/SubstructureCalculator.h"

namespace JetSubStructureUtils {
  class FoxWolfram : public SubstructureCalculator<std::map<std::string, double> > { 
    public:
      using SubstructureCalculator::result;
      virtual std::map<std::string, double> result(const fastjet::PseudoJet &jet) const;
  };
}

#endif
