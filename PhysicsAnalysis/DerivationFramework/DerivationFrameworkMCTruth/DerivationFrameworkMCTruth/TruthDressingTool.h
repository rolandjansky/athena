/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHDRESSINGTOOL_H
#define DERIVATIONFRAMEWORK_TRUTHDRESSINGTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class TruthDressingTool : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthDressingTool(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthDressingTool();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      /// Parameter: input collection key
      std::string m_particlesKey;
      /// Parameter: collection key of particles to be dressed
      std::string m_dressParticlesKey;
      /// Parameter: Use photons from hadron decays?
      bool m_usePhotonsFromHadrons;
      /// Parameter: Cone size for dressing
      float m_coneSize;
      /// Parameter: List of pdgIDs of particles to dress
      std::vector<int> m_listOfPIDs;
      /// Parameter: Use antikT algorithm for dressing?
      bool m_useAntiKt;
      /// Parameter: Name of the decoration to apply
      std::string m_decorationName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHDRESSINGTool_H
