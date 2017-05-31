////////////////////-*- C++ -*-////////////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PFlowAugmentationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_PFLOWAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_PFLOWAUGMENTATIONTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

#include <xAODPFlow/PFOContainer.h>
#include <PFlowUtils/IWeightPFOTool.h>

namespace DerivationFramework {

  class PFlowAugmentationTool : public AthAlgTool, public IAugmentationTool {
  public: 
    PFlowAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);

    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

  private:

    float m_z0sinthcut;

    ToolHandle<CP::IWeightPFOTool> m_weightPFOTool;    /// Retrieval tool
    bool m_useChargedWeights; //If true, them we make use of the charged PFO weighting scheme
    
  }; 
}

#endif // DERIVATIONFRAMEWORK_PFLOWAUGMENTATIONTOOL_H
