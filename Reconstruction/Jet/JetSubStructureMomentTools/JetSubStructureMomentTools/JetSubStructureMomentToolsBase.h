/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_jetsubstructurebase_header
#define jetsubstructuremomenttools_jetsubstructurebase_header

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "JetRec/JetModifierBase.h"

namespace fastjet {
  class PseudoJet;
}

class JetSubStructureMomentToolsBase :
  public JetModifierBase {

    public:

      // Constructor and destructor
      JetSubStructureMomentToolsBase(std::string name);

      StatusCode initialize();

    protected:

      std::string m_inputContainer;
      std::string m_prefix;

      bool SetupDecoration(fastjet::PseudoJet& pseudojet,const  xAOD::Jet& jet) const;
      bool checkForConstituents(const xAOD::Jet &jet) const {
        if(jet.numConstituents() == 0) {
          ATH_MSG_WARNING("Attempting to use a substructure tool on a jet that has no constituent");
          return false;
        } else {
          return true;
        }
      }
      fastjet::PseudoJet buildPseudoJet(const xAOD::Jet & jet) const;
      fastjet::PseudoJet buildPseudoJet(const std::vector<const xAOD::IParticle*>& iparticles) const;
      
      // Print all configurable parameters
      virtual void print() const;

  };

#endif
