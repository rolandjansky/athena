/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_energycorrelator_header
#define jetsubstructureutils_energycorrelator_header

#include "JetSubStructureUtils/SubstructureCalculator.h"
#include "fastjet/contrib/EnergyCorrelator.hh"

namespace JetSubStructureUtils {

  class EnergyCorrelator : public SubstructureCalculator<double>,
                           public fastjet::contrib::EnergyCorrelator { 
    public:
      EnergyCorrelator(int N, double beta, Measure measure = pt_R, Strategy strategy = storage_array) :
        fastjet::contrib::EnergyCorrelator(N, beta, measure, strategy) {};

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const {
        return fastjet::contrib::EnergyCorrelator::result(jet);
      };

      virtual std::string description() const{
        return fastjet::contrib::EnergyCorrelator::description();
      };
  };
}

#endif
