/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremoments_substructurecalculator_header
#define jetsubstructuremoments_substructurecalculator_header

#include <vector>
#include <map>
#include <string>
#include "xAODJet/Jet.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/FunctionOfPseudoJet.hh"
#include "JetEDM/JetConstituentFiller.h"

namespace JetSubStructureUtils {
  template<typename TOut>
  class SubstructureCalculator :
    public fastjet::FunctionOfPseudoJet<TOut>
  { 
    public:
      using fastjet::FunctionOfPseudoJet<TOut>::result;

      virtual TOut result(const xAOD::Jet &jet) const {
        std::vector<fastjet::PseudoJet> constit_pseudojets =
          jet::JetConstituentFiller::constituentPseudoJets(jet);
        fastjet::PseudoJet pjet = fastjet::join(constit_pseudojets);

        return result(pjet);
      }
  };
}

#endif
