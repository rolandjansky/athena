/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_dipolarity_header
#define jetsubstructureutils_dipolarity_header

#include "JetSubStructureUtils/SubstructureCalculator.h"
#include <vector>

namespace JetSubStructureUtils {
  class Dipolarity : public SubstructureCalculator<double> { 
    public:
      Dipolarity(unsigned int n1, unsigned int n2, bool exclusive, double subjetR);

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const;

    private:
      double dipolarity(std::vector<fastjet::PseudoJet> &constit_pseudojets,
                        fastjet::PseudoJet jet1, fastjet::PseudoJet jet2) const;

      bool m_exclusive;
      unsigned int m_n1;
      unsigned int m_n2;
      double m_subjetR;
  };
}

#endif
