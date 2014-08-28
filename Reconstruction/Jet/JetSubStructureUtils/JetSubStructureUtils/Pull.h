/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremoments_pull_header
#define jetsubstructuremoments_pull_header

#include "JetSubStructureUtils/SubstructureCalculator.h"

namespace JetSubStructureUtils {
  class Pull : public SubstructureCalculator<std::map<std::string, double> > { 
    public:
      Pull(bool useEtaInsteadOfY = false) : m_useEtaInsteadOfY(useEtaInsteadOfY) {};

      using SubstructureCalculator::result;
      virtual std::map<std::string, double> result(const fastjet::PseudoJet &jet) const;

    private:
      bool m_useEtaInsteadOfY;
  };
}

#endif
