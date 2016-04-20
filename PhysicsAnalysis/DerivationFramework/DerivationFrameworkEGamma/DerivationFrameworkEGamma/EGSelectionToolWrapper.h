/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGSelectionToolWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGSELECTIONTOOLWRAPPER_H
#define DERIVATIONFRAMEWORK_EGSELECTIONTOOLWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "ElectronPhotonSelectorTools/IAsgEGammaIsEMSelector.h"
//#include "ElectronPhotonSelectorTools/IAsgPhotonIsEMSelector.h"
//#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonShowerShapeFudgeTool/IElectronPhotonShowerShapeFudgeTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "AsgTools/IAsgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class EGSelectionToolWrapper : public AthAlgTool, public IAugmentationTool {
    public: 
      EGSelectionToolWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      //ToolHandle<IAsgSelectionTool> m_tool;
      ToolHandle<IAsgEGammaIsEMSelector> m_tool; // provides isemValue, but will not work with likelihood..
      ToolHandle<IElectronPhotonShowerShapeFudgeTool> m_fudgeMCTool;
      std::string m_cut;
      std::string m_sgName;
      std::string m_containerName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_EGSELECTIONTOOLWRAPPER_H
