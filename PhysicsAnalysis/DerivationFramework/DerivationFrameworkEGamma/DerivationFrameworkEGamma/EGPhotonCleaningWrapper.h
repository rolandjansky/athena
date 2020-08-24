/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGPhotonCleaningWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGPHOTONCLEANINGWRAPPER_H
#define DERIVATIONFRAMEWORK_EGPHOTONCLEANINGWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class EGPhotonCleaningWrapper : public AthAlgTool, public IAugmentationTool {
    public: 
      EGPhotonCleaningWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      ToolHandle<IElectronPhotonShowerShapeFudgeTool> m_fudgeMCTool;
      std::string m_sgName;
      std::string m_containerName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_EGSELECTIONTOOLWRAPPER_H
