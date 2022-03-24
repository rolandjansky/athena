/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"
//
#include "AsgTools/IAsgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEgamma/EgammaContainer.h"
//
#include <string>

namespace DerivationFramework {

class EGElectronLikelihoodToolWrapper
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  EGElectronLikelihoodToolWrapper(const std::string& t,
                                  const std::string& n,
                                  const IInterface* p);

  virtual StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  ToolHandle<IAsgElectronLikelihoodTool> m_tool{
    this,
    "EGammaElectronLikelihoodTool",
    "",
    "Electron  Likelihood Selector"
  };
  ToolHandle<IElectronPhotonShowerShapeFudgeTool>
    m_fudgeMCTool{ this, "EGammaFudgeMCTool", "", "Fudging tool" };

  SG::ReadHandleKey<xAOD::EgammaContainer> m_ContainerName{ this,
                                                            "ContainerName",
                                                            "",
                                                            "Input" };

  // Write decoration handle keys
  SG::WriteDecorHandleKey<xAOD::EgammaContainer>
    m_decoratorPass{ this, "decoratorPass", "", "" };
  SG::WriteDecorHandleKey<xAOD::EgammaContainer>
    m_decoratorIsEM{ this, "decoratorIsEM", "", "" };
  SG::WriteDecorHandleKey<xAOD::EgammaContainer>
    m_decoratorResult{ this, "decoratorResult", "", "" };
  SG::WriteDecorHandleKeyArray<xAOD::EgammaContainer, float>
    m_decoratorMultipleOutputs{this, "decoratorMultipleOutputs", {}, ""};
  std::string m_cut;
  std::string m_sgName;
  bool m_storeTResult;
  std::vector<std::string> m_sgMultipleNames;
  bool m_storeMultipleOutputs;
};
}

#endif // DERIVATIONFRAMEWORK_EGELECTRONLIKELIHOODTOOLWRAPPER_H
