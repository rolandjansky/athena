/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

// $Id: TauScaleFactorCalculator.cxx 805414 2017-05-24 12:21:29Z yili $
#include <string>

#include "TopCorrections/TauScaleFactorCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODTau/TauJetContainer.h"
#include "TopCorrections/ScaleFactorRetriever.h"

namespace top {
  TauScaleFactorCalculator::TauScaleFactorCalculator(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),
    m_tauEffCorrTool("TauAnalysisTools::TauEfficiencyCorrectionsTool"),
    m_tauEffCorrToolLoose("TauAnalysisTools::TauEfficiencyCorrectionsToolLoose"),

    m_systNominal(CP::SystematicSet()) {
    declareProperty("config", m_config);
  }

  StatusCode TauScaleFactorCalculator::initialize() {
    ATH_MSG_INFO(" top::TauScaleFactorCalculator initialize");

    top::check(m_tauEffCorrTool.retrieve(),
               "Failed to retrieve tau efficiency corrections tool");

    // How to get the recommended/affecting systematics...
    // Should be empty- but lets be sure
    m_syst_map.clear();
    // Add all recommended systematics to be clear
    // Tau-electron overlap removal up/down, true hadtau

    for (auto tauSFpair : top::tauSF_name) {
      m_syst_map[tauSFpair.second.Data()] = CP::SystematicSet(tauSFpair.second.Data());
    }

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
        tauPtr->auxdecor<float>("TAU_SF_NOMINAL") = nominalSF;
        tauPtr->auxdecor<float>("TAU_SF_NOMINAL_loose") = nominalSFLoose;

        ///-- For nominal calibration, vary the SF systematics --///
        if (currentSystematic.first == m_config->nominalHashValue()) {
          for (const auto& i : m_syst_map) {
            double SF(0.0), SF_loose(0.0);
            const std::string decoration_name = i.first;
            const std::string decoration_name_loose = i.first + "_loose";
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
