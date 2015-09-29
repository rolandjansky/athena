/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_subjetfinder_header
#define jetsubstructureutils_subjetfinder_header

#include "JetSubStructureUtils/SubstructureCalculator.h"
#include <vector>
#include "fastjet/JetDefinition.hh"

namespace JetSubStructureUtils {
  class SubjetFinder : public SubstructureCalculator<std::vector<fastjet::PseudoJet> > { 
    public:
      SubjetFinder(fastjet::JetAlgorithm fj_jetalg = fastjet::kt_algorithm, float jet_radius = 0.3, float pt_min = 5000, int exclusive_njets = -1);

      using SubstructureCalculator::result;
      virtual std::vector<fastjet::PseudoJet> result(const fastjet::PseudoJet &jet) const;

    private:
      fastjet::JetAlgorithm m_fj_jetalg;
      float m_jetrad;
      float m_ptmin;
      int m_exclusivenjets;
  };
}

#endif
