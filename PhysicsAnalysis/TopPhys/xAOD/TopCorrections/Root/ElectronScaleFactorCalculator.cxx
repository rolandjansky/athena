/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// $Id: ElectronScaleFactorCalculator.cxx 799556 2017-03-05 19:46:03Z tpelzer $
#include "TopCorrections/ElectronScaleFactorCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/ElectronContainer.h"
#include "TString.h"

namespace top {
  ElectronScaleFactorCalculator::ElectronScaleFactorCalculator(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_systNominal(CP::SystematicSet()),
    m_systTrigger_UP(CP::SystematicSet()),
    m_systTrigger_DOWN(CP::SystematicSet()),
    m_systReco_UP(CP::SystematicSet()),
    m_systReco_DOWN(CP::SystematicSet()),
    m_systID_UP(CP::SystematicSet()),
    m_systID_DOWN(CP::SystematicSet()),
    m_systIso_UP(CP::SystematicSet()),
    m_systIso_DOWN(CP::SystematicSet()),
    m_systChargeID_UP(CP::SystematicSet()),
    m_systChargeID_DOWN(CP::SystematicSet()),
    m_systChargeMisID_STAT_UP(CP::SystematicSet()),
    m_systChargeMisID_STAT_DOWN(CP::SystematicSet()),
    m_systChargeMisID_SYST_UP(CP::SystematicSet()),
    m_systChargeMisID_SYST_DOWN(CP::SystematicSet()),

    m_electronEffSFTrigger("AsgElectronEfficiencyCorrectionTool_TriggerSF"),
    m_electronEffSFTriggerLoose("AsgElectronEfficiencyCorrectionTool_TriggerSFLoose"),
    m_electronEffTrigger("AsgElectronEfficiencyCorrectionTool_Trigger"),
    m_electronEffTriggerLoose("AsgElectronEfficiencyCorrectionTool_TriggerLoose"),
    m_electronEffSFReco("AsgElectronEfficiencyCorrectionTool_Reco"),
    m_electronEffSFID("AsgElectronEfficiencyCorrectionTool_ID"),
    m_electronEffSFIDLoose("AsgElectronEfficiencyCorrectionTool_IDLoose"),
    m_electronEffIso_exists(false),
    m_electronEffIsoLoose_exists(false),
    m_electronEffSFIso("AsgElectronEfficiencyCorrectionTool_Iso"),
    m_electronEffSFIsoLoose("AsgElectronEfficiencyCorrectionTool_IsoLoose"),
    m_electronEffChargeID_exists(false),
    m_electronEffChargeIDLoose_exists(false),
    m_electronEffChargeMisID_exists(false),
    m_electronEffChargeMisIDLoose_exists(false),
    m_electronEffSFChargeID("AsgElectronEfficiencyCorrectionTool_ChargeID"),
    m_electronEffSFChargeIDLoose("AsgElectronEfficiencyCorrectionTool_ChargeIDLoose"),
    m_electronEffSFChargeMisID("ElectronChargeEfficiencyCorrection"),
    m_electronEffSFChargeMisIDLoose("ElectronChargeEfficiencyCorrectionLoose"),

    m_electronEffSFTriggerCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_TriggerSF"),
    m_electronEffSFTriggerLooseCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_TriggerSFLoose"),
    m_electronEffTriggerCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_Trigger"),
    m_electronEffTriggerLooseCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_TriggerLoose"),
    m_electronEffSFRecoCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_Reco"),
    m_electronEffSFIDCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_ID"),
    m_electronEffSFIDLooseCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_IDLoose"),
    m_electronEffSFIsoCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_Iso"),
    m_electronEffSFIsoLooseCorrModel("AsgElectronEfficiencyCorrectionTool_CorrModel_IsoLoose"),

    m_decor_triggerEff("SetMe"),
    m_decor_triggerEff_loose("SetMe"),
    m_decor_triggerSF("SetMe"),
    m_decor_triggerSF_loose("SetMe"),
    m_decor_recoSF("SetMe"),
    m_decor_idSF("SetMe"),
    m_decor_idSF_loose("SetMe"),
    m_decor_isoSF("SetMe"),
    m_decor_isoSF_loose("SetMe"),
    m_decor_chargeidSF("SetMe"),
    m_decor_chargeidSF_loose("SetMe"),
    m_decor_chargemisidSF("SetMe"),
    m_decor_chargemisidSF_loose("SetMe") {
    declareProperty("config", m_config);
  }

  StatusCode ElectronScaleFactorCalculator::initialize() {
    ATH_MSG_INFO(" top::ElectronScaleFactorCalculator initialize");

    top::check(m_electronEffSFTrigger.retrieve(), "Failed to retrieve electron SF Tool");
    top::check(m_electronEffSFTriggerLoose.retrieve(), "Failed to retrieve electron SF Tool");

    if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
      top::check(m_electronEffSFTriggerCorrModel.retrieve(),
                 "Failed to retrieve electron SF Tool for correlation model");
      top::check(
        m_electronEffSFTriggerLooseCorrModel.retrieve(), "Failed to retrieve electron SF Tool for correlation model");
    }

    top::check(m_electronEffSFReco.retrieve(), "Failed to retrieve electron SF Tool");
    if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
      top::check(m_electronEffSFRecoCorrModel.retrieve(), "Failed to retrieve electron SF Tool for correlation model");
    }

    if (asg::ToolStore::contains<IAsgElectronEfficiencyCorrectionTool> ("AsgElectronEfficiencyCorrectionTool_Iso")) {
      m_electronEffIso_exists = true;
      top::check(m_electronEffSFIso.retrieve(), "Failed to retrieve electron SF Tool");
      if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
        top::check(m_electronEffSFIsoCorrModel.retrieve(), "Failed to retrieve electron SF Tool for correlation model");
      }
    }
    if (asg::ToolStore::contains<IAsgElectronEfficiencyCorrectionTool> ("AsgElectronEfficiencyCorrectionTool_IsoLoose"))
    {
      m_electronEffIsoLoose_exists = true;
      top::check(m_electronEffSFIsoLoose.retrieve(), "Failed to retrieve loose electron SF Tool");
      if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
        top::check(
          m_electronEffSFIsoLooseCorrModel.retrieve(),
          "Failed to retrieve loose electron SF Tool for correlation model");
      }
    }
    std::ostream& msgInfo = msg(MSG::Level::INFO);
    // ChargeID efficiency and Charge mis-identification scale factors: to apply when enabling Electron Charge ID
    // Selector
    if (asg::ToolStore::contains<IAsgElectronEfficiencyCorrectionTool> ("AsgElectronEfficiencyCorrectionTool_ChargeID"))
    {
      m_electronEffChargeID_exists = true; // this scale factor currently exists only for MediumLHH electron
      top::check(m_electronEffSFChargeID.retrieve(), "Failed to retrieve electron charge flip efficiency SF Tool");
      msgInfo << "------>Systematics:\n";
      for (auto sys:m_electronEffSFChargeID->recommendedSystematics())
        msgInfo << "---> " << sys << "\n";
    }
    if (asg::ToolStore::contains<IAsgElectronEfficiencyCorrectionTool> (
          "AsgElectronEfficiencyCorrectionTool_ChargeIDLoose")) {
      m_electronEffChargeIDLoose_exists = true; // this scale factor currently exists only for MediumLHH electron
      top::check(m_electronEffSFChargeIDLoose.retrieve(), "Failed to retrieve electron charge flip efficiency SF Tool");
      msgInfo << "------>Systematics:\n";
      for (auto sys:m_electronEffSFChargeIDLoose->recommendedSystematics())
        msgInfo << "---> " << sys << "\n";
    }
    if (asg::ToolStore::contains<CP::ElectronChargeEfficiencyCorrectionTool> ("ElectronChargeEfficiencyCorrection") &&
        m_config->doPileupReweighting()) { // tool requires RandomRunNumber set from it
      m_electronEffChargeMisID_exists = true;
      top::check(m_electronEffSFChargeMisID.retrieve(), "Failed to retrieve electron charge mis-id SF Tool");
      msgInfo << "------>Systematics:\n";
      for (auto sys:m_electronEffSFChargeMisID->recommendedSystematics())
        msgInfo << "---> " << sys << "\n";
    }
    if (asg::ToolStore::contains<CP::ElectronChargeEfficiencyCorrectionTool> ("ElectronChargeEfficiencyCorrectionLoose")
        &&
        m_config->doPileupReweighting()) { // tool requires RandomRunNumber set from it
      m_electronEffChargeMisIDLoose_exists = true;
      top::check(m_electronEffSFChargeMisIDLoose.retrieve(), "Failed to retrieve electron charge mis-id SF Tool");
      msgInfo << "------>Systematics:\n";
      for (auto sys:m_electronEffSFChargeMisIDLoose->recommendedSystematics())
        msgInfo << "---> " << sys << "\n";
    }

    // If the isolation tool doesn't exist then check why...
    // valid reasons include you want no isolation or you know you are using an unsupported WP
    if (!m_electronEffIso_exists)
      if (m_config->electronIsolation() != "None")
        if (m_config->electronIsoSFs()) { // ... check if we want to continue anyway without SFs anyway
          ATH_MSG_WARNING(
            "No electron isolation SFs available. You can run with ElectronIsolation set to None if you don't want isolation, or add:\tElectronIsoSFs False\tto your config file.");
          return StatusCode::FAILURE;
        }

    if (!m_electronEffIsoLoose_exists)                                          // If the electron isolation SFs don't
                                                                                // exist
      if (m_config->electronIsolationLoose() != "None")                                                           // If
                                                                                                                  // the
                                                                                                                  // isolation
                                                                                                                  // is
                                                                                                                  // not
                                                                                                                  // 'None'
                                                                                                                  // then...
        if (m_config->electronIsoSFs()) { // ... check if we want to continue anyway without SFs anyway
          ATH_MSG_WARNING(
            "No (loose) electron isolation SFs available. You can run with ElectronIsolationLoose set to None if you don't want isolation, or add:\tElectronIsoSFs False\tto your config file.");
          return StatusCode::FAILURE;
        }

    top::check(m_electronEffSFID.retrieve(), "Failed to retrieve electron SF Tool");
    top::check(m_electronEffSFIDLoose.retrieve(), "Failed to retrieve electron SF Tool");

    if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
      top::check(m_electronEffSFIDCorrModel.retrieve(), "Failed to retrieve electron SF Tool for correlation model");
      top::check(m_electronEffSFIDLooseCorrModel.retrieve(),
                 "Failed to retrieve electron SF Tool for correlation model");
    }

    m_systTrigger_UP.insert(CP::SystematicVariation("EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR", 1));
    m_systTrigger_DOWN.insert(CP::SystematicVariation("EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR", -1));
    m_systReco_UP.insert(CP::SystematicVariation("EL_EFF_Reco_TOTAL_1NPCOR_PLUS_UNCOR", 1));
    m_systReco_DOWN.insert(CP::SystematicVariation("EL_EFF_Reco_TOTAL_1NPCOR_PLUS_UNCOR", -1));
    m_systID_UP.insert(CP::SystematicVariation("EL_EFF_ID_TOTAL_1NPCOR_PLUS_UNCOR", 1));
    m_systID_DOWN.insert(CP::SystematicVariation("EL_EFF_ID_TOTAL_1NPCOR_PLUS_UNCOR", -1));
    m_systIso_UP.insert(CP::SystematicVariation("EL_EFF_Iso_TOTAL_1NPCOR_PLUS_UNCOR", 1));
    m_systIso_DOWN.insert(CP::SystematicVariation("EL_EFF_Iso_TOTAL_1NPCOR_PLUS_UNCOR", -1));
    m_systIso_UP.insert(CP::SystematicVariation("EL_EFF_Iso_TOTAL_1NPCOR_PLUS_UNCOR", 1));
    m_systIso_DOWN.insert(CP::SystematicVariation("EL_EFF_Iso_TOTAL_1NPCOR_PLUS_UNCOR", -1));
    m_systChargeID_UP.insert(CP::SystematicVariation("EL_EFF_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR", 1));
    m_systChargeID_DOWN.insert(CP::SystematicVariation("EL_EFF_ChargeIDSel_TOTAL_1NPCOR_PLUS_UNCOR", -1));
    m_systChargeMisID_STAT_UP.insert(CP::SystematicVariation("EL_CHARGEID_STAT", 1));
    m_systChargeMisID_STAT_DOWN.insert(CP::SystematicVariation("EL_CHARGEID_STAT", -1));
    m_systChargeMisID_SYST_UP.insert(CP::SystematicVariation("EL_CHARGEID_SYStotal", 1));
    m_systChargeMisID_SYST_DOWN.insert(CP::SystematicVariation("EL_CHARGEID_SYStotal", -1));

    if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
      CP::SystematicSet triggerSet = m_electronEffSFTrigger->recommendedSystematics();  //currently we have only the
                                                                                        // TOTAL model implemented for
                                                                                        // trigger SF, it's useless to
                                                                                        // use an advanced model here
      CP::SystematicSet recoSet = m_electronEffSFRecoCorrModel->recommendedSystematics();
      CP::SystematicSet idSet = m_electronEffSFIDCorrModel->recommendedSystematics();
      CP::SystematicSet isoSet = m_electronEffSFIsoCorrModel->recommendedSystematics();

      m_systTriggerCorrModel = CP::make_systematics_vector(triggerSet);
      m_systRecoCorrModel = CP::make_systematics_vector(recoSet);
      m_systIDCorrModel = CP::make_systematics_vector(idSet);
      m_systIsoCorrModel = CP::make_systematics_vector(isoSet);

      ATH_MSG_INFO(
        "For electron RECO, ID, ISOLATION using the correlation model " <<
          m_config->electronEfficiencySystematicModel());
      int count = 0, count2 = 0;
      for (const CP::SystematicSet& isyst : m_systRecoCorrModel) {
        count++;
        if (count % 2 == 1) continue;
        TString name = isyst.name();
        name.ReplaceAll("__1down", "");
        ATH_MSG_INFO(
          "--->electron RECO " << m_config->electronEfficiencySystematicModel() << " component " << (count2++) << " is " <<
            name);
      }
      count = 0;
      count2 = 0;
      for (const CP::SystematicSet& isyst : m_systIDCorrModel) {
        count++;
        if (count % 2 == 1) continue;
        TString name = isyst.name();
        name.ReplaceAll("__1down", "");
        ATH_MSG_INFO(
          "--->electron ID " << m_config->electronEfficiencySystematicModel() << " component " << (count2++) << "  is " <<
            name);
      }
      count = 0;
      count2 = 0;
      for (const CP::SystematicSet& isyst : m_systIsoCorrModel) {
        count++;
        if (count % 2 == 1) continue;
        TString name = isyst.name();
        name.ReplaceAll("__1down", "");
        ATH_MSG_INFO(
          "--->electron ISO " << m_config->electronEfficiencySystematicModel() << " component " << (count2++) << "  is " <<
            name);
      }
    }

    m_decor_triggerEff = "EL_EFF_Trigger_" + m_config->electronID();
    m_decor_triggerEff_loose = "EL_LOOSE_EFF_Trigger_" + m_config->electronIDLoose();
    m_decor_triggerSF = "EL_SF_Trigger_" + m_config->electronID();
    m_decor_triggerSF_loose = "EL_LOOSE_SF_Trigger_" + m_config->electronIDLoose();
    m_decor_recoSF = "EL_SF_Reco";
    m_decor_idSF = "EL_SF_ID_" + m_config->electronID();
    m_decor_idSF_loose = "EL_LOOSE_SF_ID_" + m_config->electronIDLoose();
    m_decor_isoSF = "EL_SF_Iso_" + m_config->electronIsolation();
    m_decor_isoSF_loose = "EL_LOOSE_SF_Iso_" + m_config->electronIsolationLoose();
    m_decor_chargeidSF = "EL_SF_ChargeID_" + m_config->electronID() + "_" + m_config->electronIsolation();
    m_decor_chargeidSF_loose = "EL_LOOSE_SF_ChargeID_" + m_config->electronIDLoose() + "_" +
                               m_config->electronIsolationLoose();
    m_decor_chargemisidSF = "EL_SF_ChargeMisID_" + m_config->electronID() + "_" + m_config->electronIsolation();
    m_decor_chargemisidSF_loose = "EL_LOOSE_SF_ChargeMisID_" + m_config->electronIDLoose() + "_" +
                                  m_config->electronIsolationLoose();

    return StatusCode::SUCCESS;
  }

  StatusCode ElectronScaleFactorCalculator::execute() {
    ///-- Loop over all electron collections --///
    for (auto currentSystematic : *m_config->systSgKeyMapElectrons()) {
      const xAOD::ElectronContainer* electrons(nullptr);
      top::check(evtStore()->retrieve(electrons, currentSystematic.second), "failed to retrieve electrons");

      ///-- Tell the SF tools to use the nominal systematic --///
      top::check(m_electronEffSFTrigger->applySystematicVariation(m_systNominal), "Failed to set systematic");
      top::check(m_electronEffSFTriggerLoose->applySystematicVariation(m_systNominal), "Failed to set systematic");
      top::check(m_electronEffSFReco->applySystematicVariation(m_systNominal), "Failed to set systematic");
      top::check(m_electronEffSFID->applySystematicVariation(m_systNominal), "Failed to set systematic");
      top::check(m_electronEffSFIDLoose->applySystematicVariation(m_systNominal), "Failed to set systematic");
      if (m_electronEffIso_exists) top::check(m_electronEffSFIso->applySystematicVariation(
                                                m_systNominal), "Failed to set systematic");
      if (m_electronEffIsoLoose_exists) top::check(m_electronEffSFIsoLoose->applySystematicVariation(
                                                     m_systNominal), "Failed to set systematic");
      if (m_electronEffChargeID_exists) top::check(m_electronEffSFChargeID->applySystematicVariation(
                                                     m_systNominal), "Failed to set systematic");
      if (m_electronEffChargeIDLoose_exists) top::check(m_electronEffSFChargeIDLoose->applySystematicVariation(
                                                          m_systNominal), "Failed to set systematic");
      if (m_electronEffChargeMisID_exists) top::check(m_electronEffSFChargeMisID->applySystematicVariation(
                                                        m_systNominal), "Failed to set systematic");
      if (m_electronEffChargeMisIDLoose_exists) top::check(m_electronEffSFChargeMisIDLoose->applySystematicVariation(
                                                             m_systNominal), "Failed to set systematic");

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
          double Eff_Trigger(1.), Eff_TriggerLoose(1.);
          double SF_Trigger(1.), SF_TriggerLoose(1.);
          double SF_Reco(1.);
          double SF_ID(1.), SF_IDLoose(1.);
          double SF_Isol(1.), SF_IsolLoose(1.);
          double SF_ChargeID(1.), SF_ChargeIDLoose(1.);
          double SF_ChargeMisID(1.), SF_ChargeMisIDLoose(1.);

          ///-- Get Efficiencies --///
          top::check(m_electronEffTrigger->getEfficiencyScaleFactor(*electronPtr, Eff_Trigger), "Failed to get SF");
          top::check(m_electronEffTriggerLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                         Eff_TriggerLoose), "Failed to get SF");

          ///-- Get Scale Factors --///
          top::check(m_electronEffSFTrigger->getEfficiencyScaleFactor(*electronPtr, SF_Trigger), "Failed to get SF");
          top::check(m_electronEffSFTriggerLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                           SF_TriggerLoose), "Failed to get SF");
          top::check(m_electronEffSFReco->getEfficiencyScaleFactor(*electronPtr, SF_Reco), "Failed to get SF");
          top::check(m_electronEffSFID->getEfficiencyScaleFactor(*electronPtr, SF_ID), "Failed to get SF");
          top::check(m_electronEffSFIDLoose->getEfficiencyScaleFactor(*electronPtr, SF_IDLoose), "Failed to get SF");
          if (m_electronEffIso_exists) top::check(m_electronEffSFIso->getEfficiencyScaleFactor(*electronPtr,
                                                                                               SF_Isol),
                                                  "Failed to get SF");
          if (m_electronEffIsoLoose_exists) top::check(m_electronEffSFIsoLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                                                         SF_IsolLoose),
                                                       "Failed to get SF");
          if (m_electronEffChargeID_exists) top::check(m_electronEffSFChargeID->getEfficiencyScaleFactor(*electronPtr,
                                                                                                         SF_ChargeID),
                                                       "Failed to get SF");
          if (m_electronEffChargeIDLoose_exists) top::check(m_electronEffSFChargeIDLoose->getEfficiencyScaleFactor(*
                                                                                                                   electronPtr,
                                                                                                                   SF_ChargeIDLoose),
                                                            "Failed to get SF");
          if (m_electronEffChargeMisID_exists) top::check(m_electronEffSFChargeMisID->getEfficiencyScaleFactor(*
                                                                                                               electronPtr,
                                                                                                               SF_ChargeMisID),
                                                          "Failed to get SF");
          if (m_electronEffChargeMisIDLoose_exists) top::check(m_electronEffSFChargeMisIDLoose->getEfficiencyScaleFactor(
                                                                 *electronPtr,
                                                                 SF_ChargeMisIDLoose), "Failed to get SF");


          ///-- Decorate --///
          electronPtr->auxdecor<float>(m_decor_triggerEff) = Eff_Trigger;
          electronPtr->auxdecor<float>(m_decor_triggerEff_loose) = Eff_TriggerLoose;
          electronPtr->auxdecor<float>(m_decor_triggerSF) = SF_Trigger;
          electronPtr->auxdecor<float>(m_decor_triggerSF_loose) = SF_TriggerLoose;
          electronPtr->auxdecor<float>(m_decor_recoSF) = SF_Reco;
          electronPtr->auxdecor<float>(m_decor_idSF) = SF_ID;
          electronPtr->auxdecor<float>(m_decor_idSF_loose) = SF_IDLoose;
          electronPtr->auxdecor<float>(m_decor_isoSF) = SF_Isol;
          electronPtr->auxdecor<float>(m_decor_isoSF_loose) = SF_IsolLoose;
          electronPtr->auxdecor<float>(m_decor_chargeidSF) = SF_ChargeID;
          electronPtr->auxdecor<float>(m_decor_chargeidSF_loose) = SF_ChargeIDLoose;
          electronPtr->auxdecor<float>(m_decor_chargemisidSF) = SF_ChargeMisID;
          electronPtr->auxdecor<float>(m_decor_chargemisidSF_loose) = SF_ChargeMisIDLoose;

          ///-- For nominal calibration, vary the SF systematics --///
          if (currentSystematic.first == m_config->nominalHashValue()) {
            double EFF_Trigger_UP(1.), EFF_TriggerLoose_UP(1.);
            double SF_Trigger_UP(1.), SF_TriggerLoose_UP(1.);
            double SF_Reco_UP(1.);
            double SF_ID_UP(1.), SF_IDLoose_UP(1.);
            double SF_Isol_UP(1.), SF_IsolLoose_UP(1.);
            double SF_ChargeID_UP(1.), SF_ChargeIDLoose_UP(1.);
            double SF_ChargeMisID_STAT_UP(1.), SF_ChargeMisIDLoose_STAT_UP(1.);
            double SF_ChargeMisID_SYST_UP(1.), SF_ChargeMisIDLoose_SYST_UP(1.);

            double EFF_Trigger_DOWN(1.), EFF_TriggerLoose_DOWN(1.);
            double SF_Trigger_DOWN(1.), SF_TriggerLoose_DOWN(1.);
            double SF_Reco_DOWN(1.);
            double SF_ID_DOWN(1.), SF_IDLoose_DOWN(1.);
            double SF_Isol_DOWN(1.), SF_IsolLoose_DOWN(1.);
            double SF_ChargeID_DOWN(1.), SF_ChargeIDLoose_DOWN(1.);
            double SF_ChargeMisID_STAT_DOWN(1.), SF_ChargeMisIDLoose_STAT_DOWN(1.);
            double SF_ChargeMisID_SYST_DOWN(1.), SF_ChargeMisIDLoose_SYST_DOWN(1.);

            ///-- Trigger --///
            ///-- Trigger UP --///
            top::check(m_electronEffSFTrigger->applySystematicVariation(m_systTrigger_UP), "Failed to set systematic");
            top::check(m_electronEffSFTriggerLoose->applySystematicVariation(
                         m_systTrigger_UP), "Failed to set systematic");
            top::check(m_electronEffSFTrigger->getEfficiencyScaleFactor(*electronPtr, SF_Trigger_UP),
                       "Failed to get SF");
            top::check(m_electronEffSFTriggerLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                             SF_TriggerLoose_UP), "Failed to get SF");
            top::check(m_electronEffTrigger->applySystematicVariation(m_systTrigger_UP), "Failed to set systematic");
            top::check(m_electronEffTriggerLoose->applySystematicVariation(m_systTrigger_UP),
                       "Failed to set systematic");
            top::check(m_electronEffTrigger->getEfficiencyScaleFactor(*electronPtr, EFF_Trigger_UP),
                       "Failed to get SF");
            top::check(m_electronEffTriggerLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                           EFF_TriggerLoose_UP), "Failed to get SF");

            ///-- Trigger DOWN --///
            top::check(m_electronEffSFTrigger->applySystematicVariation(m_systTrigger_DOWN),
                       "Failed to set systematic");
            top::check(m_electronEffSFTriggerLoose->applySystematicVariation(
                         m_systTrigger_DOWN), "Failed to set systematic");
            top::check(m_electronEffSFTrigger->getEfficiencyScaleFactor(*electronPtr,
                                                                        SF_Trigger_DOWN), "Failed to get SF");
            top::check(m_electronEffSFTriggerLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                             SF_TriggerLoose_DOWN), "Failed to get SF");
            top::check(m_electronEffTrigger->applySystematicVariation(m_systTrigger_DOWN), "Failed to set systematic");
            top::check(m_electronEffTriggerLoose->applySystematicVariation(
                         m_systTrigger_DOWN), "Failed to set systematic");
            top::check(m_electronEffTrigger->getEfficiencyScaleFactor(*electronPtr,
                                                                      EFF_Trigger_DOWN), "Failed to get SF");
            top::check(m_electronEffTriggerLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                           EFF_TriggerLoose_DOWN), "Failed to get SF");


            ///-- Trigger reset to nominal --///
            top::check(m_electronEffSFTrigger->applySystematicVariation(m_systNominal), "Failed to set systematic");
            top::check(m_electronEffSFTriggerLoose->applySystematicVariation(m_systNominal),
                       "Failed to set systematic");
            top::check(m_electronEffTrigger->applySystematicVariation(m_systNominal), "Failed to set systematic");
            top::check(m_electronEffTriggerLoose->applySystematicVariation(m_systNominal), "Failed to set systematic");


            ///-- Reco --///
            ///-- Reco up --///
            top::check(m_electronEffSFReco->applySystematicVariation(m_systReco_UP), "Failed to set systematic");
            top::check(m_electronEffSFReco->getEfficiencyScaleFactor(*electronPtr, SF_Reco_UP), "Failed to get SF");

            ///-- Reco down --///
            top::check(m_electronEffSFReco->applySystematicVariation(m_systReco_DOWN), "Failed to set systematic");
            top::check(m_electronEffSFReco->getEfficiencyScaleFactor(*electronPtr, SF_Reco_DOWN), "Failed to get SF");

            ///-- Reco reset to nominal --///
            top::check(m_electronEffSFReco->applySystematicVariation(m_systNominal), "Failed to set systematic");

            ///-- ID --///
            ///-- ID UP --///
            top::check(m_electronEffSFID->applySystematicVariation(m_systID_UP), "Failed to set systematic");
            top::check(m_electronEffSFIDLoose->applySystematicVariation(m_systID_UP), "Failed to set systematic");
            top::check(m_electronEffSFID->getEfficiencyScaleFactor(*electronPtr, SF_ID_UP), "Failed to get SF");
            top::check(m_electronEffSFIDLoose->getEfficiencyScaleFactor(*electronPtr, SF_IDLoose_UP),
                       "Failed to get SF");
            ///-- ID DOWN --///
            top::check(m_electronEffSFID->applySystematicVariation(m_systID_DOWN), "Failed to set systematic");
            top::check(m_electronEffSFIDLoose->applySystematicVariation(m_systID_DOWN), "Failed to set systematic");
            top::check(m_electronEffSFID->getEfficiencyScaleFactor(*electronPtr, SF_ID_DOWN), "Failed to get SF");
            top::check(m_electronEffSFIDLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                        SF_IDLoose_DOWN), "Failed to get SF");

            ///-- ID reset to nominal --///
            top::check(m_electronEffSFID->applySystematicVariation(m_systNominal), "Failed to set systematic");
            top::check(m_electronEffSFIDLoose->applySystematicVariation(m_systNominal), "Failed to set systematic");

            ///-- Iso --///
            ///-- Iso UP --///
            if (m_electronEffIso_exists) {
              top::check(m_electronEffSFIso->applySystematicVariation(m_systIso_UP), "Failed to set systematic");
              top::check(m_electronEffSFIso->getEfficiencyScaleFactor(*electronPtr, SF_Isol_UP), "Failed to get SF");
              ///-- Iso DOWN --///
              top::check(m_electronEffSFIso->applySystematicVariation(m_systIso_DOWN), "Failed to set systematic");
              top::check(m_electronEffSFIso->getEfficiencyScaleFactor(*electronPtr, SF_Isol_DOWN), "Failed to get SF");
              ///-- Iso reset to nominal --///
              top::check(m_electronEffSFIso->applySystematicVariation(m_systNominal), "Failed to set systematic");
            }
            if (m_electronEffIsoLoose_exists) {
              top::check(m_electronEffSFIsoLoose->applySystematicVariation(m_systIso_UP), "Failed to set systematic");
              top::check(m_electronEffSFIsoLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                           SF_IsolLoose_UP), "Failed to get SF");
              ///-- Iso DOWN --///
              top::check(m_electronEffSFIsoLoose->applySystematicVariation(m_systIso_DOWN), "Failed to set systematic");
              top::check(m_electronEffSFIsoLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                           SF_IsolLoose_DOWN), "Failed to get SF");
              ///-- Iso reset to nominal --///
              top::check(m_electronEffSFIsoLoose->applySystematicVariation(m_systNominal), "Failed to set systematic");
            }

            ///-- Charge ID --///
            if (m_electronEffChargeID_exists) {
              ///-- Charge ID UP --///
              top::check(m_electronEffSFChargeID->applySystematicVariation(
                           m_systChargeID_UP), "Failed to set systematic");
              top::check(m_electronEffSFChargeID->getEfficiencyScaleFactor(*electronPtr,
                                                                           SF_ChargeID_UP), "Failed to get SF");
              ///-- Charge ID DOWN --///
              top::check(m_electronEffSFChargeID->applySystematicVariation(
                           m_systChargeID_DOWN), "Failed to set systematic");
              top::check(m_electronEffSFChargeID->getEfficiencyScaleFactor(*electronPtr,
                                                                           SF_ChargeID_DOWN), "Failed to get SF");
              ///-- Charge ID reset to nominal --///
              top::check(m_electronEffSFChargeID->applySystematicVariation(m_systNominal), "Failed to set systematic");
            }
            if (m_electronEffChargeIDLoose_exists) {
              ///-- Charge ID UP --///
              top::check(m_electronEffSFChargeIDLoose->applySystematicVariation(
                           m_systChargeID_UP), "Failed to set systematic");
              top::check(m_electronEffSFChargeIDLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                                SF_ChargeIDLoose_UP),
                         "Failed to get SF");
              ///-- Charge ID DOWN --///
              top::check(m_electronEffSFChargeIDLoose->applySystematicVariation(
                           m_systChargeID_DOWN), "Failed to set systematic");
              top::check(m_electronEffSFChargeIDLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                                SF_ChargeIDLoose_DOWN),
                         "Failed to get SF");
              ///-- Charge ID reset to nominal --///
              top::check(m_electronEffSFChargeIDLoose->applySystematicVariation(
                           m_systNominal), "Failed to set systematic");
            }

            ///-- Charge Mis ID --///
            if (m_electronEffChargeMisID_exists) {
              ///-- Charge Mis ID STAT_UP --///
              top::check(m_electronEffSFChargeMisID->applySystematicVariation(
                           m_systChargeMisID_STAT_UP), "Failed to set systematic");
              top::check(m_electronEffSFChargeMisID->getEfficiencyScaleFactor(*electronPtr,
                                                                              SF_ChargeMisID_STAT_UP),
                         "Failed to get SF");
              ///-- Charge Mis ID STAT_DOWN --///
              top::check(m_electronEffSFChargeMisID->applySystematicVariation(
                           m_systChargeMisID_STAT_DOWN), "Failed to set systematic");
              top::check(m_electronEffSFChargeMisID->getEfficiencyScaleFactor(*electronPtr,
                                                                              SF_ChargeMisID_STAT_DOWN),
                         "Failed to get SF");
              ///-- Charge Mis ID SYS_UP --///
              top::check(m_electronEffSFChargeMisID->applySystematicVariation(
                           m_systChargeMisID_SYST_UP), "Failed to set systematic");
              top::check(m_electronEffSFChargeMisID->getEfficiencyScaleFactor(*electronPtr,
                                                                              SF_ChargeMisID_SYST_UP),
                         "Failed to get SF");
              ///-- Charge Mis ID SYS_DOWN --///
              top::check(m_electronEffSFChargeMisID->applySystematicVariation(
                           m_systChargeMisID_SYST_DOWN), "Failed to set systematic");
              top::check(m_electronEffSFChargeMisID->getEfficiencyScaleFactor(*electronPtr,
                                                                              SF_ChargeMisID_SYST_DOWN),
                         "Failed to get SF");
              ///-- Charge Mis ID reset to nominal --///
              top::check(m_electronEffSFChargeMisID->applySystematicVariation(m_systNominal),
                         "Failed to set systematic");
            }
            if (m_electronEffChargeMisIDLoose_exists) {
              ///-- Charge Mis ID STAT_UP --///
              top::check(m_electronEffSFChargeMisIDLoose->applySystematicVariation(
                           m_systChargeMisID_STAT_UP), "Failed to set systematic");
              top::check(m_electronEffSFChargeMisIDLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                                   SF_ChargeMisIDLoose_STAT_UP),
                         "Failed to get SF");
              ///-- Charge Mis ID STAT_DOWN --///
              top::check(m_electronEffSFChargeMisIDLoose->applySystematicVariation(
                           m_systChargeMisID_STAT_DOWN), "Failed to set systematic");
              top::check(m_electronEffSFChargeMisIDLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                                   SF_ChargeMisIDLoose_STAT_DOWN),
                         "Failed to get SF");
              ///-- Charge Mis ID SYS_UP --///
              top::check(m_electronEffSFChargeMisIDLoose->applySystematicVariation(
                           m_systChargeMisID_SYST_UP), "Failed to set systematic");
              top::check(m_electronEffSFChargeMisIDLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                                   SF_ChargeMisIDLoose_SYST_UP),
                         "Failed to get SF");
              ///-- Charge Mis ID SYS_DOWN --///
              top::check(m_electronEffSFChargeMisIDLoose->applySystematicVariation(
                           m_systChargeMisID_SYST_DOWN), "Failed to set systematic");
              top::check(m_electronEffSFChargeMisIDLoose->getEfficiencyScaleFactor(*electronPtr,
                                                                                   SF_ChargeMisIDLoose_SYST_DOWN),
                         "Failed to get SF");
              ///-- Charge Mis ID reset to nominal --///
              top::check(m_electronEffSFChargeMisIDLoose->applySystematicVariation(
                           m_systNominal), "Failed to set systematic");
            }

            ///-- Decorate --///
            electronPtr->auxdecor<float>(m_decor_triggerEff + "_UP") = EFF_Trigger_UP;
            electronPtr->auxdecor<float>(m_decor_triggerEff_loose + "_UP") = EFF_TriggerLoose_UP;
            electronPtr->auxdecor<float>(m_decor_triggerSF + "_UP") = SF_Trigger_UP;
            electronPtr->auxdecor<float>(m_decor_triggerSF_loose + "_UP") = SF_TriggerLoose_UP;
            electronPtr->auxdecor<float>(m_decor_recoSF + "_UP") = SF_Reco_UP;
            electronPtr->auxdecor<float>(m_decor_idSF + "_UP") = SF_ID_UP;
            electronPtr->auxdecor<float>(m_decor_idSF_loose + "_UP") = SF_IDLoose_UP;
            electronPtr->auxdecor<float>(m_decor_isoSF + "_UP") = SF_Isol_UP;
            electronPtr->auxdecor<float>(m_decor_isoSF_loose + "_UP") = SF_IsolLoose_UP;
            electronPtr->auxdecor<float>(m_decor_chargeidSF + "_UP") = SF_ChargeID_UP;
            electronPtr->auxdecor<float>(m_decor_chargeidSF_loose + "_UP") = SF_ChargeIDLoose_UP;
            electronPtr->auxdecor<float>(m_decor_chargemisidSF + "_STAT_UP") = SF_ChargeMisID_STAT_UP;
            electronPtr->auxdecor<float>(m_decor_chargemisidSF_loose + "_STAT_UP") = SF_ChargeMisIDLoose_STAT_UP;
            electronPtr->auxdecor<float>(m_decor_chargemisidSF + "_SYST_UP") = SF_ChargeMisID_SYST_UP;
            electronPtr->auxdecor<float>(m_decor_chargemisidSF_loose + "_SYST_UP") = SF_ChargeMisIDLoose_SYST_UP;

            electronPtr->auxdecor<float>(m_decor_triggerEff + "_DOWN") = EFF_Trigger_DOWN;
            electronPtr->auxdecor<float>(m_decor_triggerEff_loose + "_DOWN") = EFF_TriggerLoose_DOWN;
            electronPtr->auxdecor<float>(m_decor_triggerSF + "_DOWN") = SF_Trigger_DOWN;
            electronPtr->auxdecor<float>(m_decor_triggerSF_loose + "_DOWN") = SF_TriggerLoose_DOWN;
            electronPtr->auxdecor<float>(m_decor_recoSF + "_DOWN") = SF_Reco_DOWN;
            electronPtr->auxdecor<float>(m_decor_idSF + "_DOWN") = SF_ID_DOWN;
            electronPtr->auxdecor<float>(m_decor_idSF_loose + "_DOWN") = SF_IDLoose_DOWN;
            electronPtr->auxdecor<float>(m_decor_isoSF + "_DOWN") = SF_Isol_DOWN;
            electronPtr->auxdecor<float>(m_decor_isoSF_loose + "_DOWN") = SF_IsolLoose_DOWN;
            electronPtr->auxdecor<float>(m_decor_chargeidSF + "_DOWN") = SF_ChargeID_DOWN;
            electronPtr->auxdecor<float>(m_decor_chargeidSF_loose + "_DOWN") = SF_ChargeIDLoose_DOWN;
            electronPtr->auxdecor<float>(m_decor_chargemisidSF + "_STAT_DOWN") = SF_ChargeMisID_STAT_DOWN;
            electronPtr->auxdecor<float>(m_decor_chargemisidSF_loose + "_STAT_DOWN") = SF_ChargeMisIDLoose_STAT_DOWN;
            electronPtr->auxdecor<float>(m_decor_chargemisidSF + "_SYST_DOWN") = SF_ChargeMisID_SYST_DOWN;
            electronPtr->auxdecor<float>(m_decor_chargemisidSF_loose + "_SYST_DOWN") = SF_ChargeMisIDLoose_SYST_DOWN;

            ///-- Debug messages --///

            ATH_MSG_DEBUG("Electron Trigger Eff = " << electronPtr->auxdataConst<float>(m_decor_triggerEff) << " + " <<
                          electronPtr->auxdataConst<float>(m_decor_triggerEff + "_UP") << " - " <<
                          electronPtr->auxdataConst<float>(m_decor_triggerEff + "_DOWN")
                          );

            ATH_MSG_DEBUG("Electron Trigger MediumLH SF = " << electronPtr->auxdataConst<float>(
                            m_decor_triggerSF) << " + " <<
                          electronPtr->auxdataConst<float>(m_decor_triggerSF + "_UP") << " - " <<
                          electronPtr->auxdataConst<float>(m_decor_triggerSF + "_DOWN")
                          );

            ATH_MSG_DEBUG("Electron Reco SF = " << electronPtr->auxdataConst<float>(m_decor_recoSF) << " + " <<
                          electronPtr->auxdataConst<float>(m_decor_recoSF + "_UP") << " - " <<
                          electronPtr->auxdataConst<float>(m_decor_recoSF + "_DOWN")
                          );

            ATH_MSG_DEBUG("Electron ID SF = " << electronPtr->auxdataConst<float>(m_decor_idSF) << " + " <<
                          electronPtr->auxdataConst<float>(m_decor_idSF + "_UP") << " - " <<
                          electronPtr->auxdataConst<float>(m_decor_idSF + "_DOWN")
                          );

            ATH_MSG_DEBUG("Electron Charge ID SF = " << electronPtr->auxdataConst<float>(m_decor_chargeidSF) << " + " <<
                          electronPtr->auxdataConst<float>(m_decor_chargeidSF + "_UP") << " - " <<
                          electronPtr->auxdataConst<float>(m_decor_chargeidSF + "_DOWN")
                          );

            ATH_MSG_DEBUG("Electron Charge Mis-ID SF = " << electronPtr->auxdataConst<float>(
                            m_decor_chargemisidSF) << " + " <<
                          electronPtr->auxdataConst<float>(m_decor_chargemisidSF + "_STAT_UP") << " - " <<
                          electronPtr->auxdataConst<float>(m_decor_chargemisidSF + "_STAT_DOWN") << " + " <<
                          electronPtr->auxdataConst<float>(m_decor_chargemisidSF + "_SYST_UP") << " - " <<
                          electronPtr->auxdataConst<float>(m_decor_chargemisidSF + "_SYST_DOWN")
                          );


            /////now taking care of additional systematic model if needed
            if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
              std::vector<float> vec_Eff_Trigger_UP, vec_Eff_Trigger_DOWN, vec_Eff_TriggerLoose_UP,
                                 vec_Eff_TriggerLoose_DOWN;
              std::vector<float> vec_SF_Trigger_UP, vec_SF_Trigger_DOWN, vec_SF_TriggerLoose_UP,
                                 vec_SF_TriggerLoose_DOWN;
              std::vector<float> vec_SF_Reco_UP, vec_SF_Reco_DOWN;
              std::vector<float> vec_SF_ID_UP, vec_SF_ID_DOWN, vec_SF_IDLoose_UP, vec_SF_IDLoose_DOWN;
              std::vector<float> vec_SF_Isol_UP, vec_SF_Isol_DOWN, vec_SF_IsolLoose_UP, vec_SF_IsolLoose_DOWN;
              std::vector<float> vec_SF_ChargeID_UP, vec_SF_ChargeID_DOWN, vec_SF_ChargeIDLoose_UP,
                                 vec_SF_ChargeIDLoose_DOWN;
              std::vector<float> vec_SF_ChargeMisID_UP, vec_SF_ChargeMisID_DOWN, vec_SF_ChargeMisIDLoose_UP,
                                 vec_SF_ChargeMisIDLoose_DOWN;

              double EFF_Trigger(1.), EFF_TriggerLoose(1.);
              double SF_Trigger(1.), SF_TriggerLoose(1.);
              double SF_Reco(1.);
              double SF_ID(1.), SF_IDLoose(1.);
              double SF_Isol(1.), SF_IsolLoose(1.);

              /// --Trigger-- ///
              int count = 0;
              for (const CP::SystematicSet& isyst : m_systTriggerCorrModel) {
                if (count == 0) { //0 is nominal
                  count++;
                  continue;
                }

                top::check(m_electronEffSFTriggerCorrModel->applySystematicVariation(isyst),
                           "Failed to set systematic");
                top::check(m_electronEffSFTriggerLooseCorrModel->applySystematicVariation(
                             isyst), "Failed to set systematic");
                top::check(m_electronEffSFTriggerCorrModel->getEfficiencyScaleFactor(*electronPtr,
                                                                                     SF_Trigger), "Failed to get SF");
                top::check(m_electronEffSFTriggerLooseCorrModel->getEfficiencyScaleFactor(*electronPtr,
                                                                                          SF_TriggerLoose),
                           "Failed to get SF");
                top::check(m_electronEffTriggerCorrModel->applySystematicVariation(isyst), "Failed to set systematic");
                top::check(m_electronEffTriggerLooseCorrModel->applySystematicVariation(
                             isyst), "Failed to set systematic");
                top::check(m_electronEffTriggerCorrModel->getEfficiencyScaleFactor(*electronPtr,
                                                                                   EFF_Trigger), "Failed to get SF");
                top::check(m_electronEffTriggerLooseCorrModel->getEfficiencyScaleFactor(*electronPtr,
                                                                                        EFF_TriggerLoose),
                           "Failed to get SF");

                // split by down and up
                if (count % 2 == 1) {
                  vec_SF_Trigger_DOWN.emplace_back(SF_Trigger);
                  vec_SF_TriggerLoose_DOWN.emplace_back(SF_TriggerLoose);
                  vec_Eff_Trigger_DOWN.emplace_back(EFF_Trigger);
                  vec_Eff_TriggerLoose_DOWN.emplace_back(EFF_TriggerLoose);
                } else {
                  vec_SF_Trigger_UP.emplace_back(SF_Trigger);
                  vec_SF_TriggerLoose_UP.emplace_back(SF_TriggerLoose);
                  vec_Eff_Trigger_UP.emplace_back(EFF_Trigger);
                  vec_Eff_TriggerLoose_UP.emplace_back(EFF_TriggerLoose);
                }

                count++;
              }

              // Do some sanity check
              if (vec_SF_Trigger_DOWN.size() != vec_SF_Trigger_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of trigger SF for up and down are different"
                };
              }
              if (vec_SF_TriggerLoose_DOWN.size() != vec_SF_TriggerLoose_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of triggerLoose SF for up and down are different"
                };
              }
              if (vec_Eff_Trigger_DOWN.size() != vec_Eff_Trigger_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of trigger Eff for up and down are different"
                };
              }
              if (vec_Eff_TriggerLoose_DOWN.size() != vec_Eff_TriggerLoose_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of triggerLoose Eff for up and down are different"
                };
              }

              ///-- Trigger reset to nominal --///
              top::check(m_electronEffSFTriggerCorrModel->applySystematicVariation(
                           m_systNominal), "Failed to set systematic");
              top::check(m_electronEffSFTriggerLooseCorrModel->applySystematicVariation(
                           m_systNominal), "Failed to set systematic");
              top::check(m_electronEffTriggerCorrModel->applySystematicVariation(
                           m_systNominal), "Failed to set systematic");
              top::check(m_electronEffTriggerLooseCorrModel->applySystematicVariation(
                           m_systNominal), "Failed to set systematic");

              /// --Reco-- ///
              count = 0;
              for (const CP::SystematicSet& isyst : m_systRecoCorrModel) {
                if (count == 0) { //0 is nominal
                  count++;
                  continue;
                }

                top::check(m_electronEffSFRecoCorrModel->applySystematicVariation(isyst), "Failed to set systematic");
                top::check(m_electronEffSFRecoCorrModel->getEfficiencyScaleFactor(*electronPtr,
                                                                                  SF_Reco), "Failed to get SF");

                if (count % 2 == 1) {
                  vec_SF_Reco_DOWN.emplace_back(SF_Reco);
                } else {
                  vec_SF_Reco_UP.emplace_back(SF_Reco);
                }
                ++count;
              }
              if (vec_SF_Reco_DOWN.size() != vec_SF_Reco_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of Reco SF for up and down are different"
                };
              }

              ///-- Reco reset to nominal --///
              top::check(m_electronEffSFRecoCorrModel->applySystematicVariation(
                           m_systNominal), "Failed to set systematic");

              /// --ID-- ///
              count = 0;
              for (const CP::SystematicSet& isyst : m_systIDCorrModel) {
                if (count == 0) { //0 is nominal
                  count++;
                  continue;
                }

                top::check(m_electronEffSFIDCorrModel->applySystematicVariation(isyst), "Failed to set systematic");
                top::check(m_electronEffSFIDLooseCorrModel->applySystematicVariation(isyst),
                           "Failed to set systematic");
                top::check(m_electronEffSFIDCorrModel->getEfficiencyScaleFactor(*electronPtr, SF_ID),
                           "Failed to get SF");
                top::check(m_electronEffSFIDLooseCorrModel->getEfficiencyScaleFactor(*electronPtr,
                                                                                     SF_IDLoose), "Failed to get SF");

                if (count % 2 == 1) {
                  vec_SF_ID_DOWN.emplace_back(SF_ID);
                  vec_SF_IDLoose_DOWN.emplace_back(SF_IDLoose);
                } else {
                  vec_SF_ID_UP.emplace_back(SF_ID);
                  vec_SF_IDLoose_UP.emplace_back(SF_IDLoose);
                }
                ++count;
              }
              if (vec_SF_ID_DOWN.size() != vec_SF_ID_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of ID SF for up and down are different"
                };
              }
              if (vec_SF_IDLoose_DOWN.size() != vec_SF_IDLoose_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of IDLoose SF for up and down are different"
                };
              }

              ///-- ID reset to nominal --///
              top::check(m_electronEffSFIDCorrModel->applySystematicVariation(m_systNominal),
                         "Failed to set systematic");
              top::check(m_electronEffSFIDLooseCorrModel->applySystematicVariation(
                           m_systNominal), "Failed to set systematic");

              ///-- Iso --///
              count = 0;
              for (const CP::SystematicSet& isyst : m_systIsoCorrModel) {
                if (count == 0) { //0 is nominal
                  count++;
                  continue;
                }

                if (m_electronEffIso_exists) {
                  top::check(m_electronEffSFIsoCorrModel->applySystematicVariation(isyst), "Failed to set systematic");
                  top::check(m_electronEffSFIsoCorrModel->getEfficiencyScaleFactor(*electronPtr,
                                                                                   SF_Isol), "Failed to get SF");
                  ///-- Iso reset to nominal --///
                  top::check(m_electronEffSFIsoCorrModel->applySystematicVariation(
                               m_systNominal), "Failed to set systematic");
                }
                if (m_electronEffIsoLoose_exists) {
                  top::check(m_electronEffSFIsoLooseCorrModel->applySystematicVariation(
                               isyst), "Failed to set systematic");
                  top::check(m_electronEffSFIsoLooseCorrModel->getEfficiencyScaleFactor(*electronPtr,
                                                                                        SF_IsolLoose),
                             "Failed to get SF");
                  ///-- Iso reset to nominal --///
                  top::check(m_electronEffSFIsoLooseCorrModel->applySystematicVariation(
                               m_systNominal), "Failed to set systematic");
                }
                if (count % 2 == 1) {
                  vec_SF_Isol_DOWN.emplace_back(SF_Isol);
                  vec_SF_IsolLoose_DOWN.emplace_back(SF_IsolLoose);
                } else {
                  vec_SF_Isol_UP.emplace_back(SF_Isol);
                  vec_SF_IsolLoose_UP.emplace_back(SF_IsolLoose);
                }
                ++count;
              }
              if (vec_SF_Isol_DOWN.size() != vec_SF_Isol_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of Isol SF for up and down are different"
                };
              }
              if (vec_SF_IsolLoose_DOWN.size() != vec_SF_IsolLoose_UP.size()) {
                throw std::runtime_error {
                        "ElectronScaleFactorCalculator::execute: Sizes of IsolLoose SF for up and down are different"
                };
              }


              ///-- Decorate --///

              electronPtr->auxdecor<std::vector<float> >(m_decor_triggerEff + "_CorrModel_UP") = vec_Eff_Trigger_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_triggerEff_loose +
                                                         "_CorrModel_UP") = vec_Eff_TriggerLoose_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_triggerSF + "_CorrModel_UP") = vec_SF_Trigger_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_triggerSF_loose +
                                                         "_CorrModel_UP") = vec_SF_TriggerLoose_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_recoSF + "_CorrModel_UP") = vec_SF_Reco_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_idSF + "_CorrModel_UP") = vec_SF_ID_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_idSF_loose + "_CorrModel_UP") = vec_SF_IDLoose_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_isoSF + "_CorrModel_UP") = vec_SF_Isol_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_isoSF_loose + "_CorrModel_UP") = vec_SF_IsolLoose_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_chargeidSF + "_CorrModel_UP") = vec_SF_ChargeID_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_chargeidSF_loose +
                                                         "_CorrModel_UP") = vec_SF_ChargeIDLoose_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_chargemisidSF +
                                                         "_CorrModel_UP") = vec_SF_ChargeMisID_UP;
              electronPtr->auxdecor<std::vector<float> >(m_decor_chargemisidSF_loose +
                                                         "_CorrModel_UP") = vec_SF_ChargeMisIDLoose_UP;

              electronPtr->auxdecor<std::vector<float> >(m_decor_triggerEff + "_CorrModel_DOWN") = vec_Eff_Trigger_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_triggerEff_loose +
                                                         "_CorrModel_DOWN") = vec_Eff_TriggerLoose_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_triggerSF + "_CorrModel_DOWN") = vec_SF_Trigger_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_triggerSF_loose +
                                                         "_CorrModel_DOWN") = vec_SF_TriggerLoose_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_recoSF + "_CorrModel_DOWN") = vec_SF_Reco_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_idSF + "_CorrModel_DOWN") = vec_SF_ID_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_idSF_loose + "_CorrModel_DOWN") = vec_SF_IDLoose_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_isoSF + "_CorrModel_DOWN") = vec_SF_Isol_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_isoSF_loose +
                                                         "_CorrModel_DOWN") = vec_SF_IsolLoose_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_chargeidSF + "_CorrModel_DOWN") = vec_SF_ChargeID_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_chargeidSF_loose +
                                                         "_CorrModel_DOWN") = vec_SF_ChargeIDLoose_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_chargemisidSF +
                                                         "_CorrModel_DOWN") = vec_SF_ChargeMisID_DOWN;
              electronPtr->auxdecor<std::vector<float> >(m_decor_chargemisidSF_loose +
                                                         "_CorrModel_DOWN") = vec_SF_ChargeMisIDLoose_DOWN;
            }//end of saving additional systematic model


            /////////////////////////////////////////
          } // Calibration systematic is nominal, so calculate SF systematics
        }
      }
    }


    return StatusCode::SUCCESS;
  }
}
