/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGElectronLikelihoodToolWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)

#include "DerivationFrameworkEGamma/EGElectronLikelihoodToolWrapper.h"
//
#include "PATCore/AcceptData.h"
#include "PATCore/AcceptInfo.h"
//
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

namespace DerivationFramework {

EGElectronLikelihoodToolWrapper::EGElectronLikelihoodToolWrapper(
  const std::string& t,
  const std::string& n,
  const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_cut("")
  , m_sgName("")
  , m_storeTResult(false)
  , m_sgMultipleNames({})
  , m_storeMultipleOutputs(false)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("CutType", m_cut);
  declareProperty("StoreGateEntryName", m_sgName);
  declareProperty("StoreTResult", m_storeTResult);
  declareProperty("StoreGateEntryMultipleNames", m_sgMultipleNames);
  declareProperty("StoreMultipleOutputs", m_storeMultipleOutputs);
}

StatusCode
EGElectronLikelihoodToolWrapper::initialize()
{
  if (m_sgName.empty()) {
    ATH_MSG_ERROR(
      "No SG name provided for the output of EGElectronLikelihoodToolWrapper!");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_tool.retrieve());

  if (!(m_fudgeMCTool.name().empty())) {
    ATH_CHECK(m_fudgeMCTool.retrieve());
  } else {
    m_fudgeMCTool.disable();
  }

  ATH_CHECK(m_ContainerName.initialize());
  //
  m_decoratorPass = m_ContainerName.key() + "." + m_sgName;
  m_decoratorIsEM = m_ContainerName.key() + "." + m_sgName + "IsEMValue";
  ATH_CHECK(m_decoratorPass.initialize());
  ATH_CHECK(m_decoratorIsEM.initialize());
  //
  if (m_storeTResult) {
    m_decoratorResult = m_ContainerName.key() + "." + m_sgName + "Result";
    ATH_CHECK(m_decoratorResult.initialize(m_storeTResult));
  }
  if (m_storeMultipleOutputs) {
    for (auto& sgName : m_sgMultipleNames) {
      m_decoratorMultipleOutputs.emplace_back(m_ContainerName.key() + "." + sgName);
    }
    ATH_CHECK(m_decoratorMultipleOutputs.initialize(m_storeMultipleOutputs));
  }
  return StatusCode::SUCCESS;
}

StatusCode
EGElectronLikelihoodToolWrapper::addBranches() const
{
  // retrieve container
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::EgammaContainer> particles{ m_ContainerName, ctx };

  // Decorators
  SG::WriteDecorHandle<xAOD::EgammaContainer, char> decoratorPass{
    m_decoratorPass, ctx
  };
  SG::WriteDecorHandle<xAOD::EgammaContainer, unsigned int> decoratorIsEM{
    m_decoratorIsEM, ctx
  };

  std::unique_ptr<SG::WriteDecorHandle<xAOD::EgammaContainer, double>>
    decoratorResult = nullptr;
  if (m_storeTResult) {
    decoratorResult =
      std::make_unique<SG::WriteDecorHandle<xAOD::EgammaContainer, double>>(
        m_decoratorResult, ctx);
  }

  auto decoratorMultipleOutputs = m_decoratorMultipleOutputs.makeHandles();

  bool applyFF = (!m_fudgeMCTool.empty());
  // Write mask for each element and record to SG for subsequent selection
  for (const xAOD::Egamma* par : *particles) {
    // point to the input which we do not own ,
    // in case we fundge this will become
    // a corrected one which needs to be deleted
    // corrected copy can be a bit awkward
    const xAOD::Egamma* pCopy = par;
    if (applyFF) {
      xAOD::Type::ObjectType type = par->type();
      // apply the shower shape corrections
      CP::CorrectionCode correctionCode = CP::CorrectionCode::Ok;
      if (type == xAOD::Type::Electron) {
        const xAOD::Electron* eg = static_cast<const xAOD::Electron*>(par);
        xAOD::Electron* el = nullptr;
        correctionCode = m_fudgeMCTool->correctedCopy(*eg, el);
        pCopy = el;
      } else {
        const xAOD::Photon* eg = static_cast<const xAOD::Photon*>(par);
        xAOD::Photon* ph = nullptr;
        correctionCode = m_fudgeMCTool->correctedCopy(*eg, ph);
        pCopy = ph;
      }
      if (correctionCode == CP::CorrectionCode::Ok) { // All OK
      } else if (correctionCode == CP::CorrectionCode::Error) {
        Error("addBranches()",
              "Error applying fudge factors to current photon");
      } else if (correctionCode == CP::CorrectionCode::OutOfValidityRange) {
        Warning(
          "addBranches()",
          "Current object has no valid fudge factors due to out-of-range");
      } else {
        Warning(
          "addBranches()",
          "Unknown correction code %d from ElectronPhotonShowerShapeFudgeTool",
          (int)correctionCode);
      }
    }
    // compute the output of the selector
    asg::AcceptData theAccept(m_tool->accept(ctx, pCopy));
    const unsigned int isEM =
      (unsigned int)theAccept.getCutResultInvertedBitSet()
        .to_ulong(); // this should work for both the
                     // cut-based and the LH selectors

    // decorate the original object
    if (m_cut.empty()) {
      const bool pass_selection = (bool)theAccept;
      if (pass_selection) {
        decoratorPass(*par) = 1;
      } else {
        decoratorPass(*par) = 0;
      }
      decoratorIsEM(*par) = isEM;
      if (decoratorResult) {
        (*decoratorResult)(*par) =
          static_cast<double>(m_tool->calculate(ctx, pCopy));
      }
      if (m_storeMultipleOutputs) {
        // calculateMultipleOutputs only supports xAOD::Electron as input
        const xAOD::Electron *eCopy = static_cast<const xAOD::Electron *>(pCopy);
        std::vector<float> toolOutput = m_tool->calculateMultipleOutputs(ctx, eCopy);
        for (size_t i = 0; i < toolOutput.size(); i++){
          decoratorMultipleOutputs.at(i)(*par) = toolOutput.at(i);
        }
      }
    } else {
      if (theAccept.getCutResult(m_cut)) {
        decoratorPass(*par) = 1;
      } else {
        decoratorPass(*par) = 0;
      }
      decoratorIsEM(*par) = isEM;
      if (decoratorResult) {
        static const SG::AuxElement::Decorator<double> decResult(m_sgName +
                                                                 "Result");
        (*decoratorResult)(*par) =
          static_cast<double>(m_tool->calculate(ctx, pCopy));
      }
      if (m_storeMultipleOutputs) {
        // calculateMultipleOutputs only supports xAOD::Electron as input
        const xAOD::Electron* eCopy = static_cast<const xAOD::Electron*>(pCopy);
        std::vector<float> toolOutput = m_tool->calculateMultipleOutputs(ctx, eCopy);
        for (size_t i = 0; i < toolOutput.size(); i++){
          decoratorMultipleOutputs.at(i)(*par) = toolOutput.at(i);
        }
      }
    }
    // delete the particle copy
    if (applyFF) {
      delete pCopy;
    }
  }

  return StatusCode::SUCCESS;
}
}
