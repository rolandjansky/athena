/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGSelectionToolWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGSELECTIONTOOLWRAPPER_H
#define DERIVATIONFRAMEWORK_EGSELECTIONTOOLWRAPPER_H

#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteHandleKey.h"
//
#include "AsgTools/IAsgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "EgammaAnalysisInterfaces/IAsgEGammaIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEgamma/EgammaContainer.h"
//
#include <string>

namespace DerivationFramework {

class EGSelectionToolWrapper
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  EGSelectionToolWrapper(const std::string& t,
                         const std::string& n,
                         const IInterface* p);

  virtual StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  ToolHandle<IAsgEGammaIsEMSelector> m_tool{
    this,
    "EGammaSelectionTool",
    "",
    "Selector tool",
  };
  ToolHandle<IElectronPhotonShowerShapeFudgeTool>
    m_fudgeMCTool{ this, "EGammaFudgeMCTool", "", "Fudging tool" };

  SG::ReadHandleKey<xAOD::EgammaContainer> m_ContainerName{ this,
                                                            "ContainerN"
                                                            "ame",
                                                            "",
                                                            "Input" };

  // Write decoration handle keys
  // these are not really configuarable
  SG::WriteDecorHandleKey<xAOD::EgammaContainer> m_decoratorPass{};
  SG::WriteDecorHandleKey<xAOD::EgammaContainer> m_decoratorIsEM{};
  Gaudi::Property<std::string> m_cut{ this, "CutType", "", "cut type" };

  Gaudi::Property<std::string> m_sgName{ this,
                                         "StoreGateEntryName",
                                         "",
                                         "Store entry name" };
};
}

#endif // DERIVATIONFRAMEWORK_EGSELECTIONTOOLWRAPPER_H
