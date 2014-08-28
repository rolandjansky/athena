/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_zcut_header
#define jetsubstructureutils_zcut_header

#include "JetSubStructureUtils/SubstructureCalculator.h"

namespace JetSubStructureUtils {
  class ZCut : public SubstructureCalculator<double> { 
    public:
      ZCut(unsigned int nSubJets) : m_nSubJets(nSubJets) {};

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const;

    private:
      unsigned int m_nSubJets;
  };
}

#endif
