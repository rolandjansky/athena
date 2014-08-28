/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_ktdeltar_header
#define jetsubstructureutils_ktdeltar_header

#include "JetSubStructureUtils/SubstructureCalculator.h"

namespace JetSubStructureUtils {
  class KtDeltaR : public SubstructureCalculator<double> { 
    public:
      KtDeltaR(double jetrad) : m_jetrad(jetrad) {};

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const;

    private:
      double m_jetrad;
  };
}

#endif
