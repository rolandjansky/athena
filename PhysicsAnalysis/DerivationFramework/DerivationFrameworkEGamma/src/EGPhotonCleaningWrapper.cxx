/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGPhotonCleaningWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
//

#include "DerivationFrameworkEGamma/EGPhotonCleaningWrapper.h"
#include "xAODEgamma/Photon.h"
#include <EgammaAnalysisHelpers/PhotonHelpers.h>

namespace DerivationFramework {

EGPhotonCleaningWrapper::EGPhotonCleaningWrapper(const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_sgName("DFCommonPhotonsCleaning")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("StoreGateEntryName", m_sgName);
}

StatusCode
EGPhotonCleaningWrapper::initialize()
{
  if (m_sgName.empty()) {
    ATH_MSG_ERROR(
      "No SG name provided for the output of EGPhotonCleaningWrapper");
    return StatusCode::FAILURE;
  }
  if (!m_fudgeMCTool.name().empty()) {
    CHECK(m_fudgeMCTool.retrieve());
  }
  ATH_CHECK(m_containerName.initialize());
  m_decoratorPass = m_containerName.key() + "." + m_sgName;
  m_decoratorPassDelayed = m_containerName.key() + "." + m_sgName + "NoTime";
  ATH_CHECK(m_decoratorPass.initialize());
  ATH_CHECK(m_decoratorPassDelayed.initialize());

  return StatusCode::SUCCESS;
}

StatusCode
EGPhotonCleaningWrapper::addBranches() const
{

  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::PhotonContainer> photons{ m_containerName, ctx };
  SG::WriteDecorHandle<xAOD::PhotonContainer, char> decoratorPass{
    m_decoratorPass, ctx
  };
  SG::WriteDecorHandle<xAOD::PhotonContainer, char> decoratorPassDelayed{
    m_decoratorPassDelayed, ctx
  };

  // Write mask for each element and record to SG for subsequent selection
  for (const xAOD::Photon* photon : *photons) {

    bool passSelection = false;
    bool passSelectionDelayed = false;
    bool applyFF = (!m_fudgeMCTool.empty());

    if (!applyFF) {
      passSelection = PhotonHelpers::passOQquality(photon);
      passSelectionDelayed = PhotonHelpers::passOQqualityDelayed(photon);
    } else {
      // apply the shower shape corrections
      CP::CorrectionCode correctionCode = CP::CorrectionCode::Ok;
      xAOD::Photon* ph = nullptr;
      correctionCode = m_fudgeMCTool->correctedCopy(*photon, ph);
      if (correctionCode == CP::CorrectionCode::Ok) {
        passSelection = PhotonHelpers::passOQquality(ph);
        passSelectionDelayed = PhotonHelpers::passOQqualityDelayed(ph);
      } else if (correctionCode == CP::CorrectionCode::Error) {
        Error("addBranches()",
              "Error applying fudge factors to current photon");
      } else if (correctionCode == CP::CorrectionCode::OutOfValidityRange) {
        Warning(
          "addBranches()",
          "Current photon has no valid fudge factors due to out-of-range");
      } else {
        Warning("addBranches()",
                "Unknown correction code %d from "
                "ElectronPhotonShowerShapeFudgeTool",
                (int)correctionCode);
      }
      delete ph;
    }

    // decorate the original object
    if (passSelection) {
      decoratorPass(*photon) = 1;
    } else {
      decoratorPass(*photon) = 0;
    }
    if (passSelectionDelayed) {
      decoratorPassDelayed(*photon) = 1;
    } else {
      decoratorPassDelayed(*photon) = 0;
    }
  }
  return StatusCode::SUCCESS;
}
} // end namespace DerivationFramework
