/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TruthLinkRepointTool_H
#define DERIVATIONFRAMEWORK_TruthLinkRepointTool_H

// Interface classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// Types for functions. Note these are typedefs, so can't forward reference.
#include "xAODTruth/TruthParticleContainer.h"

// STL includes
#include <string>
#include <vector>

namespace DerivationFramework {

  class TruthLinkRepointTool : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthLinkRepointTool(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthLinkRepointTool();
      virtual StatusCode addBranches() const override final;

    private:
      /// Parameter: input collection key
      std::string m_recoKey;
      /// Parameter: output decoration
      std::string m_decOutput;
      /// Parameter: target collection
      std::vector<std::string> m_targetKeys;
      // Helper function for finding matching truth particle and warning consistently
      int find_match(const xAOD::TruthParticle* p, const DataHandle<xAOD::TruthParticleContainer> & c) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TruthLinkRepointTool_H
