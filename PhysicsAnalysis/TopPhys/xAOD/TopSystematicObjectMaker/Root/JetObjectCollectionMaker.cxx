/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetObjectCollectionMaker.cxx 809674 2017-08-23 14:10:24Z iconnell $
#include "TopSystematicObjectMaker/JetObjectCollectionMaker.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/TreeFilter.h"
#include "TopEvent/EventTools.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODMissingET/MissingETContainer.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "TopJetSubstructure/TopJetSubstructure.h"
#include "TopJetSubstructure/LargeJetTrimmer.h"
#include "TopJetSubstructure/SubjetMaker.h"

namespace top {
  JetObjectCollectionMaker::JetObjectCollectionMaker(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),
    m_doJER(false),
    m_doFull_JER(false),
    m_doFull_JER_Pseudodata(false),
    m_isMC(false),
    m_doMultipleJES(false),

    m_specifiedSystematics(),
    m_specifiedSystematicsLargeR(),
    m_specifiedSystematicsTrackJets(),
    m_recommendedSystematics(),
    m_nominalSystematicSet(),

    m_jetCalibrationTool("JetCalibrationTool"),
    m_jetCalibrationToolLargeR("JetCalibrationToolLargeR"),

    m_jetUncertaintiesTool("JetUncertaintiesTool"),
    m_jetUncertaintiesToolFrozenJMS("JetUncertaintiesToolFrozenJMS"),
    m_jetUncertaintiesToolReducedNPScenario1("JetUncertaintiesToolReducedNPScenario1"),
    m_jetUncertaintiesToolReducedNPScenario2("JetUncertaintiesToolReducedNPScenario2"),
    m_jetUncertaintiesToolReducedNPScenario3("JetUncertaintiesToolReducedNPScenario3"),
    m_jetUncertaintiesToolReducedNPScenario4("JetUncertaintiesToolReducedNPScenario4"),
    m_jetUncertaintiesToolLargeR("JetUncertaintiesToolLargeR"),

    m_jetUpdateJvtTool("JetUpdateJvtTool"),
    m_jetSelectfJvtTool("JetSelectfJvtTool"),

    m_jetSubstructure(nullptr),

    m_systMap_AllNP(),
    m_systMap_AllNP_FrozenJMS(),
    m_systMap_ReducedNPScenario1(),
    m_systMap_ReducedNPScenario2(),
    m_systMap_ReducedNPScenario3(),
    m_systMap_ReducedNPScenario4(),
    m_systMap_LargeR() {
    declareProperty("config", m_config);

    declareProperty("JetCalibrationTool", m_jetCalibrationTool);
    declareProperty("JetCalibrationToolLargeR", m_jetCalibrationToolLargeR);

    declareProperty("JetUncertaintiesTool", m_jetUncertaintiesTool);
    declareProperty("JetUncertaintiesToolFrozenJMS", m_jetUncertaintiesToolFrozenJMS);
    declareProperty("JetUncertaintiesToolReducedNPScenario1", m_jetUncertaintiesToolReducedNPScenario1);
    declareProperty("JetUncertaintiesToolReducedNPScenario2", m_jetUncertaintiesToolReducedNPScenario2);
    declareProperty("JetUncertaintiesToolReducedNPScenario3", m_jetUncertaintiesToolReducedNPScenario3);
    declareProperty("JetUncertaintiesToolReducedNPScenario4", m_jetUncertaintiesToolReducedNPScenario4);
    declareProperty("JetUncertaintiesToolLargeR", m_jetUncertaintiesToolLargeR);

    declareProperty("JetUpdateJvtTool", m_jetUpdateJvtTool);

    declareProperty("TruthJetCollectionForHSTagging", m_truthJetCollForHS = "AntiKt4TruthJets");
  }

  StatusCode JetObjectCollectionMaker::initialize() {
    ATH_MSG_INFO(" top::JetObjectCollectionMaker initialize");

    ///-- Lets do the nominal systematics --///
    ///-- JetObjectCollectionMaker is a little different from the others --///
    m_specifiedSystematics.push_back(CP::SystematicSet());
    m_specifiedSystematicsLargeR.push_back(CP::SystematicSet());
    m_specifiedSystematicsTrackJets.push_back(CP::SystematicSet());
    m_recommendedSystematics.push_back(CP::SystematicSet());


    top::check(m_jetCalibrationTool.retrieve(), "Failed to retrieve JetCalibrationTool");
    if (m_config->useLargeRJets()) {
      top::check(m_jetCalibrationToolLargeR.retrieve(),
                 "Failed to retrieve JetCalibrationToolLargeR");
      top::check(m_jetUncertaintiesToolLargeR.retrieve(),
                 "Failed to retrieve JetUncertaintiesToolLargeR");
    }

    if (m_config->getDerivationStream() == "PHYS") {
      m_truthJetCollForHS = "AntiKt4TruthDressedWZJets";
    }

    ///-- JER uncertainties model --///
    if (m_config->jetJERSmearingModel() == "Full" || m_config->jetJERSmearingModel() == "All") m_doFull_JER = true;
    if (m_config->jetJERSmearingModel() == "Simple") m_doFull_JER = false;
    if (m_config->jetJERSmearingModel() == "Full_PseudoData" ||
        m_config->jetJERSmearingModel() == "All_PseudoData") m_doFull_JER_Pseudodata = true;
    else m_doFull_JER_Pseudodata = false;


    m_isMC = m_config->isMC();
    m_doMultipleJES = m_config->doMultipleJES();

    ///-- Are we doing JER? Let's not repeat this logic over and over --///
    if (m_isMC) m_doJER = true;
    if (!m_isMC && m_doFull_JER) m_doJER = true;
    /// NB: for "Full_PseudoData", no need to smear the data, so m_doJER is false on data unless it's "Full"

    if (m_isMC || m_doFull_JER) {
      if (!m_doMultipleJES) {
        top::check(m_jetUncertaintiesTool.retrieve(), "Failed to retrieve JetUncertaintiesTool");
        if (m_config->jetCalibSequence() == "JMS") {
          top::check(m_jetUncertaintiesToolFrozenJMS.retrieve(), "Failed to retrieve JetUncertaintiesTool (FrozenJMS)");
        }
      } else {
        top::check(
          m_jetUncertaintiesToolReducedNPScenario1.retrieve(),
          "Failed to retrieve JetUncertaintiesToolReducedNPScenario1");
        top::check(
          m_jetUncertaintiesToolReducedNPScenario2.retrieve(),
          "Failed to retrieve JetUncertaintiesToolReducedNPScenario2");
        top::check(
          m_jetUncertaintiesToolReducedNPScenario3.retrieve(),
          "Failed to retrieve JetUncertaintiesToolReducedNPScenario3");
        top::check(
          m_jetUncertaintiesToolReducedNPScenario4.retrieve(),
          "Failed to retrieve JetUncertaintiesToolReducedNPScenario4");
      }
    }






    top::check(m_jetUpdateJvtTool.retrieve(), "Failed to retrieve JetUpdateJvtTool");
    //fJVT tool isn't setup unless requested
    if (m_config->doForwardJVTinMET() || m_config->getfJVTWP() != "None") {
      top::check(m_jetSelectfJvtTool.retrieve(), "Failed to retrieve JetSelectfJvtTool");
    }
    // Take this from the TopConfiguration
    // A blank vector will setup all systematics

    const std:: string& syststr = m_config->systematics();
    std::set<std::string> syst, systLargeR;

    if (!m_config->isSystNominal(syststr) && !m_config->isSystAll(syststr)) {
      bool ok = m_config->getSystematicsList(syststr, syst);
      bool okLargeR = m_config->getSystematicsList(syststr, systLargeR);
      if (!ok || !okLargeR) {
        ATH_MSG_ERROR(" top::JetObjectCollectionMaker could not determine systematic list");
        return StatusCode::FAILURE;
      }
      //here the idea is that if the user specifies AllXXX, we leave syst as an empty string, so that all recommended CP
      // systematics are then used
      if (m_config->contains(syst, "AllJets")) {
        syst.clear();
        systLargeR.clear();
      }
      if (m_config->contains(syst, "AllSmallRJets")) {
        syst.clear();
      }
      if (m_config->contains(systLargeR, "AllLargeRJets")) {
        systLargeR.clear();
      }
    }


    ///-- JES systematics --///
    if (m_isMC || m_doFull_JER) {
      std::string allNP(m_config->jetUncertainties_NPModel() + "_"),
      np1("SR_Scenario1_"), np2("SR_Scenario2_"), np3("SR_Scenario3_"), np4("SR_Scenario4_");
      std::string allNP_FrozenJMS(m_config->jetUncertainties_NPModel() + "_FrozenJMS_");

      bool onlyJER = ((!m_isMC) && m_doFull_JER) || (m_isMC && m_doFull_JER_Pseudodata);

      if (!m_doMultipleJES) {
        addSystematics(syst, m_jetUncertaintiesTool->recommendedSystematics(), m_systMap_AllNP, allNP, false, onlyJER);
        if (m_config->jetCalibSequence() == "JMS") {
          addSystematics(syst,
                         m_jetUncertaintiesToolFrozenJMS->recommendedSystematics(), m_systMap_AllNP_FrozenJMS, allNP_FrozenJMS, false,
                         onlyJER);
        }
      } else {
        addSystematics(syst,
                       m_jetUncertaintiesToolReducedNPScenario1->recommendedSystematics(), m_systMap_ReducedNPScenario1, np1, false,
                       onlyJER);
        addSystematics(syst,
                       m_jetUncertaintiesToolReducedNPScenario2->recommendedSystematics(), m_systMap_ReducedNPScenario2, np2, false,
                       onlyJER);
        addSystematics(syst,
                       m_jetUncertaintiesToolReducedNPScenario3->recommendedSystematics(), m_systMap_ReducedNPScenario3, np3, false,
                       onlyJER);
        addSystematics(syst,
                       m_jetUncertaintiesToolReducedNPScenario4->recommendedSystematics(), m_systMap_ReducedNPScenario4, np4, false,
                       onlyJER);
      }
    }
    ///-- Large-R JES systematics --///
    std::string largeRModName = getLargeRModName(m_config->largeRJetUncertainties_NPModel());
    CP::SystematicSet largeRsysts;
    if (m_config->useLargeRJets() && m_config->isMC()) { //No JES uncertainties for Data at the moment
      if ((m_config->largeRJESJMSConfig() == "CombMass") || (m_config->largeRJESJMSConfig() == "TCCMass")) { //TA mass
                                                                                                             // and
                                                                                                             // combined
                                                                                                             // mass not
                                                                                                             // supported
                                                                                                             // for
                                                                                                             // JMS/JMR
                                                                                                             // for now
        largeRsysts.insert(m_jetUncertaintiesToolLargeR->recommendedSystematics());
      } else {
        ATH_MSG_WARNING(
          "TA Mass & Calo Mass are not supported for large-R jet uncertainties at the moment. Large-R jet systemtatics skipped!");
      }
    }

    ///-- Large R jet tagger scale factor uncertainties -- ///
    if (m_config->isMC() && m_config->useLargeRJets()) {
      for (const std::pair<std::string, std::string>& name : m_config->boostedTaggerSFnames()) {
        ToolHandle<ICPJetUncertaintiesTool> tmp_SF_uncert_tool("JetSFuncert_" + name.first);
        if (tmp_SF_uncert_tool.retrieve()) {
          largeRsysts.insert(tmp_SF_uncert_tool->recommendedSystematics());
          m_tagSFuncertTool[name.first] = tmp_SF_uncert_tool;
        }
      }
    }

    // add the merged set of systematics for large-R jets including the tagging SF systs
    addSystematics(systLargeR, largeRsysts, m_systMap_LargeR, largeRModName, true);

    ///-- Large R jet substructure --///
    if (m_config->jetSubstructureName() == "Trimmer") m_jetSubstructure.reset(new top::LargeJetTrimmer);

    if (m_config->jetSubstructureName() == "SubjetMaker") m_jetSubstructure.reset(new top::SubjetMaker);

    ///-- Large R jet truth labeling --///
    m_jetTruthLabelingTool = nullptr;
    if (m_config->isMC() && m_config->useLargeRJets()) {
      m_jetTruthLabelingTool = std::unique_ptr<JetTruthLabelingTool>(new JetTruthLabelingTool("JetTruthLabeling"));
      // For DAOD_PHYS we need to pass few more arguments as it uses TRUTH3
      if (m_config->getDerivationStream() == "PHYS") {
        top::check(m_jetTruthLabelingTool->setProperty("UseTRUTH3", true), "Failed to set UseTRUTH3 for m_jetTruthLabelingTool");
        top::check(m_jetTruthLabelingTool->setProperty("TruthBosonContainerName", "TruthBoson"), "Failed to set truth container name for m_jetTruthLabelingTool");
        top::check(m_jetTruthLabelingTool->setProperty("TruthTopQuarkContainerName", "TruthTop"), "Failed to set truth container name for m_jetTruthLabelingTool");
      }
      top::check(m_jetTruthLabelingTool->initialize(), "Failed to initialize m_jetTruthLabelingTool");
    }

    // set the systematics list
    m_config->systematicsJets(specifiedSystematics());
    m_config->systematicsLargeRJets(specifiedSystematicsLargeR());
    m_config->systematicsTrackJets(m_specifiedSystematicsTrackJets);

    ///-- DL1 Decoration --///
    for (const std::pair<std::string, std::string>& algo : m_config->bTagAlgo_selToolNames()) {
      m_btagSelToolsDL1Decor[algo.first] = algo.second;
      top::check(m_btagSelToolsDL1Decor[algo.first].retrieve(), "Failed to retrieve " + algo.first + " btagging selector for " + m_config->sgKeyJets() + ". This is required for b-tagging score decorations in EventSaver!");
      if (DLx.count(algo.first) == 0) {
        DLx.emplace(algo.first, SG::AuxElement::Decorator<float>("AnalysisTop_" + algo.first));
      }
    }

    if (m_config->useTrackJets()) {
      for (const std::pair<std::string, std::string>& algo : m_config->bTagAlgo_selToolNames_trkJet()) {
        m_btagSelToolsDL1Decor_trkJet[algo.first] = algo.second;
        top::check(m_btagSelToolsDL1Decor_trkJet[algo.first].retrieve(), "Failed to retrieve " + algo.first + " btagging selector for " + m_config->sgKeyTrackJets() + ". This is required for b-tagging score decorations in EventSaver!");
        if (DLx.count(algo.first) == 0) {
          DLx.emplace(algo.first, SG::AuxElement::Decorator<float>("AnalysisTop_" + algo.first));
        }
      }
    }

    // initialize boosted jet taggers -- we have to do it here instead pf TopObjectSelectionTools
    // because we have to apply tagger inbetween JES uncertainty tool and the tagging SF tool
    if (m_config->useLargeRJets()) {
      for (const std::pair<std::string, std::string>& name : m_config->boostedJetTaggers()) {
        std::string fullName = name.first + "_" + name.second;
        m_boostedJetTaggers[fullName] = ToolHandle<IJetSelectorTool>(fullName);
        top::check(m_boostedJetTaggers[fullName].retrieve(), "Failed to retrieve " + fullName);
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::executeJets(bool executeNominal) {
    bool isLargeR(false);

    return execute(isLargeR, executeNominal);
  }

  StatusCode JetObjectCollectionMaker::executeLargeRJets(bool executeNominal) {
    bool isLargeR(true);

    return execute(isLargeR, executeNominal);
  }

  StatusCode JetObjectCollectionMaker::execute(const bool isLargeR, bool executeNominal) {
    ///-- Run nominal first, if executing nominal
    if (executeNominal) {
      // decorating the HS jets with truth info on which are HS jets
      if (!isLargeR & m_isMC) {
        top::check(
          decorateHSJets(),
          "Failed to decorate jets with truth info of which are HS - this is needed for JVT scale-factors!");
        if (m_isMC && m_config->jetResponseMatchingDeltaR() > 0) {
          top::check(decorateMatchedTruth(), "Failed to decorate matched jet");
        }
      }

      // Decorate the DL1 variable
      top::check(decorateDL1(), "Failed to decorate jets with DL1 b-tagging discriminant");

      ///-- First calibrate the nominal jets, everything else comes from this, so let's only do it once not 3000 times
      // --///
      top::check(calibrate(isLargeR), "Failed to calibrate jets");

      ///-- Return after calibrating the nominal --///
      return StatusCode::SUCCESS;
    }

    ///-- Systematics from here --///

    ///-- JES, JER regular atk4 for now --///
    if (!isLargeR) {
      ///-- JES --///
      if (m_isMC || m_doFull_JER) {
        if (!m_doMultipleJES) {
          top::check(applySystematic(m_jetUncertaintiesTool, m_systMap_AllNP), "Failed to apply JES");
          if (m_config->jetCalibSequence() == "JMS") {
            top::check(applySystematic(m_jetUncertaintiesToolFrozenJMS,
                                       m_systMap_AllNP_FrozenJMS), "Failed to apply JES Frozen JMS");
          }
        }
        if (m_doMultipleJES) {
          top::check(applySystematic(m_jetUncertaintiesToolReducedNPScenario1,
                                     m_systMap_ReducedNPScenario1), "Failed to apply JES");
          top::check(applySystematic(m_jetUncertaintiesToolReducedNPScenario2,
                                     m_systMap_ReducedNPScenario2), "Failed to apply JES");
          top::check(applySystematic(m_jetUncertaintiesToolReducedNPScenario3,
                                     m_systMap_ReducedNPScenario3), "Failed to apply JES");
          top::check(applySystematic(m_jetUncertaintiesToolReducedNPScenario4,
                                     m_systMap_ReducedNPScenario4), "Failed to apply JES");
        }
      }
    } else {
      // tag calibrated (nominal) jets -- the tagging information will be available
      // for systematically-shifted shallow copies as well
      top::check(tagNominalLargeRJets(), "Failed to tag large-R jets");
      if (m_config->isMC()) {
        top::check(applySystematic(m_jetUncertaintiesToolLargeR, m_systMap_LargeR,
                                   true), "Failed to apply large-R syst.");
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::calibrate(const bool isLargeR) {
    ///-- Get base jets from xAOD --///
    std::string sgKey = isLargeR ? m_config->sgKeyLargeRJets() : m_config->sgKeyJets();

    const xAOD::JetContainer* xaod(nullptr);
    top::check(evtStore()->retrieve(xaod, sgKey), "Failed to retrieve Jets");

    ///-- Shallow copy of the xAOD --///
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer(*xaod);

    ///-- Loop over the xAOD Container --///
    for (auto jet : *(shallow_xaod_copy.first)) {
      if (!isLargeR) { ///-- small-R jets used in most analyses --///
        ///-- Calibrate jet --///

        top::check(m_jetCalibrationTool->applyCalibration(*jet), "Failed to applyCalibration");

        // only multiply by JSF and bJSF if one of them != 1.0 (used by top mass analysis)
        float JSF = m_config->JSF();
        float bJSF = m_config->bJSF();

        if (JSF != 1.0 || bJSF != 1.0) {
          int truthflav = -1;
          if (jet->isAvailable<int>("PartonTruthLabelID")) {
            jet->getAttribute("PartonTruthLabelID", truthflav);
          }

          xAOD::JetFourMom_t jet_p4 = jet->jetP4() * JSF;
          if (truthflav == 5) jet_p4 = jet_p4 * bJSF;

          jet->setJetP4(jet_p4);
        }
        // end application JSF/bJSF


        top::check(decorateBJets(*jet), "Failed to decorate if b-jet");
      }

      if (isLargeR && m_jetSubstructure.get() != nullptr) {
        m_jetSubstructure->correctJet(*jet);
      }

      if (isLargeR) {
        ///-- Calibrate jet --///

        float tau3 = jet->getAttribute<float>("Tau3_wta");
        float tau2 = jet->getAttribute<float>("Tau2_wta");
        float tau1 = jet->getAttribute<float>("Tau1_wta");
        float ECF1 = jet->getAttribute<float>("ECF1");
        float ECF2 = jet->getAttribute<float>("ECF2");
        float ECF3 = jet->getAttribute<float>("ECF3");

        jet->auxdecor<float>("Tau32_wta") = fabs(tau2) > 1.e-6 ? (tau3 / tau2) : -999;  // 999 to match
                                                                                        // JetSubStructureMomentTools/NSubjettinessRatiosTool
        jet->auxdecor<float>("Tau21_wta") = fabs(tau1) > 1.e-6 ? (tau2 / tau1) : -999;  // 999 to match
                                                                                        // JetSubStructureMomentTools/NSubjettinessRatiosTool
        // Same definition as in JetSubStructureMomentTools/Root/EnergyCorrelatorRatiosTool.cxx
        jet->auxdecor<float>("D2") = (ECF2 > 1e-8) ? (ECF3*ECF1*ECF1*ECF1) / (ECF2*ECF2*ECF2) : -999;
        jet->auxdecor<float>("C2") = (ECF2 > 1e-8) ? (ECF3*ECF1) / (ECF2*ECF2) : -999;
        jet->auxdecor<float>("E3") = (ECF1 > 1e-8) ? ECF3 / (ECF1*ECF1*ECF1) : -999;
                                                                                      

        top::check(m_jetCalibrationToolLargeR->applyCalibration(*jet), "Failed to applyCalibration");

        ///-- for TA mass or calo mass, the calibrated mass or pt needs special treatment --///
        const std::string calibChoice = m_config->largeRJESJMSConfig();
        if (m_config->isMC()) {
          ///-- Truth labeling required by the large-R jet uncertainties --///
          top::check(m_jetTruthLabelingTool->modifyJet(*jet), "Failed to do truth labeling for large-R jet");
          if (calibChoice == "TAMass") {
            xAOD::JetFourMom_t jet_calib_p4;
            jet->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA", jet_calib_p4);
            jet->setJetP4(jet_calib_p4);
          }
        } else { //For data, there's only one config file so special method is required for TA mass and Calos mass
          if (calibChoice == "CaloMass") {
            xAOD::JetFourMom_t jetInsituP4_calo;
            jet->getAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentumCalo", jetInsituP4_calo);
            jet->setJetP4(jetInsituP4_calo);
          } else if (calibChoice == "TAMass") {
            xAOD::JetFourMom_t jetInsituP4_ta;
            jet->getAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentumTA", jetInsituP4_ta);
            jet->setJetP4(jetInsituP4_ta);
          }
        }
      }

      ///-- Update JVT --///
      if (!isLargeR) {
        jet->auxdecor<float>("AnalysisTop_JVT") = m_jetUpdateJvtTool->updateJvt(*jet);
      }
    }

    // Check if the derivation we are running on contains
    // MET_Track (once), if so apply the fJVT decoration
    // if not then don't
    if (!isLargeR && (m_config->doForwardJVTinMET() || m_config->getfJVTWP() != "None")) {
      static bool checked_track_MET = false;
      if (!checked_track_MET) {
	if (evtStore()->contains<xAOD::MissingETContainer>("MET_Track")) {
	  m_do_fjvt = true;
	} else {
	  ATH_MSG_ERROR(" Cannot retrieve MET_Track, fJVT values can't be calculated correctly!!"); 
	  return StatusCode::FAILURE; 
	}
	checked_track_MET = true;
      }
    }
    if (m_do_fjvt) {
      top::check(!m_jetSelectfJvtTool->modify(*shallow_xaod_copy.first),
                 "Failed to apply fJVT decoration");
    }

    if (!isLargeR) {
      ///-- Save calibrated jet to TStore --///
      ///-- set links to original objects- needed for MET calculation --///
      // NOTE, if we use one of the b-tagging re-trained collections, we need to load
      // the original uncalibrated jet container to which the b-tagging shallow-copy is pointing to
      const xAOD::JetContainer* xaod_original(nullptr);
      // for small-R jet collections, set links to uncalibrated *original* jets (not BTagging shallow-copy)
      if (m_config->sgKeyJets() != m_config->sgKeyJetsType()) {
        top::check(evtStore()->retrieve(xaod_original,
                                        m_config->sgKeyJetsType()),
                   "Failed to retrieve uncalibrated Jets for METMaker!");
      } else {
        xaod_original = xaod;
      }
      bool setLinks = xAOD::setOriginalObjectLink(*xaod_original, *shallow_xaod_copy.first);
      if (!setLinks) ATH_MSG_ERROR(" Cannot set original object links for jets, MET recalculation may struggle");
    }

    ///-- Save corrected xAOD Container to StoreGate / TStore --///
    std::string outputSGKey;
    if (!isLargeR) {
      outputSGKey = m_config->sgKeyJetsStandAlone(m_nominalSystematicSet.hash());
    }
    if (isLargeR) {
      outputSGKey = m_config->sgKeyLargeRJets(m_nominalSystematicSet.hash());
    }

    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);
    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::applySystematic(ToolHandle<ICPJetUncertaintiesTool>& tool,
                                                       const std::unordered_map<CP::SystematicSet,
                                                                                CP::SystematicSet>& map,
                                                       bool isLargeR) {
    ///-- Get calibrated jets --///
    const xAOD::JetContainer* xaod(nullptr);

    if (!isLargeR) {
      top::check(evtStore()->retrieve(xaod, m_config->sgKeyJetsStandAlone(
                                        m_nominalSystematicSet.hash())), "Failed to retrieve Jets");
    } else {
      top::check(evtStore()->retrieve(xaod, m_config->sgKeyLargeRJets(
                                        m_nominalSystematicSet.hash())), "Failed to retrieve Jets");
    }

    ///-- Loop over the systematics --///

    for (Itr syst = map.begin(); syst != map.end(); ++syst) {
      ///-- Don't do the nominal, we've already done that --///
      if ((*syst).second.hash() != m_nominalSystematicSet.hash()) {
        ///-- Tell the tool which systematic to use --///
        ///-- Here we use the second, original CP::SystematicSet --///
        top::check(tool->applySystematicVariation((*syst).second),
                   "Failed to applySystematicVariation");

        if (isLargeR && m_config->isMC()) {
          // for boosted tagging SFs, apply syst variation for all initialized WPs
          for (std::pair<const std::string, ToolHandle<ICPJetUncertaintiesTool> >& tagSF : m_tagSFuncertTool) {
            top::check(tagSF.second->applySystematicVariation((*syst).second), "Failed to applySystematicVariation");
          }
        }

        ///-- Shallow copy of the xAOD --///
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* >
        shallow_xaod_copy = xAOD::shallowCopyContainer(*xaod);

        ///-- Loop over the xAOD Container --///
        for (auto jet : *(shallow_xaod_copy.first)) {
          if (isLargeR && m_config->isMC()) { //JES for large-R jets only exist for MC
            ///-- Only large R jets with the following properties can be calibrated.--///
            bool calibratable_jet = (std::fabs(jet->eta()) <= 2.0
                                     && jet->pt() > 175e3); //lower than 200e3 to allow studying on migration
            std::string jetCalibrationNameLargeR = m_config->sgKeyLargeRJets();
            if (jetCalibrationNameLargeR.find("TrackCaloCluster") != std::string::npos) { //TCC jet
              calibratable_jet = (jet->m() / jet->pt() <= 1.
                                  && jet->m() / jet->pt() >= 0.
                                  && std::fabs(jet->eta()) <= 2.0
                                  && jet->pt() > 150e3
                                  && jet->pt() < 3000e3);
            }
            if (!calibratable_jet) continue;
          }
          ///-- Apply Corrrection --///
          if (!(isLargeR && !m_config->isMC())) { //Large-R jet uncertainties don't apply to Data
            top::check(tool->applyCorrection(*jet), "Failed to applyCorrection");
          }
          ///-- Update JVT --///
          if (!isLargeR) jet->auxdecor<float>("AnalysisTop_JVT") = m_jetUpdateJvtTool->updateJvt(*jet);
	  
          ///-- Decorate fJVT for systematics too --///
	  // Check if the derivation we are running on contains
	  // MET_Track (once) before applying the fJVT decoration
	  if (!isLargeR && (m_config->doForwardJVTinMET() || m_config->getfJVTWP() != "None")) {
	    static bool checked_track_MET = false;
	    if (!checked_track_MET) {
	      if (evtStore()->contains<xAOD::MissingETContainer>("MET_Track")) {
		m_do_fjvt = true;
	      } else {
		ATH_MSG_ERROR(" Cannot retrieve MET_Track, fJVT values can't be calculated correctly!!"); 
		return StatusCode::FAILURE;
	      }
	      checked_track_MET = true;
	    }
	  }
	  if (m_do_fjvt) {
	    top::check(!m_jetSelectfJvtTool->modify(*shallow_xaod_copy.first),
		       "Failed to apply fJVT decoration");
	  }


          ///-- Apply large-R jet tagging SF uncertainties --///
          if (isLargeR && m_config->isMC()) {
            for (std::pair<const std::string, ToolHandle<ICPJetUncertaintiesTool> >& tagSF : m_tagSFuncertTool) {
              top::check(tagSF.second->applyCorrection(*jet), "Failed to applyCorrection");
            }
          }
        }

        ///-- set links to original objects- needed for MET calculation --///
        bool setLinks = xAOD::setOriginalObjectLink(*xaod,
                                                    *shallow_xaod_copy.first);
        if (!setLinks) ATH_MSG_ERROR(" Cannot set original object links"
                                     " for jets, MET recalculation may struggle");

        ///-- Save corrected xAOD Container to StoreGate / TStore --///
        ///-- Here we use the first, AnalysisTop modified CP::SystematicSer --///
        ///-- This allows us to run multiple JES scenarios, which all have the same hash values --///
        std::string outputSGKey;
        if (isLargeR) {
          outputSGKey = m_config->sgKeyLargeRJets((*syst).first.hash());
        } else {
          outputSGKey = m_config->sgKeyJetsStandAlone((*syst).first.hash());
        }
        std::string outputSGKeyAux = outputSGKey + "Aux.";

        xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
        xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);
        if (!save || !saveAux) {
          return StatusCode::FAILURE;
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::executeTrackJets(bool executeNominal) {
    ///-- No calibrations or systematics yet --///
    ///-- Only run this on the nominal execution --///
    if (!executeNominal) return StatusCode::SUCCESS;

    top::check(decorateDL1(true), "Failed to decorate track jets with DL1 b-tagging discriminant");


    ///-- Just make a shallow copy to keep these in line with everything else --///

    const xAOD::JetContainer* xaod(nullptr);
    top::check(evtStore()->retrieve(xaod, m_config->sgKeyTrackJets()), "Failed to retrieve Jets");

    ///-- Shallow copy of the xAOD --///
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer(*xaod);

    ///-- set links to original objects --///
    bool setLinks = xAOD::setOriginalObjectLink(*xaod, *shallow_xaod_copy.first);
    if (!setLinks) ATH_MSG_ERROR(" Cannot set original object links for track jets");

    ///-- Save corrected xAOD Container to StoreGate / TStore --///
    std::string outputSGKey = m_config->sgKeyTrackJets(m_config->nominalHashValue());
    std::string outputSGKeyAux = outputSGKey + "Aux.";

    xAOD::TReturnCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
    xAOD::TReturnCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);
    if (!save || !saveAux) {
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::printoutJets() {
    bool isLargeR(false);

    return this->printout(isLargeR);
  }

  StatusCode JetObjectCollectionMaker::printoutLargeRJets() {
    bool isLargeR(true);

    return this->printout(isLargeR);
  }

  StatusCode JetObjectCollectionMaker::printout(const bool isLargeR) {
    ///-- Loop over all systematics --///
    for (auto s : m_specifiedSystematics) {
      std::string sgKey = isLargeR ? m_config->sgKeyLargeRJets() : m_config->sgKeyJets();

      const xAOD::JetContainer* xaod(nullptr);
      top::check(evtStore()->retrieve(xaod, sgKey), "Failed to retrieve Jets");

      ATH_MSG_INFO(" Jets with sgKey = " << sgKey);
      for (auto x : *xaod) {
        ATH_MSG_INFO("   Jet pT , eta  = " << x->pt() << " , " << x->eta());
      }
    }

    return StatusCode::SUCCESS;
  }

  std::string JetObjectCollectionMaker::getLargeRModName(const std::string& NPModel) const {
    if (NPModel.find("CategoryReduction")!=std::string::npos) return "CategoryReduction_";
    return NPModel+"_"; 
  }

  void JetObjectCollectionMaker::addSystematics(const std::set<std::string>& specifiedSystematics,
                                                const CP::SystematicSet& recommendedSysts,
                                                std::unordered_map<CP::SystematicSet, CP::SystematicSet>& map,
                                                const std::string& modName, bool isLargeR, bool onlyJER) {
    ///-- Get the recommended systematics from the tool, in std::vector format --///
    const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector(recommendedSysts);
    

    for (const CP::SystematicSet& s : systList) {
      if (s.size() == 1) {
        CP::SystematicSet::const_iterator ss = s.begin();
	if(!m_config->getTreeFilter()->filterTree(modName + ss->name())) continue; // Applying tree filter

        if (onlyJER && ss->name().find("JER") == std::string::npos) continue;

        CP::SystematicSet modSet(modName + ss->name());

        m_recommendedSystematics.push_back(modSet);
        if (!m_config->isSystNominal(m_config->systematics())) {
          if (specifiedSystematics.size() == 0) {
            if (!isLargeR) m_specifiedSystematics.push_back(modSet);
            else m_specifiedSystematicsLargeR.push_back(modSet);
            map.insert(std::make_pair(modSet, s));
          } else if (specifiedSystematics.size() > 0) {
            for (std::string i : specifiedSystematics) {
	      TreeFilter filter(i);
              if (!filter.filterTree(modSet.name())) {
                if (!isLargeR) m_specifiedSystematics.push_back(modSet);
                else m_specifiedSystematicsLargeR.push_back(modSet);
                map.insert(std::make_pair(modSet, s));
              }
            }
          } // User has specified a systematic
        } // Don't do anything if the user requests nominal only
      } // size() == 1 -- this means that there is a CP::SystematicVariation
    } // Loop over systList

    m_recommendedSystematics.sort();
    m_recommendedSystematics.unique();

    m_specifiedSystematics.sort();
    m_specifiedSystematics.unique();

    m_specifiedSystematicsLargeR.sort();
    m_specifiedSystematicsLargeR.unique();
  }

  StatusCode JetObjectCollectionMaker::tagNominalLargeRJets() {
    const xAOD::JetContainer* xaod_calibrated_jets(nullptr);

    top::check(evtStore()->retrieve(xaod_calibrated_jets, m_config->sgKeyLargeRJets(
                                      m_nominalSystematicSet.hash())),
               "Failed to retrieve nominal calibrated large-R jets");
    for (const xAOD::Jet* jet : *xaod_calibrated_jets) {
      top::check(tagLargeRJet(*jet), "Failed to tag large-R jet");
    }
    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::tagLargeRJet(const xAOD::Jet& jet) {
    //decorate with boosted-tagging flags
    for (const std::pair<std::string, std::string>& name : m_config->boostedJetTaggers()) {
      std::string fullName = name.first + "_" + name.second;
      const Root::TAccept& result = m_boostedJetTaggers[fullName]->tag(jet);
      // TAccept has bool operator overloaded, but let's be more explicit in the output to char
      jet.auxdecor<char>("isTagged_" + fullName) = (result ? 1 : 0);
      // for users to extract more detailed tagging result information in custom event saver
      jet.auxdecor<unsigned long>("TAccept_bitmap_" + fullName) = result.getCutResultBitSet().to_ulong();
    }
    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::decorateBJets(xAOD::Jet& jet) {
    // initialise decorator and accessor
    static SG::AuxElement::Decorator<char> isbjet("IsBjet");
    static const std::string labelB = "PartonTruthLabelID";
    static SG::AuxElement::Accessor<int> truth_label(labelB);

    // Is b-jet if truth label == 5 and pT > 15 GeV
    isbjet(jet) = (jet.pt() > 15000. && truth_label(jet) == 5);

    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::decorateHSJets() {
    // initialise decorator
    static SG::AuxElement::Decorator<char> isHS("AnalysisTop_isHS");

    // retrieve small-R jets collection
    const xAOD::JetContainer* jets(nullptr);

    top::check(evtStore()->retrieve(jets,
                                    m_config->sgKeyJets()),
               "Failed to retrieve small-R jet collection" + m_config->sgKeyJets());

    // retrieve truth jet collection
    const xAOD::JetContainer* truthJets = nullptr;
    top::check(evtStore()->retrieve(truthJets,
                                    m_truthJetCollForHS),
               "Unable to retrieve truth jet container " + m_truthJetCollForHS +
               " - this is needed to define HS jets for application of JVT");

    for (const auto& jet : *jets) {
      bool ishs = false;
      for (const auto& tjet : *truthJets) {
        if (tjet->p4().DeltaR(jet->p4()) < 0.3 && tjet->pt() > 10e3) ishs = true;
      }
      isHS(*jet) = ishs;
    }

    return StatusCode::SUCCESS;
  }
  
  StatusCode JetObjectCollectionMaker::decorateMatchedTruth() {
    static const SG::AuxElement::Decorator<float> matchedPt("AnalysisTop_MatchedTruthJetPt");
    // retrieve small-R jets collection
    const xAOD::JetContainer* jets(nullptr);

    top::check(evtStore()->retrieve(jets,
                                    m_config->sgKeyJets()),
               "Failed to retrieve small-R jet collection" + m_config->sgKeyJets());

    const xAOD::JetContainer* truthJets = nullptr;
    top::check(asg::AsgTool::evtStore()->retrieve(truthJets, m_config->sgKeyTruthJets()), "Failed to retrieve the truth jets");

    const xAOD::Jet* matchedTruthJet = nullptr;
    double deltaR(9999);
    
    for (const auto& jet : *jets) {
      // loop over truth jets
      for (const auto& iTruthJet : *truthJets) {
        TLorentzVector truthJetTLV;
        truthJetTLV.SetPtEtaPhiE(iTruthJet->pt(),iTruthJet->eta(),iTruthJet->phi(),iTruthJet->e());

        // do the matching
        if(!matchedTruthJet) {
          matchedTruthJet = iTruthJet;
        } else {
          const double newdR = jet->p4().DeltaR(iTruthJet->p4());
          if(newdR < deltaR) {
            deltaR = newdR;
            matchedTruthJet = iTruthJet;
          }
        }
      }
      if (deltaR > m_config->jetResponseMatchingDeltaR()) {
        matchedPt(*jet) = -9999;
        continue;
      }
      if (!matchedTruthJet) {
        matchedPt(*jet) = -9999;
        continue;
      }
      matchedPt(*jet) = matchedTruthJet->pt(); 
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetObjectCollectionMaker::decorateDL1(bool trackJets) {
    // retrieve small-R jets collection -- either calo or track jets
    const xAOD::JetContainer* jets(nullptr);

    if (trackJets) {
      top::check(evtStore()->retrieve(jets,
                                      m_config->sgKeyTrackJets()),
                 "Failed to retrieve track jet collection" + m_config->sgKeyTrackJets());
    } else {
      top::check(evtStore()->retrieve(jets,
                                      m_config->sgKeyJets()),
                 "Failed to retrieve small-R jet collection" + m_config->sgKeyJets());
    }

    for (const auto& jet : *jets) {
      // loop over either calo or track jet btag selection tools to calculate the DL1x scores
      const std::unordered_map<std::string, ToolHandle<IBTaggingSelectionTool>>& m_btagDecorTools \
        = (trackJets ? m_btagSelToolsDL1Decor_trkJet : m_btagSelToolsDL1Decor);
      for (std::pair<std::string, ToolHandle<IBTaggingSelectionTool>> algo : m_btagDecorTools) {
        double DL1_weight = -999., dl1_pb = -10., dl1_pc = -10., dl1_pu = -10.;
        if (jet->btagging()->pb(algo.first, dl1_pb)
            && jet->btagging()->pc(algo.first, dl1_pc)
            && jet->btagging()->pu(algo.first, dl1_pu)) {
          if (!algo.second->getTaggerWeight(dl1_pb, dl1_pc, dl1_pu, DL1_weight)) {
            DL1_weight = -999.; // value for errors from retrieving DL1x weight
          }
        } else {
          DL1_weight = -100.; // value for errors from nonexistence of probabilities
        }
        DLx.at(algo.first)(*jet) = DL1_weight;
      }
    }

    return StatusCode::SUCCESS;
  }
}  // namespace top
