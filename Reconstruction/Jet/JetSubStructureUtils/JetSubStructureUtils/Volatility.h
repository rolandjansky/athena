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
        _num_iterations(num_iterations),
        _zcut(zcut), _dcut_fctr(dcut_fctr), _exp_min(exp_min), _exp_max(exp_max),
        _rigidity(rigidity), _truncation_fctr(truncation_fctr),
        _seed(-1)
        {};

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const;

      void setSeed(unsigned int seed);

    private:
      unsigned int _num_iterations;
      double _zcut, _dcut_fctr, _exp_min, _exp_max, _rigidity, _truncation_fctr;
      int _seed;
  };
}

#endif
