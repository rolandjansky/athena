/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauScaleFactorCalculator.cxx 712396 2015-12-03 16:25:25Z tneep $
#include <string>

#include "TopCorrections/TauScaleFactorCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODTau/TauJetContainer.h"

namespace top {

TauScaleFactorCalculator::TauScaleFactorCalculator(const std::string& name) :
  asg::AsgTool(name),
  m_config(nullptr),
  m_tauEffCorrTool("TauAnalysisTools::TauEfficiencyCorrectionsTool"),
  m_tauEffCorrToolLoose("TauAnalysisTools::TauEfficiencyCorrectionsToolLoose"),

  m_systNominal(CP::SystematicSet()) {
  declareProperty("config" , m_config);
}

StatusCode TauScaleFactorCalculator::initialize() {
  ATH_MSG_INFO(" top::TauScaleFactorCalculator initialize");

  top::check(m_tauEffCorrTool.retrieve(),
              "Failed to retrieve tau efficiency corrections tool");

  // How to get the recommended/affecting systematics...
  // CP::SystematicSet m_syst_rec = m_tauEffCorrTool->recommendedSystematics();
  // CP::SystematicSet m_syst_aff = m_tauEffCorrTool->affectingSystematics();

  const std::string tauSysPrefix = "TAUS_TRUEHADTAU_EFF_";
  // Should be empty- but lets be sure
  m_syst_map.clear();
  // Add all recommended systematics to be clear
  // Tau-electron overlap removal up/down
  m_syst_map["tauSF_eleolr_total_down"]
    = CP::SystematicSet(tauSysPrefix+"ELEOLR_TOTAL__1down");
  m_syst_map["tauSF_eleolr_total_up"]
    = CP::SystematicSet(tauSysPrefix+"ELEOLR_TOTAL__1up");
  // Tau Jet ID WP up/down
  m_syst_map["tauSF_jetid_total_down"]
    = CP::SystematicSet(tauSysPrefix+"JETID_TOTAL__1down");
  m_syst_map["tauSF_jetid_total_up"]
    = CP::SystematicSet(tauSysPrefix+"JETID_TOTAL__1up");
  // Tau reconstruction up/down
  m_syst_map["tauSF_reco_total_down"]
    = CP::SystematicSet(tauSysPrefix+"RECO_TOTAL__1down");
  m_syst_map["tauSF_reco_total_up"]
    = CP::SystematicSet(tauSysPrefix+"RECO_TOTAL__1up");

  return StatusCode::SUCCESS;
}


StatusCode TauScaleFactorCalculator::execute() {
  ///-- Loop over all muon collections --///
  for (auto currentSystematic : *m_config->systSgKeyMapTaus()) {
    const xAOD::TauJetContainer* taus(nullptr);
    top::check(evtStore()->retrieve(taus, currentSystematic.second),
               "failed to retrieve taus");

    ///-- Tell the SF tools to use the nominal systematic --///

    /// -- Loop over all taus in each collection --///
    for (auto tauPtr : *taus) {
      /// -- Does the tau pass object selection? --///
      bool passSelection(false);
      if (tauPtr->isAvailable<char>("passPreORSelection")) {
        if (tauPtr->auxdataConst<char>("passPreORSelection") == 1) {
          passSelection = true;
        }
      }
      if (tauPtr->isAvailable<char>("passPreORSelectionLoose")) {
        if (tauPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) {
          passSelection = true;
        }
      }

      ///-- If we aren't selecting the tau, then don't bother --///
      if (!passSelection) continue;

      //============================================================
      // Find the nominal SF for the tight/loose tau IDs for every
      // systematic uncertainty.
      //
      // Make sure we set both tools to nominal for each tau
      //============================================================

      top::check(m_tauEffCorrTool->applySystematicVariation(m_systNominal),
                 "Failed to set tau efficiency correction"
                 " tool to nominal value");
      top::check(m_tauEffCorrToolLoose->applySystematicVariation(m_systNominal),
                 "Failed to set (loose) tau efficiency correction"
                 " tool to nominal value");

      double nominalSF(0.0), nominalSFLoose(0.0);

      top::check(m_tauEffCorrTool->getEfficiencyScaleFactor(*tauPtr, nominalSF),
                 "Failed to get nominal tau SF");
      top::check(m_tauEffCorrToolLoose->getEfficiencyScaleFactor(*tauPtr,
                                                                 nominalSFLoose),
                 "Failed to get nominal (loose) tau SF");

      ///-- Decorate the tau with the tight/loose SFs --///
      tauPtr->auxdecor<float>("tauSF") = nominalSF;
      tauPtr->auxdecor<float>("tauSF_loose") = nominalSFLoose;

      ///-- For nominal calibration, vary the SF systematics --///
      if (currentSystematic.first == m_config->nominalHashValue()) {
        for (const auto& i : m_syst_map) {
          double SF(0.0), SF_loose(0.0);
          const std::string decoration_name = i.first;
          const std::string decoration_name_loose = i.first+"_loose";
          top::check(m_tauEffCorrTool->applySystematicVariation(i.second),
                     "Failed to set tau efficiency correction"
                     " tool to nominal value");
          top::check(m_tauEffCorrToolLoose->applySystematicVariation(i.second),
                     "Failed to set (loose) tau efficiency correction"
                     " tool to nominal value");
          top::check(m_tauEffCorrTool->getEfficiencyScaleFactor(*tauPtr, SF),
                     "Failed to get nominal tau SF");
          top::check(m_tauEffCorrToolLoose->getEfficiencyScaleFactor(*tauPtr,
                                                                     SF_loose),
                     "Failed to get nominal (loose) tau SF");
          tauPtr->auxdecor<float>(decoration_name) = SF;
          tauPtr->auxdecor<float>(decoration_name_loose) = SF_loose;
        }
      }  // Calibration systematic is nominal, so calculate SF systematics
    }
  }


  return StatusCode::SUCCESS;
}

}  // namespace top
