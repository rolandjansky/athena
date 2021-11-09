/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCPTools/TopIsolationCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Isolation include(s):
#include "IsolationSelection/IsolationSelectionTool.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"

namespace top {
  IsolationCPTools::IsolationCPTools(const std::string& name) :
    asg::AsgTool(name),
    m_isolationCalibFile("IsolationSelection/v1/MC15_Z_Jpsi_cutMap.root") {
    declareProperty("config", m_config);

    declareProperty("IsolationCorrectionTool", m_isolationCorr);
  }

  StatusCode IsolationCPTools::initialize() {
    ATH_MSG_INFO("top::IsolationCPTools initialize...");

    if (!m_config->makeAllCPTools()) {
      ATH_MSG_INFO("top::IsolationCPTools: no need to initialise for mini-xAOD");
      return StatusCode::SUCCESS;
    }

    if (!m_config->useElectrons() && !m_config->useMuons()) {
      ATH_MSG_INFO("top::IsolationCPTools: no need to initialise since neither using electrons nor muons");
      return StatusCode::SUCCESS;
    }
    top::check(setupIsolation(), "Failed to setup Isolation tools");
    return StatusCode::SUCCESS;
  }

  StatusCode IsolationCPTools::setupIsolation() {
    // needed both for electrons and photons to apply the leakage correction,
    // [http://cern.ch/go/j6Lx]
    std::string iso_corr_tool_name = "IsolationCorrectionTool";
    if (asg::ToolStore::contains<CP::IIsolationCorrectionTool>(iso_corr_tool_name)) {
      m_isolationCorr = asg::ToolStore::get<CP::IIsolationCorrectionTool>(iso_corr_tool_name);
    } else {
      CP::IIsolationCorrectionTool* isolationCorr = new CP::IsolationCorrectionTool(iso_corr_tool_name);
      top::check(asg::setProperty(isolationCorr, "IsMC", m_config->isMC()),
                 "Failed to setProperty IsMC");  // if MC, else false
      top::check(asg::setProperty(isolationCorr, "AFII_corr", m_config->isAFII()),
                 "Failed to setProperty AFII_corr"); // if AFII, else false
      if (m_config->useEgammaLeakageCorrection()) {
        top::check(asg::setProperty(isolationCorr, "Apply_SC_leakcorr", true),
                   "Failed to setProperty Apply_SC_leakcorr"); // super cluster based core correction
        top::check(asg::setProperty(isolationCorr, "Apply_etaEDParPU_correction", true),
                   "Failed to setProperty Apply_etaEDParPU_correction"); // improved energy density based pileup
                                                                         // correction with eta variations
        top::check(asg::setProperty(isolationCorr, "Apply_etaEDPar_mc_correction", m_config->isMC()),
                   "Failed to setProperty Apply_etaEDPar_mc_correction"); // pileup dependent correction to MC
      }
      top::check(isolationCorr->initialize(), "Failed to initialize");

      m_isolationCorr = isolationCorr;
    }

    top::check(setupPerObjectWPs(m_config->electronIsolationWPs(), "ElectronWP"), "Failed to initialize electron isolation CP tools");
    top::check(setupPerObjectWPs(m_config->muonIsolationWPs(), "MuonWP"), "Failed to initialize muon isolation CP tools");
    top::check(setupPerObjectWPs(m_config->photonIsolationWPs(), "PhotonWP"), "Failed to initialize photon isolation CP tools");

    return StatusCode::SUCCESS;
  }

  StatusCode IsolationCPTools::setupPerObjectWPs(const std::vector<std::string>& WPs, const std::string& objectWPtype) {
  for (const std::string& isoWP : WPs) {
      std::string tool_name = "IsolationTool_" + objectWPtype + "_" + isoWP;
      ATH_MSG_INFO("Initializing isolation tool: " << tool_name);
      if (!asg::ToolStore::contains<CP::IIsolationSelectionTool>(tool_name)) {
        CP::IIsolationSelectionTool* iso_tool = new CP::IsolationSelectionTool(tool_name);
        top::check(asg::setProperty(iso_tool, "CalibFileName", m_isolationCalibFile),
                   "Failed to set CalibFileName for " + tool_name);
        top::check(asg::setProperty(iso_tool, objectWPtype, isoWP),
                   "Failed to set " + objectWPtype + " for " + tool_name);
        top::check(iso_tool->initialize(), "Failed to initialize " + tool_name);
        m_isolationTools.push_back(iso_tool);
      }
    }
    return StatusCode::SUCCESS;
  }
}  // namespace top
