/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremoments_substructurecalculator_header
#define jetsubstructuremoments_substructurecalculator_header

#include "xAODJet/Jet.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/FunctionOfPseudoJet.hh"
// #include "JetEDM/JetConstituentFiller.h"

#include <vector>
#include <algorithm>
#include <iostream>

namespace JetSubStructureUtils {
  template<typename TOut>
  class SubstructureCalculator :
    public fastjet::FunctionOfPseudoJet<TOut>
  { 
    public:
      using fastjet::FunctionOfPseudoJet<TOut>::result;

      virtual TOut result(const xAOD::Jet &jet) const {
        // PS 4/18 master developent
        // std::vector<fastjet::PseudoJet> constit_pseudojets =
        //   jet::JetConstituentFiller::constituentPseudoJets(jet);

        std::vector<fastjet::PseudoJet> constit_pseudojets;
        std::transform(jet.getConstituents().begin(),
                       jet.getConstituents().end(),
                       std::back_inserter(constit_pseudojets),
                       [](const auto& c){
                         const xAOD::IParticle* ip = c->rawConstituent();
                         return
                           // fastjet::PseudoJet((c->rawConstituent())->p4());
                           fastjet::PseudoJet(ip->p4());
                       });

        fastjet::PseudoJet pjet = fastjet::join(constit_pseudojets);

        return result(pjet);
      }
  };
}

#endif
