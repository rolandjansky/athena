/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauScaleFactorCalculator.cxx 805414 2017-05-24 12:21:29Z yili $
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
  //CP::SystematicSet m_syst_rec = m_tauEffCorrTool->recommendedSystematics();
  //for (auto sysname : m_syst_rec.getBaseNames() ) std::cout<<"recommended "<<sysname<<std::endl;
  //CP::SystematicSet m_syst_aff = m_tauEffCorrTool->affectingSystematics();
  //for (auto sysname : m_syst_aff.getBaseNames() )  std::cout<<"affecting "<<sysname<<std::endl;
  
  const std::string trueTauHadSysPrefix = "TAUS_TRUEHADTAU_EFF_";
  const std::string trueElectronSysPrefix = "TAUS_TRUEELECTRON_EFF_";
  // Should be empty- but lets be sure
  m_syst_map.clear();
  // Add all recommended systematics to be clear
  // Tau-electron overlap removal up/down, true hadtau
  m_syst_map["tauSF_eleolr_total_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"ELEOLR_TOTAL__1down");
  m_syst_map["tauSF_eleolr_total_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"ELEOLR_TOTAL__1up");
  
  // Tau-electron overlap removal up/down, true elec
  m_syst_map["tauSF_trueelectron_eleolr_highmu_down"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEOLR_STATHIGHMU__1down");
  m_syst_map["tauSF_trueelectron_eleolr_highmu_up"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEOLR_STATHIGHMU__1up");
  
  m_syst_map["tauSF_trueelectron_eleolr_lowmu_down"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEOLR_STATLOWMU__1down");
  m_syst_map["tauSF_trueelectron_eleolr_lowmu_up"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEOLR_STATLOWMU__1up");
  
  m_syst_map["tauSF_trueelectron_eleolr_syst_down"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEOLR_SYST__1down");
  m_syst_map["tauSF_trueelectron_eleolr_syst_up"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEOLR_SYST__1up");
  
  m_syst_map["tauSF_trueelectron_elebdt_mc16a_down"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEBDT_STATMC16A__1down");
  m_syst_map["tauSF_trueelectron_elebdt_mc16a_up"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEBDT_STATMC16A__1up");
  
  m_syst_map["tauSF_trueelectron_elebdt_mc16d_down"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEBDT_STATMC16D__1down");
  m_syst_map["tauSF_trueelectron_elebdt_mc16d_up"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEBDT_STATMC16D__1up");
  
  m_syst_map["tauSF_trueelectron_elebdt_syst_down"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEBDT_SYST__1down");
  m_syst_map["tauSF_trueelectron_elebdt_syst_up"]
    = CP::SystematicSet(trueElectronSysPrefix+"ELEBDT_SYST__1up");
  
  
  // Tau Jet ID WP up/down
  m_syst_map["tauSF_jetid_1p2025_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_1PRONGSTATSYSTUNCORR2025__1down");
  m_syst_map["tauSF_jetid_1p2025_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_1PRONGSTATSYSTUNCORR2025__1up");
  
  m_syst_map["tauSF_jetid_1p2530_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_1PRONGSTATSYSTUNCORR2530__1down");
  m_syst_map["tauSF_jetid_1p2530_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_1PRONGSTATSYSTUNCORR2530__1up");
  
  m_syst_map["tauSF_jetid_1p3040_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_1PRONGSTATSYSTUNCORR3040__1down");
  m_syst_map["tauSF_jetid_1p3040_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_1PRONGSTATSYSTUNCORR3040__1up");
  
  m_syst_map["tauSF_jetid_1pge40_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_1PRONGSTATSYSTUNCORRGE40__1down");
  m_syst_map["tauSF_jetid_1pge40_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_1PRONGSTATSYSTUNCORRGE40__1up");
  
  m_syst_map["tauSF_jetid_3p2030_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_3PRONGSTATSYSTUNCORR2030__1down");
  m_syst_map["tauSF_jetid_3p2030_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_3PRONGSTATSYSTUNCORR2030__1up");
  
  m_syst_map["tauSF_jetid_3pge30_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_3PRONGSTATSYSTUNCORRGE30__1down");
  m_syst_map["tauSF_jetid_3pge30_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_3PRONGSTATSYSTUNCORRGE30__1up");
  
  m_syst_map["tauSF_jetid_highpt_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_HIGHPT__1down");
  m_syst_map["tauSF_jetid_highpt_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_HIGHPT__1up");
  
  m_syst_map["tauSF_jetid_syst_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_SYST__1down");
  m_syst_map["tauSF_jetid_syst_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_SYST__1up");
  
  m_syst_map["tauSF_jetid_af2_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_AFII__1down");
  m_syst_map["tauSF_jetid_af2_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"JETID_AFII__1up");
  
  // Tau reconstruction up/down
  m_syst_map["tauSF_reco_total_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"RECO_TOTAL__1down");
  m_syst_map["tauSF_reco_total_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"RECO_TOTAL__1up");
  
  m_syst_map["tauSF_reco_highpt_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"RECO_HIGHPT__1down");
  m_syst_map["tauSF_reco_highpt_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"RECO_HIGHPT__1up");
  
  m_syst_map["tauSF_reco_af2_down"]
    = CP::SystematicSet(trueTauHadSysPrefix+"RECO_AFII__1down");
  m_syst_map["tauSF_reco_af2_up"]
    = CP::SystematicSet(trueTauHadSysPrefix+"RECO_AFII__1up");

  

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
