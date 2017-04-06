/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_volatility_header
#define jetsubstructureutils_volatility_header

#include "JetSubStructureUtils/SubstructureCalculator.h"
#include <vector>

namespace JetSubStructureUtils {
  class Volatility : public SubstructureCalculator<double> { 
    public:
      Volatility(unsigned int num_iterations, double zcut, double dcut_fctr, double exp_min, double exp_max,
          double rigidity, double truncation_fctr = 0.0) :
        m_num_iterations(num_iterations),
        m_zcut(zcut), m_dcut_fctr(dcut_fctr), m_exp_min(exp_min), m_exp_max(exp_max),
        m_rigidity(rigidity), m_truncation_fctr(truncation_fctr),
        m_seed(-1)
        {};

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const;

      void setSeed(unsigned int seed);

    private:
      unsigned int m_num_iterations;
      double m_zcut, m_dcut_fctr, m_exp_min, m_exp_max, m_rigidity, m_truncation_fctr;
      int m_seed;
  };
}

#endif
