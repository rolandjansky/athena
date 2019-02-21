/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_energycorrelatorgeneralized_header
#define jetsubstructureutils_energycorrelatorgeneralized_header

#include "JetSubStructureUtils/SubstructureCalculator.h"
#include "fastjet/contrib/EnergyCorrelator.hh"

namespace JetSubStructureUtils {

  class EnergyCorrelatorGeneralized : public SubstructureCalculator<double>,
                           public fastjet::contrib::EnergyCorrelatorGeneralized { 
    public:
      EnergyCorrelatorGeneralized(int v_angles, int N, double beta, fastjet::contrib::EnergyCorrelator::Measure measure = fastjet::contrib::EnergyCorrelator::pt_R, fastjet::contrib::EnergyCorrelator::Strategy strategy = fastjet::contrib::EnergyCorrelator::storage_array) :
        fastjet::contrib::EnergyCorrelatorGeneralized(v_angles, N, beta, measure, strategy) {};

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const {
        return fastjet::contrib::EnergyCorrelatorGeneralized::result(jet);
      };

      virtual std::string description() const{
        return fastjet::contrib::EnergyCorrelatorGeneralized::description();
      };
  };
}

#endif
