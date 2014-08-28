/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_ktsplittingscale_header
#define jetsubstructureutils_ktsplittingscale_header

#include "JetSubStructureUtils/SubstructureCalculator.h"

namespace JetSubStructureUtils {
  class KtSplittingScale : public SubstructureCalculator<double> { 
    public:
      KtSplittingScale(unsigned int nSubJets) : m_nSubJets(nSubJets) {};

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const;

    private:
      unsigned int m_nSubJets;
  };
}

#endif
