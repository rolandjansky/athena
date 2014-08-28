/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_nsubjettiness_header
#define jetsubstructureutils_nsubjettiness_header

#include "JetSubStructureUtils/SubstructureCalculator.h"
#include "fastjet/contrib/Nsubjettiness.hh"

namespace JetSubStructureUtils {

  class Nsubjettiness : public SubstructureCalculator<double>,
                           public fastjet::contrib::Nsubjettiness { 
    public:
      Nsubjettiness(int N, const fastjet::contrib::AxesDefinition& axes_def,
          const fastjet::contrib::MeasureDefinition& measure_def) :
        fastjet::contrib::Nsubjettiness(N, axes_def, measure_def) {};

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const {
        return fastjet::contrib::Nsubjettiness::result(jet);
      };

      virtual std::string description() const{
        return fastjet::contrib::Nsubjettiness::description();
      };
  };
}

#endif
