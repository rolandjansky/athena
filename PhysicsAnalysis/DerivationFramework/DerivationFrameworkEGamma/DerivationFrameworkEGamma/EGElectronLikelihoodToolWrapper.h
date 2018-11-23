/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGLikelihoodToolWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
// Note: while EGSelectionToolWrapper permits to only store the boolean 
// accept and the isEM-like mask (works for both isEM and likelihood selectors), 
// this tool (EGElectronLikelihoodToolWrapper) allows also to store
// the double TResult output (i.e. the value of the likelihood or the ECIDS BDT)
// if StoreTResult is set to true. Otherwise one can simply use the other tool.


#ifndef DERIVATIONFRAMEWORK_EGELECTRONLIKELIHOODTOOLWRAPPER_H
#define DERIVATIONFRAMEWORK_EGELECTRONLIKELIHOODTOOLWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "AsgTools/IAsgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class EGElectronLikelihoodToolWrapper : public AthAlgTool, public IAugmentationTool {
    public: 
      EGElectronLikelihoodToolWrapper(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      ToolHandle<IAsgElectronLikelihoodTool> m_tool;
      ToolHandle<IElectronPhotonShowerShapeFudgeTool> m_fudgeMCTool;
      std::string m_cut;
      std::string m_sgName;
      std::string m_containerName;
      bool m_storeTResult;
  }; 
}

#endif // DERIVATIONFRAMEWORK_EGELECTRONLIKELIHOODTOOLWRAPPER_H
