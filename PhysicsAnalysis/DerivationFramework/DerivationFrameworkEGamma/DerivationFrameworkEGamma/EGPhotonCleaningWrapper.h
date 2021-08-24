/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGPhotonCleaningWrapper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGPHOTONCLEANINGWRAPPER_H
#define DERIVATIONFRAMEWORK_EGPHOTONCLEANINGWRAPPER_H

#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODEgamma/PhotonContainer.h"
//
#include <string>
namespace DerivationFramework {

class EGPhotonCleaningWrapper
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  EGPhotonCleaningWrapper(const std::string& t,
                          const std::string& n,
                          const IInterface* p);

  virtual StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  ToolHandle<IElectronPhotonShowerShapeFudgeTool> m_fudgeMCTool{
    this,
    "EGammaFudgeMCTool",
    "",
    "Handle to the Fudging Tool"
  };
  SG::ReadHandleKey<xAOD::PhotonContainer> m_containerName{ this,
                                                            "ContainerName",
                                                            "",
                                                            "Input" };

  // Write decoration handle keys
  SG::WriteDecorHandleKey<xAOD::PhotonContainer>
    m_decoratorPass{ this, "decoratorPass", "", "" };
  SG::WriteDecorHandleKey<xAOD::PhotonContainer>
    m_decoratorPassDelayed{ this, "decoratorPassDelayed", "", "" };
  std::string m_sgName;
};
}

#endif // DERIVATIONFRAMEWORK_EGSELECTIONTOOLWRAPPER_H
