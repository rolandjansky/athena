/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

// $Id: ElectronScaleFactorCalculator.cxx 799556 2017-03-05 19:46:03Z tpelzer $
#include "TopCorrections/FwdElectronScaleFactorCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/ElectronContainer.h"


namespace top {
  FwdElectronScaleFactorCalculator::FwdElectronScaleFactorCalculator(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_systNominal(CP::SystematicSet()),
    m_systID_UP(CP::SystematicSet()),
    m_systID_DOWN(CP::SystematicSet()),
    m_electronEffSFID("AsgFwdElectronEfficiencyCorrectionTool_ID"),
    m_electronEffSFIDLoose("AsgFwdElectronEfficiencyCorrectionTool_IDLoose"),
    m_decor_idSF("SetMe"),
    m_decor_idSF_loose("SetMe") {
    declareProperty("config", m_config);
  }

  StatusCode FwdElectronScaleFactorCalculator::initialize() {
    ATH_MSG_INFO(" top::FwdElectronScaleFactorCalculator initialize");

    top::check(m_electronEffSFID.retrieve(), "Failed to retrieve FWD electron SF Tool");
    top::check(m_electronEffSFIDLoose.retrieve(), "Failed to retrieve FWD electron Loose SF Tool");

    m_systID_UP.insert(CP::SystematicVariation("EL_EFF_FwdID_TOTAL_1NPCOR_PLUS_UNCOR", 1));
    m_systID_DOWN.insert(CP::SystematicVariation("EL_EFF_FwdID_TOTAL_1NPCOR_PLUS_UNCOR", -1));

    m_decor_idSF = "FWDEL_SF_ID_" + m_config->fwdElectronID();
    m_decor_idSF_loose = "FWDEL_LOOSE_SF_ID_" + m_config->fwdElectronIDLoose();

    return StatusCode::SUCCESS;
  }

  StatusCode FwdElectronScaleFactorCalculator::execute() {
    ///-- Loop over all electron collections --///
    for (auto currentSystematic : *m_config->systSgKeyMapFwdElectrons()) {
      const xAOD::ElectronContainer* electrons(nullptr);
      top::check(evtStore()->retrieve(electrons, currentSystematic.second), "failed to retrieve electrons");

      ///-- Tell the SF tools to use the nominal systematic --///
      top::check(m_electronEffSFID->applySystematicVariation(m_systNominal), "Failed to set systematic");
      top::check(m_electronEffSFIDLoose->applySystematicVariation(m_systNominal), "Failed to set systematic");

      /// -- Loop over all electrons in each collection --///
      for (auto electronPtr : *electrons) {
        /// -- Does the electron pass object selection? --///
        bool passSelection(false);
        if (electronPtr->isAvailable<char>("passPreORSelection")) {
          if (electronPtr->auxdataConst<char>("passPreORSelection") == 1) {
            passSelection = true;
          }
        }
        if (electronPtr->isAvailable<char>("passPreORSelectionLoose")) {
          if (electronPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) {
            passSelection = true;
          }
        }

        if (passSelection) {
          double SF_ID(1.), SF_IDLoose(1.);

          ///-- Get Efficiencies --///

          top::check(m_electronEffSFID->getEfficiencyScaleFactor(*electronPtr, SF_ID), "Failed to get SF");
          top::check(m_electronEffSFIDLoose->getEfficiencyScaleFactor(*electronPtr, SF_IDLoose), "Failed to get SF");


          ///-- Decorate --///
          electronPtr->auxdecor<float>(m_decor_idSF) = SF_ID;
          electronPtr->auxdecor<float>(m_decor_idSF_loose) = SF_IDLoose;

          ///-- For nominal calibration, vary the SF systematics --///
          if (currentSystematic.first == m_config->nominalHashValue()) {
            double SF_ID_UP(1.), SF_IDLoose_UP(1.);
            double SF_ID_DOWN(1.), SF_IDLoose_DOWN(1.);

            ///-- ID --///
            ///-- ID up --///
            top::check(m_electronEffSFID->applySystematicVariation(m_systID_UP), "Failed to set systematic");
            top::check(m_electronEffSFID->getEfficiencyScaleFactor(*electronPtr, SF_ID_UP), "Failed to get SF");
            top::check(m_electronEffSFIDLoose->applySystematicVariation(m_systID_UP), "Failed to set systematic");
            top::check(m_electronEffSFIDLoose->getEfficiencyScaleFactor(*electronPtr, SF_IDLoose_UP),
                       "Failed to get SF");

            ///-- ID down --///
            top::check(m_electronEffSFID->applySystematicVariation(m_systID_DOWN), "Failed to set systematic");
            top::check(m_electronEffSFID->getEfficiencyScaleFactor(*electronPtr, SF_ID_DOWN), "Failed to get SF");
            top::check(m_electronEffSFIDLoose->applySystematicVariation(m_systID_DOWN), "Failed to set systematic");
            top::check(m_electronEffSFIDLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                        SF_IDLoose_DOWN), "Failed to get SF");

            ///-- ID reset to nominal --///
            top::check(m_electronEffSFID->applySystematicVariation(m_systNominal), "Failed to set systematic");
            top::check(m_electronEffSFIDLoose->applySystematicVariation(m_systNominal), "Failed to set systematic");

            ///-- Decorate --///
            electronPtr->auxdecor<float>(m_decor_idSF + "_UP") = SF_ID_UP;
            electronPtr->auxdecor<float>(m_decor_idSF + "_DOWN") = SF_ID_DOWN;
            electronPtr->auxdecor<float>(m_decor_idSF_loose + "_UP") = SF_IDLoose_UP;
            electronPtr->auxdecor<float>(m_decor_idSF_loose + "_DOWN") = SF_IDLoose_DOWN;

            ///-- Debug messages --///

            ATH_MSG_DEBUG("Fwd Electron ID SF = " << electronPtr->auxdataConst<float>(m_decor_idSF) << " + " <<
                          electronPtr->auxdataConst<float>(m_decor_idSF + "_UP") << " - " <<
                          electronPtr->auxdataConst<float>(m_decor_idSF + "_DOWN")
                          );
          } // Calibration systematic is nominal, so calculate SF systematics
        }
      }
    }


    return StatusCode::SUCCESS;
  }
}
