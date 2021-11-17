/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MuonTPJpsiVertexFittingAlg__H
#define MuonTPJpsiVertexFittingAlg__H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

// algorithm that calls the jpsi finder and applies BPhys decorations to the vertices
// essentially takes the role of the derivation kernel when running in AOD analysis mode
class MuonTPJpsiVertexFittingAlg : public AthAlgorithm {
public:
    MuonTPJpsiVertexFittingAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~MuonTPJpsiVertexFittingAlg() = default;

    /// Athena algorithm's Hooks
    StatusCode initialize();
    StatusCode execute();

private:
    ToolHandleArray<DerivationFramework::IAugmentationTool> m_augmentation_tools;
};

#endif  // MuonTPJpsiVertexFittingAlg__H
