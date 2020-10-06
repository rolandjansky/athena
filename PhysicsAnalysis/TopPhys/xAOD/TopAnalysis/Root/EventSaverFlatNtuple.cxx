/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopAnalysis/EventSaverFlatNtuple.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEventSelectionTools/TreeManager.h"
#include "TopParticleLevel/TruthTools.h"
#include "xAODMissingET/MissingETContainer.h"

#include "AthContainers/AuxTypeRegistry.h"

#include "TFile.h"
#include <cmath>
#include <iostream>
#include <regex>
#include <boost/algorithm/string.hpp>

#include "TopParticleLevel/ParticleLevelEvent.h"

#include "FakeBkgTools/AsymptMatrixTool.h"

#include "xAODTracking/TrackParticlexAODHelpers.h"

namespace top {
  EventSaverFlatNtuple::EventSaverFlatNtuple() :
    asg::AsgTool("top::EventSaverFlatNtuple"),
    m_config(nullptr),
    m_sfRetriever(nullptr),
    m_outputFile(nullptr),
    m_truthTreeManager(nullptr),
    m_particleLevelTreeManager(nullptr),

    m_weight_mc(0.),
    m_weight_pileup(0.),

    m_weight_fwdElSF(0.),
    m_weight_fwdElSF_FWDEL_SF_ID_UP(0.),
    m_weight_fwdElSF_FWDEL_SF_ID_DOWN(0.),

    // cumulative SF
    m_weight_leptonSF(0.),

    m_weight_leptonSF_EL_SF_Trigger_UP(0.),
    m_weight_leptonSF_EL_SF_Trigger_DOWN(0.),
    m_weight_leptonSF_EL_SF_Reco_UP(0.),
    m_weight_leptonSF_EL_SF_Reco_DOWN(0.),
    m_weight_leptonSF_EL_SF_ID_UP(0.),
    m_weight_leptonSF_EL_SF_ID_DOWN(0.),
    m_weight_leptonSF_EL_SF_Isol_UP(0.),
    m_weight_leptonSF_EL_SF_Isol_DOWN(0.),

    m_weight_leptonSF_MU_SF_Trigger_STAT_UP(0.),
    m_weight_leptonSF_MU_SF_Trigger_STAT_DOWN(0.),
    m_weight_leptonSF_MU_SF_Trigger_SYST_UP(0.),
    m_weight_leptonSF_MU_SF_Trigger_SYST_DOWN(0.),
    // Muon ID SF systematics (regular)
    m_weight_leptonSF_MU_SF_ID_STAT_UP(0.),
    m_weight_leptonSF_MU_SF_ID_STAT_DOWN(0.),
    m_weight_leptonSF_MU_SF_ID_SYST_UP(0.),
    m_weight_leptonSF_MU_SF_ID_SYST_DOWN(0.),
    // Muon ID SF systematics (low pt)
    m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP(0.),
    m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN(0.),
    m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP(0.),
    m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN(0.),
    // Muon isolation SF systematics
    m_weight_leptonSF_MU_SF_Isol_STAT_UP(0.),
    m_weight_leptonSF_MU_SF_Isol_STAT_DOWN(0.),
    m_weight_leptonSF_MU_SF_Isol_SYST_UP(0.),
    m_weight_leptonSF_MU_SF_Isol_SYST_DOWN(0.),
    m_weight_leptonSF_MU_SF_TTVA_STAT_UP(0.),
    m_weight_leptonSF_MU_SF_TTVA_STAT_DOWN(0.),
    m_weight_leptonSF_MU_SF_TTVA_SYST_UP(0.),
    m_weight_leptonSF_MU_SF_TTVA_SYST_DOWN(0.),
    // Special global lepton trigger SF + systematics
    m_weight_globalLeptonTriggerSF(0.),
    m_weight_globalLeptonTriggerSF_EL_Trigger_UP(0.),
    m_weight_globalLeptonTriggerSF_EL_Trigger_DOWN(0.),
    m_weight_globalLeptonTriggerSF_MU_Trigger_STAT_UP(0.),
    m_weight_globalLeptonTriggerSF_MU_Trigger_STAT_DOWN(0.),
    m_weight_globalLeptonTriggerSF_MU_Trigger_SYST_UP(0.),
    m_weight_globalLeptonTriggerSF_MU_Trigger_SYST_DOWN(0.),
    m_weight_oldTriggerSF(0.),
    m_weight_oldTriggerSF_EL_Trigger_UP(0.),
    m_weight_oldTriggerSF_EL_Trigger_DOWN(0.),
    m_weight_oldTriggerSF_MU_Trigger_STAT_UP(0.),
    m_weight_oldTriggerSF_MU_Trigger_STAT_DOWN(0.),
    m_weight_oldTriggerSF_MU_Trigger_SYST_UP(0.),
    m_weight_oldTriggerSF_MU_Trigger_SYST_DOWN(0.),

    // individual components electrons
    m_weight_indiv_SF_EL_Reco(0.),
    m_weight_indiv_SF_EL_Reco_UP(0.),
    m_weight_indiv_SF_EL_Reco_DOWN(0.),
    m_weight_indiv_SF_EL_ID(0.),
    m_weight_indiv_SF_EL_ID_UP(0.),
    m_weight_indiv_SF_EL_ID_DOWN(0.),
    m_weight_indiv_SF_EL_Isol(0.),
    m_weight_indiv_SF_EL_Isol_UP(0.),
    m_weight_indiv_SF_EL_Isol_DOWN(0.),
    m_weight_indiv_SF_EL_ChargeID(0.),
    m_weight_indiv_SF_EL_ChargeID_UP(0.),
    m_weight_indiv_SF_EL_ChargeID_DOWN(0.),
    m_weight_indiv_SF_EL_ChargeMisID(0.),
    m_weight_indiv_SF_EL_ChargeMisID_STAT_UP(0.),
    m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN(0.),
    m_weight_indiv_SF_EL_ChargeMisID_SYST_UP(0.),
    m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN(0.),

    // Muon ID SF systematics (regular)
    m_weight_indiv_SF_MU_ID(0.),
    m_weight_indiv_SF_MU_ID_STAT_UP(0.),
    m_weight_indiv_SF_MU_ID_STAT_DOWN(0.),
    m_weight_indiv_SF_MU_ID_SYST_UP(0.),
    m_weight_indiv_SF_MU_ID_SYST_DOWN(0.),
    // Muon ID SF systematics (low pt)
    m_weight_indiv_SF_MU_ID_STAT_LOWPT_UP(0.),
    m_weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN(0.),
    m_weight_indiv_SF_MU_ID_SYST_LOWPT_UP(0.),
    m_weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN(0.),
    // Muon isolation SF systematics
    m_weight_indiv_SF_MU_Isol(0.),
    m_weight_indiv_SF_MU_Isol_SYST_UP(0.),
    m_weight_indiv_SF_MU_Isol_SYST_DOWN(0.),
    m_weight_indiv_SF_MU_Isol_STAT_UP(0.),
    m_weight_indiv_SF_MU_Isol_STAT_DOWN(0.),
    m_weight_indiv_SF_MU_TTVA(0.),
    m_weight_indiv_SF_MU_TTVA_SYST_UP(0.),
    m_weight_indiv_SF_MU_TTVA_SYST_DOWN(0.),
    m_weight_indiv_SF_MU_TTVA_STAT_UP(0.),
    m_weight_indiv_SF_MU_TTVA_STAT_DOWN(0.),

    m_ASMsize(0.),
    m_ASMweights(),
    m_ASMweights_Syst(),
    m_ASMweights_Systname(),

    m_eventNumber(0),
    m_runNumber(0),
    m_randomRunNumber(0),
    m_mcChannelNumber(0),

    m_mu_original(0.),
    m_mu(0.),
    m_mu_actual_original(0.),
    m_mu_actual(0.),

    m_backgroundFlags(0),
    m_hasBadMuon(0),
    m_makeRCJets(false),
    m_makeVarRCJets(false),
    m_useRCJSS(false),
    m_useRCAdditionalJSS(false),
    m_useVarRCJSS(false),
    m_useVarRCAdditionalJSS(false),
    m_useElectronChargeIDSelection(false),
    m_met_met(0.),
    m_met_phi(0.),
    m_met_met_withLooseObjects(0.),
    m_met_phi_withLooseObjects(0.) {
    m_weight_leptonSF_EL_SF_CorrModel_Reco_UP = std::vector<float>();
    m_weight_leptonSF_EL_SF_CorrModel_Reco_DOWN = std::vector<float>();
    m_weight_leptonSF_EL_SF_CorrModel_ID_UP = std::vector<float>();
    m_weight_leptonSF_EL_SF_CorrModel_ID_DOWN = std::vector<float>();
    m_weight_leptonSF_EL_SF_CorrModel_Iso_UP = std::vector<float>();
    m_weight_leptonSF_EL_SF_CorrModel_Iso_DOWN = std::vector<float>();
  }

  EventSaverFlatNtuple::~EventSaverFlatNtuple() {
    for (std::unordered_map<std::string, int*>::iterator i = m_extraTruthVars_int.begin();
         i != m_extraTruthVars_int.end(); ++i)
      delete (*i).second;

    for (std::unordered_map<std::string, float*>::iterator i = m_extraTruthVars_float.begin();
         i != m_extraTruthVars_float.end(); ++i)
      delete (*i).second;
  }

  std::shared_ptr<top::TopConfig> EventSaverFlatNtuple::topConfig() {
    return m_config;
  }

  std::vector<std::shared_ptr<top::TreeManager> > EventSaverFlatNtuple::treeManagers() {
    return m_treeManagers;
  }

  std::shared_ptr<top::TreeManager> EventSaverFlatNtuple::truthTreeManager() {
    return m_truthTreeManager;
  }

  std::shared_ptr<top::TreeManager> EventSaverFlatNtuple::particleLevelTreeManager() {
    return m_particleLevelTreeManager;
  }

  //std::shared_ptr<top::ScaleFactorRetriever> EventSaverFlatNtuple::scaleFactorRetriever()
  top::ScaleFactorRetriever* EventSaverFlatNtuple::scaleFactorRetriever() {
    return m_sfRetriever;
  }

  std::shared_ptr<top::TreeManager> EventSaverFlatNtuple::upgradeTreeManager() {
    return m_upgradeTreeManager;
  }

  std::vector<top::TreeManager::BranchFilter>& EventSaverFlatNtuple::branchFilters() {
    return m_branchFilters;
  }

  void EventSaverFlatNtuple::initialize(std::shared_ptr<top::TopConfig> config, TFile* file,
                                        const std::vector<std::string>& extraBranches) {
    m_config = config;
    m_outputFile = file;
    m_extraBranches = extraBranches;
    m_selectionDecisions.resize(m_extraBranches.size());

    // set the branch filters
    branchFilters().push_back(std::bind(&EventSaverFlatNtuple::filterBranches, this, std::placeholders::_1,
                                        std::placeholders::_2));

    // fixed-R re-clustering (RC)
    if (config->useRCJets()) {
      m_makeRCJets = true;
      m_useRCJSS = config->useRCJetSubstructure();
      m_useRCAdditionalJSS = config->useRCJetAdditionalSubstructure();
    }

    // variable-R re-clustering (VarRC)
    if (config->useVarRCJets()) {
      m_makeVarRCJets = true;
      m_VarRCjetBranches.clear();    // clear map of branches just in case
      m_VarRCjetsubBranches.clear();
      m_VarRCjetBranchesParticle.clear();
      m_VarRCjetsubBranchesParticle.clear();


      boost::split(m_VarRCJetRho, config->VarRCJetRho(), boost::is_any_of(","));
      boost::split(m_VarRCJetMassScale, config->VarRCJetMassScale(), boost::is_any_of(","));

      m_useVarRCJSS = config->useVarRCJetSubstructure();
      m_useVarRCAdditionalJSS = config->useVarRCJetAdditionalSubstructure();
    } // end make VarRC jets

    if (config->useElectronChargeIDSelection()) {
      m_useElectronChargeIDSelection = true;
    }

    //make a tree for each systematic
    std::string nominalTTreeName("SetMe"), nominalLooseTTreeName("SetMe");
    if (m_config->doTightEvents() && !m_config->HLLHC()) {
      for (auto treeName : *config->systAllTTreeNames()) {
        if (treeName.first == m_config->nominalHashValue() || m_config->doTightSysts()) {
          m_treeManagers.push_back(std::shared_ptr<top::TreeManager>(new top::TreeManager(treeName.second, file,
                                                                                          m_config->
                                                                                           outputFileNEventAutoFlush(),
                                                                                          m_config->
          outputFileBasketSizePrimitive(),
                                                                                          m_config->
                                                                                           outputFileBasketSizeVector())));
          m_treeManagers.back()->branchFilters() = branchFilters();
          if (treeName.first == m_config->nominalHashValue()) {
            nominalTTreeName = treeName.second;
          }
        }
      }
    }

    if (m_config->doLooseEvents()) {
      for (auto treeName : *config->systAllTTreeNames()) {
        if (treeName.first == m_config->nominalHashValue() || m_config->doLooseSysts()) {
          m_treeManagers.push_back(std::shared_ptr<top::TreeManager>(new top::TreeManager(treeName.second + "_Loose",
                                                                                          file,
                                                                                          m_config->
                                                                                           outputFileNEventAutoFlush(),
          m_config->outputFileBasketSizePrimitive(),
                                                                                          m_config->
                                                                                           outputFileBasketSizeVector())));
          m_treeManagers.back()->branchFilters() = branchFilters();
          if (treeName.first == m_config->nominalHashValue()) {
            nominalLooseTTreeName = treeName.second + "_Loose";
          }
        }
      }
    }

    // Truth tree
    if (m_config->isMC()) {
      if (asg::ToolStore::contains<ScaleFactorRetriever>("top::ScaleFactorRetriever")) {
        m_sfRetriever = asg::ToolStore::get<ScaleFactorRetriever>("top::ScaleFactorRetriever");
      } else {
        top::ScaleFactorRetriever* topSFR = new top::ScaleFactorRetriever("top::ScaleFactorRetriever");
        top::check(asg::setProperty(topSFR, "config", m_config), "Failed to set config");
        top::check(topSFR->initialize(), "Failed to initalialise");
        m_sfRetriever = topSFR;
      }

      m_truthTreeManager =
        std::shared_ptr<top::TreeManager>(new top::TreeManager("truth", file, m_config->outputFileNEventAutoFlush(),
                                                               m_config->outputFileBasketSizePrimitive(),
                                                               m_config->outputFileBasketSizeVector()));
      m_truthTreeManager->branchFilters() = branchFilters();
      m_truthTreeManager->makeOutputVariable(m_weight_mc, "weight_mc");
      m_truthTreeManager->makeOutputVariable(m_eventNumber, "eventNumber");
      m_truthTreeManager->makeOutputVariable(m_runNumber, "runNumber");
      m_truthTreeManager->makeOutputVariable(m_mu, "mu");
      m_truthTreeManager->makeOutputVariable(m_mu_actual, "mu_actual");
      m_truthTreeManager->makeOutputVariable(m_weight_pileup, "weight_pileup");
      if (m_config->isMC() && m_config->doPileupReweighting()) m_truthTreeManager->makeOutputVariable(m_randomRunNumber,
                                                                                                      "randomRunNumber");

      m_truthTreeManager->makeOutputVariable(m_mcChannelNumber, "mcChannelNumber");

      // Only if you really really want it - this is BIG
      if (m_config->useTruthParticles() && m_config->doTruthBlockInfo()) {
        m_truthTreeManager->makeOutputVariable(m_mc_pt, "mc_pt");
        m_truthTreeManager->makeOutputVariable(m_mc_eta, "mc_eta");
        m_truthTreeManager->makeOutputVariable(m_mc_phi, "mc_phi");
        m_truthTreeManager->makeOutputVariable(m_mc_e, "mc_e");
        m_truthTreeManager->makeOutputVariable(m_mc_pdgId, "mc_pdgId");
        m_truthTreeManager->makeOutputVariable(m_mc_charge, "mc_charge");
        m_truthTreeManager->makeOutputVariable(m_mc_status, "mc_status");
        m_truthTreeManager->makeOutputVariable(m_mc_barcode, "mc_barcode");
      }

      // PDF information
      if (m_config->doTruthPDFInfo()) {
        if (m_config->doTruthPDFInfoInNominalTrees()) {//if PDF info requested to be saved in nominal trees of truth one
          for (auto systematicTree : m_treeManagers) {//loop on systematic trees to only get the nominal ones
            if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName) {
              systematicTree->makeOutputVariable(m_PDFinfo_X1, "PDFinfo_X1");
              systematicTree->makeOutputVariable(m_PDFinfo_X2, "PDFinfo_X2");
              systematicTree->makeOutputVariable(m_PDFinfo_PDGID1, "PDFinfo_PDGID1");
              systematicTree->makeOutputVariable(m_PDFinfo_PDGID2, "PDFinfo_PDGID2");
              systematicTree->makeOutputVariable(m_PDFinfo_Q, "PDFinfo_Q");
              systematicTree->makeOutputVariable(m_PDFinfo_XF1, "PDFinfo_XF1");
              systematicTree->makeOutputVariable(m_PDFinfo_XF2, "PDFinfo_XF2");
            }
          }
        } else {
          m_truthTreeManager->makeOutputVariable(m_PDFinfo_X1, "PDFinfo_X1");
          m_truthTreeManager->makeOutputVariable(m_PDFinfo_X2, "PDFinfo_X2");
          m_truthTreeManager->makeOutputVariable(m_PDFinfo_PDGID1, "PDFinfo_PDGID1");
          m_truthTreeManager->makeOutputVariable(m_PDFinfo_PDGID2, "PDFinfo_PDGID2");
          m_truthTreeManager->makeOutputVariable(m_PDFinfo_Q, "PDFinfo_Q");
          m_truthTreeManager->makeOutputVariable(m_PDFinfo_XF1, "PDFinfo_XF1");
          m_truthTreeManager->makeOutputVariable(m_PDFinfo_XF2, "PDFinfo_XF2");
        }
      }

      // MC Generator weights
      if (m_config->doMCGeneratorWeights()) {
        if (m_config->doMCGeneratorWeightsInNominalTrees()) {//if MCGeneratorWeights requested to be saved in nominal
                                                             // trees of truth one
          for (auto systematicTree : m_treeManagers) {//loop on systematic trees to only get the nominal ones
            if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName) {
              systematicTree->makeOutputVariable(m_mc_generator_weights, "mc_generator_weights");
            }
          }
        } else {
          m_truthTreeManager->makeOutputVariable(m_mc_generator_weights, "mc_generator_weights");
        }
      }

      if (m_config->saveLHAPDFEvent()) {
        for (const auto& pdfset : m_config->LHAPDFSets())
          m_PDF_eventWeights[ pdfset ] = std::vector<float>();

        for (auto& pdf : m_PDF_eventWeights) {
          if (m_config->doLHAPDFInNominalTrees()) {//if PDF weights requested to be saved in nominal trees instead of
                                                   // truth one
            for (auto systematicTree : m_treeManagers) {//loop on systematic trees to only get the nominal ones
              if (systematicTree->name() == nominalTTreeName ||
                  systematicTree->name() ==
                  nominalLooseTTreeName) systematicTree->makeOutputVariable(pdf.second, pdf.first);
            }
          } else {
            m_truthTreeManager->makeOutputVariable(pdf.second, pdf.first);
          }
        }
      }
    }

    if (m_config->useLargeRJets()) {
      for (const std::pair<std::string, std::string>& taggerName : m_config->boostedJetTaggers())
        m_boostedJetTaggersNames.push_back(taggerName.first + "_" + taggerName.second);
      for (const std::pair<std::string, std::string>& taggerSF : m_config->boostedTaggerSFnames())
        m_boostedJetTaggersNamesCalibrated.push_back(taggerSF.first);
    }

    if (m_config->useJets()) {
      for (const std::string& algo : m_config->bTagAlgo_available()) {
        if (DLx.count(algo) == 0) {
          DLx.emplace(algo, SG::AuxElement::ConstAccessor<float>("AnalysisTop_" + algo));
          m_jet_DLx[algo] = std::vector<float>();
          m_jet_DLx_pb[algo] = std::vector<float>();
          m_jet_DLx_pc[algo] = std::vector<float>();
          m_jet_DLx_pu[algo] = std::vector<float>();
        }
      }
    }

    if (m_config->useTrackJets()) {
      for (const std::string& algo : m_config->bTagAlgo_available_trkJet()) {
        if (DLx.count(algo) == 0) {
          DLx.emplace(algo, SG::AuxElement::ConstAccessor<float>("AnalysisTop_" + algo));
          m_tjet_DLx[algo] = std::vector<float>();
          m_tjet_DLx_pb[algo] = std::vector<float>();
          m_tjet_DLx_pc[algo] = std::vector<float>();
          m_tjet_DLx_pu[algo] = std::vector<float>();
        }
      }
    }

    //loop over systematics and attach variables
    for (auto systematicTree : m_treeManagers) {
      if (m_config->isMC()) {
        systematicTree->makeOutputVariable(m_weight_mc, "weight_mc");

        //some event weights
        systematicTree->makeOutputVariable(m_weight_pileup, "weight_pileup");
        systematicTree->makeOutputVariable(m_weight_leptonSF, "weight_leptonSF");

        if (m_config->useFwdElectrons()) systematicTree->makeOutputVariable(m_weight_fwdElSF, "weight_fwdElSF");

        if (m_config->usePhotons()) systematicTree->makeOutputVariable(m_weight_photonSF, "weight_photonSF");

        if (m_config->useTaus()) systematicTree->makeOutputVariable(m_weight_tauSF,
                                                                    "weight_tauSF");

        if (m_config->useGlobalTriggerConfiguration()) systematicTree->makeOutputVariable(
            m_weight_globalLeptonTriggerSF, "weight_globalLeptonTriggerSF");
        systematicTree->makeOutputVariable(m_weight_oldTriggerSF, "weight_oldTriggerSF");

        // nominal b-tagging SFs
        for (auto& tagWP : m_config->bTagWP_available()) {
          // skip uncalibrated though available WPs
          if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(),
                        tagWP) == m_config->bTagWP_calibrated().end()) continue;
          m_weight_bTagSF[tagWP] = 0.;
          systematicTree->makeOutputVariable(m_weight_bTagSF[tagWP], "weight_bTagSF_" + shortBtagWP(tagWP));
          if (m_config->storePerJetBtagSFs() && m_config->isMC()) {
            m_perjet_weight_bTagSF[tagWP] = std::vector<float>();
            m_perjet_weight_bTagSF_eigen_B_up[tagWP] = std::vector<std::vector<float> >();
            m_perjet_weight_bTagSF_eigen_B_down[tagWP] = std::vector<std::vector<float> >();
            m_perjet_weight_bTagSF_eigen_C_up[tagWP] = std::vector<std::vector<float> >();
            m_perjet_weight_bTagSF_eigen_C_down[tagWP] = std::vector<std::vector<float> >();
            m_perjet_weight_bTagSF_eigen_Light_up[tagWP] = std::vector<std::vector<float> >();
            m_perjet_weight_bTagSF_eigen_Light_down[tagWP] = std::vector<std::vector<float> >();
            systematicTree->makeOutputVariable(m_perjet_weight_bTagSF[tagWP],
                                               "weight_perjet_bTagSF_" + shortBtagWP(tagWP));
            systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_eigen_B_up[tagWP], "weight_perjet_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_B_up");
            systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_eigen_B_down[tagWP], "weight_perjet_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_B_down");
            systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_eigen_C_up[tagWP], "weight_perjet_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_C_up");
            systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_eigen_C_down[tagWP], "weight_perjet_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_C_down");
            systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_eigen_Light_up[tagWP], "weight_perjet_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_Light_up");
            systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_eigen_Light_down[tagWP], "weight_perjet_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_Light_down");
            for (const std::string& name : m_config->btagging_namedSysts(tagWP)) {
              systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_named_up[tagWP][name], "weight_perjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_" + betterBtagNamedSyst(name) + "_up");
              systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_named_down[tagWP][name], "weight_perjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_" + betterBtagNamedSyst(name) + "_down");
            }
          }
        }
        if (m_config->useTrackJets()) {
          for (auto& tagWP : m_config->bTagWP_available_trkJet()) {
            // skip uncalibrated though available WPs
            if (std::find(m_config->bTagWP_calibrated_trkJet().begin(), m_config->bTagWP_calibrated_trkJet().end(),
                          tagWP) == m_config->bTagWP_calibrated_trkJet().end()) continue;
            m_weight_trackjet_bTagSF[tagWP] = 0.;
            systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF[tagWP],
                                               "weight_trackjet_bTagSF_" + shortBtagWP(tagWP));
            if (m_config->storePerJetBtagSFs() && m_config->isMC()) {
              m_perjet_weight_trackjet_bTagSF[tagWP] = std::vector<float>();
              m_perjet_weight_trackjet_bTagSF_eigen_B_up[tagWP] = std::vector<std::vector<float> >();
              m_perjet_weight_trackjet_bTagSF_eigen_B_down[tagWP] = std::vector<std::vector<float> >();
              m_perjet_weight_trackjet_bTagSF_eigen_C_up[tagWP] = std::vector<std::vector<float> >();
              m_perjet_weight_trackjet_bTagSF_eigen_C_down[tagWP] = std::vector<std::vector<float> >();
              m_perjet_weight_trackjet_bTagSF_eigen_Light_up[tagWP] = std::vector<std::vector<float> >();
              m_perjet_weight_trackjet_bTagSF_eigen_Light_down[tagWP] = std::vector<std::vector<float> >();
              systematicTree->makeOutputVariable(m_perjet_weight_trackjet_bTagSF[tagWP], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP));
              systematicTree->makeOutputVariable(m_perjet_weight_trackjet_bTagSF_eigen_B_up[tagWP], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_B_up");
              systematicTree->makeOutputVariable(m_perjet_weight_trackjet_bTagSF_eigen_B_down[tagWP], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_B_down");
              systematicTree->makeOutputVariable(m_perjet_weight_trackjet_bTagSF_eigen_C_up[tagWP], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_C_up");
              systematicTree->makeOutputVariable(m_perjet_weight_trackjet_bTagSF_eigen_C_down[tagWP], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_C_down");
              systematicTree->makeOutputVariable(m_perjet_weight_trackjet_bTagSF_eigen_Light_up[tagWP], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_Light_up");
              systematicTree->makeOutputVariable(m_perjet_weight_trackjet_bTagSF_eigen_Light_down[tagWP], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_Light_down");
              for (const std::string& name : m_config->trkjet_btagging_namedSysts(tagWP)) {
                systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_named_up[tagWP][name], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                     tagWP) + "_" + betterBtagNamedSyst(name) + "_up");
                systematicTree->makeOutputVariable(m_perjet_weight_bTagSF_named_down[tagWP][name], "weight_perjet_trackjet_bTagSF_" + shortBtagWP(
                                                     tagWP) + "_" + betterBtagNamedSyst(name) + "_down");
              }
            }
          }
        }


        systematicTree->makeOutputVariable(m_weight_jvt, "weight_jvt");
        if (m_config->getfJVTWP() != "None") systematicTree->makeOutputVariable(m_weight_forwardjvt, "weight_forwardjvt");

        if (m_config->isSherpa22Vjets()) systematicTree->makeOutputVariable(m_weight_sherpa_22_vjets,
                                                                            "weight_sherpa_22_vjets");

        if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName) {
          systematicTree->makeOutputVariable(m_weight_pileup_UP, "weight_pileup_UP");
          systematicTree->makeOutputVariable(m_weight_pileup_DOWN, "weight_pileup_DOWN");

          if (m_config->useFwdElectrons()) {
            systematicTree->makeOutputVariable(m_weight_fwdElSF_FWDEL_SF_ID_UP, "weight_fwdElSF_FWDEL_SF_ID_UP");
            systematicTree->makeOutputVariable(m_weight_fwdElSF_FWDEL_SF_ID_DOWN, "weight_fwdElSF_FWDEL_SF_ID_DOWN");
          }
          systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Trigger_UP, "weight_leptonSF_EL_SF_Trigger_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Trigger_DOWN,
                                             "weight_leptonSF_EL_SF_Trigger_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Reco_UP, "weight_leptonSF_EL_SF_Reco_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Reco_DOWN, "weight_leptonSF_EL_SF_Reco_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_ID_UP, "weight_leptonSF_EL_SF_ID_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_ID_DOWN, "weight_leptonSF_EL_SF_ID_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Isol_UP, "weight_leptonSF_EL_SF_Isol_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_Isol_DOWN, "weight_leptonSF_EL_SF_Isol_DOWN");

          if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
            systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_CorrModel_Reco_UP,
                                               "weight_leptonSF_EL_SF_" + m_config->electronEfficiencySystematicModel() +
                                               "_Reco_UP");
            systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_CorrModel_Reco_DOWN,
                                               "weight_leptonSF_EL_SF_" + m_config->electronEfficiencySystematicModel() +
                                               "_Reco_DOWN");
            systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_CorrModel_ID_UP,
                                               "weight_leptonSF_EL_SF_" + m_config->electronEfficiencySystematicModel() +
                                               "_ID_UP");
            systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_CorrModel_ID_DOWN,
                                               "weight_leptonSF_EL_SF_" + m_config->electronEfficiencySystematicModel() +
                                               "_ID_DOWN");
            systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_CorrModel_Iso_UP,
                                               "weight_leptonSF_EL_SF_" + m_config->electronEfficiencySystematicModel() +
                                               "_Iso_UP");
            systematicTree->makeOutputVariable(m_weight_leptonSF_EL_SF_CorrModel_Iso_DOWN,
                                               "weight_leptonSF_EL_SF_" + m_config->electronEfficiencySystematicModel() +
                                               "_Iso_DOWN");
          }

          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Trigger_STAT_UP,
                                             "weight_leptonSF_MU_SF_Trigger_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Trigger_STAT_DOWN,
                                             "weight_leptonSF_MU_SF_Trigger_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Trigger_SYST_UP,
                                             "weight_leptonSF_MU_SF_Trigger_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Trigger_SYST_DOWN,
                                             "weight_leptonSF_MU_SF_Trigger_SYST_DOWN");
          // Muon ID SF systematics (regular)
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_STAT_UP, "weight_leptonSF_MU_SF_ID_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_STAT_DOWN,
                                             "weight_leptonSF_MU_SF_ID_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_SYST_UP, "weight_leptonSF_MU_SF_ID_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_SYST_DOWN,
                                             "weight_leptonSF_MU_SF_ID_SYST_DOWN");
          // Muon ID SF systematics (low pt)
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP,
                                             "weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN,
                                             "weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP,
                                             "weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN,
                                             "weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN");
          // Muon isolation SF systematics
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Isol_STAT_UP,
                                             "weight_leptonSF_MU_SF_Isol_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Isol_STAT_DOWN,
                                             "weight_leptonSF_MU_SF_Isol_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Isol_SYST_UP,
                                             "weight_leptonSF_MU_SF_Isol_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_Isol_SYST_DOWN,
                                             "weight_leptonSF_MU_SF_Isol_SYST_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_TTVA_STAT_UP,
                                             "weight_leptonSF_MU_SF_TTVA_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_TTVA_STAT_DOWN,
                                             "weight_leptonSF_MU_SF_TTVA_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_TTVA_SYST_UP,
                                             "weight_leptonSF_MU_SF_TTVA_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_leptonSF_MU_SF_TTVA_SYST_DOWN,
                                             "weight_leptonSF_MU_SF_TTVA_SYST_DOWN");
          // Special global lepton trigger SF when requested
          if (m_config->useGlobalTriggerConfiguration()) {
            systematicTree->makeOutputVariable(m_weight_globalLeptonTriggerSF_EL_Trigger_UP,
                                               "weight_globalLeptonTriggerSF_EL_Trigger_UP");
            systematicTree->makeOutputVariable(m_weight_globalLeptonTriggerSF_EL_Trigger_DOWN,
                                               "weight_globalLeptonTriggerSF_EL_Trigger_DOWN");
            systematicTree->makeOutputVariable(m_weight_globalLeptonTriggerSF_MU_Trigger_STAT_UP,
                                               "weight_globalLeptonTriggerSF_MU_Trigger_STAT_UP");
            systematicTree->makeOutputVariable(m_weight_globalLeptonTriggerSF_MU_Trigger_STAT_DOWN,
                                               "weight_globalLeptonTriggerSF_MU_Trigger_STAT_DOWN");
            systematicTree->makeOutputVariable(m_weight_globalLeptonTriggerSF_MU_Trigger_SYST_UP,
                                               "weight_globalLeptonTriggerSF_MU_Trigger_SYST_UP");
            systematicTree->makeOutputVariable(m_weight_globalLeptonTriggerSF_MU_Trigger_SYST_DOWN,
                                               "weight_globalLeptonTriggerSF_MU_Trigger_SYST_DOWN");
          }
          systematicTree->makeOutputVariable(m_weight_oldTriggerSF_EL_Trigger_UP, "weight_oldTriggerSF_EL_Trigger_UP");
          systematicTree->makeOutputVariable(m_weight_oldTriggerSF_EL_Trigger_DOWN,
                                             "weight_oldTriggerSF_EL_Trigger_DOWN");
          systematicTree->makeOutputVariable(m_weight_oldTriggerSF_MU_Trigger_STAT_UP,
                                             "weight_oldTriggerSF_MU_Trigger_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_oldTriggerSF_MU_Trigger_STAT_DOWN,
                                             "weight_oldTriggerSF_MU_Trigger_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_oldTriggerSF_MU_Trigger_SYST_UP,
                                             "weight_oldTriggerSF_MU_Trigger_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_oldTriggerSF_MU_Trigger_SYST_DOWN,
                                             "weight_oldTriggerSF_MU_Trigger_SYST_DOWN");

          // write also out the individual components:

          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Reco, "weight_indiv_SF_EL_Reco");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Reco_UP, "weight_indiv_SF_EL_Reco_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Reco_DOWN, "weight_indiv_SF_EL_Reco_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ID, "weight_indiv_SF_EL_ID");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ID_UP, "weight_indiv_SF_EL_ID_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ID_DOWN, "weight_indiv_SF_EL_ID_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Isol, "weight_indiv_SF_EL_Isol");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Isol_UP, "weight_indiv_SF_EL_Isol_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_Isol_DOWN, "weight_indiv_SF_EL_Isol_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeID, "weight_indiv_SF_EL_ChargeID");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeID_UP, "weight_indiv_SF_EL_ChargeID_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeID_DOWN, "weight_indiv_SF_EL_ChargeID_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID, "weight_indiv_SF_EL_ChargeMisID");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID_STAT_UP,
                                             "weight_indiv_SF_EL_ChargeMisID_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN,
                                             "weight_indiv_SF_EL_ChargeMisID_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID_SYST_UP,
                                             "weight_indiv_SF_EL_ChargeMisID_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN,
                                             "weight_indiv_SF_EL_ChargeMisID_SYST_DOWN");

          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID, "weight_indiv_SF_MU_ID");
          // Muon ID SF systematics (regular)
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_STAT_UP, "weight_indiv_SF_MU_ID_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_STAT_DOWN, "weight_indiv_SF_MU_ID_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_SYST_UP, "weight_indiv_SF_MU_ID_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_SYST_DOWN, "weight_indiv_SF_MU_ID_SYST_DOWN");
          // Muon ID SF systematics (low pt)
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_STAT_LOWPT_UP,
                                             "weight_indiv_SF_MU_ID_STAT_LOWPT_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN,
                                             "weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_SYST_LOWPT_UP,
                                             "weight_indiv_SF_MU_ID_SYST_LOWPT_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN,
                                             "weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN");
          // Muon isolation SF systematics
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol, "weight_indiv_SF_MU_Isol");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol_STAT_UP, "weight_indiv_SF_MU_Isol_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol_STAT_DOWN, "weight_indiv_SF_MU_Isol_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol_SYST_UP, "weight_indiv_SF_MU_Isol_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_Isol_SYST_DOWN, "weight_indiv_SF_MU_Isol_SYST_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA, "weight_indiv_SF_MU_TTVA");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA_STAT_UP, "weight_indiv_SF_MU_TTVA_STAT_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA_STAT_DOWN, "weight_indiv_SF_MU_TTVA_STAT_DOWN");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA_SYST_UP, "weight_indiv_SF_MU_TTVA_SYST_UP");
          systematicTree->makeOutputVariable(m_weight_indiv_SF_MU_TTVA_SYST_DOWN, "weight_indiv_SF_MU_TTVA_SYST_DOWN");

          if (m_config->useTaus()) {
            // Tau-electron overlap removal
            for (top::topSFSyst i = top::topSFSyst(top::topSFSyst::TAU_SF_NOMINAL + 1); i < top::topSFSyst::TAU_SF_END;
                 i = top::topSFSyst(i + 1)) {
              if (top::tauSF_alias.find(i) == top::tauSF_alias.end()) continue;
              if (m_config->tauSFDoRNNID() == false && top::tauSF_name.at(i).Contains("RNN")) continue;
              if (m_config->tauSFDoBDTID() == false && top::tauSF_name.at(i).Contains("JETID")) continue;
              m_weight_tauSF_variations[i] = 1;
              systematicTree->makeOutputVariable(m_weight_tauSF_variations[i], ("weight_tauSF_" + top::tauSF_alias.at(
                                                                                  i)).Data());
            }
          }

          if (m_config->usePhotons()) {
            systematicTree->makeOutputVariable(m_weight_photonSF_ID_UP,
                                               "weight_photonSF_ID_UP");
            systematicTree->makeOutputVariable(m_weight_photonSF_ID_DOWN,
                                               "weight_photonSF_ID_DOWN");
            systematicTree->makeOutputVariable(m_weight_photonSF_effIso_UP,
                                               "weight_photonSF_effIso_UP");
            systematicTree->makeOutputVariable(m_weight_photonSF_effIso_DOWN,
                                               "weight_photonSF_effIso_DOWN");
          }

          systematicTree->makeOutputVariable(m_weight_jvt_up, "weight_jvt_UP");
          systematicTree->makeOutputVariable(m_weight_jvt_down, "weight_jvt_DOWN");
	  if (m_config->getfJVTWP() != "None") {
	    systematicTree->makeOutputVariable(m_weight_forwardjvt_up, "weight_forwardjvt_UP");
	    systematicTree->makeOutputVariable(m_weight_forwardjvt_down, "weight_forwardjvt_DOWN");
	  }
        }

        // for b-tagging SFs, can also have systematic-shifted in systematics trees
        if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName ||
            m_config->dumpBtagSystsInSystTrees()) {
          // b-tagging SFs: eigenvectors and named systematics
          for (auto& tagWP : m_config->bTagWP_available()) {
            // skip uncalibrated though available WPs
            if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(),
                          tagWP) == m_config->bTagWP_calibrated().end()) continue;
            // up
            systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_B_up[tagWP], "weight_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_B_up");
            systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_C_up[tagWP], "weight_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_C_up");
            systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_Light_up[tagWP],
                                               "weight_bTagSF_" + shortBtagWP(tagWP) + "_eigenvars_Light_up");
            // down
            systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_B_down[tagWP], "weight_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_B_down");
            systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_C_down[tagWP], "weight_bTagSF_" + shortBtagWP(
                                                 tagWP) + "_eigenvars_C_down");
            systematicTree->makeOutputVariable(m_weight_bTagSF_eigen_Light_down[tagWP],
                                               "weight_bTagSF_" + shortBtagWP(tagWP) + "_eigenvars_Light_down");
            // named systematics
            for (auto name : m_config->btagging_namedSysts(tagWP)) {
              systematicTree->makeOutputVariable(m_weight_bTagSF_named_up[tagWP][name],
                                                 "weight_bTagSF_" + shortBtagWP(tagWP) + "_" + betterBtagNamedSyst(
                                                   name) + "_up");
              systematicTree->makeOutputVariable(m_weight_bTagSF_named_down[tagWP][name], "weight_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_" + betterBtagNamedSyst(name) + "_down");
            }
          }
          if (m_config->useTrackJets()) {
            for (auto& tagWP : m_config->bTagWP_available_trkJet()) {
              // skip uncalibrated though available WPs
              if (std::find(m_config->bTagWP_calibrated_trkJet().begin(), m_config->bTagWP_calibrated_trkJet().end(),
                            tagWP) == m_config->bTagWP_calibrated_trkJet().end()) continue;
              // up
              systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_B_up[tagWP], "weight_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_B_up");
              systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_C_up[tagWP], "weight_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_C_up");
              systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_Light_up[tagWP], "weight_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_Light_up");
              // down
              systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_B_down[tagWP], "weight_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_B_down");
              systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_C_down[tagWP], "weight_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_C_down");
              systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_eigen_Light_down[tagWP], "weight_trackjet_bTagSF_" + shortBtagWP(
                                                   tagWP) + "_eigenvars_Light_down");
              // named systematics
              for (auto name : m_config->trkjet_btagging_namedSysts(tagWP)) {
                systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_named_up[tagWP][name], "weight_trackjet_bTagSF_" + shortBtagWP(
                                                     tagWP) + "_" + betterBtagNamedSyst(name) + "_up");
                systematicTree->makeOutputVariable(m_weight_trackjet_bTagSF_named_down[tagWP][name], "weight_trackjet_bTagSF_" + shortBtagWP(
                                                     tagWP) + "_" + betterBtagNamedSyst(name) + "_down");
              }
            }
          }
        }
      }

      ///-- weights for matrix-method fakes estimate by IFF --///
      if (!m_config->isMC() && systematicTree->name() == nominalLooseTTreeName && m_config->doFakesMMWeightsIFF()) {
        std::vector<CP::AsymptMatrixTool*> fakesMMWeightCalcIFF;
        while (asg::ToolStore::contains<CP::AsymptMatrixTool>("AsymptMatrixTool_" + std::to_string (m_ASMsize))) {
          fakesMMWeightCalcIFF.push_back(asg::ToolStore::get<CP::AsymptMatrixTool>("AsymptMatrixTool_" + std::to_string (m_ASMsize)));
          ++m_ASMsize;
        }
        std::string ASMweights_branch_name = "ASM_weight";
        std::string ASMweights_Syst_branch_name = "ASM_weight_Syst";
        std::string ASMweights_Systname_branch_name = "ASM_weight_Systname";
        systematicTree->makeOutputVariable(m_ASMweights, ASMweights_branch_name);
        m_ASMweights_Syst.resize(m_ASMsize);
        m_ASMweights_Systname.resize(m_ASMsize);
        for (int mmi = 0; mmi < m_ASMsize; ++mmi) {
          systematicTree->makeOutputVariable(m_ASMweights_Syst[mmi],
                                             ASMweights_Syst_branch_name + "_" + std::to_string(mmi));
          systematicTree->makeOutputVariable(m_ASMweights_Systname[mmi], ASMweights_Systname_branch_name + "_" + std::to_string(
                                               mmi));
        }
      }

      /// Bootstrapping poisson weights
      if (m_config->saveBootstrapWeights()) {
        systematicTree->makeOutputVariable(m_weight_poisson, "weight_poisson");
      }

      //event info
      systematicTree->makeOutputVariable(m_eventNumber, "eventNumber");
      systematicTree->makeOutputVariable(m_runNumber, "runNumber");
      if (m_config->isMC() && m_config->doPileupReweighting()) systematicTree->makeOutputVariable(m_randomRunNumber,
                                                                                                  "randomRunNumber");
      systematicTree->makeOutputVariable(m_mcChannelNumber, "mcChannelNumber");
      systematicTree->makeOutputVariable(m_mu, "mu");
      systematicTree->makeOutputVariable(m_mu_actual, "mu_actual");
      if (!m_config->isMC()) {
        systematicTree->makeOutputVariable(m_mu_original, "mu_original_xAOD");
        systematicTree->makeOutputVariable(m_mu_actual_original, "mu_actual_original_xAOD");
      }
      systematicTree->makeOutputVariable(m_backgroundFlags, "backgroundFlags");
      if (m_config->useMuons()) systematicTree->makeOutputVariable(m_hasBadMuon, "hasBadMuon");

      //electrons
      if (m_config->useElectrons()) {
        systematicTree->makeOutputVariable(m_el_pt, "el_pt");
        systematicTree->makeOutputVariable(m_el_eta, "el_eta");
        systematicTree->makeOutputVariable(m_el_cl_eta, "el_cl_eta");
        systematicTree->makeOutputVariable(m_el_phi, "el_phi");
        systematicTree->makeOutputVariable(m_el_e, "el_e");
        systematicTree->makeOutputVariable(m_el_charge, "el_charge");
        systematicTree->makeOutputVariable(m_el_topoetcone20, "el_topoetcone20");
        systematicTree->makeOutputVariable(m_el_ptvarcone20, "el_ptvarcone20");
        if (systematicTree->name().find("Loose") != std::string::npos) {
          systematicTree->makeOutputVariable(m_el_isTight, "el_isTight");
        }
        systematicTree->makeOutputVariable(m_el_CF, "el_CF");
        systematicTree->makeOutputVariable(m_el_d0sig, "el_d0sig");
        systematicTree->makeOutputVariable(m_el_delta_z0_sintheta, "el_delta_z0_sintheta");
        if (m_useElectronChargeIDSelection) {
          systematicTree->makeOutputVariable(m_el_ECIDS, "el_ECIDS");
          systematicTree->makeOutputVariable(m_el_ECIDSResult, "el_ECIDSResult");
        }
        if (m_config->isMC()) {
          systematicTree->makeOutputVariable(m_el_true_type, "el_true_type");
          systematicTree->makeOutputVariable(m_el_true_origin, "el_true_origin");
          systematicTree->makeOutputVariable(m_el_true_firstEgMotherTruthType, "el_true_firstEgMotherTruthType");
          systematicTree->makeOutputVariable(m_el_true_firstEgMotherTruthOrigin, "el_true_firstEgMotherTruthOrigin");
          systematicTree->makeOutputVariable(m_el_true_firstEgMotherPdgId, "el_true_firstEgMotherPdgId");
          systematicTree->makeOutputVariable(m_el_true_isPrompt, "el_true_isPrompt");
          systematicTree->makeOutputVariable(m_el_true_isChargeFl, "el_true_isChargeFl");
        }
	if (m_config->enablePromptLeptonImprovedVetoStudies()) {
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonRNN_conversion, "PLIV_el_PromptLeptonRNN_conversion");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonRNN_non_prompt_b, "PLIV_el_PromptLeptonRNN_non_prompt_b");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonRNN_non_prompt_c, "PLIV_el_PromptLeptonRNN_non_prompt_c");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonRNN_prompt, "PLIV_el_PromptLeptonRNN_prompt");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_MVAXBin, "PLIV_el_PromptLeptonImprovedInput_MVAXBin");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_TrackJetNTrack, "PLIV_el_PromptLeptonImprovedInput_TrackJetNTrack");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_topoetcone30rel, "PLIV_el_PromptLeptonImprovedInput_topoetcone30rel");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_ptvarcone30rel, "PLIV_el_PromptLeptonImprovedInput_ptvarcone30rel");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_PtFrac, "PLIV_el_PromptLeptonImprovedInput_PtFrac");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_DRlj, "PLIV_el_PromptLeptonImprovedInput_DRlj");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_CaloClusterSumEtRel, "PLIV_el_PromptLeptonImprovedInput_CaloClusterSumEtRel");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_PtRel, "PLIV_el_PromptLeptonImprovedInput_PtRel");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx, "PLIV_el_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedVetoBARR, "PLIV_el_PromptLeptonImprovedVetoBARR");
	  systematicTree->makeOutputVariable(m_PLIV_el_PromptLeptonImprovedVetoECAP, "PLIV_el_PromptLeptonImprovedVetoECAP");
	}
      }

      //forward electrons
      if (m_config->useFwdElectrons()) {
        systematicTree->makeOutputVariable(m_fwdel_pt, "fwdel_pt");
        systematicTree->makeOutputVariable(m_fwdel_eta, "fwdel_eta");
        systematicTree->makeOutputVariable(m_fwdel_phi, "fwdel_phi");
        systematicTree->makeOutputVariable(m_fwdel_e, "fwdel_e");
        systematicTree->makeOutputVariable(m_fwdel_etcone20, "fwdel_etcone20");
        systematicTree->makeOutputVariable(m_fwdel_etcone30, "fwdel_etcone30");
        systematicTree->makeOutputVariable(m_fwdel_etcone40, "fwdel_etcone40");
        if (systematicTree->name().find("Loose") != std::string::npos) {
          systematicTree->makeOutputVariable(m_fwdel_isTight, "fwdel_isTight");
        }
      }//end of fwd electrons branches

      //muons
      if (m_config->useMuons()) {
        systematicTree->makeOutputVariable(m_mu_pt, "mu_pt");
        systematicTree->makeOutputVariable(m_mu_eta, "mu_eta");
        systematicTree->makeOutputVariable(m_mu_phi, "mu_phi");
        systematicTree->makeOutputVariable(m_mu_e, "mu_e");
        systematicTree->makeOutputVariable(m_mu_charge, "mu_charge");
        systematicTree->makeOutputVariable(m_mu_topoetcone20, "mu_topoetcone20");
        systematicTree->makeOutputVariable(m_mu_ptvarcone30, "mu_ptvarcone30");
        if (systematicTree->name().find("Loose") != std::string::npos) {
          systematicTree->makeOutputVariable(m_mu_isTight, "mu_isTight");
        }
        systematicTree->makeOutputVariable(m_mu_d0sig, "mu_d0sig");
        systematicTree->makeOutputVariable(m_mu_delta_z0_sintheta, "mu_delta_z0_sintheta");
        if (m_config->isMC()) {
          systematicTree->makeOutputVariable(m_mu_true_type, "mu_true_type");
          systematicTree->makeOutputVariable(m_mu_true_origin, "mu_true_origin");
          systematicTree->makeOutputVariable(m_mu_true_isPrompt, "mu_true_isPrompt");
        }
	if (m_config->enablePromptLeptonImprovedVetoStudies()) {
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonRNN_non_prompt_b, "PLIV_mu_PromptLeptonRNN_non_prompt_b");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonRNN_non_prompt_c, "PLIV_mu_PromptLeptonRNN_non_prompt_c");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonRNN_prompt, "PLIV_mu_PromptLeptonRNN_prompt");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonImprovedInput_MVAXBin, "PLIV_mu_PromptLeptonImprovedInput_MVAXBin");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonImprovedInput_topoetcone30rel, "PLIV_mu_PromptLeptonImprovedInput_topoetcone30rel");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonImprovedInput_PtFrac, "PLIV_mu_PromptLeptonImprovedInput_PtFrac");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonImprovedInput_DRlj, "PLIV_mu_PromptLeptonImprovedInput_DRlj");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel, "PLIV_mu_PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonImprovedInput_CaloClusterERel, "PLIV_mu_PromptLeptonImprovedInput_CaloClusterERel");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest, "PLIV_mu_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest");
	  systematicTree->makeOutputVariable(m_PLIV_mu_PromptLeptonImprovedVeto, "PLIV_mu_PromptLeptonImprovedVeto");
	}
      }

      //soft muons
      if (m_config->useSoftMuons()) {
        systematicTree->makeOutputVariable(m_softmu_pt, "softmu_pt");
        systematicTree->makeOutputVariable(m_softmu_eta, "softmu_eta");
        systematicTree->makeOutputVariable(m_softmu_phi, "softmu_phi");
        systematicTree->makeOutputVariable(m_softmu_e, "softmu_e");
        systematicTree->makeOutputVariable(m_softmu_charge, "softmu_charge");
        systematicTree->makeOutputVariable(m_softmu_d0, "softmu_d0");
        systematicTree->makeOutputVariable(m_softmu_d0sig, "softmu_d0sig");
        systematicTree->makeOutputVariable(m_softmu_delta_z0_sintheta, "softmu_delta_z0_sintheta");
        if (m_config->isMC()) {
          systematicTree->makeOutputVariable(m_softmu_true_type, "softmu_true_type");
          systematicTree->makeOutputVariable(m_softmu_true_origin, "softmu_true_origin");
          systematicTree->makeOutputVariable(m_softmu_SF_ID, "softmu_SF_ID");

          if (systematicTree->name() == nominalTTreeName || systematicTree->name() == nominalLooseTTreeName) {
            systematicTree->makeOutputVariable(m_softmu_SF_ID_STAT_UP, "softmu_SF_ID_STAT_UP");
            systematicTree->makeOutputVariable(m_softmu_SF_ID_STAT_DOWN, "softmu_SF_ID_STAT_DOWN");
            systematicTree->makeOutputVariable(m_softmu_SF_ID_SYST_UP, "softmu_SF_ID_SYST_UP");
            systematicTree->makeOutputVariable(m_softmu_SF_ID_SYST_DOWN, "softmu_SF_ID_SYST_DOWN");
            systematicTree->makeOutputVariable(m_softmu_SF_ID_STAT_LOWPT_UP, "softmu_SF_ID_STAT_LOWPT_UP");
            systematicTree->makeOutputVariable(m_softmu_SF_ID_STAT_LOWPT_DOWN, "softmu_SF_ID_STAT_LOWPT_DOWN");
            systematicTree->makeOutputVariable(m_softmu_SF_ID_SYST_LOWPT_UP, "softmu_SF_ID_SYST_LOWPT_UP");
            systematicTree->makeOutputVariable(m_softmu_SF_ID_SYST_LOWPT_DOWN, "softmu_SF_ID_SYST_LOWPT_DOWN");
          }
          if(m_config->softmuonAdditionalTruthInfo())
          {
            if(m_config->softmuonAdditionalTruthInfoCheckPartonOrigin()) systematicTree->makeOutputVariable(m_softmu_parton_origin_flag, "softmu_parton_origin_flag");
            systematicTree->makeOutputVariable(m_softmu_particle_origin_flag, "softmu_particle_origin_flag");
            systematicTree->makeOutputVariable(m_softmu_parent_pdgid,"softmu_parent_pdgid");
            systematicTree->makeOutputVariable(m_softmu_b_hadron_parent_pdgid,"softmu_b_hadron_parent_pdgid");
            systematicTree->makeOutputVariable(m_softmu_c_hadron_parent_pdgid,"softmu_c_hadron_parent_pdgid");
          }
        }//end of if (m_config->isMC())
      }//end of if (m_config->useSoftMuons())

      //photons
      if (m_config->usePhotons()) {
        systematicTree->makeOutputVariable(m_ph_pt, "ph_pt");
        systematicTree->makeOutputVariable(m_ph_eta, "ph_eta");
        systematicTree->makeOutputVariable(m_ph_phi, "ph_phi");
        systematicTree->makeOutputVariable(m_ph_e, "ph_e");
        systematicTree->makeOutputVariable(m_ph_iso, "ph_iso");
      }

      //taus
      if (m_config->useTaus()) {
        systematicTree->makeOutputVariable(m_tau_pt, "tau_pt");
        systematicTree->makeOutputVariable(m_tau_eta, "tau_eta");
        systematicTree->makeOutputVariable(m_tau_phi, "tau_phi");
        systematicTree->makeOutputVariable(m_tau_charge, "tau_charge");
      }

      //jets
      if (m_config->useJets()) {
        systematicTree->makeOutputVariable(m_jet_pt, "jet_pt");
        systematicTree->makeOutputVariable(m_jet_eta, "jet_eta");
        systematicTree->makeOutputVariable(m_jet_phi, "jet_phi");
        systematicTree->makeOutputVariable(m_jet_e, "jet_e");
        if (m_config->bTagAlgo_MV2c10_used()) {
          systematicTree->makeOutputVariable(m_jet_mv2c10, "jet_mv2c10");
        }
        systematicTree->makeOutputVariable(m_jet_jvt, "jet_jvt");
	if (m_config->doForwardJVTinMET() || m_config->getfJVTWP() != "None") {
	  systematicTree->makeOutputVariable(m_jet_fjvt, "jet_forwardjvt");
	  systematicTree->makeOutputVariable(m_jet_passfjvt, "jet_passforwardjvt");
	}
        if (m_config->isMC() && m_config->jetStoreTruthLabels()) {
          systematicTree->makeOutputVariable(m_jet_truthflav, "jet_truthflav");
          systematicTree->makeOutputVariable(m_jet_truthPartonLabel, "jet_truthPartonLabel");
          systematicTree->makeOutputVariable(m_jet_isTrueHS, "jet_isTrueHS");
          systematicTree->makeOutputVariable(m_jet_HadronConeExclExtendedTruthLabelID, "jet_truthflavExtended");
        }


        if (m_config->useJetGhostTrack()) {
          systematicTree->makeOutputVariable(m_jet_ghostTrack_pt, "jet_ghostTrack_pt");
          systematicTree->makeOutputVariable(m_jet_ghostTrack_eta, "jet_ghostTrack_eta");
          systematicTree->makeOutputVariable(m_jet_ghostTrack_phi, "jet_ghostTrack_phi");
          systematicTree->makeOutputVariable(m_jet_ghostTrack_e, "jet_ghostTrack_e");
          systematicTree->makeOutputVariable(m_jet_ghostTrack_d0, "jet_ghostTrack_d0");
          systematicTree->makeOutputVariable(m_jet_ghostTrack_z0, "jet_ghostTrack_z0");
          systematicTree->makeOutputVariable(m_jet_ghostTrack_qOverP, "jet_ghostTrack_qOverP");
        }


        for (auto& tagWP : m_config->bTagWP_available()) {
          if (tagWP.find("Continuous") == std::string::npos) systematicTree->makeOutputVariable(m_jet_isbtagged[tagWP], "jet_isbtagged_" + shortBtagWP(
              tagWP));
          else systematicTree->makeOutputVariable(m_jet_tagWeightBin[tagWP], "jet_tagWeightBin_" + tagWP);
        }

        for (const std::string& algo : m_config->bTagAlgo_available()) {
          systematicTree->makeOutputVariable(m_jet_DLx[algo], "jet_" + algo);
          systematicTree->makeOutputVariable(m_jet_DLx_pb[algo], "jet_" + algo + "_pb");
          systematicTree->makeOutputVariable(m_jet_DLx_pc[algo], "jet_" + algo + "_pc");
          systematicTree->makeOutputVariable(m_jet_DLx_pu[algo], "jet_" + algo + "_pu");
        }
      }

      // fail-JVT jets
      if (m_config->saveFailJVTJets()) {
        systematicTree->makeOutputVariable(m_failJvt_jet_pt, "failJvt_jet_pt");
        systematicTree->makeOutputVariable(m_failJvt_jet_eta, "failJvt_jet_eta");
        systematicTree->makeOutputVariable(m_failJvt_jet_phi, "failJvt_jet_phi");
        systematicTree->makeOutputVariable(m_failJvt_jet_e, "failJvt_jet_e");
        systematicTree->makeOutputVariable(m_failJvt_jet_jvt, "failJvt_jet_jvt");
	if (m_config->doForwardJVTinMET() || m_config->getfJVTWP() != "None") {
	  systematicTree->makeOutputVariable(m_failJvt_jet_fjvt, "failJvt_jet_forwardjvt");
	  systematicTree->makeOutputVariable(m_failJvt_jet_passfjvt, "failJvt_jet_passforwardjvt");
	}
        if (m_config->isMC() && m_config->jetStoreTruthLabels()) {
          systematicTree->makeOutputVariable(m_failJvt_jet_truthflav, "failJvt_jet_truthflav");
          systematicTree->makeOutputVariable(m_failJvt_jet_truthPartonLabel, "failJvt_jet_truthPartonLabel");
          systematicTree->makeOutputVariable(m_failJvt_jet_isTrueHS, "failJvt_jet_isTrueHS");
          systematicTree->makeOutputVariable(m_failJvt_jet_HadronConeExclExtendedTruthLabelID,
                                             "failJvt_jet_truthflavExtended");
        }

        if (m_config->useJetGhostTrack()) {
          systematicTree->makeOutputVariable(m_failJvt_jet_ghostTrack_pt, "failJvt_jet_ghostTrack_pt");
          systematicTree->makeOutputVariable(m_failJvt_jet_ghostTrack_eta, "failJvt_jet_ghostTrack_eta");
          systematicTree->makeOutputVariable(m_failJvt_jet_ghostTrack_phi, "failJvt_jet_ghostTrack_phi");
          systematicTree->makeOutputVariable(m_failJvt_jet_ghostTrack_e, "failJvt_jet_ghostTrack_e");
          systematicTree->makeOutputVariable(m_failJvt_jet_ghostTrack_d0, "failJvt_jet_ghostTrack_d0");
          systematicTree->makeOutputVariable(m_failJvt_jet_ghostTrack_z0, "failJvt_jet_ghostTrack_z0");
          systematicTree->makeOutputVariable(m_failJvt_jet_ghostTrack_qOverP, "failJvt_jet_ghostTrack_qOverP");
        }
      }

      // fail-FJVT jets
      if (m_config->getfJVTWP() != "None" && m_config->saveFailForwardJVTJets()) {
        systematicTree->makeOutputVariable(m_failFJvt_jet_pt, "failforwardJvt_jet_pt");
        systematicTree->makeOutputVariable(m_failFJvt_jet_eta, "failforwardJvt_jet_eta");
        systematicTree->makeOutputVariable(m_failFJvt_jet_phi, "failforwardJvt_jet_phi");
        systematicTree->makeOutputVariable(m_failFJvt_jet_e, "failforwardJvt_jet_e");
        systematicTree->makeOutputVariable(m_failFJvt_jet_jvt, "failforwardJvt_jet_jvt");
	systematicTree->makeOutputVariable(m_failFJvt_jet_fjvt, "failforwardJvt_jet_forwardjvt");
        systematicTree->makeOutputVariable(m_failFJvt_jet_passjvt, "failforwardJvt_jet_passjvt");

        if (m_config->isMC() && m_config->jetStoreTruthLabels()) {
          systematicTree->makeOutputVariable(m_failFJvt_jet_truthflav, "failforwardJvt_jet_truthflav");
          systematicTree->makeOutputVariable(m_failFJvt_jet_truthPartonLabel, "failforwardJvt_jet_truthPartonLabel");
          systematicTree->makeOutputVariable(m_failFJvt_jet_isTrueHS, "failforwardJvt_jet_isTrueHS");
          systematicTree->makeOutputVariable(m_failFJvt_jet_HadronConeExclExtendedTruthLabelID,
                                             "failforwardJvt_jet_truthflavExtended");
        }

        if (m_config->useJetGhostTrack()) {
          systematicTree->makeOutputVariable(m_failFJvt_jet_ghostTrack_pt, "failforwardJvt_jet_ghostTrack_pt");
          systematicTree->makeOutputVariable(m_failFJvt_jet_ghostTrack_eta, "failforwardJvt_jet_ghostTrack_eta");
          systematicTree->makeOutputVariable(m_failFJvt_jet_ghostTrack_phi, "failforwardJvt_jet_ghostTrack_phi");
          systematicTree->makeOutputVariable(m_failFJvt_jet_ghostTrack_e, "failforwardJvt_jet_ghostTrack_e");
          systematicTree->makeOutputVariable(m_failFJvt_jet_ghostTrack_d0, "failforwardJvt_jet_ghostTrack_d0");
          systematicTree->makeOutputVariable(m_failFJvt_jet_ghostTrack_z0, "failforwardJvt_jet_ghostTrack_z0");
          systematicTree->makeOutputVariable(m_failFJvt_jet_ghostTrack_qOverP, "failforwardJvt_jet_ghostTrack_qOverP");
        }
      }

      //large-R jets
      if (m_config->useLargeRJets()) {
        systematicTree->makeOutputVariable(m_ljet_pt, "ljet_pt");
        systematicTree->makeOutputVariable(m_ljet_eta, "ljet_eta");
        systematicTree->makeOutputVariable(m_ljet_phi, "ljet_phi");
        systematicTree->makeOutputVariable(m_ljet_e, "ljet_e");
        systematicTree->makeOutputVariable(m_ljet_m, "ljet_m");

	for (const std::pair<std::string,std::string>& it : m_config->largeRJetSubstructureVariables()) {
	  systematicTree->makeOutputVariable(m_ljet_substructure[it.first],"ljet_"+it.first);
	} 

        for (const std::string& taggerName : m_boostedJetTaggersNames) {
          systematicTree->makeOutputVariable(m_ljet_isTagged[taggerName], "ljet_isTagged_" + taggerName);
        }

        if (m_config->isMC()) {
          systematicTree->makeOutputVariable(m_ljet_truthLabel, "ljet_truthLabel");
          for (const std::string& taggerName : m_boostedJetTaggersNamesCalibrated) {
            systematicTree->makeOutputVariable(m_ljet_tagSF[taggerName], "ljet_tagSF_" + taggerName);
          }
        }
      }

      //track jets
      if (m_config->useTrackJets()) {
        systematicTree->makeOutputVariable(m_tjet_pt, "tjet_pt");
        systematicTree->makeOutputVariable(m_tjet_eta, "tjet_eta");
        systematicTree->makeOutputVariable(m_tjet_phi, "tjet_phi");
        systematicTree->makeOutputVariable(m_tjet_e, "tjet_e");
        if (m_config->bTagAlgo_MV2c10_used_trkJet()) {
          systematicTree->makeOutputVariable(m_tjet_mv2c10, "tjet_mv2c10");
        }
        for (auto& tagWP : m_config->bTagWP_available_trkJet()) {
          if (tagWP.find("Continuous") == std::string::npos) systematicTree->makeOutputVariable(m_tjet_isbtagged[tagWP], "tjet_isbtagged_" + shortBtagWP(tagWP));
          else systematicTree->makeOutputVariable(m_tjet_tagWeightBin[tagWP], "tjet_tagWeightBin_" + tagWP);
        }

        for (const std::string& algo : m_config->bTagAlgo_available_trkJet()) {
          systematicTree->makeOutputVariable(m_tjet_DLx[algo], "tjet_" + algo);
          systematicTree->makeOutputVariable(m_tjet_DLx_pb[algo], "tjet_" + algo + "_pb");
          systematicTree->makeOutputVariable(m_tjet_DLx_pc[algo], "tjet_" + algo + "_pc");
          systematicTree->makeOutputVariable(m_tjet_DLx_pu[algo], "tjet_" + algo + "_pu");
        }
      }

      if (m_config->useTracks()) {
	systematicTree->makeOutputVariable(m_track_pt,     "track_pt");
	systematicTree->makeOutputVariable(m_track_eta,    "track_eta");
	systematicTree->makeOutputVariable(m_track_phi,    "track_phi");
	systematicTree->makeOutputVariable(m_track_e,      "track_e");
	systematicTree->makeOutputVariable(m_track_charge, "track_charge");
	systematicTree->makeOutputVariable(m_track_d0,     "track_d0");
	systematicTree->makeOutputVariable(m_track_d0_significance, "track_d0_significance");
	systematicTree->makeOutputVariable(m_track_z0,     "track_z0");
	systematicTree->makeOutputVariable(m_track_z0_significance, "track_z0_significance");
	systematicTree->makeOutputVariable(m_track_phi0,   "track_phi0");
	systematicTree->makeOutputVariable(m_track_theta,  "track_theta");
	systematicTree->makeOutputVariable(m_track_qOverP, "track_qOverP");
	systematicTree->makeOutputVariable(m_track_chiSquared, "track_chiSquared");
	systematicTree->makeOutputVariable(m_track_numberDoF, "track_numberDoF");
      }
      


      // RC branches
      if (m_makeRCJets) {
        systematicTree->makeOutputVariable(m_rcjet_pt, "rcjet_pt");
        systematicTree->makeOutputVariable(m_rcjet_eta, "rcjet_eta");
        systematicTree->makeOutputVariable(m_rcjet_phi, "rcjet_phi");
        systematicTree->makeOutputVariable(m_rcjet_e, "rcjet_e");
        systematicTree->makeOutputVariable(m_rcjet_d12, "rcjet_d12"); // requires >= 2 subjets
        systematicTree->makeOutputVariable(m_rcjet_d23, "rcjet_d23"); // requires >= 3 subjets
        systematicTree->makeOutputVariable(m_rcjetsub_pt, "rcjetsub_pt");  // vector of vectors for subjet info
        systematicTree->makeOutputVariable(m_rcjetsub_eta, "rcjetsub_eta");
        systematicTree->makeOutputVariable(m_rcjetsub_phi, "rcjetsub_phi");
        systematicTree->makeOutputVariable(m_rcjetsub_e, "rcjetsub_e");
        if (m_config->bTagAlgo_MV2c10_used()) {
          systematicTree->makeOutputVariable(m_rcjetsub_mv2c10, "rcjetsub_mv2c10");
        }

        if (m_useRCJSS || m_useRCAdditionalJSS) {
          systematicTree->makeOutputVariable(m_rrcjet_pt, "rrcjet_pt");
          systematicTree->makeOutputVariable(m_rrcjet_eta, "rrcjet_eta");
          systematicTree->makeOutputVariable(m_rrcjet_phi, "rrcjet_phi");
          systematicTree->makeOutputVariable(m_rrcjet_e, "rrcjet_e");
        }
        if (m_useRCJSS) {
          // RCJet SS from Clusters
          systematicTree->makeOutputVariable(m_rcjet_tau32_clstr, "rcjet_tau32_clstr");
          systematicTree->makeOutputVariable(m_rcjet_tau21_clstr, "rcjet_tau21_clstr");
          systematicTree->makeOutputVariable(m_rcjet_tau3_clstr, "rcjet_tau3_clstr");
          systematicTree->makeOutputVariable(m_rcjet_tau2_clstr, "rcjet_tau2_clstr");
          systematicTree->makeOutputVariable(m_rcjet_tau1_clstr, "rcjet_tau1_clstr");
          systematicTree->makeOutputVariable(m_rcjet_d12_clstr, "rcjet_d12_clstr");
          systematicTree->makeOutputVariable(m_rcjet_d23_clstr, "rcjet_d23_clstr");
          systematicTree->makeOutputVariable(m_rcjet_Qw_clstr, "rcjet_Qw_clstr");
          systematicTree->makeOutputVariable(m_rcjet_nconstituent_clstr, "rcjet_nconstituent_clstr");
	  systematicTree->makeOutputVariable(m_rcjet_D2_clstr, "rcjet_D2_clstr");
          systematicTree->makeOutputVariable(m_rcjet_ECF1_clstr, "rcjet_ECF1_clstr");
          systematicTree->makeOutputVariable(m_rcjet_ECF2_clstr, "rcjet_ECF2_clstr");
          systematicTree->makeOutputVariable(m_rcjet_ECF3_clstr, "rcjet_ECF3_clstr");
        }
        if (m_useRCAdditionalJSS) {
          systematicTree->makeOutputVariable(m_rcjet_gECF332_clstr, "rcjet_gECF332_clstr");
          systematicTree->makeOutputVariable(m_rcjet_gECF461_clstr, "rcjet_gECF461_clstr");
          systematicTree->makeOutputVariable(m_rcjet_gECF322_clstr, "rcjet_gECF322_clstr");
          systematicTree->makeOutputVariable(m_rcjet_gECF331_clstr, "rcjet_gECF331_clstr");
          systematicTree->makeOutputVariable(m_rcjet_gECF422_clstr, "rcjet_gECF422_clstr");
          systematicTree->makeOutputVariable(m_rcjet_gECF441_clstr, "rcjet_gECF441_clstr");
          systematicTree->makeOutputVariable(m_rcjet_gECF212_clstr, "rcjet_gECF212_clstr");
          systematicTree->makeOutputVariable(m_rcjet_gECF321_clstr, "rcjet_gECF321_clstr");
          systematicTree->makeOutputVariable(m_rcjet_gECF311_clstr, "rcjet_gECF311_clstr");
          systematicTree->makeOutputVariable(m_rcjet_L1_clstr, "rcjet_L1_clstr");
          systematicTree->makeOutputVariable(m_rcjet_L2_clstr, "rcjet_L2_clstr");
          systematicTree->makeOutputVariable(m_rcjet_L3_clstr, "rcjet_L3_clstr");
          systematicTree->makeOutputVariable(m_rcjet_L4_clstr, "rcjet_L4_clstr");
          systematicTree->makeOutputVariable(m_rcjet_L5_clstr, "rcjet_L5_clstr");
        }
      }
      // vRC branches
      if (m_makeVarRCJets) {
        std::string VarRC = "vrcjet";

        for (auto& rho : m_VarRCJetRho) {
          for (auto& mass_scale : m_VarRCJetMassScale) {
            std::replace(rho.begin(), rho.end(), '.', '_');
            std::string name = rho + mass_scale;
            systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_pt"], VarRC + "_" + name + "_pt");
            systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_eta"], VarRC + "_" + name + "_eta");
            systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_phi"], VarRC + "_" + name + "_phi");
            systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_e"], VarRC + "_" + name + "_e");
            systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_d12"], VarRC + "_" + name + "_d12"); // requires
                                                                                                                              // >=
                                                                                                                              // 2
                                                                                                                              // subjets
            systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_d23"], VarRC + "_" + name + "_d23"); // requires
                                                                                                                              // >=
                                                                                                                              // 3
                                                                                                                              // subjets
            systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_pt"], VarRC + "sub_" + name + "_pt");  // vector
                                                                                                                                       // of
                                                                                                                                       // vectors
                                                                                                                                       // for
                                                                                                                                       // subjet
                                                                                                                                       // info
            systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_eta"], VarRC + "sub_" + name + "_eta");
            systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_phi"], VarRC + "sub_" + name + "_phi");
            systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_e"], VarRC + "sub_" + name + "_e");
            if (m_config->bTagAlgo_MV2c10_used()) {
              systematicTree->makeOutputVariable(m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_mv2c10"], VarRC + "sub_" + name + "_mv2c10");
            }

            if (m_useVarRCJSS || m_useVarRCAdditionalJSS) {
              systematicTree->makeOutputVariable(m_VarRCjetBranches["vrrcjet_" + name + "_pt"], "vrrcjet_" + name + "_pt");
              systematicTree->makeOutputVariable(m_VarRCjetBranches["vrrcjet_" + name + "_eta"], "vrrcjet_" + name + "_eta");
              systematicTree->makeOutputVariable(m_VarRCjetBranches["vrrcjet_" + name + "_phi"], "vrrcjet_" + name + "_phi");
              systematicTree->makeOutputVariable(m_VarRCjetBranches["vrrcjet_" + name + "_e"], "vrrcjet_" + name + "_e");
            }
            if (m_useVarRCJSS) {
              // RCJet SS from Clusters
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_tau32_clstr"], VarRC + "_" + name + "_tau32_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_tau21_clstr"], VarRC + "_" + name + "_tau21_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_tau3_clstr"], VarRC + "_" + name + "_tau3_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_tau2_clstr"], VarRC + "_" + name + "_tau2_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_tau1_clstr"], VarRC + "_" + name + "_tau1_clstr");

              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_d12_clstr"], VarRC + "_" + name + "_d12_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_d23_clstr"], VarRC + "_" + name + "_d23_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_Qw_clstr"], VarRC + "_" + name + "_Qw_clstr");
	      
	      systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_D2_clstr"], VarRC + "_" + name + "_D2_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_ECF1_clstr"], VarRC + "_" + name + "_ECF1_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_ECF2_clstr"], VarRC + "_" + name + "_ECF2_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_ECF3_clstr"], VarRC + "_" + name + "_ECF3_clstr");
            }
            if (m_useVarRCAdditionalJSS) {

              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF332_clstr"], VarRC + "_" + name + "_gECF332_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF461_clstr"], VarRC + "_" + name + "_gECF461_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF322_clstr"], VarRC + "_" + name + "_gECF322_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF331_clstr"], VarRC + "_" + name + "_gECF331_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF422_clstr"], VarRC + "_" + name + "_gECF422_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF441_clstr"], VarRC + "_" + name + "_gECF441_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF212_clstr"], VarRC + "_" + name + "_gECF212_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF321_clstr"], VarRC + "_" + name + "_gECF321_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_gECF311_clstr"], VarRC + "_" + name + "_gECF311_clstr");

              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_L1_clstr"], VarRC + "_" + name + "_L1_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_L2_clstr"], VarRC + "_" + name + "_L2_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_L3_clstr"], VarRC + "_" + name + "_L3_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_L4_clstr"], VarRC + "_" + name + "_L4_clstr");
              systematicTree->makeOutputVariable(m_VarRCjetBranches[VarRC + "_" + name + "_L5_clstr"], VarRC + "_" + name + "_L5_clstr");
            }
          } // end loop over mass parameters
        } // end loop over multipliers for mass scale
      } // end if VarRC jets

      //met
      systematicTree->makeOutputVariable(m_met_met, "met_met");
      systematicTree->makeOutputVariable(m_met_phi, "met_phi");
      //these are for specific studies on the met, turned off by default, and turned on with the WriteMETBuiltWithLooseObjects option
      if(m_config->writeMETBuiltWithLooseObjects())
      {
        systematicTree->makeOutputVariable(m_met_met_withLooseObjects, "met_met_withLooseObjects");
        systematicTree->makeOutputVariable(m_met_phi_withLooseObjects, "met_phi_withLooseObjects");
      }

      if (m_config->doKLFitter()) {
        /// Global result
        systematicTree->makeOutputVariable(m_klfitter_logLikelihood, "klfitter_logLikelihood");
        systematicTree->makeOutputVariable(m_klfitter_eventProbability, "klfitter_eventProbability");
        systematicTree->makeOutputVariable(m_klfitter_selected, "klfitter_selected");

        // If FULL information is requested
        if (m_config->KLFitterOutput() == "FULL") {
          /// Debugging information
          systematicTree->makeOutputVariable(m_klfitter_selection, "klfitter_selection");
          systematicTree->makeOutputVariable(m_klfitter_minuitDidNotConverge, "klfitter_minuitDidNotConverge");
          systematicTree->makeOutputVariable(m_klfitter_fitAbortedDueToNaN, "klfitter_fitAbortedDueToNaN");
          systematicTree->makeOutputVariable(m_klfitter_atLeastOneFitParameterAtItsLimit, "klfitter_atLeastOneFitParameterAtItsLimit");
          systematicTree->makeOutputVariable(m_klfitter_invalidTransferFunctionAtConvergence, "klfitter_invalidTransferFunctionAtConvergence");
          /// Global
          systematicTree->makeOutputVariable(m_klfitter_parameters_size, "klfitter_parameters_size");
          systematicTree->makeOutputVariable(m_klfitter_parameters, "klfitter_parameters");
          systematicTree->makeOutputVariable(m_klfitter_parameterErrors, "klfitter_parameterErrors");
          systematicTree->makeOutputVariable(m_klfitter_bestPermutation, "klfitter_bestPermutation");
        }

        if (m_config->KLFitterOutput() == "FULL" || m_config->KLFitterOutput() == "JETPERM_ONLY") {
          /// Model
          if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles" || m_config->KLFitterLH() == "ttbar_BoostedLJets") {
            systematicTree->makeOutputVariable(m_klfitter_model_bhad_pt, "klfitter_model_bhad_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_bhad_eta, "klfitter_model_bhad_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_bhad_phi, "klfitter_model_bhad_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_bhad_E, "klfitter_model_bhad_E");
            systematicTree->makeOutputVariable(m_klfitter_model_bhad_jetIndex, "klfitter_model_bhad_jetIndex");

            systematicTree->makeOutputVariable(m_klfitter_model_blep_pt, "klfitter_model_blep_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_blep_eta, "klfitter_model_blep_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_blep_phi, "klfitter_model_blep_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_blep_E, "klfitter_model_blep_E");
            systematicTree->makeOutputVariable(m_klfitter_model_blep_jetIndex, "klfitter_model_blep_jetIndex");

            systematicTree->makeOutputVariable(m_klfitter_model_lq1_pt, "klfitter_model_lq1_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_lq1_eta, "klfitter_model_lq1_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_lq1_phi, "klfitter_model_lq1_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_lq1_E, "klfitter_model_lq1_E");
            systematicTree->makeOutputVariable(m_klfitter_model_lq1_jetIndex, "klfitter_model_lq1_jetIndex");

            systematicTree->makeOutputVariable(m_klfitter_model_lep_pt, "klfitter_model_lep_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_lep_eta, "klfitter_model_lep_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_lep_phi, "klfitter_model_lep_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_lep_E, "klfitter_model_lep_E");

            systematicTree->makeOutputVariable(m_klfitter_model_nu_pt, "klfitter_model_nu_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_nu_eta, "klfitter_model_nu_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_nu_phi, "klfitter_model_nu_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_nu_E, "klfitter_model_nu_E");

            if (m_config->KLFitterLH() != "ttbar_BoostedLJets") {
              systematicTree->makeOutputVariable(m_klfitter_model_lq2_pt, "klfitter_model_lq2_pt");
              systematicTree->makeOutputVariable(m_klfitter_model_lq2_eta, "klfitter_model_lq2_eta");
              systematicTree->makeOutputVariable(m_klfitter_model_lq2_phi, "klfitter_model_lq2_phi");
              systematicTree->makeOutputVariable(m_klfitter_model_lq2_E, "klfitter_model_lq2_E");
              systematicTree->makeOutputVariable(m_klfitter_model_lq2_jetIndex, "klfitter_model_lq2_jetIndex");

              if (m_config->KLFitterLH() == "ttZTrilepton") {
                systematicTree->makeOutputVariable(m_klfitter_model_lep_index, "klfitter_model_lep_index");

                systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_pt, "klfitter_model_lepZ1_pt");
                systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_eta, "klfitter_model_lepZ1_eta");
                systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_phi, "klfitter_model_lepZ1_phi");
                systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_E, "klfitter_model_lepZ1_E");
                systematicTree->makeOutputVariable(m_klfitter_model_lepZ1_index, "klfitter_model_lepZ1_index");

                systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_pt, "klfitter_model_lepZ2_pt");
                systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_eta, "klfitter_model_lepZ2_eta");
                systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_phi, "klfitter_model_lepZ2_phi");
                systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_E, "klfitter_model_lepZ2_E");
                systematicTree->makeOutputVariable(m_klfitter_model_lepZ2_index, "klfitter_model_lepZ2_index");
              }

              if (m_config->KLFitterLH() == "ttH") {
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_pt, "klfitter_model_Higgs_b1_pt");
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_eta, "klfitter_model_Higgs_b1_eta");
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_phi, "klfitter_model_Higgs_b1_phi");
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_E, "klfitter_model_Higgs_b1_E");
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b1_jetIndex, "klfitter_model_Higgs_b1_jetIndex");

                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_pt, "klfitter_model_Higgs_b2_pt");
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_eta, "klfitter_model_Higgs_b2_eta");
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_phi, "klfitter_model_Higgs_b2_phi");
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_E, "klfitter_model_Higgs_b2_E");
                systematicTree->makeOutputVariable(m_klfitter_model_Higgs_b2_jetIndex, "klfitter_model_Higgs_b2_jetIndex");
              }
            }
          } else if (m_config->KLFitterLH() == "ttbar_AllHadronic") {
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_pt, "klfitter_model_b_from_top1_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_eta, "klfitter_model_b_from_top1_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_phi, "klfitter_model_b_from_top1_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_E, "klfitter_model_b_from_top1_E");
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top1_jetIndex, "klfitter_model_b_from_top1_jetIndex");

            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_pt, "klfitter_model_b_from_top2_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_eta, "klfitter_model_b_from_top2_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_phi, "klfitter_model_b_from_top2_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_E, "klfitter_model_b_from_top2_E");
            systematicTree->makeOutputVariable(m_klfitter_model_b_from_top2_jetIndex, "klfitter_model_b_from_top2_jetIndex");

            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_pt, "klfitter_model_lj1_from_top1_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_eta, "klfitter_model_lj1_from_top1_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_phi, "klfitter_model_lj1_from_top1_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_E, "klfitter_model_lj1_from_top1_E");
            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top1_jetIndex, "klfitter_model_lj1_from_top1_jetIndex");

            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_pt, "klfitter_model_lj2_from_top1_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_eta, "klfitter_model_lj2_from_top1_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_phi, "klfitter_model_lj2_from_top1_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_E, "klfitter_model_lj2_from_top1_E");
            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top1_jetIndex, "klfitter_model_lj2_from_top1_jetIndex");

            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_pt, "klfitter_model_lj1_from_top2_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_eta, "klfitter_model_lj1_from_top2_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_phi, "klfitter_model_lj1_from_top2_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_E, "klfitter_model_lj1_from_top2_E");
            systematicTree->makeOutputVariable(m_klfitter_model_lj1_from_top2_jetIndex, "klfitter_model_lj1_from_top2_jetIndex");

            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_pt, "klfitter_model_lj2_from_top2_pt");
            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_eta, "klfitter_model_lj2_from_top2_eta");
            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_phi, "klfitter_model_lj2_from_top2_phi");
            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_E, "klfitter_model_lj2_from_top2_E");
            systematicTree->makeOutputVariable(m_klfitter_model_lj2_from_top2_jetIndex, "klfitter_model_lj2_from_top2_jetIndex");
          }
        }

        if (m_config->KLFitterOutput() == "FULL" || m_config->KLFitterOutput() == "FITTEDTOPS_ONLY") {
          if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles") {
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_pt, "klfitter_bestPerm_topLep_pt");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_eta, "klfitter_bestPerm_topLep_eta");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_phi, "klfitter_bestPerm_topLep_phi");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_E, "klfitter_bestPerm_topLep_E");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topLep_m, "klfitter_bestPerm_topLep_m");

            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_pt, "klfitter_bestPerm_topHad_pt");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_eta, "klfitter_bestPerm_topHad_eta");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_phi, "klfitter_bestPerm_topHad_phi");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_E, "klfitter_bestPerm_topHad_E");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_topHad_m, "klfitter_bestPerm_topHad_m");

            systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_pt, "klfitter_bestPerm_ttbar_pt");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_eta, "klfitter_bestPerm_ttbar_eta");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_phi, "klfitter_bestPerm_ttbar_phi");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_E, "klfitter_bestPerm_ttbar_E");
            systematicTree->makeOutputVariable(m_klfitter_bestPerm_ttbar_m, "klfitter_bestPerm_ttbar_m");
          }
        }
      }

      if (m_config->doPseudoTop()) {
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_ttbar_pt, "PseudoTop_Reco_ttbar_pt");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_ttbar_eta, "PseudoTop_Reco_ttbar_eta");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_ttbar_phi, "PseudoTop_Reco_ttbar_phi");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_ttbar_m, "PseudoTop_Reco_ttbar_m");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_had_pt, "PseudoTop_Reco_top_had_pt");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_had_eta, "PseudoTop_Reco_top_had_eta");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_had_phi, "PseudoTop_Reco_top_had_phi");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_had_m, "PseudoTop_Reco_top_had_m");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_lep_pt, "PseudoTop_Reco_top_lep_pt");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_lep_eta, "PseudoTop_Reco_top_lep_eta");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_lep_phi, "PseudoTop_Reco_top_lep_phi");
        systematicTree->makeOutputVariable(m_PseudoTop_Reco_top_lep_m, "PseudoTop_Reco_top_lep_m");
      }

      //extra branches telling you if the event passed / failed a selection
      int index(0);
      for (const auto& branchName : m_extraBranches) {
        m_selectionDecisions[index] = 0;
        systematicTree->makeOutputVariable(m_selectionDecisions[index], branchName);
        // Add all triggers to a map so we don't get any duplicates
        for (auto& trigger_name : m_config->allTriggers_Tight(branchName)) {
          m_triggerDecisions [trigger_name] = 0;
        }
        for (auto& trigger_name : m_config->allTriggers_Loose(branchName)) {
          // let's make sure this isn't done twice
          if (m_triggerDecisions.find(trigger_name) != m_triggerDecisions.end()
              && m_triggerPrescales.find(trigger_name) != m_triggerPrescales.end()) continue;
          m_triggerDecisions [trigger_name] = 0;
        }
        for (auto& trigger_name : m_config->electronTriggers_Tight(branchName))
          m_el_trigMatched [trigger_name] = std::vector<char>();
        for (auto& trigger_name : m_config->electronTriggers_Loose(branchName)) {
          // let's make sure this isn't done twice
          if (m_el_trigMatched.find(trigger_name) != m_el_trigMatched.end()) continue;
          m_el_trigMatched [trigger_name] = std::vector<char>();
        }
        for (auto& trigger_name : m_config->muonTriggers_Tight(branchName))
          m_mu_trigMatched [trigger_name] = std::vector<char>();
        for (auto& trigger_name : m_config->muonTriggers_Loose(branchName)) {
          // let's make sure this isn't done twice
          if (m_mu_trigMatched.find(trigger_name) != m_mu_trigMatched.end()) continue;
          m_mu_trigMatched [trigger_name] = std::vector<char>();
        }
        ++index;
      }

      for (auto& trig_name : m_triggerDecisions)
        systematicTree->makeOutputVariable(trig_name.second, trig_name.first);
      for (auto& trig_name : m_el_trigMatched)
        systematicTree->makeOutputVariable(trig_name.second, "el_trigMatch_" + trig_name.first);
      for (auto& trig_name : m_mu_trigMatched)
        systematicTree->makeOutputVariable(trig_name.second, "mu_trigMatch_" + trig_name.first);
    }

    setupUpgradeTreeManager();
    setupParticleLevelTreeManager();
  }

  void EventSaverFlatNtuple::execute() {
    if (m_config->isMC() && m_config->doMCGeneratorWeights()) {
      loadMCGeneratorWeights();
    }

    if (m_config->isMC() && m_config->doTruthPDFInfo()) {
      loadPdfInfo();
    }

    if (m_config->isMC() && m_config->saveLHAPDFEvent()) {
      loadPdfWeights();
    }
  }

  void EventSaverFlatNtuple::setupParticleLevelTreeManager(/*const top::ParticleLevelEvent& plEvent*/) {
    // Quick return if particle level is disabled or the tree is already initialised.
    // If particle level is disabled, no tree will be created.
    if (not m_config->doTopParticleLevel() or m_particleLevelTreeManager) {
      return;
    }

    m_particleLevelTreeManager = std::make_shared<top::TreeManager>("particleLevel", m_outputFile, m_config->outputFileNEventAutoFlush(), m_config->outputFileBasketSizePrimitive(), m_config->outputFileBasketSizeVector());

    m_particleLevelTreeManager->branchFilters() = branchFilters();
    m_particleLevelTreeManager->makeOutputVariable(m_weight_mc, "weight_mc");

    //event info
    m_particleLevelTreeManager->makeOutputVariable(m_eventNumber, "eventNumber");
    m_particleLevelTreeManager->makeOutputVariable(m_runNumber, "runNumber");
    if (m_config->isMC() && m_config->doPileupReweighting()) m_particleLevelTreeManager->makeOutputVariable(m_randomRunNumber, "randomRunNumber");
    m_particleLevelTreeManager->makeOutputVariable(m_mcChannelNumber, "mcChannelNumber");
    m_particleLevelTreeManager->makeOutputVariable(m_mu, "mu");
    m_particleLevelTreeManager->makeOutputVariable(m_mu_actual, "mu_actual");
    m_particleLevelTreeManager->makeOutputVariable(m_weight_pileup, "weight_pileup");

    if (m_config->doPseudoTop()) {
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_ttbar_pt, "PseudoTop_Particle_ttbar_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_ttbar_eta, "PseudoTop_Particle_ttbar_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_ttbar_phi, "PseudoTop_Particle_ttbar_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_ttbar_m, "PseudoTop_Particle_ttbar_m");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_had_pt, "PseudoTop_Particle_top_had_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_had_eta, "PseudoTop_Particle_top_had_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_had_phi, "PseudoTop_Particle_top_had_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_had_m, "PseudoTop_Particle_top_had_m");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_lep_pt, "PseudoTop_Particle_top_lep_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_lep_eta, "PseudoTop_Particle_top_lep_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_lep_phi, "PseudoTop_Particle_top_lep_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_PseudoTop_Particle_top_lep_m, "PseudoTop_Particle_top_lep_m");
    }

    //electrons
    if (m_config->useTruthElectrons()) {
      m_particleLevelTreeManager->makeOutputVariable(m_el_pt, "el_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_el_eta, "el_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_el_phi, "el_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_el_e, "el_e");
      m_particleLevelTreeManager->makeOutputVariable(m_el_charge, "el_charge");
      
      m_particleLevelTreeManager->makeOutputVariable(m_el_true_type, "el_true_type");
      m_particleLevelTreeManager->makeOutputVariable(m_el_true_origin, "el_true_origin");

      m_particleLevelTreeManager->makeOutputVariable(m_el_pt_bare, "el_pt_bare");
      m_particleLevelTreeManager->makeOutputVariable(m_el_eta_bare, "el_eta_bare");
      m_particleLevelTreeManager->makeOutputVariable(m_el_phi_bare, "el_phi_bare");
      m_particleLevelTreeManager->makeOutputVariable(m_el_e_bare, "el_e_bare");
    }

    //muons
    if (m_config->useTruthMuons()) {
      m_particleLevelTreeManager->makeOutputVariable(m_mu_pt, "mu_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_mu_eta, "mu_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_mu_phi, "mu_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_mu_e, "mu_e");
      m_particleLevelTreeManager->makeOutputVariable(m_mu_charge, "mu_charge");
      
      m_particleLevelTreeManager->makeOutputVariable(m_mu_true_type, "mu_true_type");
      m_particleLevelTreeManager->makeOutputVariable(m_mu_true_origin, "mu_true_origin");

      m_particleLevelTreeManager->makeOutputVariable(m_mu_pt_bare, "mu_pt_bare");
      m_particleLevelTreeManager->makeOutputVariable(m_mu_eta_bare, "mu_eta_bare");
      m_particleLevelTreeManager->makeOutputVariable(m_mu_phi_bare, "mu_phi_bare");
      m_particleLevelTreeManager->makeOutputVariable(m_mu_e_bare, "mu_e_bare");
      
      if(m_config->useSoftMuons())
      {
        m_particleLevelTreeManager->makeOutputVariable(m_softmu_pt, "softmu_pt");
        m_particleLevelTreeManager->makeOutputVariable(m_softmu_eta, "softmu_eta");
        m_particleLevelTreeManager->makeOutputVariable(m_softmu_phi, "softmu_phi");
        m_particleLevelTreeManager->makeOutputVariable(m_softmu_e, "softmu_e");
        m_particleLevelTreeManager->makeOutputVariable(m_softmu_charge, "softmu_charge");
        
        m_particleLevelTreeManager->makeOutputVariable(m_softmu_true_type, "softmu_true_type");
        m_particleLevelTreeManager->makeOutputVariable(m_softmu_true_origin, "softmu_true_origin");
        
        if(m_config->softmuonAdditionalTruthInfo())
        {
          if(m_config->softmuonAdditionalTruthInfoCheckPartonOrigin()) m_particleLevelTreeManager->makeOutputVariable(m_softmu_parton_origin_flag, "softmu_parton_origin_flag");
          m_particleLevelTreeManager->makeOutputVariable(m_softmu_particle_origin_flag, "softmu_particle_origin_flag");
          m_particleLevelTreeManager->makeOutputVariable(m_softmu_parent_pdgid,"softmu_parent_pdgid");
          m_particleLevelTreeManager->makeOutputVariable(m_softmu_b_hadron_parent_pdgid,"softmu_b_hadron_parent_pdgid");
          m_particleLevelTreeManager->makeOutputVariable(m_softmu_c_hadron_parent_pdgid,"softmu_c_hadron_parent_pdgid");
        }
      }
    }//end of muons branches

    //photons
    if (m_config->useTruthPhotons()) {
      m_particleLevelTreeManager->makeOutputVariable(m_ph_pt, "ph_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_ph_eta, "ph_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_ph_phi, "ph_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_ph_e, "ph_e");
    }

    //jets
    if (m_config->useTruthJets()) {
      m_particleLevelTreeManager->makeOutputVariable(m_jet_pt, "jet_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_jet_eta, "jet_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_jet_phi, "jet_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_jet_e, "jet_e");

      m_particleLevelTreeManager->makeOutputVariable(m_jet_Ghosts_BHadron_Final_Count, "jet_nGhosts_bHadron");
      m_particleLevelTreeManager->makeOutputVariable(m_jet_Ghosts_CHadron_Final_Count, "jet_nGhosts_cHadron");
    }

    //large R jets
    if (m_config->useTruthLargeRJets()) {
      m_particleLevelTreeManager->makeOutputVariable(m_ljet_pt, "ljet_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_ljet_eta, "ljet_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_ljet_phi, "ljet_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_ljet_e, "ljet_e");

      m_particleLevelTreeManager->makeOutputVariable(m_ljet_Ghosts_BHadron_Final_Count, "ljet_nGhosts_bHadron");
      m_particleLevelTreeManager->makeOutputVariable(m_ljet_Ghosts_CHadron_Final_Count, "ljet_nGhosts_cHadron");
    }

    //taus
    if (m_config->useTruthTaus()) {
      m_particleLevelTreeManager->makeOutputVariable(m_tau_pt, "tau_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_tau_eta, "tau_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_tau_phi, "tau_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_tau_e, "tau_e");
      m_particleLevelTreeManager->makeOutputVariable(m_tau_charge, "tau_charge");
      m_particleLevelTreeManager->makeOutputVariable(m_tau_isHadronic, "tau_isHadronic");
    }

    // RC branches
    if (m_makeRCJets) {
      // create the branches
      m_particleLevelTreeManager->makeOutputVariable(m_rcjet_pt, "rcjet_pt");
      m_particleLevelTreeManager->makeOutputVariable(m_rcjet_eta, "rcjet_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_rcjet_phi, "rcjet_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_rcjet_e, "rcjet_e");
      m_particleLevelTreeManager->makeOutputVariable(m_rcjet_d12, "rcjet_d12"); // requires >= 2 subjets
      m_particleLevelTreeManager->makeOutputVariable(m_rcjet_d23, "rcjet_d23"); // requires >= 3 subjets
      m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_pt, "rcjetsub_pt");  // vector of vectors for subjet
                                                                                     // info
      m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_eta, "rcjetsub_eta");
      m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_phi, "rcjetsub_phi");
      m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_e, "rcjetsub_e");
      m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_Ghosts_BHadron_Final_Count, "rcjetsub_nGhosts_bHadron");
      m_particleLevelTreeManager->makeOutputVariable(m_rcjetsub_Ghosts_CHadron_Final_Count, "rcjetsub_nGhosts_cHadron");

      if (m_useRCJSS || m_useRCAdditionalJSS) {
        m_particleLevelTreeManager->makeOutputVariable(m_rrcjet_pt, "rrcjet_pt");
        m_particleLevelTreeManager->makeOutputVariable(m_rrcjet_eta, "rrcjet_eta");
        m_particleLevelTreeManager->makeOutputVariable(m_rrcjet_phi, "rrcjet_phi");
        m_particleLevelTreeManager->makeOutputVariable(m_rrcjet_e, "rrcjet_e");
      }

      if (m_useRCJSS) {
        // RCJet SS from Clusters
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_tau32_clstr, "rcjet_tau32_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_tau21_clstr, "rcjet_tau21_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_tau3_clstr, "rcjet_tau3_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_tau2_clstr, "rcjet_tau2_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_tau1_clstr, "rcjet_tau1_clstr");

        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_d12_clstr, "rcjet_d12_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_d23_clstr, "rcjet_d23_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_Qw_clstr, "rcjet_Qw_clstr");

        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_nconstituent_clstr, "rcjet_nconstituent_clstr");
	
	m_particleLevelTreeManager->makeOutputVariable(m_rcjet_D2_clstr, "rcjet_D2_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_ECF1_clstr, "rcjet_ECF1_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_ECF2_clstr, "rcjet_ECF2_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_ECF3_clstr, "rcjet_ECF3_clstr");

      }
      if (m_useRCAdditionalJSS) {
        
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF332_clstr, "rcjet_gECF332_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF461_clstr, "rcjet_gECF461_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF322_clstr, "rcjet_gECF322_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF331_clstr, "rcjet_gECF331_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF422_clstr, "rcjet_gECF422_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF441_clstr, "rcjet_gECF441_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF212_clstr, "rcjet_gECF212_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF321_clstr, "rcjet_gECF321_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_gECF311_clstr, "rcjet_gECF311_clstr");

        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_L1_clstr, "rcjet_L1_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_L2_clstr, "rcjet_L2_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_L3_clstr, "rcjet_L3_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_L4_clstr, "rcjet_L4_clstr");
        m_particleLevelTreeManager->makeOutputVariable(m_rcjet_L5_clstr, "rcjet_L5_clstr");
      }
    }

    if (m_makeVarRCJets) {
      std::string VarRC = "vrcjet";

      for (auto& rho : m_VarRCJetRho) {
        for (auto& mass_scale : m_VarRCJetMassScale) {
          std::replace(rho.begin(), rho.end(), '.', '_');
          std::string name = rho + mass_scale;
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_pt"], VarRC + "_" + name + "_pt");
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_eta"], VarRC + "_" + name + "_eta");
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_phi"], VarRC + "_" + name + "_phi");
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_e"], VarRC + "_" + name + "_e");
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d12"], VarRC + "_" + name + "_d12"); // requires
                                                                                                                                                // >=
                                                                                                                                                // 2
                                                                                                                                                // subjets
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d23"], VarRC + "_" + name + "_d23"); // requires
                                                                                                                                                // >=
                                                                                                                                                // 3
                                                                                                                                                // subjets
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_pt"], VarRC + "sub_" + name + "_pt");  // vector
                                                                                                                                                         // of
                                                                                                                                                         // vectors
                                                                                                                                                         // for
                                                                                                                                                         // subjet
                                                                                                                                                         // info
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_eta"], VarRC + "sub_" + name + "_eta");
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_phi"], VarRC + "sub_" + name + "_phi");
          m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_e"], VarRC + "sub_" + name + "_e");

          if (m_useVarRCJSS || m_useVarRCAdditionalJSS) {
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle["vrrcjet_" + name + "_pt"], "vrrcjet_" + name + "_pt");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle["vrrcjet_" + name + "_eta"], "vrrcjet_" + name + "_eta");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle["vrrcjet_" + name + "_phi"], "vrrcjet_" + name + "_phi");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle["vrrcjet_" + name + "_e"], "vrrcjet_" + name + "_e");
          }
          if (m_useVarRCJSS) {
            // RCJet SS from Clusters
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau32_clstr"], VarRC + "_" + name + "_tau32_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau21_clstr"], VarRC + "_" + name + "_tau21_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau3_clstr"], VarRC + "_" + name + "_tau3_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau2_clstr"], VarRC + "_" + name + "_tau2_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau1_clstr"], VarRC + "_" + name + "_tau1_clstr");

            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d12_clstr"], VarRC + "_" + name + "_d12_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d23_clstr"], VarRC + "_" + name + "_d23_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_Qw_clstr"], VarRC + "_" + name + "_Qw_clstr");
          }
          if (m_useVarRCAdditionalJSS) {
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF332_clstr"], VarRC + "_" + name + "_gECF332_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF461_clstr"], VarRC + "_" + name + "_gECF461_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF322_clstr"], VarRC + "_" + name + "_gECF322_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF331_clstr"], VarRC + "_" + name + "_gECF331_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF422_clstr"], VarRC + "_" + name + "_gECF422_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF441_clstr"], VarRC + "_" + name + "_gECF441_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF212_clstr"], VarRC + "_" + name + "_gECF212_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF321_clstr"], VarRC + "_" + name + "_gECF321_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF311_clstr"], VarRC + "_" + name + "_gECF311_clstr");

            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L1_clstr"], VarRC + "_" + name + "_L1_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L2_clstr"], VarRC + "_" + name + "_L2_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L3_clstr"], VarRC + "_" + name + "_L3_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L4_clstr"], VarRC + "_" + name + "_L4_clstr");
            m_particleLevelTreeManager->makeOutputVariable(m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L5_clstr"], VarRC + "_" + name + "_L5_clstr");
          }
        } // end loop over mass parameters
      } // end loop over multipliers for mass scale
    } // end if VarRC jets

    //met
    if (m_config->useTruthMET()) {
      m_particleLevelTreeManager->makeOutputVariable(m_met_met, "met_met");
      m_particleLevelTreeManager->makeOutputVariable(m_met_phi, "met_phi");
    }

    // Setup PDF info branches if PDF info is requested.
    if (m_config->doTruthPDFInfo()) {
      m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_X1, "PDFinfo_X1");
      m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_X2, "PDFinfo_X2");
      m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_PDGID1, "PDFinfo_PDGID1");
      m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_PDGID2, "PDFinfo_PDGID2");
      m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_Q, "PDFinfo_Q");
      m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_XF1, "PDFinfo_XF1");
      m_particleLevelTreeManager->makeOutputVariable(m_PDFinfo_XF2, "PDFinfo_XF2");
    }

    // Setup MC generator weights branch if requested.
    if (m_config->doMCGeneratorWeights()) {
      m_particleLevelTreeManager->makeOutputVariable(m_mc_generator_weights, "mc_generator_weights");
    }

    // Setup PDF weight branches if PDF systematics are requested,
    if (m_config->saveLHAPDFEvent()) {
      for (auto& pdf : m_PDF_eventWeights) {
        m_particleLevelTreeManager->makeOutputVariable(pdf.second, pdf.first);
      }
    }

    // Setup the selection decision branches. This is a two-step process:
    //
    // (1) First, we need to create an array of variables which will be used to
    //     store the data in the TTree. The variable
    //     m_particleLevel_SelectionDecisions is just that: A std::vector of
    //     pairs, thwere the .first corresponds to the name of the selection,
    //     and the .second is the actual variable.
    // (2) Secondly, we need to attach the newly created variables to the
    //     particle level tree manager.
    m_particleLevel_SelectionDecisions.reserve(m_extraBranches.size());
    for (const auto& selection : m_extraBranches) {
      m_particleLevel_SelectionDecisions.push_back(std::make_pair(selection, int()));
    }

    for (auto& selectionDecision : m_particleLevel_SelectionDecisions) {
      m_particleLevelTreeManager->makeOutputVariable(selectionDecision.second, selectionDecision.first);
    }
  }

  void EventSaverFlatNtuple::setupUpgradeTreeManager(/*const top::ParticleLevelEvent& upgradeEvent*/) {
    // Quick return if upgrade is disabled or the tree is already initialised.
    if (not m_config->HLLHC() or m_upgradeTreeManager) {
      return;
    }

    m_upgradeTreeManager = std::make_shared<top::TreeManager>("upgrade", m_outputFile, m_config->outputFileNEventAutoFlush(), m_config->outputFileBasketSizePrimitive(), m_config->outputFileBasketSizeVector());

    m_upgradeTreeManager->makeOutputVariable(m_weight_mc, "weight_mc");

    if (m_config->doMCGeneratorWeights()) {
      m_upgradeTreeManager->makeOutputVariable(m_mc_generator_weights, "mc_generator_weights");
    }

    //event info
    m_upgradeTreeManager->makeOutputVariable(m_eventNumber, "eventNumber");
    m_upgradeTreeManager->makeOutputVariable(m_runNumber, "runNumber");
    m_upgradeTreeManager->makeOutputVariable(m_mcChannelNumber, "mcChannelNumber");
    m_upgradeTreeManager->makeOutputVariable(m_mu, "mu");
    m_upgradeTreeManager->makeOutputVariable(m_mu_actual, "mu_actual");

    // electrons
    m_upgradeTreeManager->makeOutputVariable(m_el_pt, "el_pt");
    m_upgradeTreeManager->makeOutputVariable(m_el_eta, "el_eta");
    m_upgradeTreeManager->makeOutputVariable(m_el_phi, "el_phi");
    m_upgradeTreeManager->makeOutputVariable(m_el_e, "el_e");
    m_upgradeTreeManager->makeOutputVariable(m_el_charge, "el_charge");
    m_upgradeTreeManager->makeOutputVariable(m_el_true_type, "el_true_type");
    m_upgradeTreeManager->makeOutputVariable(m_el_true_origin, "el_true_origin");
    m_upgradeTreeManager->makeOutputVariable(m_el_etcone20, "el_etcone20");
    m_upgradeTreeManager->makeOutputVariable(m_el_ptcone30, "el_ptcone30");
    //m_upgradeTreeManager->makeOutputVariable(m_el_true_firstEgMotherTruthType,   "el_true_firstEgMotherTruthType");
    //m_upgradeTreeManager->makeOutputVariable(m_el_true_firstEgMotherTruthOrigin, "el_true_firstEgMotherTruthOrigin");
    //m_upgradeTreeManager->makeOutputVariable(m_el_true_isPrompt, "el_true_isPrompt");
    if (m_config->HLLHCFakes()) m_upgradeTreeManager->makeOutputVariable(m_el_faketype, "el_faketype"); // 0 true 2
                                                                                                        // hfake

    //forward electrons
    if (m_config->useFwdElectrons()) {
      m_upgradeTreeManager->makeOutputVariable(m_fwdel_pt, "fwdel_pt");
      m_upgradeTreeManager->makeOutputVariable(m_fwdel_eta, "fwdel_eta");
      m_upgradeTreeManager->makeOutputVariable(m_fwdel_phi, "fwdel_phi");
      m_upgradeTreeManager->makeOutputVariable(m_fwdel_e, "fwdel_e");
      m_upgradeTreeManager->makeOutputVariable(m_fwdel_etcone20, "fwdel_etcone20");
      m_upgradeTreeManager->makeOutputVariable(m_fwdel_etcone30, "fwdel_etcone30");
      m_upgradeTreeManager->makeOutputVariable(m_fwdel_etcone40, "fwdel_etcone40");
    }

    // muons
    m_upgradeTreeManager->makeOutputVariable(m_mu_pt, "mu_pt");
    m_upgradeTreeManager->makeOutputVariable(m_mu_eta, "mu_eta");
    m_upgradeTreeManager->makeOutputVariable(m_mu_phi, "mu_phi");
    m_upgradeTreeManager->makeOutputVariable(m_mu_e, "mu_e");
    m_upgradeTreeManager->makeOutputVariable(m_mu_charge, "mu_charge");
    m_upgradeTreeManager->makeOutputVariable(m_mu_true_type, "mu_true_type");
    m_upgradeTreeManager->makeOutputVariable(m_mu_true_origin, "mu_true_origin");
    m_upgradeTreeManager->makeOutputVariable(m_mu_etcone20, "mu_etcone20");
    m_upgradeTreeManager->makeOutputVariable(m_mu_ptcone30, "mu_ptcone30");
    m_upgradeTreeManager->makeOutputVariable(m_mu_true_isPrompt, "mu_true_isPrompt");
    m_upgradeTreeManager->makeOutputVariable(m_mu_prodVtx_z, "mu_prodVtx_z");
    m_upgradeTreeManager->makeOutputVariable(m_mu_prodVtx_perp, "mu_prodVtx_perp");
    m_upgradeTreeManager->makeOutputVariable(m_mu_prodVtx_phi, "mu_prodVtx_phi");

    // jets
    m_upgradeTreeManager->makeOutputVariable(m_jet_pt, "jet_pt");
    m_upgradeTreeManager->makeOutputVariable(m_jet_eta, "jet_eta");
    m_upgradeTreeManager->makeOutputVariable(m_jet_phi, "jet_phi");
    m_upgradeTreeManager->makeOutputVariable(m_jet_e, "jet_e");
    m_upgradeTreeManager->makeOutputVariable(m_jet_mv1eff, "jet_mv1eff");
    m_upgradeTreeManager->makeOutputVariable(m_jet_isPileup, "jet_isPileup");

    m_upgradeTreeManager->makeOutputVariable(m_jet_Ghosts_BHadron_Final_Count, "jet_nGhosts_bHadron");
    m_upgradeTreeManager->makeOutputVariable(m_jet_Ghosts_CHadron_Final_Count, "jet_nGhosts_cHadron");

    //large R jets
    if (m_config->useTruthLargeRJets()) {
      m_upgradeTreeManager->makeOutputVariable(m_ljet_pt, "ljet_pt");
      m_upgradeTreeManager->makeOutputVariable(m_ljet_eta, "ljet_eta");
      m_upgradeTreeManager->makeOutputVariable(m_ljet_phi, "ljet_phi");
      m_upgradeTreeManager->makeOutputVariable(m_ljet_e, "ljet_e");

      m_upgradeTreeManager->makeOutputVariable(m_ljet_Ghosts_BHadron_Final_Count, "ljet_nGhosts_bHadron");
      m_upgradeTreeManager->makeOutputVariable(m_ljet_Ghosts_CHadron_Final_Count, "ljet_nGhosts_cHadron");
    }

    if (m_config->useTruthPhotons()) {
      m_upgradeTreeManager->makeOutputVariable(m_ph_pt, "ph_pt");
      m_upgradeTreeManager->makeOutputVariable(m_ph_eta, "ph_eta");
      m_upgradeTreeManager->makeOutputVariable(m_ph_phi, "ph_phi");
      m_upgradeTreeManager->makeOutputVariable(m_ph_e, "ph_e");
      m_upgradeTreeManager->makeOutputVariable(m_ph_true_type, "ph_true_type");
      m_upgradeTreeManager->makeOutputVariable(m_ph_true_origin, "ph_true_origin");
      if (m_config->HLLHCFakes()) m_upgradeTreeManager->makeOutputVariable(m_ph_faketype, "ph_faketype");                                                                                                                   // 0
                                                                                                                                                                                                                            // true
                                                                                                                                                                                                                            // 1
                                                                                                                                                                                                                            // efake
                                                                                                                                                                                                                            // 2
                                                                                                                                                                                                                            // hfake
    }

    // MET
    m_upgradeTreeManager->makeOutputVariable(m_met_met, "met_met");
    m_upgradeTreeManager->makeOutputVariable(m_met_phi, "met_phi");

    m_upgrade_SelectionDecisions.reserve(m_extraBranches.size());
    for (const auto& selection : m_extraBranches) {
      m_upgrade_SelectionDecisions.push_back(std::make_pair(selection, int()));
    }

    for (auto& selectionDecision : m_upgrade_SelectionDecisions) {
      m_upgradeTreeManager->makeOutputVariable(selectionDecision.second, selectionDecision.first);
    }
  }//setupUpgradeTreeManager

  void EventSaverFlatNtuple::recordSelectionDecision(const top::Event& event) {
    int index(0);

    for (const auto& branchName : m_extraBranches) {
      m_selectionDecisions[index] = event.m_info->auxdataConst<int>(branchName);
      ++index;
    }
  }

  void EventSaverFlatNtuple::recordTriggerDecision(const top::Event& event) {
    for (const auto& trigger : m_triggerDecisions)
      m_triggerDecisions[trigger.first] = event.m_info->auxdataConst<char>("TRIGDEC_" + trigger.first);
  }

  void EventSaverFlatNtuple::saveEvent(const top::Event& event) {
    // record the event?
    if (m_config->saveOnlySelectedEvents() && !event.m_saveEvent) return;

    this->cleanEvent();
    this->calculateEvent(event);
    this->fillEvent(event);
  }

  void EventSaverFlatNtuple::cleanEvent() {
    /* Doing nothing for now, but we should put here the (re-)initialisation to dummy values
     * of all variables later calculated in calculateEvent(const top::Event& event)
     * For example all weights set to 1, all vectors cleaned, all kinematic or angular variables set to -99999.
     */
  }

  void EventSaverFlatNtuple::calculateEvent(const top::Event& event) {
    //We have decorated event-info with a variable for each selection - with pass / fail
    recordSelectionDecision(event);

    recordTriggerDecision(event);

    //some event weights
    m_weight_mc = 0.;
    if (m_config->isMC()) m_weight_mc = event.m_info->auxdataConst<float>("AnalysisTop_eventWeight");

    if (m_config->isMC()) {
      m_weight_pileup = m_sfRetriever->pileupSF(event);

      m_weight_leptonSF = m_sfRetriever->leptonSF(event, top::topSFSyst::nominal);
      if (m_config->useFwdElectrons()) m_weight_fwdElSF = m_sfRetriever->fwdElectronSF(event, top::topSFSyst::nominal);

      if (m_config->useTaus()) m_weight_tauSF = m_sfRetriever->tauSF(event, top::topSFSyst::nominal);

      if (m_config->usePhotons()) m_weight_photonSF = m_sfRetriever->photonSF(event, top::topSFSyst::nominal);

      if (m_config->useGlobalTriggerConfiguration()) m_weight_globalLeptonTriggerSF = m_sfRetriever->globalTriggerSF(event, top::topSFSyst::nominal);
      m_weight_oldTriggerSF = m_sfRetriever->oldTriggerSF(event, top::topSFSyst::nominal);

      for (auto& tagWP : m_config->bTagWP_available()) {
        if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
        m_weight_bTagSF[tagWP] = m_sfRetriever->btagSF(event, top::topSFSyst::nominal, tagWP);
      }
      if (m_config->useTrackJets()) {
        for (auto& tagWP : m_config->bTagWP_available_trkJet()) {
          if (std::find(m_config->bTagWP_calibrated_trkJet().begin(), m_config->bTagWP_calibrated_trkJet().end(), tagWP) == m_config->bTagWP_calibrated_trkJet().end()) continue;
          m_weight_trackjet_bTagSF[tagWP] = m_sfRetriever->btagSF(event, top::topSFSyst::nominal, tagWP, true);
        }
      }

      m_weight_jvt = m_sfRetriever->jvtSF(event, top::topSFSyst::nominal);
      if (m_config->getfJVTWP() != "None") m_weight_forwardjvt = m_sfRetriever->fjvtSF(event, top::topSFSyst::nominal);

      if (m_config->isSherpa22Vjets()) {
        if (event.m_info->isAvailable<double>("Sherpa22VJetsWeight")) {
          m_weight_sherpa_22_vjets = event.m_info->auxdataConst<double>("Sherpa22VJetsWeight");
        }
      }

      // writing the systematic-shifted SFs only in the nominal (or nominal_Loose) tree
      if (event.m_hashValue == m_config->nominalHashValue()) {
        if (m_config->useFwdElectrons()) {
          m_weight_fwdElSF_FWDEL_SF_ID_UP = m_sfRetriever->fwdElectronSF(event, top::topSFSyst::FWDEL_SF_ID_UP);
          m_weight_fwdElSF_FWDEL_SF_ID_DOWN = m_sfRetriever->fwdElectronSF(event, top::topSFSyst::FWDEL_SF_ID_DOWN);
        }

        m_weight_pileup_UP = m_sfRetriever->pileupSF(event, +1); // up variation
        m_weight_pileup_DOWN = m_sfRetriever->pileupSF(event, -1); // down variation

        m_weight_leptonSF_EL_SF_Trigger_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::EL_SF_Trigger_UP);
        m_weight_leptonSF_EL_SF_Trigger_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::EL_SF_Trigger_DOWN);
        m_weight_leptonSF_EL_SF_Reco_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::EL_SF_Reco_UP);
        m_weight_leptonSF_EL_SF_Reco_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::EL_SF_Reco_DOWN);
        m_weight_leptonSF_EL_SF_ID_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::EL_SF_ID_UP);
        m_weight_leptonSF_EL_SF_ID_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::EL_SF_ID_DOWN);
        m_weight_leptonSF_EL_SF_Isol_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::EL_SF_Isol_UP);
        m_weight_leptonSF_EL_SF_Isol_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::EL_SF_Isol_DOWN);

        if (m_config->electronEfficiencySystematicModel() != "TOTAL") {
          m_weight_leptonSF_EL_SF_CorrModel_Reco_UP = m_sfRetriever->electronSFSystVariationVector(event, top::topSFComp::RECO, 1);
          m_weight_leptonSF_EL_SF_CorrModel_Reco_DOWN = m_sfRetriever->electronSFSystVariationVector(event, top::topSFComp::RECO, -1);
          m_weight_leptonSF_EL_SF_CorrModel_ID_UP = m_sfRetriever->electronSFSystVariationVector(event, top::topSFComp::ID, 1);
          m_weight_leptonSF_EL_SF_CorrModel_ID_DOWN = m_sfRetriever->electronSFSystVariationVector(event, top::topSFComp::ID, -1);
          m_weight_leptonSF_EL_SF_CorrModel_Iso_UP = m_sfRetriever->electronSFSystVariationVector(event, top::topSFComp::ISOLATION, 1);
          m_weight_leptonSF_EL_SF_CorrModel_Iso_DOWN = m_sfRetriever->electronSFSystVariationVector(event, top::topSFComp::ISOLATION, -1);
        }
        m_weight_leptonSF_MU_SF_Trigger_STAT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_Trigger_STAT_UP);
        m_weight_leptonSF_MU_SF_Trigger_STAT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_Trigger_STAT_DOWN);
        m_weight_leptonSF_MU_SF_Trigger_SYST_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_Trigger_SYST_UP);
        m_weight_leptonSF_MU_SF_Trigger_SYST_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_Trigger_SYST_DOWN);
        // Muon ID SF systematics (regular)
        m_weight_leptonSF_MU_SF_ID_STAT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_STAT_UP);
        m_weight_leptonSF_MU_SF_ID_STAT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_STAT_DOWN);
        m_weight_leptonSF_MU_SF_ID_SYST_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_SYST_UP);
        m_weight_leptonSF_MU_SF_ID_SYST_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_SYST_DOWN);
        // Muon ID SF systematics (regular)
        m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_STAT_LOWPT_UP);
        m_weight_leptonSF_MU_SF_ID_STAT_LOWPT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_STAT_LOWPT_DOWN);
        m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_SYST_LOWPT_UP);
        m_weight_leptonSF_MU_SF_ID_SYST_LOWPT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_ID_SYST_LOWPT_DOWN);
        // Muon isolation SF systematics
        m_weight_leptonSF_MU_SF_Isol_STAT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_Isol_STAT_UP);
        m_weight_leptonSF_MU_SF_Isol_STAT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_Isol_STAT_DOWN);
        m_weight_leptonSF_MU_SF_Isol_SYST_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_Isol_SYST_UP);
        m_weight_leptonSF_MU_SF_Isol_SYST_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_Isol_SYST_DOWN);
        m_weight_leptonSF_MU_SF_TTVA_STAT_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_TTVA_STAT_UP);
        m_weight_leptonSF_MU_SF_TTVA_STAT_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_TTVA_STAT_DOWN);
        m_weight_leptonSF_MU_SF_TTVA_SYST_UP = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_TTVA_SYST_UP);
        m_weight_leptonSF_MU_SF_TTVA_SYST_DOWN = m_sfRetriever->leptonSF(event, top::topSFSyst::MU_SF_TTVA_SYST_DOWN);
        // Special global lepton trigger SF systematics if requested
        if (m_config->useGlobalTriggerConfiguration()) {
          m_weight_globalLeptonTriggerSF_EL_Trigger_UP = m_sfRetriever->globalTriggerSF(event, top::topSFSyst::EL_SF_Trigger_UP);
          m_weight_globalLeptonTriggerSF_EL_Trigger_DOWN = m_sfRetriever->globalTriggerSF(event, top::topSFSyst::EL_SF_Trigger_DOWN);
          m_weight_globalLeptonTriggerSF_MU_Trigger_STAT_UP = m_sfRetriever->globalTriggerSF(event, top::topSFSyst::MU_SF_Trigger_STAT_UP);
          m_weight_globalLeptonTriggerSF_MU_Trigger_STAT_DOWN = m_sfRetriever->globalTriggerSF(event, top::topSFSyst::MU_SF_Trigger_STAT_DOWN);
          m_weight_globalLeptonTriggerSF_MU_Trigger_SYST_UP = m_sfRetriever->globalTriggerSF(event, top::topSFSyst::MU_SF_Trigger_SYST_UP);
          m_weight_globalLeptonTriggerSF_MU_Trigger_SYST_DOWN = m_sfRetriever->globalTriggerSF(event, top::topSFSyst::MU_SF_Trigger_SYST_DOWN);
        }
        m_weight_oldTriggerSF_EL_Trigger_UP = m_sfRetriever->oldTriggerSF(event, top::topSFSyst::EL_SF_Trigger_UP);
        m_weight_oldTriggerSF_EL_Trigger_DOWN = m_sfRetriever->oldTriggerSF(event, top::topSFSyst::EL_SF_Trigger_DOWN);
        m_weight_oldTriggerSF_MU_Trigger_STAT_UP = m_sfRetriever->oldTriggerSF(event, top::topSFSyst::MU_SF_Trigger_STAT_UP);
        m_weight_oldTriggerSF_MU_Trigger_STAT_DOWN = m_sfRetriever->oldTriggerSF(event, top::topSFSyst::MU_SF_Trigger_STAT_DOWN);
        m_weight_oldTriggerSF_MU_Trigger_SYST_UP = m_sfRetriever->oldTriggerSF(event, top::topSFSyst::MU_SF_Trigger_SYST_UP);
        m_weight_oldTriggerSF_MU_Trigger_SYST_DOWN = m_sfRetriever->oldTriggerSF(event, top::topSFSyst::MU_SF_Trigger_SYST_DOWN);

        m_weight_indiv_SF_EL_Reco = m_sfRetriever->electronSF(event, top::topSFSyst::nominal, top::topSFComp::RECO);
        m_weight_indiv_SF_EL_Reco_UP = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_Reco_UP, top::topSFComp::RECO);
        m_weight_indiv_SF_EL_Reco_DOWN = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_Reco_DOWN, top::topSFComp::RECO);
        m_weight_indiv_SF_EL_ID = m_sfRetriever->electronSF(event, top::topSFSyst::nominal, top::topSFComp::ID);
        m_weight_indiv_SF_EL_ID_UP = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_ID_UP, top::topSFComp::ID);
        m_weight_indiv_SF_EL_ID_DOWN = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_ID_DOWN, top::topSFComp::ID);
        m_weight_indiv_SF_EL_Isol = m_sfRetriever->electronSF(event, top::topSFSyst::nominal, top::topSFComp::ISOLATION);
        m_weight_indiv_SF_EL_Isol_UP = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_Isol_UP, top::topSFComp::ISOLATION);
        m_weight_indiv_SF_EL_Isol_DOWN = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_Isol_DOWN, top::topSFComp::ISOLATION);
        m_weight_indiv_SF_EL_ChargeID = m_sfRetriever->electronSF(event, top::topSFSyst::nominal, top::topSFComp::CHARGEID);
        m_weight_indiv_SF_EL_ChargeID_UP = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_ChargeID_UP, top::topSFComp::CHARGEID);
        m_weight_indiv_SF_EL_ChargeID_DOWN = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_ChargeID_DOWN, top::topSFComp::CHARGEID);
        m_weight_indiv_SF_EL_ChargeMisID = m_sfRetriever->electronSF(event, top::topSFSyst::nominal, top::topSFComp::CHARGEMISID);
        m_weight_indiv_SF_EL_ChargeMisID_STAT_UP = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_ChargeMisID_STAT_UP, top::topSFComp::CHARGEMISID);
        m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_ChargeMisID_STAT_DOWN, top::topSFComp::CHARGEMISID);
        m_weight_indiv_SF_EL_ChargeMisID_SYST_UP = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_ChargeMisID_SYST_UP, top::topSFComp::CHARGEMISID);
        m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN = m_sfRetriever->electronSF(event, top::topSFSyst::EL_SF_ChargeMisID_SYST_DOWN, top::topSFComp::CHARGEMISID);
        ATH_MSG_DEBUG("Electron Reco SF = " << m_weight_indiv_SF_EL_Reco << " + " << m_weight_indiv_SF_EL_Reco_UP << " - " << m_weight_indiv_SF_EL_Reco_DOWN);
        ATH_MSG_DEBUG("Electron ID SF = " << m_weight_indiv_SF_EL_ID << " + " << m_weight_indiv_SF_EL_ID_UP << " - " << m_weight_indiv_SF_EL_ID_DOWN);
        ATH_MSG_DEBUG("Electron Charge ID SF = " << m_weight_indiv_SF_EL_ChargeID << " + " << m_weight_indiv_SF_EL_ChargeID_UP << " - " << m_weight_indiv_SF_EL_ChargeID_DOWN);
        ATH_MSG_DEBUG("Electron Charge Mis ID SF = " << m_weight_indiv_SF_EL_ChargeMisID << " + " << m_weight_indiv_SF_EL_ChargeMisID_STAT_UP << " - " << m_weight_indiv_SF_EL_ChargeMisID_STAT_DOWN << " + " << m_weight_indiv_SF_EL_ChargeMisID_SYST_UP << " - " << m_weight_indiv_SF_EL_ChargeMisID_SYST_DOWN);

        m_weight_indiv_SF_MU_ID = m_sfRetriever->muonSF(event, top::topSFSyst::nominal, top::topSFComp::ID);
        // Muon ID SF systematics (regular)
        m_weight_indiv_SF_MU_ID_STAT_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_STAT_UP, top::topSFComp::ID);
        m_weight_indiv_SF_MU_ID_STAT_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_STAT_DOWN, top::topSFComp::ID);
        m_weight_indiv_SF_MU_ID_SYST_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_SYST_UP, top::topSFComp::ID);
        m_weight_indiv_SF_MU_ID_SYST_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_SYST_DOWN, top::topSFComp::ID);
        // Muon ID SF systematics (low pt)
        m_weight_indiv_SF_MU_ID_STAT_LOWPT_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_STAT_LOWPT_UP, top::topSFComp::ID);
        m_weight_indiv_SF_MU_ID_STAT_LOWPT_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_STAT_LOWPT_DOWN, top::topSFComp::ID);
        m_weight_indiv_SF_MU_ID_SYST_LOWPT_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_SYST_LOWPT_UP, top::topSFComp::ID);
        m_weight_indiv_SF_MU_ID_SYST_LOWPT_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_ID_SYST_LOWPT_DOWN, top::topSFComp::ID);
        // Muon isolation SF systematics
        m_weight_indiv_SF_MU_Isol = m_sfRetriever->muonSF(event, top::topSFSyst::nominal, top::topSFComp::ISOLATION);
        m_weight_indiv_SF_MU_Isol_STAT_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_Isol_STAT_UP, top::topSFComp::ISOLATION);
        m_weight_indiv_SF_MU_Isol_STAT_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_Isol_STAT_DOWN, top::topSFComp::ISOLATION);
        m_weight_indiv_SF_MU_Isol_SYST_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_Isol_SYST_UP, top::topSFComp::ISOLATION);
        m_weight_indiv_SF_MU_Isol_SYST_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_Isol_SYST_DOWN, top::topSFComp::ISOLATION);
        m_weight_indiv_SF_MU_TTVA = m_sfRetriever->muonSF(event, top::topSFSyst::nominal, top::topSFComp::TTVA);
        m_weight_indiv_SF_MU_TTVA_STAT_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_TTVA_STAT_UP, top::topSFComp::TTVA);
        m_weight_indiv_SF_MU_TTVA_STAT_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_TTVA_STAT_DOWN, top::topSFComp::TTVA);
        m_weight_indiv_SF_MU_TTVA_SYST_UP = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_TTVA_SYST_UP, top::topSFComp::TTVA);
        m_weight_indiv_SF_MU_TTVA_SYST_DOWN = m_sfRetriever->muonSF(event, top::topSFSyst::MU_SF_TTVA_SYST_DOWN, top::topSFComp::TTVA);


        ATH_MSG_DEBUG("Muon ID SF = " << m_weight_indiv_SF_MU_ID << " + " << m_weight_indiv_SF_MU_ID_STAT_UP << " - " << m_weight_indiv_SF_MU_ID_STAT_DOWN << "     + " << m_weight_indiv_SF_MU_ID_SYST_UP << " - " << m_weight_indiv_SF_MU_ID_SYST_DOWN);

        if (m_config->useTaus()) {
          // Tau-electron overlap removal
          for (auto& tauSFvariation: m_weight_tauSF_variations) {
            tauSFvariation.second = m_sfRetriever->tauSF(event, tauSFvariation.first);
          }
        }

        if (m_config->usePhotons()) {
          m_weight_photonSF_ID_UP = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_IDSF_UP);
          m_weight_photonSF_ID_DOWN = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_IDSF_DOWN);
          m_weight_photonSF_effIso_UP = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_EFF_ISO_UP);
          m_weight_photonSF_effIso_DOWN = m_sfRetriever->photonSF(event, top::topSFSyst::PHOTON_EFF_ISO_DOWN);
        }


        m_weight_jvt_up = m_sfRetriever->jvtSF(event, top::topSFSyst::JVT_UP);
        m_weight_jvt_down = m_sfRetriever->jvtSF(event, top::topSFSyst::JVT_DOWN);
	if (m_config->getfJVTWP() != "None"){
	  m_weight_forwardjvt_up = m_sfRetriever->fjvtSF(event, top::topSFSyst::FJVT_UP);
	  m_weight_forwardjvt_down = m_sfRetriever->fjvtSF(event, top::topSFSyst::FJVT_DOWN);
	}
      }

      // for b-tagging SFs, can also have systematic-shifted in systematics trees
      if (event.m_hashValue == m_config->nominalHashValue() || m_config->dumpBtagSystsInSystTrees()) {
        for (auto& tagWP : m_config->bTagWP_available()) {
          // skip uncalibrated though available WPs
          if (std::find(m_config->bTagWP_calibrated().begin(), m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
          m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_B,
                                           m_weight_bTagSF_eigen_B_up[tagWP],
                                           m_weight_bTagSF_eigen_B_down[tagWP], tagWP);
          m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_C,
                                           m_weight_bTagSF_eigen_C_up[tagWP],
                                           m_weight_bTagSF_eigen_C_down[tagWP], tagWP);
          m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_LIGHT,
                                           m_weight_bTagSF_eigen_Light_up[tagWP],
                                           m_weight_bTagSF_eigen_Light_down[tagWP], tagWP);
          for (auto name : m_config->btagging_namedSysts(tagWP)) {
            m_weight_bTagSF_named_up[tagWP][name] = m_sfRetriever->btagSF(event, top::topSFSyst::BTAG_SF_NAMED_UP, tagWP, false, name);
            m_weight_bTagSF_named_down[tagWP][name] = m_sfRetriever->btagSF(event, top::topSFSyst::BTAG_SF_NAMED_DOWN, tagWP, false, name);
          }
        }
        if (m_config->useTrackJets()) {
          for (auto& tagWP : m_config->bTagWP_available_trkJet()) {
            // skip uncalibrated though available WPs
            if (std::find(m_config->bTagWP_calibrated_trkJet().begin(), m_config->bTagWP_calibrated_trkJet().end(), tagWP) == m_config->bTagWP_calibrated_trkJet().end()) continue;
            m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_B,
                                             m_weight_trackjet_bTagSF_eigen_B_up[tagWP],
                                             m_weight_trackjet_bTagSF_eigen_B_down[tagWP], tagWP, true);
            m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_C,
                                             m_weight_trackjet_bTagSF_eigen_C_up[tagWP],
                                             m_weight_trackjet_bTagSF_eigen_C_down[tagWP], tagWP, true);
            m_sfRetriever->btagSF_eigen_vars(event, top::topSFSyst::BTAG_SF_EIGEN_LIGHT,
                                             m_weight_trackjet_bTagSF_eigen_Light_up[tagWP],
                                             m_weight_trackjet_bTagSF_eigen_Light_down[tagWP], tagWP, true);
            for (auto name : m_config->trkjet_btagging_namedSysts(tagWP)) {
              m_weight_trackjet_bTagSF_named_up[tagWP][name] = m_sfRetriever->btagSF(event, top::topSFSyst::BTAG_SF_NAMED_UP, tagWP, true, name);
              m_weight_trackjet_bTagSF_named_down[tagWP][name] = m_sfRetriever->btagSF(event, top::topSFSyst::BTAG_SF_NAMED_DOWN, tagWP, true, name);
            }
          }
        }
      }
    }

    /// Bootstrapping poisson weights - Moved to run for MC and data
    if (m_config->saveBootstrapWeights()) {
      m_weight_poisson = event.m_info->auxdataConst<std::vector<int> >("weight_poisson");
    }

    ///-- weights for matrix-method fakes estimate by IFF --///
    if (event.m_hashValue == m_config->nominalHashValue() && !m_config->isMC() && m_config->doFakesMMWeightsIFF()) {
      std::vector<CP::AsymptMatrixTool*> fakesMMWeightCalcIFF;
      for (int mmi = 0; mmi < m_ASMsize; ++mmi) {
        fakesMMWeightCalcIFF.push_back(asg::ToolStore::get<CP::AsymptMatrixTool>("AsymptMatrixTool_" + std::to_string (mmi)));
      }
      std::string ASMweights_branch_name = "ASMWeight";
      std::string decorName = "ASMWeight";
      if (event.m_info->isAvailable<std::vector<float> >(decorName.c_str())) {
        m_ASMweights = event.m_info->auxdataConst<std::vector<float> >(decorName.c_str());
        if (event.m_info->isAvailable<std::vector<std::vector<float> > >((decorName + "_Syst").c_str())) {
          m_ASMweights_Syst = event.m_info->auxdataConst<std::vector<std::vector<float> > >((decorName + "_Syst").c_str());
          m_ASMweights_Systname = event.m_info->auxdataConst<std::vector<std::vector<std::string> > >((decorName + "_Systname").c_str());
        }
      }
    }

    //event info
    m_eventNumber = event.m_info->eventNumber();
    m_runNumber = event.m_info->runNumber();
    m_randomRunNumber = 0;
    if (m_config->isMC() && m_config->doPileupReweighting()) {
      if (event.m_info->isAvailable<unsigned int>("RandomRunNumber")) m_randomRunNumber = event.m_info->auxdataConst<unsigned int>("RandomRunNumber");
    }
    m_mcChannelNumber = 0;

    if (m_config->isMC()) m_mcChannelNumber = event.m_info->mcChannelNumber();

    // (non-collision-)background flags
    m_backgroundFlags = 0;
    if (event.m_info->isAvailable<unsigned int>("backgroundFlags")) m_backgroundFlags = event.m_info->auxdataConst<unsigned int>("backgroundFlags");

    // hasBadMuon flag
    m_hasBadMuon = 0;
    if (m_config->useMuons() && event.m_info->isAvailable<char>("AnalysisTop_HASBADMUON")) m_hasBadMuon = event.m_info->auxdataConst<char>("AnalysisTop_HASBADMUON");

    // mu values (original and corrected by pileup reweighting tool)
    m_mu_original = event.m_info->averageInteractionsPerCrossing();
    m_mu_actual_original = event.m_info->actualInteractionsPerCrossing();

    // set these all to zero (maybe not the most sensible value...)
    m_mu = 0;
    m_mu_actual = 0;

    if (m_config->isMC()) {
      m_mu = m_mu_original;
      m_mu_actual = m_mu_actual_original;
    }
    if (!m_config->isMC()) {
      // If we have the corrected mu value from pileup reweighting then save that
      // instead of mu value in (d)xAOD.
      if (event.m_info->isAvailable<float>("corrected_averageInteractionsPerCrossing")) m_mu = event.m_info->auxdataConst<float>("corrected_averageInteractionsPerCrossing");
      if (event.m_info->isAvailable<float>("corrected_actualInteractionsPerCrossing")) m_mu_actual = event.m_info->auxdataConst<float>("corrected_actualInteractionsPerCrossing");
    }

    ATH_MSG_DEBUG(" mu = " << m_mu_original << " -> " << m_mu);

    //forward electrons
    if (m_config->useFwdElectrons()) {
      unsigned int i(0);
      unsigned int n_electrons = event.m_fwdElectrons.size();
      m_fwdel_pt.resize(n_electrons);
      m_fwdel_eta.resize(n_electrons);
      m_fwdel_phi.resize(n_electrons);
      m_fwdel_e.resize(n_electrons);
      m_fwdel_etcone20.resize(n_electrons);
      m_fwdel_etcone30.resize(n_electrons);
      m_fwdel_etcone40.resize(n_electrons);
      m_fwdel_isTight.resize(n_electrons);

      for (const auto* const elPtr : event.m_fwdElectrons) {
        m_fwdel_pt[i] = elPtr->pt();
        m_fwdel_eta[i] = elPtr->eta();
        m_fwdel_phi[i] = elPtr->phi();
        m_fwdel_e[i] = elPtr->e();

        float etcone20 = 0.0;
        elPtr->isolationValue(etcone20, xAOD::Iso::topoetcone20);
        float etcone30 = 0.0;
        elPtr->isolationValue(etcone30, xAOD::Iso::topoetcone30);
        float etcone40 = 0.0;
        elPtr->isolationValue(etcone40, xAOD::Iso::topoetcone40);

        m_fwdel_etcone20[i] = etcone20;
        m_fwdel_etcone30[i] = etcone30;
        m_fwdel_etcone40[i] = etcone40;

        if (event.m_isLoose) {
          if (elPtr->isAvailable<char>("passPreORSelection")) {
            m_fwdel_isTight[i] = elPtr->auxdataConst<char>("passPreORSelection");
          }
        }

        ++i;
      }//end of loop on fwd electrons
    }//end of fwd electrons filling

    //electrons
    if (m_config->useElectrons()) {
      unsigned int i(0);
      unsigned int n_electrons = event.m_electrons.size();
      m_el_pt.resize(n_electrons);
      m_el_eta.resize(n_electrons);
      m_el_cl_eta.resize(n_electrons);
      m_el_phi.resize(n_electrons);
      m_el_e.resize(n_electrons);
      m_el_charge.resize(n_electrons);
      m_el_topoetcone20.resize(n_electrons);
      m_el_ptvarcone20.resize(n_electrons);
      m_el_isTight.resize(n_electrons);
      m_el_CF.resize(n_electrons);
      for (const auto& trigger : m_el_trigMatched)
        m_el_trigMatched[trigger.first].resize(n_electrons);
      m_el_d0sig.resize(n_electrons);
      m_el_delta_z0_sintheta.resize(n_electrons);
      if (m_useElectronChargeIDSelection) {
        m_el_ECIDS.resize(n_electrons);
        m_el_ECIDSResult.resize(n_electrons);
      }
      if (m_config->isMC()) {
        m_el_true_type.resize(n_electrons);
        m_el_true_origin.resize(n_electrons);
        m_el_true_firstEgMotherTruthOrigin.resize(n_electrons);
        m_el_true_firstEgMotherTruthType.resize(n_electrons);
        m_el_true_firstEgMotherPdgId.resize(n_electrons);
        m_el_true_isPrompt.resize(n_electrons);
        m_el_true_isChargeFl.resize(n_electrons);
      }
      if (m_config->enablePromptLeptonImprovedVetoStudies()) {
	m_PLIV_el_PromptLeptonRNN_conversion.resize(n_electrons);
	m_PLIV_el_PromptLeptonRNN_non_prompt_b.resize(n_electrons);
	m_PLIV_el_PromptLeptonRNN_non_prompt_c.resize(n_electrons);
	m_PLIV_el_PromptLeptonRNN_prompt.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_MVAXBin.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_TrackJetNTrack.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_topoetcone30rel.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_ptvarcone30rel.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_PtFrac.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_DRlj.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_CaloClusterSumEtRel.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_PtRel.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedVetoBARR.resize(n_electrons);
	m_PLIV_el_PromptLeptonImprovedVetoECAP.resize(n_electrons);
      }

      static const SG::AuxElement::Accessor<char> accECIDS("DFCommonElectronsECIDS");
      static const SG::AuxElement::Accessor<double> accECIDSResult("DFCommonElectronsECIDSResult");

      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonRNN_conversion("PromptLeptonRNN_conversion");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonRNN_non_prompt_b("PromptLeptonRNN_non_prompt_b");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonRNN_non_prompt_c("PromptLeptonRNN_non_prompt_c");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonRNN_prompt("PromptLeptonRNN_prompt");
      static const SG::AuxElement::Accessor<short> PLIV_el_PromptLeptonImprovedInput_MVAXBin("PromptLeptonImprovedInput_MVAXBin");
      static const SG::AuxElement::Accessor<short> PLIV_el_PromptLeptonImprovedInput_TrackJetNTrack("PromptLeptonImprovedInput_TrackJetNTrack");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedInput_topoetcone30rel("PromptLeptonImprovedInput_topoetcone30rel");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedInput_ptvarcone30rel("PromptLeptonImprovedInput_ptvarcone30rel");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedInput_PtFrac("PromptLeptonImprovedInput_PtFrac");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedInput_DRlj("PromptLeptonImprovedInput_DRlj");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedInput_CaloClusterSumEtRel("PromptLeptonImprovedInput_CaloClusterSumEtRel");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedInput_PtRel("PromptLeptonImprovedInput_PtRel");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx("PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedVetoBARR("PromptLeptonImprovedVetoBARR");
      static const SG::AuxElement::Accessor<float> PLIV_el_PromptLeptonImprovedVetoECAP("PromptLeptonImprovedVetoECAP");

      for (const auto* const elPtr : event.m_electrons) {
        m_el_pt[i] = elPtr->pt();
        m_el_eta[i] = elPtr->eta();
        m_el_cl_eta[i] = elPtr->caloCluster()->etaBE(2);
        m_el_phi[i] = elPtr->phi();
        m_el_e[i] = elPtr->e();
        m_el_charge[i] = elPtr->charge();
        elPtr->isolationValue(m_el_topoetcone20[i], xAOD::Iso::topoetcone20);
        elPtr->isolationValue(m_el_ptvarcone20[i], xAOD::Iso::ptvarcone20);
        if (event.m_isLoose) {
          if (elPtr->isAvailable<char>("passPreORSelection")) {
            m_el_isTight[i] = elPtr->auxdataConst<char>("passPreORSelection");
          }
        }
        if (elPtr->isAvailable<char>("passChargeID")) m_el_CF[i] = elPtr->auxdataConst<char>("passChargeID");
        for (const auto& trigger : m_el_trigMatched) {
          std::string trig = "TRIGMATCH_" + trigger.first;
          m_el_trigMatched[trigger.first][i] = elPtr->auxdataConst<char>(trig);
        }
        if (elPtr->isAvailable<float>("d0sig")) m_el_d0sig[i] = elPtr->auxdataConst<float>("d0sig");
        if (elPtr->isAvailable<float>("delta_z0_sintheta")) m_el_delta_z0_sintheta[i] = elPtr->auxdataConst<float>("delta_z0_sintheta");

        if (m_useElectronChargeIDSelection) {
          m_el_ECIDS[i] = accECIDS.isAvailable(*elPtr) ? accECIDS(*elPtr) : 'n';
          m_el_ECIDSResult[i] = accECIDSResult.isAvailable(*elPtr) ? accECIDSResult(*elPtr) : -999.;
        }

	if (m_config->enablePromptLeptonImprovedVetoStudies()) {
	  m_PLIV_el_PromptLeptonRNN_conversion[i] = PLIV_el_PromptLeptonRNN_conversion.isAvailable(*elPtr) ? PLIV_el_PromptLeptonRNN_conversion(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonRNN_non_prompt_b[i] = PLIV_el_PromptLeptonRNN_non_prompt_b.isAvailable(*elPtr) ? PLIV_el_PromptLeptonRNN_non_prompt_b(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonRNN_non_prompt_c[i] = PLIV_el_PromptLeptonRNN_non_prompt_c.isAvailable(*elPtr) ? PLIV_el_PromptLeptonRNN_non_prompt_c(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonRNN_prompt[i] = PLIV_el_PromptLeptonRNN_prompt.isAvailable(*elPtr) ? PLIV_el_PromptLeptonRNN_prompt(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_MVAXBin[i] = PLIV_el_PromptLeptonImprovedInput_MVAXBin.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_MVAXBin(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_TrackJetNTrack[i] = PLIV_el_PromptLeptonImprovedInput_TrackJetNTrack.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_TrackJetNTrack(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_topoetcone30rel[i] = PLIV_el_PromptLeptonImprovedInput_topoetcone30rel.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_topoetcone30rel(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_ptvarcone30rel[i] = PLIV_el_PromptLeptonImprovedInput_ptvarcone30rel.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_ptvarcone30rel(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_PtFrac[i] = PLIV_el_PromptLeptonImprovedInput_PtFrac.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_PtFrac(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_DRlj[i] = PLIV_el_PromptLeptonImprovedInput_DRlj.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_DRlj(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_CaloClusterSumEtRel[i] = PLIV_el_PromptLeptonImprovedInput_CaloClusterSumEtRel.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_CaloClusterSumEtRel(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_PtRel[i] = PLIV_el_PromptLeptonImprovedInput_PtRel.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_PtRel(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx[i] = PLIV_el_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedVetoBARR[i] = PLIV_el_PromptLeptonImprovedVetoBARR.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedVetoBARR(*elPtr) : -999.;
	  m_PLIV_el_PromptLeptonImprovedVetoECAP[i] = PLIV_el_PromptLeptonImprovedVetoECAP.isAvailable(*elPtr) ? PLIV_el_PromptLeptonImprovedVetoECAP(*elPtr) : -999.;
	}

        //retrieve the truth-matching variables from MCTruthClassifier
        if (m_config->isMC()) {
          m_el_true_type[i] = 0;
          m_el_true_origin[i] = 0;
          m_el_true_firstEgMotherTruthType[i] = 0;
          m_el_true_firstEgMotherTruthOrigin[i] = 0;
          m_el_true_firstEgMotherPdgId[i] = 0;
          static const SG::AuxElement::Accessor<int> typeel("truthType");
          static const SG::AuxElement::Accessor<int> origel("truthOrigin");
          static const SG::AuxElement::Accessor<int> firstEgMotherTruthType("firstEgMotherTruthType");
          static const SG::AuxElement::Accessor<int> firstEgMotherTruthOrigin("firstEgMotherTruthOrigin");
          static const SG::AuxElement::Accessor<int> firstEgMotherPdgId("firstEgMotherPdgId");

          if (typeel.isAvailable(*elPtr)) m_el_true_type[i] = typeel(*elPtr);
          if (origel.isAvailable(*elPtr)) m_el_true_origin[i] = origel(*elPtr);
          if (firstEgMotherTruthType.isAvailable(*elPtr)) m_el_true_firstEgMotherTruthType[i] = firstEgMotherTruthType(*elPtr);
          if (firstEgMotherTruthOrigin.isAvailable(*elPtr)) m_el_true_firstEgMotherTruthOrigin[i] = firstEgMotherTruthOrigin(*elPtr);
          if (firstEgMotherPdgId.isAvailable(*elPtr)) m_el_true_firstEgMotherPdgId[i] = firstEgMotherPdgId(*elPtr);

          std::pair<bool, bool> isPrompt_isChargeFl = isPromptElectron(m_el_true_type[i], m_el_true_origin[i], m_el_true_firstEgMotherTruthType[i], m_el_true_firstEgMotherTruthOrigin[i], m_el_true_firstEgMotherPdgId[i], m_el_charge[i]);
          m_el_true_isPrompt[i] = isPrompt_isChargeFl.first;
          m_el_true_isChargeFl[i] = isPrompt_isChargeFl.second;
        }
        ++i;
      }
    }

    //muons
    if (m_config->useMuons()) {
      unsigned int i(0);
      unsigned int n_muons = event.m_muons.size();
      m_mu_pt.resize(n_muons);
      m_mu_eta.resize(n_muons);
      m_mu_phi.resize(n_muons);
      m_mu_e.resize(n_muons);
      m_mu_charge.resize(n_muons);
      m_mu_topoetcone20.resize(n_muons);
      m_mu_ptvarcone30.resize(n_muons);
      m_mu_isTight.resize(n_muons);
      for (const auto& trigger : m_mu_trigMatched)
        m_mu_trigMatched[trigger.first].resize(n_muons);
      m_mu_d0sig.resize(n_muons);
      m_mu_delta_z0_sintheta.resize(n_muons);
      if (m_config->isMC()) {
        m_mu_true_type.resize(n_muons);
        m_mu_true_origin.resize(n_muons);
        m_mu_true_isPrompt.resize(n_muons);
      }
      if (m_config->enablePromptLeptonImprovedVetoStudies()) {
	m_PLIV_mu_PromptLeptonRNN_non_prompt_b.resize(n_muons);
	m_PLIV_mu_PromptLeptonRNN_non_prompt_c.resize(n_muons);
	m_PLIV_mu_PromptLeptonRNN_prompt.resize(n_muons);
	m_PLIV_mu_PromptLeptonImprovedInput_MVAXBin.resize(n_muons);
	m_PLIV_mu_PromptLeptonImprovedInput_topoetcone30rel.resize(n_muons);
	m_PLIV_mu_PromptLeptonImprovedInput_PtFrac.resize(n_muons);
	m_PLIV_mu_PromptLeptonImprovedInput_DRlj.resize(n_muons);
	m_PLIV_mu_PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel.resize(n_muons);
	m_PLIV_mu_PromptLeptonImprovedInput_CaloClusterERel.resize(n_muons);
	m_PLIV_mu_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest.resize(n_muons);
	m_PLIV_mu_PromptLeptonImprovedVeto.resize(n_muons);
      }

      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonRNN_non_prompt_b("PromptLeptonRNN_non_prompt_b");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonRNN_non_prompt_c("PromptLeptonRNN_non_prompt_c");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonRNN_prompt("PromptLeptonRNN_prompt");
      static const SG::AuxElement::Accessor<short> PLIV_mu_PromptLeptonImprovedInput_MVAXBin("PromptLeptonImprovedInput_MVAXBin");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonImprovedInput_topoetcone30rel("PromptLeptonImprovedInput_topoetcone30rel");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonImprovedInput_PtFrac("PromptLeptonImprovedInput_PtFrac");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonImprovedInput_DRlj("PromptLeptonImprovedInput_DRlj");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel("PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonImprovedInput_CaloClusterERel("PromptLeptonImprovedInput_CaloClusterERel");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest("PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest");
      static const SG::AuxElement::Accessor<float> PLIV_mu_PromptLeptonImprovedVeto("PromptLeptonImprovedVeto");

      for (const auto* const muPtr : event.m_muons) {
        m_mu_pt[i] = muPtr->pt();
        m_mu_eta[i] = muPtr->eta();
        m_mu_phi[i] = muPtr->phi();
        m_mu_e[i] = muPtr->e();
        m_mu_charge[i] = muPtr->charge();
        muPtr->isolation(m_mu_topoetcone20[i], xAOD::Iso::topoetcone20);
        muPtr->isolation(m_mu_ptvarcone30[i], xAOD::Iso::ptvarcone30);
        if (event.m_isLoose) {
          if (muPtr->isAvailable<char>("passPreORSelection")) {
            m_mu_isTight[i] = muPtr->auxdataConst<char>("passPreORSelection");
          }
        }
        for (const auto& trigger : m_mu_trigMatched) {
          std::string trig = "TRIGMATCH_" + trigger.first;
          m_mu_trigMatched[trigger.first][i] = muPtr->auxdataConst<char>(trig);
        }
        if (muPtr->isAvailable<float>("d0sig")) m_mu_d0sig[i] = muPtr->auxdataConst<float>("d0sig");
        if (muPtr->isAvailable<float>("delta_z0_sintheta")) m_mu_delta_z0_sintheta[i] = muPtr->auxdataConst<float>("delta_z0_sintheta");

	if (m_config->enablePromptLeptonImprovedVetoStudies()) {
	  m_PLIV_mu_PromptLeptonRNN_non_prompt_b[i] = PLIV_mu_PromptLeptonRNN_non_prompt_b.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonRNN_non_prompt_b(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonRNN_non_prompt_c[i] = PLIV_mu_PromptLeptonRNN_non_prompt_c.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonRNN_non_prompt_c(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonRNN_prompt[i] = PLIV_mu_PromptLeptonRNN_prompt.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonRNN_prompt(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonImprovedInput_MVAXBin[i] = PLIV_mu_PromptLeptonImprovedInput_MVAXBin.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonImprovedInput_MVAXBin(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonImprovedInput_topoetcone30rel[i] = PLIV_mu_PromptLeptonImprovedInput_topoetcone30rel.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonImprovedInput_topoetcone30rel(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonImprovedInput_PtFrac[i] = PLIV_mu_PromptLeptonImprovedInput_PtFrac.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonImprovedInput_PtFrac(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonImprovedInput_DRlj[i] = PLIV_mu_PromptLeptonImprovedInput_DRlj.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonImprovedInput_DRlj(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel[i] = PLIV_mu_PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonImprovedInput_ptvarcone30_TightTTVA_pt500rel(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonImprovedInput_CaloClusterERel[i] = PLIV_mu_PromptLeptonImprovedInput_CaloClusterERel.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonImprovedInput_CaloClusterERel(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest[i] = PLIV_mu_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonImprovedInput_CandVertex_normDistToPriVtxLongitudinalBest(*muPtr) : -999.;
	  m_PLIV_mu_PromptLeptonImprovedVeto[i] = PLIV_mu_PromptLeptonImprovedVeto.isAvailable(*muPtr) ? PLIV_mu_PromptLeptonImprovedVeto(*muPtr) : -999.;
	}

        //retrieve the truth-matching variables from MCTruthClassifier
        if (m_config->isMC()) {
          static const SG::AuxElement::Accessor<int> acc_mctt("truthType");
          static const SG::AuxElement::Accessor<int> acc_mcto("truthOrigin");
          m_mu_true_type[i] = 0;
          m_mu_true_origin[i] = 0;
          if (acc_mctt.isAvailable(*muPtr)) m_mu_true_type[i] = acc_mctt(*muPtr);
          if (acc_mcto.isAvailable(*muPtr)) m_mu_true_origin[i] = acc_mcto(*muPtr);
          m_mu_true_isPrompt[i] = isPromptMuon(m_mu_true_type[i], m_mu_true_origin[i]);
        }
        ++i;
      }
    }

    //soft muons
    if (m_config->useSoftMuons()) {
      unsigned int i(0);
      unsigned int n_muons = event.m_softmuons.size();

      m_softmu_pt.resize(n_muons);
      m_softmu_eta.resize(n_muons);
      m_softmu_phi.resize(n_muons);
      m_softmu_e.resize(n_muons);
      m_softmu_charge.resize(n_muons);
      m_softmu_d0.resize(n_muons);
      m_softmu_d0sig.resize(n_muons);
      m_softmu_delta_z0_sintheta.resize(n_muons);

      if (m_config->isMC()) {
        m_softmu_true_type.resize(n_muons);
        m_softmu_true_origin.resize(n_muons);
        m_softmu_true_isPrompt.resize(n_muons);
        m_softmu_SF_ID.resize(n_muons);
        m_softmu_SF_ID_STAT_UP.resize(n_muons);
        m_softmu_SF_ID_STAT_DOWN.resize(n_muons);
        m_softmu_SF_ID_SYST_UP.resize(n_muons);
        m_softmu_SF_ID_SYST_DOWN.resize(n_muons);
        m_softmu_SF_ID_STAT_LOWPT_UP.resize(n_muons);
        m_softmu_SF_ID_STAT_LOWPT_DOWN.resize(n_muons);
        m_softmu_SF_ID_SYST_LOWPT_UP.resize(n_muons);
        m_softmu_SF_ID_SYST_LOWPT_DOWN.resize(n_muons);
        
        if(m_config->softmuonAdditionalTruthInfo())
        {
          m_softmu_parton_origin_flag.resize(n_muons);
          m_softmu_particle_origin_flag.resize(n_muons);
          m_softmu_parent_pdgid.resize(n_muons);
          m_softmu_b_hadron_parent_pdgid.resize(n_muons);
          m_softmu_c_hadron_parent_pdgid.resize(n_muons);
        }
      }

      for (const auto* const muPtr : event.m_softmuons) {
        m_softmu_pt[i] = muPtr->pt();
        m_softmu_eta[i] = muPtr->eta();
        m_softmu_phi[i] = muPtr->phi();
        m_softmu_e[i] = muPtr->e();
        m_softmu_charge[i] = muPtr->charge();

        m_softmu_d0[i] = 999.;
        m_softmu_d0sig[i] = 999.;
        m_softmu_delta_z0_sintheta[i] = 999.;

        const xAOD::TrackParticle* mutrack = muPtr->primaryTrackParticle();

        if (mutrack) m_softmu_d0[i] = mutrack->d0();
        if (muPtr->isAvailable<float>("d0sig")) m_softmu_d0sig[i] = muPtr->auxdataConst<float>("d0sig");
        if (muPtr->isAvailable<float>("delta_z0_sintheta")) m_softmu_delta_z0_sintheta[i] = muPtr->auxdataConst<float>("delta_z0_sintheta");

        if (m_config->isMC()) {
          m_softmu_SF_ID[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::nominal);
          if (event.m_hashValue == m_config->nominalHashValue()) {
            m_softmu_SF_ID_STAT_UP[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::MU_SF_ID_STAT_UP);
            m_softmu_SF_ID_STAT_DOWN[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::MU_SF_ID_STAT_DOWN);
            m_softmu_SF_ID_SYST_UP[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::MU_SF_ID_SYST_UP);
            m_softmu_SF_ID_SYST_DOWN[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::MU_SF_ID_SYST_DOWN);
            m_softmu_SF_ID_STAT_LOWPT_UP[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::MU_SF_ID_STAT_LOWPT_UP);
            m_softmu_SF_ID_STAT_LOWPT_DOWN[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::MU_SF_ID_STAT_LOWPT_DOWN);
            m_softmu_SF_ID_SYST_LOWPT_UP[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::MU_SF_ID_SYST_LOWPT_UP);
            m_softmu_SF_ID_SYST_LOWPT_DOWN[i] = m_sfRetriever->softmuonSF_ID(*muPtr, top::topSFSyst::MU_SF_ID_SYST_LOWPT_DOWN);
          }

          static const SG::AuxElement::Accessor<int> acc_mctt("truthType");
          static const SG::AuxElement::Accessor<int> acc_mcto("truthOrigin");
          m_softmu_true_type[i] = 0;
          m_softmu_true_origin[i] = 0;
          if (acc_mctt.isAvailable(*muPtr)) m_softmu_true_type[i] = acc_mctt(*muPtr);
          if (acc_mcto.isAvailable(*muPtr)) m_softmu_true_origin[i] = acc_mcto(*muPtr);
          m_softmu_true_isPrompt[i] = isPromptMuon(m_softmu_true_type[i], m_softmu_true_origin[i]);
          
          if(m_config->softmuonAdditionalTruthInfo())
          {
            //these truth info are written in SoftMuonObjectCollectionMaker.cxx
            
            m_softmu_parton_origin_flag[i]=0;
            if(m_config->softmuonAdditionalTruthInfoCheckPartonOrigin())
            {
              static const SG::AuxElement::Accessor<top::LepPartonOriginFlag> leppartonoriginflag("LepPartonOriginFlag");
              if(leppartonoriginflag.isAvailable(*muPtr)) m_softmu_parton_origin_flag[i]=static_cast<int>(leppartonoriginflag(*muPtr));
            }
            m_softmu_particle_origin_flag[i]=0;
            m_softmu_parent_pdgid[i]=0;
            m_softmu_b_hadron_parent_pdgid[i]=0;
            m_softmu_c_hadron_parent_pdgid[i]=0;
            static const SG::AuxElement::Accessor<top::LepParticleOriginFlag> lepparticleoriginflag("LepParticleOriginFlag");
            if(lepparticleoriginflag.isAvailable(*muPtr)) m_softmu_particle_origin_flag[i]=static_cast<int>(lepparticleoriginflag(*muPtr));
            
            static const SG::AuxElement::Accessor<const xAOD::TruthParticle*> Mother("truthMotherLink");
            const xAOD::TruthParticle* mother = 0;
            if(Mother.isAvailable(*muPtr)) mother=Mother(*muPtr);
            if(mother) m_softmu_parent_pdgid[i]=mother->pdgId();
            
            static const SG::AuxElement::Accessor<const xAOD::TruthParticle*> BMother("truthBMotherLink");
            const xAOD::TruthParticle* Bmother = 0;
            if(BMother.isAvailable(*muPtr)) Bmother=BMother(*muPtr);
            if(Bmother) m_softmu_b_hadron_parent_pdgid[i]=Bmother->pdgId();
            
            static const SG::AuxElement::Accessor<const xAOD::TruthParticle*> CMother("truthCMotherLink");
            const xAOD::TruthParticle* Cmother = 0;
            if(CMother.isAvailable(*muPtr)) Cmother=CMother(*muPtr);
            if(Cmother) m_softmu_c_hadron_parent_pdgid[i]=Cmother->pdgId();
            
            if(m_config->softmuonAdditionalTruthInfoDoVerbose()) asg::msgUserCode::ATH_MSG_INFO("writing soft muon with pt="<<m_softmu_pt[i] <<" parton_origin_flag="<<m_softmu_parton_origin_flag[i]<<" particle_origin_flag="<<m_softmu_particle_origin_flag[i]<<" parent_pdg_id="<<m_softmu_parent_pdgid[i]<<" b_hadron_parent_pdg_id="<<m_softmu_b_hadron_parent_pdgid[i]<<" c_hadron_parent_pdg_id="<<m_softmu_c_hadron_parent_pdgid[i]);
          }
        }//end of if (m_config->isMC())
        ++i;
      }//end of loop on softmuons
    }//end of if (m_config->useSoftMuons())

    //photons
    if (m_config->usePhotons()) {
      unsigned int i(0);
      m_ph_pt.resize(event.m_photons.size());
      m_ph_eta.resize(event.m_photons.size());
      m_ph_phi.resize(event.m_photons.size());
      m_ph_e.resize(event.m_photons.size());
      m_ph_iso.resize(event.m_photons.size());
      for (const auto* const phPtr : event.m_photons) {
        m_ph_pt[i] = phPtr->pt();
        m_ph_eta[i] = phPtr->eta();
        m_ph_phi[i] = phPtr->phi();
        m_ph_e[i] = phPtr->e();

        m_ph_iso[i] = 0.;
        if (phPtr->isAvailable<float>("ptvarcone20")) m_ph_iso[i] = phPtr->auxdata<float>("ptvarcone20");

        ++i;
      }
    }


    //taus
    if (m_config->useTaus()) {
      unsigned int i(0);
      m_tau_pt.resize(event.m_tauJets.size());
      m_tau_eta.resize(event.m_tauJets.size());
      m_tau_phi.resize(event.m_tauJets.size());
      m_tau_charge.resize(event.m_tauJets.size());
      for (const auto* const tauPtr : event.m_tauJets) {
        m_tau_pt[i] = tauPtr->pt();
        m_tau_eta[i] = tauPtr->eta();
        m_tau_phi[i] = tauPtr->phi();
        m_tau_charge[i] = tauPtr->charge();
        ++i;
      }
    }

    // tracks                   
    if (m_config->useTracks()) {

      m_track_pt.resize(event.m_tracks.size());
      m_track_eta.resize(event.m_tracks.size());
      m_track_phi.resize(event.m_tracks.size());
      m_track_e.resize(event.m_tracks.size());
      m_track_charge.resize(event.m_tracks.size());
      m_track_d0.resize(event.m_tracks.size());
      m_track_d0_significance.resize(event.m_tracks.size());
      m_track_z0.resize(event.m_tracks.size());
      m_track_z0_significance.resize(event.m_tracks.size());
      m_track_phi0.resize(event.m_tracks.size());
      m_track_theta.resize(event.m_tracks.size());
      m_track_qOverP.resize(event.m_tracks.size());
      m_track_chiSquared.resize(event.m_tracks.size());
      m_track_numberDoF.resize(event.m_tracks.size());

      unsigned int i = 0;
      for (const auto* const trkPtr :  event.m_tracks){

        m_track_pt[i]  = trkPtr->pt();
        m_track_eta[i] = trkPtr->eta();
        m_track_phi[i] = trkPtr->phi();
        m_track_e[i]   = trkPtr->e();
        m_track_charge[i] = trkPtr->charge();
        m_track_d0[i] = trkPtr->d0();
        m_track_d0_significance[i] = xAOD::TrackingHelpers::d0significance(trkPtr);
        m_track_z0[i] = trkPtr->z0();
        m_track_z0_significance[i] = xAOD::TrackingHelpers::z0significance(trkPtr);
        m_track_phi0[i]   = trkPtr->phi0();
        m_track_theta[i]  = trkPtr->theta();
        m_track_qOverP[i] = trkPtr->qOverP();
        m_track_chiSquared[i] = trkPtr->chiSquared();
        m_track_numberDoF[i]  = trkPtr->numberDoF();

        ++i;

      }

    }



    //jets
    if (m_config->useJets()) {
      unsigned int i(0);
      m_jet_pt.resize(event.m_jets.size());
      m_jet_eta.resize(event.m_jets.size());
      m_jet_phi.resize(event.m_jets.size());
      m_jet_e.resize(event.m_jets.size());
      if (m_config->bTagAlgo_MV2c10_used()) {
        m_jet_mv2c10.resize(event.m_jets.size());
      }
      m_jet_jvt.resize(event.m_jets.size());
      m_jet_fjvt.resize(event.m_jets.size());
      m_jet_passfjvt.resize(event.m_jets.size());

      // ghost tracks
      if (m_config->useJetGhostTrack()) {
        m_jet_ghostTrack_pt.clear();
        m_jet_ghostTrack_eta.clear();
        m_jet_ghostTrack_phi.clear();
        m_jet_ghostTrack_e.clear();
        m_jet_ghostTrack_d0.clear();
        m_jet_ghostTrack_z0.clear();
        m_jet_ghostTrack_qOverP.clear();


        m_jet_ghostTrack_pt.resize(event.m_jets.size());
        m_jet_ghostTrack_eta.resize(event.m_jets.size());
        m_jet_ghostTrack_phi.resize(event.m_jets.size());
        m_jet_ghostTrack_e.resize(event.m_jets.size());
        m_jet_ghostTrack_d0.resize(event.m_jets.size());
        m_jet_ghostTrack_z0.resize(event.m_jets.size());
        m_jet_ghostTrack_qOverP.resize(event.m_jets.size());
      }

      // R21 b-tagging
      for (const std::string& algo : m_config->bTagAlgo_available()) {
        m_jet_DLx[algo].resize(event.m_jets.size());
        m_jet_DLx_pb[algo].resize(event.m_jets.size());
        m_jet_DLx_pc[algo].resize(event.m_jets.size());
        m_jet_DLx_pu[algo].resize(event.m_jets.size());
      }
      if (m_config->isMC()) {
        m_jet_truthflav.resize(event.m_jets.size());
        m_jet_truthPartonLabel.resize(event.m_jets.size());
        m_jet_isTrueHS.resize(event.m_jets.size());
        m_jet_HadronConeExclExtendedTruthLabelID.resize(event.m_jets.size());
      }
      for (auto& tagWP : m_config->bTagWP_available()) {

        if (tagWP.find("Continuous") == std::string::npos) {
          m_jet_isbtagged[tagWP].resize(event.m_jets.size());
          if (std::find(m_config->bTagWP_calibrated().begin(),
                m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;

          if (m_config->isMC() && m_config->storePerJetBtagSFs()) {
            m_perjet_weight_bTagSF[tagWP].resize(event.m_jets.size());
            m_perjet_weight_bTagSF_eigen_B_up[tagWP].resize(event.m_jets.size(), std::vector<float>(m_config->btagging_num_B_eigenvars(tagWP)));
            m_perjet_weight_bTagSF_eigen_B_down[tagWP].resize(event.m_jets.size(), std::vector<float>(m_config->btagging_num_B_eigenvars(tagWP)));
            m_perjet_weight_bTagSF_eigen_C_up[tagWP].resize(event.m_jets.size(), std::vector<float>(m_config->btagging_num_C_eigenvars(tagWP)));
            m_perjet_weight_bTagSF_eigen_C_down[tagWP].resize(event.m_jets.size(), std::vector<float>(m_config->btagging_num_C_eigenvars(tagWP)));
            m_perjet_weight_bTagSF_eigen_Light_up[tagWP].resize(event.m_jets.size(), std::vector<float>(m_config->btagging_num_Light_eigenvars(tagWP)));
            m_perjet_weight_bTagSF_eigen_Light_down[tagWP].resize(event.m_jets.size(), std::vector<float>(m_config->btagging_num_Light_eigenvars(tagWP)));
            for (const std::string& name : m_config->btagging_namedSysts(tagWP)) {
              m_perjet_weight_bTagSF_named_up[tagWP][name].resize(event.m_jets.size());
              m_perjet_weight_bTagSF_named_down[tagWP][name].resize(event.m_jets.size());
            }
          }
        } else m_jet_tagWeightBin[tagWP].resize(event.m_jets.size());
      }
      for (const auto* const jetPtr : event.m_jets) {
        m_jet_pt[i] = jetPtr->pt();
        m_jet_eta[i] = jetPtr->eta();
        m_jet_phi[i] = jetPtr->phi();
        m_jet_e[i] = jetPtr->e();
        // In R21, list of b-tagging variables is changing and this is outdated
        const xAOD::BTagging* btag(nullptr);
        btag = jetPtr->btagging();
        if (m_config->isMC()) {
          m_jet_truthflav[i] = -99;
          if (jetPtr->isAvailable<int>("HadronConeExclTruthLabelID")) {
            jetPtr->getAttribute("HadronConeExclTruthLabelID", m_jet_truthflav[i]);
          }
          m_jet_truthPartonLabel[i] = -99;
          if (jetPtr->isAvailable<int>("PartonTruthLabelID")) {
            jetPtr->getAttribute("PartonTruthLabelID", m_jet_truthPartonLabel[i]);
          }
          m_jet_isTrueHS[i] = false;
          if (jetPtr->isAvailable<char>("AnalysisTop_isHS")) {
            jetPtr->getAttribute("AnalysisTop_isHS", m_jet_isTrueHS[i]);
          }
          m_jet_HadronConeExclExtendedTruthLabelID[i] = -99;
          if (jetPtr->isAvailable<int>("HadronConeExclExtendedTruthLabelID")) {
            jetPtr->getAttribute("HadronConeExclExtendedTruthLabelID", m_jet_HadronConeExclExtendedTruthLabelID[i]);
          }
        }

        if (m_config->useJetGhostTrack() && m_jet_pt[i] > m_config->jetPtGhostTracks() && std::abs(m_jet_eta[i])<m_config->jetEtaGhostTracks()) {
          static const SG::AuxElement::Accessor< float > accD0("d0");
          static const SG::AuxElement::Accessor< float > accZ0("z0");
          static const SG::AuxElement::Accessor< float > accQOverP("qOverP");

	  std::vector<const xAOD::TrackParticle*> ghostTracks;

          if(jetPtr->getAssociatedObjects<xAOD::TrackParticle>(m_config->decoKeyJetGhostTrack(event.m_hashValue),ghostTracks)) {

	    const unsigned int nghostTracks = ghostTracks.size();
            
            m_jet_ghostTrack_pt[i].clear();
	    m_jet_ghostTrack_eta[i].clear();
	    m_jet_ghostTrack_phi[i].clear();
	    m_jet_ghostTrack_e[i].clear();
	    m_jet_ghostTrack_d0[i].clear();
	    m_jet_ghostTrack_z0[i].clear();
	    m_jet_ghostTrack_qOverP[i].clear();
              
	    m_jet_ghostTrack_pt[i].reserve(nghostTracks);
	    m_jet_ghostTrack_eta[i].reserve(nghostTracks);
	    m_jet_ghostTrack_phi[i].reserve(nghostTracks);
	    m_jet_ghostTrack_e[i].reserve(nghostTracks);
	    m_jet_ghostTrack_d0[i].reserve(nghostTracks);
	    m_jet_ghostTrack_z0[i].reserve(nghostTracks);
	    m_jet_ghostTrack_qOverP[i].reserve(nghostTracks);
            
  
	    for (unsigned int iGhost = 0; iGhost < nghostTracks; ++iGhost) {  
              
              top::check(ghostTracks.at(iGhost), "Error in EventSaverFlatNtuple: Found jet with null pointer in ghost track vector.");  
                
	      if(ghostTracks.at(iGhost)->auxdataConst< char >("passPreORSelection") != 1)
		{ continue;}

	      m_jet_ghostTrack_pt[i].emplace_back(ghostTracks.at(iGhost)->pt());
	      m_jet_ghostTrack_eta[i].emplace_back(ghostTracks.at(iGhost)->eta());
	      m_jet_ghostTrack_phi[i].emplace_back(ghostTracks.at(iGhost)->phi());
	      m_jet_ghostTrack_e[i].emplace_back(ghostTracks.at(iGhost)->e());
	      m_jet_ghostTrack_d0[i].emplace_back(accD0(*ghostTracks.at(iGhost)));
	      m_jet_ghostTrack_z0[i].emplace_back(accZ0(*ghostTracks.at(iGhost)));
	      m_jet_ghostTrack_qOverP[i].emplace_back(accQOverP(*ghostTracks.at(iGhost)));

	    }
	
	  }
	}


        for (auto& tagWP : m_config->bTagWP_available()) {
          if (tagWP.find("Continuous") == std::string::npos) {
            m_jet_isbtagged[tagWP][i] = false;
            if (jetPtr->isAvailable<char>("isbtagged_" + tagWP)) m_jet_isbtagged[tagWP][i] = jetPtr->auxdataConst<char>("isbtagged_" + tagWP);
            if (m_config->isMC() && m_config->storePerJetBtagSFs()) {
              if (std::find(m_config->bTagWP_calibrated().begin(),
                m_config->bTagWP_calibrated().end(), tagWP) == m_config->bTagWP_calibrated().end()) continue;
              m_perjet_weight_bTagSF[tagWP][i] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_nom");
              for (size_t ivar = 0; ivar < m_config->btagging_num_B_eigenvars(tagWP); ++ivar) {
                m_perjet_weight_bTagSF_eigen_B_up[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_B_" + std::to_string(ivar) + "__1up");
                m_perjet_weight_bTagSF_eigen_B_down[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_B_" + std::to_string(ivar) + "__1down");
              }
              for (size_t ivar = 0; ivar < m_config->btagging_num_C_eigenvars(tagWP); ++ivar) {
                m_perjet_weight_bTagSF_eigen_C_up[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_C_" + std::to_string(ivar) + "__1up");
                m_perjet_weight_bTagSF_eigen_C_down[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_C_" + std::to_string(ivar) + "__1down");
              }
              for (size_t ivar = 0; ivar < m_config->btagging_num_Light_eigenvars(tagWP); ++ivar) {
                m_perjet_weight_bTagSF_eigen_Light_up[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_Light_" + std::to_string(ivar) + "__1up");
                m_perjet_weight_bTagSF_eigen_Light_down[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_Light_" + std::to_string(ivar) + "__1down");
              }
              for (const std::string& name : m_config->btagging_namedSysts(tagWP)) {
                m_perjet_weight_bTagSF_named_up[tagWP][name][i] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_" + name + "__1up");
                m_perjet_weight_bTagSF_named_down[tagWP][name][i] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_" + name + "__1down");
              }
            }
          } else {
            m_jet_tagWeightBin[tagWP][i] = -2;// AT default value
            if (jetPtr->isAvailable<int>("tagWeightBin_" + tagWP)) m_jet_tagWeightBin[tagWP][i] = jetPtr->auxdataConst<int>("tagWeightBin_" + tagWP);
          }
        }

        // for studies on high performance b-tagging
        // the following are in DC14

        if (m_config->bTagAlgo_MV2c10_used()) {
          double mvx = -999;
          if (btag) btag->MVx_discriminant("MV2c10", mvx);
          m_jet_mv2c10[i] = mvx;
        }

        m_jet_jvt[i] = -1;
        if (jetPtr->isAvailable<float>("AnalysisTop_JVT")) {
          m_jet_jvt[i] = jetPtr->auxdataConst<float>("AnalysisTop_JVT");
        }

        m_jet_fjvt[i] = -1;
        if (jetPtr->isAvailable<float>("fJvt")){
          m_jet_fjvt[i] = jetPtr->auxdataConst<float>("fJvt");
        }
        m_jet_passfjvt[i] = -1;
	if (jetPtr->isAvailable<char>("AnalysisTop_fJVTdecision")){
          m_jet_passfjvt[i] = jetPtr->getAttribute<char>("AnalysisTop_fJVTdecision");
	}

        ++i;
      }
      // loop over selected DL1 algos and fill all calo jet b-tagging information
      // the accessor uses decoration created in TopSystematicObjectMaker/JetObjectCollectionMaker
      // calculated by BtaggingSelectionTool
      for (const std::string& algo : m_config->bTagAlgo_available()) {
        std::vector<float>& m_jet_DLx_pick = m_jet_DLx.at(algo);
        std::vector<float>& m_jet_DLx_pb_pick = m_jet_DLx_pb.at(algo);
        std::vector<float>& m_jet_DLx_pc_pick = m_jet_DLx_pc.at(algo);
        std::vector<float>& m_jet_DLx_pu_pick = m_jet_DLx_pu.at(algo);
        const SG::AuxElement::ConstAccessor<float>& DLx_acc = DLx.at(algo);
        i = 0;
        for (const auto* const jetPtr : event.m_jets) {
          m_jet_DLx_pick[i] = DLx_acc(*jetPtr);

          const xAOD::BTagging* btag(nullptr);
          btag = jetPtr->btagging();
          if (btag) {
            double pu = -999;
            double pc = -999;
            double pb = -999;
            btag->pu(algo, pu);
            btag->pc(algo, pc);
            btag->pb(algo, pb);
            m_jet_DLx_pb_pick[i] = pb;
            m_jet_DLx_pc_pick[i] = pc;
            m_jet_DLx_pu_pick[i] = pu;
          }
          ++i;
        }
      }
    }

    // fail-JVT jets
    // btagging info is removed since btagging calibration isn't available for fail-JVT jets
    if (m_config->saveFailJVTJets()) {
      unsigned int i(0);
      m_failJvt_jet_pt.resize(event.m_failJvt_jets.size());
      m_failJvt_jet_eta.resize(event.m_failJvt_jets.size());
      m_failJvt_jet_phi.resize(event.m_failJvt_jets.size());
      m_failJvt_jet_e.resize(event.m_failJvt_jets.size());
      m_failJvt_jet_jvt.resize(event.m_failJvt_jets.size());
      m_failJvt_jet_fjvt.resize(event.m_failJvt_jets.size());
      m_failJvt_jet_passfjvt.resize(event.m_failJvt_jets.size());
      
      // ghost tracks
      // fail-JVT jet could still have some ghost tracks, so these variables are kept
      if (m_config->useJetGhostTrack()) {

        m_failJvt_jet_ghostTrack_pt.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_ghostTrack_eta.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_ghostTrack_phi.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_ghostTrack_e.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_ghostTrack_d0.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_ghostTrack_z0.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_ghostTrack_qOverP.resize(event.m_failJvt_jets.size());
      }

      if (m_config->isMC()) {
        m_failJvt_jet_truthflav.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_truthPartonLabel.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_isTrueHS.resize(event.m_failJvt_jets.size());
        m_failJvt_jet_HadronConeExclExtendedTruthLabelID.resize(event.m_failJvt_jets.size());
      }

      for (const auto* const jetPtr : event.m_failJvt_jets) {
        m_failJvt_jet_pt[i] = jetPtr->pt();
        m_failJvt_jet_eta[i] = jetPtr->eta();
        m_failJvt_jet_phi[i] = jetPtr->phi();
        m_failJvt_jet_e[i] = jetPtr->e();
        if (m_config->isMC()) {
          m_failJvt_jet_truthflav[i] = -99;
          if (jetPtr->isAvailable<int>("HadronConeExclTruthLabelID")) {
            jetPtr->getAttribute("HadronConeExclTruthLabelID", m_failJvt_jet_truthflav[i]);
          }
          m_failJvt_jet_truthPartonLabel[i] = -99;
          if (jetPtr->isAvailable<int>("PartonTruthLabelID")) {
            jetPtr->getAttribute("PartonTruthLabelID", m_failJvt_jet_truthPartonLabel[i]);
          }
          m_failJvt_jet_isTrueHS[i] = false;
          if (jetPtr->isAvailable<char>("AnalysisTop_isHS")) {
            jetPtr->getAttribute("AnalysisTop_isHS", m_failJvt_jet_isTrueHS[i]);
          }
          m_failJvt_jet_HadronConeExclExtendedTruthLabelID[i] = -99;
          if (jetPtr->isAvailable<int>("HadronConeExclExtendedTruthLabelID")) {
            jetPtr->getAttribute("HadronConeExclExtendedTruthLabelID", m_failJvt_jet_HadronConeExclExtendedTruthLabelID[i]);
          }
        }

        if (m_config->useJetGhostTrack() && m_failJvt_jet_pt[i] > m_config->jetPtGhostTracks() && std::abs(m_failJvt_jet_eta[i])<m_config->jetEtaGhostTracks()) {
          static const SG::AuxElement::Accessor< float > accD0("d0");
          static const SG::AuxElement::Accessor< float > accZ0("z0");
          static const SG::AuxElement::Accessor< float > accQOverP("qOverP");

          std::vector<const xAOD::TrackParticle*> ghostTracks;
          if(jetPtr->getAssociatedObjects<xAOD::TrackParticle>(m_config->decoKeyJetGhostTrack(event.m_hashValue),ghostTracks)) {

	    const unsigned int nghostTracks = ghostTracks.size();
  
	    m_failJvt_jet_ghostTrack_pt[i].clear();
	    m_failJvt_jet_ghostTrack_eta[i].clear();
	    m_failJvt_jet_ghostTrack_phi[i].clear();
	    m_failJvt_jet_ghostTrack_e[i].clear();
	    m_failJvt_jet_ghostTrack_d0[i].clear();
	    m_failJvt_jet_ghostTrack_z0[i].clear();
	    m_failJvt_jet_ghostTrack_qOverP[i].clear();
            
            m_failJvt_jet_ghostTrack_pt[i].reserve(nghostTracks);
	    m_failJvt_jet_ghostTrack_eta[i].reserve(nghostTracks);
	    m_failJvt_jet_ghostTrack_phi[i].reserve(nghostTracks);
	    m_failJvt_jet_ghostTrack_e[i].reserve(nghostTracks);
	    m_failJvt_jet_ghostTrack_d0[i].reserve(nghostTracks);
	    m_failJvt_jet_ghostTrack_z0[i].reserve(nghostTracks);
	    m_failJvt_jet_ghostTrack_qOverP[i].reserve(nghostTracks);
  
	    for (unsigned int iGhost = 0; iGhost < nghostTracks; ++iGhost) {
	      
             top::check(ghostTracks.at(iGhost), "Error in EventSaverFlatNtuple: Found jet with null pointer in ghost track vector.");
              
             if(ghostTracks.at(iGhost)->auxdataConst< char >("passPreORSelection") != 1)
		{continue;}
                
	      m_failJvt_jet_ghostTrack_pt[i].emplace_back(ghostTracks.at(iGhost)->pt());
	      m_failJvt_jet_ghostTrack_eta[i].emplace_back(ghostTracks.at(iGhost)->eta());
	      m_failJvt_jet_ghostTrack_phi[i].emplace_back(ghostTracks.at(iGhost)->phi());
	      m_failJvt_jet_ghostTrack_e[i].emplace_back(ghostTracks.at(iGhost)->e());
	      m_failJvt_jet_ghostTrack_d0[i].emplace_back(accD0(*ghostTracks.at(iGhost)));
	      m_failJvt_jet_ghostTrack_z0[i].emplace_back(accZ0(*ghostTracks.at(iGhost)));
	      m_failJvt_jet_ghostTrack_qOverP[i].emplace_back(accQOverP(*ghostTracks.at(iGhost)));
	    }
            
	  }
	}

        m_failJvt_jet_jvt[i] = -1;
        if (jetPtr->isAvailable<float>("AnalysisTop_JVT")) {
          m_failJvt_jet_jvt[i] = jetPtr->auxdataConst<float>("AnalysisTop_JVT");
        }

        m_failJvt_jet_fjvt[i] = -1;
        if (jetPtr->isAvailable<float>("fJvt")) {
	  m_failJvt_jet_fjvt[i] = jetPtr->auxdataConst<float>("fJvt");
        }
        m_failJvt_jet_passfjvt[i] = -1;
        if (jetPtr->isAvailable<char>("AnalysisTop_fJVTdecision")) {
          m_failJvt_jet_passfjvt[i] = jetPtr->getAttribute<char>("AnalysisTop_fJVTdecision");
        }

        ++i;
      }
    }//ifSaveJVT
      	
    // fail-FJVT jets
    // btagging info removed as this is only looking at forward jets
    if (m_config->getfJVTWP() != "None" && m_config->saveFailForwardJVTJets()) {
      unsigned int i(0);
      m_failFJvt_jet_pt.resize(event.m_failFJvt_jets.size());
      m_failFJvt_jet_eta.resize(event.m_failFJvt_jets.size());
      m_failFJvt_jet_phi.resize(event.m_failFJvt_jets.size());
      m_failFJvt_jet_e.resize(event.m_failFJvt_jets.size());
      m_failFJvt_jet_jvt.resize(event.m_failFJvt_jets.size());
      m_failFJvt_jet_fjvt.resize(event.m_failFJvt_jets.size());
      m_failFJvt_jet_passjvt.resize(event.m_failFJvt_jets.size());

      // ghost tracks
      // fail-FJVT jet could still have some ghost tracks, so these variables are kept
      if (m_config->useJetGhostTrack()) {

        m_failFJvt_jet_ghostTrack_pt.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_ghostTrack_eta.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_ghostTrack_phi.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_ghostTrack_e.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_ghostTrack_d0.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_ghostTrack_z0.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_ghostTrack_qOverP.resize(event.m_failFJvt_jets.size());
      }

      if (m_config->isMC()) {
        m_failFJvt_jet_truthflav.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_truthPartonLabel.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_isTrueHS.resize(event.m_failFJvt_jets.size());
        m_failFJvt_jet_HadronConeExclExtendedTruthLabelID.resize(event.m_failFJvt_jets.size());
      }

      for (const auto* const jetPtr : event.m_failFJvt_jets) {
        m_failFJvt_jet_pt[i] = jetPtr->pt();
        m_failFJvt_jet_eta[i] = jetPtr->eta();
        m_failFJvt_jet_phi[i] = jetPtr->phi();
        m_failFJvt_jet_e[i] = jetPtr->e();
        if (m_config->isMC()) {
          m_failFJvt_jet_truthflav[i] = -99;
          if (jetPtr->isAvailable<int>("HadronConeExclTruthLabelID")) {
            jetPtr->getAttribute("HadronConeExclTruthLabelID", m_failFJvt_jet_truthflav[i]);
          }
          m_failFJvt_jet_truthPartonLabel[i] = -99;
          if (jetPtr->isAvailable<int>("PartonTruthLabelID")) {
            jetPtr->getAttribute("PartonTruthLabelID", m_failFJvt_jet_truthPartonLabel[i]);
          }
          m_failFJvt_jet_isTrueHS[i] = false;
          if (jetPtr->isAvailable<char>("AnalysisTop_isHS")) {
            jetPtr->getAttribute("AnalysisTop_isHS", m_failFJvt_jet_isTrueHS[i]);
          }
          m_failFJvt_jet_HadronConeExclExtendedTruthLabelID[i] = -99;
          if (jetPtr->isAvailable<int>("HadronConeExclExtendedTruthLabelID")) {
            jetPtr->getAttribute("HadronConeExclExtendedTruthLabelID", m_failFJvt_jet_HadronConeExclExtendedTruthLabelID[i]);
          }
        }

        if (m_config->useJetGhostTrack() && m_failFJvt_jet_pt[i] > m_config->jetPtGhostTracks() && std::abs(m_failFJvt_jet_eta[i])<m_config->jetEtaGhostTracks()) {
          static const SG::AuxElement::Accessor< float > accD0("d0");
          static const SG::AuxElement::Accessor< float > accZ0("z0");
          static const SG::AuxElement::Accessor< float > accQOverP("qOverP");

          std::vector<const xAOD::TrackParticle*> ghostTracks;
          if(jetPtr->getAssociatedObjects<xAOD::TrackParticle>(m_config->decoKeyJetGhostTrack(event.m_hashValue),ghostTracks)) {

	    const unsigned int nghostTracks = ghostTracks.size();
            
            m_failFJvt_jet_ghostTrack_pt[i].clear();
	    m_failFJvt_jet_ghostTrack_eta[i].clear();
	    m_failFJvt_jet_ghostTrack_phi[i].clear();
	    m_failFJvt_jet_ghostTrack_e[i].clear();
	    m_failFJvt_jet_ghostTrack_d0[i].clear();
	    m_failFJvt_jet_ghostTrack_z0[i].clear();
	    m_failFJvt_jet_ghostTrack_qOverP[i].clear();
  
	    m_failFJvt_jet_ghostTrack_pt[i].reserve(nghostTracks);
	    m_failFJvt_jet_ghostTrack_eta[i].reserve(nghostTracks);
	    m_failFJvt_jet_ghostTrack_phi[i].reserve(nghostTracks);
	    m_failFJvt_jet_ghostTrack_e[i].reserve(nghostTracks);
	    m_failFJvt_jet_ghostTrack_d0[i].reserve(nghostTracks);
	    m_failFJvt_jet_ghostTrack_z0[i].reserve(nghostTracks);
	    m_failFJvt_jet_ghostTrack_qOverP[i].reserve(nghostTracks);

            
	    for (unsigned int iGhost = 0; iGhost < nghostTracks; ++iGhost) {
	      top::check(ghostTracks.at(iGhost), "Error in EventSaverFlatNtuple: Found jet with null pointer in ghost track vector.");
              
              if(ghostTracks.at(iGhost)->auxdataConst< char >("passPreORSelection") != 1)
		{ continue;}
		
  
	      m_failFJvt_jet_ghostTrack_pt[i].emplace_back(ghostTracks.at(iGhost)->pt());
	      m_failFJvt_jet_ghostTrack_eta[i].emplace_back(ghostTracks.at(iGhost)->eta());
	      m_failFJvt_jet_ghostTrack_phi[i].emplace_back(ghostTracks.at(iGhost)->phi());
	      m_failFJvt_jet_ghostTrack_e[i].emplace_back(ghostTracks.at(iGhost)->e());
	      m_failFJvt_jet_ghostTrack_d0[i].emplace_back(accD0(*ghostTracks.at(iGhost)));
	      m_failFJvt_jet_ghostTrack_z0[i].emplace_back(accZ0(*ghostTracks.at(iGhost)));
	      m_failFJvt_jet_ghostTrack_qOverP[i].emplace_back(accQOverP(*ghostTracks.at(iGhost)));
	    }
	    
            
	  }
	}

        m_failFJvt_jet_jvt[i] = -1;
        if (jetPtr->isAvailable<float>("AnalysisTop_JVT")) {
          m_failFJvt_jet_jvt[i] = jetPtr->auxdataConst<float>("AnalysisTop_JVT");
        }
        m_failFJvt_jet_passjvt[i] = -1;
        if (jetPtr->isAvailable<char>("passJVT")) {
          m_failFJvt_jet_passjvt[i] = jetPtr->getAttribute<char>("passJVT");
        }
        m_failFJvt_jet_fjvt[i] = -1;
        if (jetPtr->isAvailable<float>("fJvt")) { 
	  m_failFJvt_jet_fjvt[i] = jetPtr->auxdataConst<float>("fJvt");
        }

        ++i;
      }
    }//ifSaveFJVT


    //large-R jets
    if (m_config->useLargeRJets()) {
      unsigned int i(0);
      const unsigned int nLargeRJets = event.m_largeJets.size();
      m_ljet_pt.resize(nLargeRJets);
      m_ljet_eta.resize(nLargeRJets);
      m_ljet_phi.resize(nLargeRJets);
      m_ljet_e.resize(nLargeRJets);
      m_ljet_m.resize(nLargeRJets);

      for (const std::pair<std::string,std::string>& it : m_config->largeRJetSubstructureVariables()) {
	m_ljet_substructure[it.first].resize(nLargeRJets);
      }

      for (const std::string& taggerName : m_boostedJetTaggersNames)
        m_ljet_isTagged[taggerName].resize(nLargeRJets);
      if (m_config->isMC()) {
        m_ljet_truthLabel.resize(nLargeRJets);
        for (const std::string& taggerName : m_boostedJetTaggersNamesCalibrated)
          m_ljet_tagSF[taggerName].resize(nLargeRJets);
      }

      for (const auto* const jetPtr : event.m_largeJets) {
        m_ljet_pt[i] = jetPtr->pt();
        m_ljet_eta[i] = jetPtr->eta();
        m_ljet_phi[i] = jetPtr->phi();
        m_ljet_e[i] = jetPtr->e();
        m_ljet_m[i] = jetPtr->m();

	for (const std::pair<std::string,std::string>& it : m_config->largeRJetSubstructureVariables()) {
	  m_ljet_substructure[it.first][i] = jetPtr->isAvailable<float>(it.second) ? jetPtr->auxdata<float>(it.second) : -999;
	}

        for (const std::string& taggerName : m_boostedJetTaggersNames) {
          m_ljet_isTagged[taggerName][i] = jetPtr->getAttribute<char>("isTagged_" + taggerName);
        }

        if (m_config->isMC()) {
          m_ljet_truthLabel[i] = jetPtr->auxdata<int>("R10TruthLabel_R21Consolidated");
          for (const std::pair<std::string, std::string>& tagSF : m_config->boostedTaggerSFnames()) {
            m_ljet_tagSF[tagSF.first][i] = jetPtr->auxdata<float>(tagSF.second);
          }
        }

        ++i;
      }
    }

    //track jets
    if (m_config->useTrackJets()) {
      unsigned int i(0);
      m_tjet_pt.resize(event.m_trackJets.size());
      m_tjet_eta.resize(event.m_trackJets.size());
      m_tjet_phi.resize(event.m_trackJets.size());
      m_tjet_e.resize(event.m_trackJets.size());
      if (m_config->bTagAlgo_MV2c10_used_trkJet()) {
        m_tjet_mv2c10.resize(event.m_trackJets.size());
      }
      for (const std::string& algo : m_config->bTagAlgo_available_trkJet()) {
        m_tjet_DLx[algo].resize(event.m_trackJets.size());
        m_tjet_DLx_pb[algo].resize(event.m_trackJets.size());
        m_tjet_DLx_pc[algo].resize(event.m_trackJets.size());
        m_tjet_DLx_pu[algo].resize(event.m_trackJets.size());
      }
      for (auto& tagWP : m_config->bTagWP_available_trkJet()) {
        if (tagWP.find("Continuous") == std::string::npos) {
          m_tjet_isbtagged[tagWP].resize(event.m_trackJets.size());
          if (std::find(m_config->bTagWP_calibrated_trkJet().begin(),
            m_config->bTagWP_calibrated_trkJet().end(), tagWP) == m_config->bTagWP_calibrated_trkJet().end()) continue;
          if (m_config->isMC() && m_config->storePerJetBtagSFs()) {
            m_perjet_weight_trackjet_bTagSF[tagWP].resize(event.m_trackJets.size());
            m_perjet_weight_trackjet_bTagSF_eigen_B_up[tagWP].resize(event.m_trackJets.size(), std::vector<float>(m_config->trkjet_btagging_num_B_eigenvars(tagWP)));
            m_perjet_weight_trackjet_bTagSF_eigen_B_down[tagWP].resize(event.m_trackJets.size(), std::vector<float>(m_config->trkjet_btagging_num_B_eigenvars(tagWP)));
            m_perjet_weight_trackjet_bTagSF_eigen_C_up[tagWP].resize(event.m_trackJets.size(), std::vector<float>(m_config->trkjet_btagging_num_C_eigenvars(tagWP)));
            m_perjet_weight_trackjet_bTagSF_eigen_C_down[tagWP].resize(event.m_trackJets.size(), std::vector<float>(m_config->trkjet_btagging_num_C_eigenvars(tagWP)));
            m_perjet_weight_trackjet_bTagSF_eigen_Light_up[tagWP].resize(event.m_trackJets.size(), std::vector<float>(m_config->trkjet_btagging_num_Light_eigenvars(tagWP)));
            m_perjet_weight_trackjet_bTagSF_eigen_Light_down[tagWP].resize(event.m_trackJets.size(), std::vector<float>(m_config->trkjet_btagging_num_Light_eigenvars(tagWP)));
            for (const std::string& name : m_config->trkjet_btagging_namedSysts(tagWP)) {
              m_perjet_weight_trackjet_bTagSF_named_up[tagWP][name].resize(event.m_trackJets.size());
              m_perjet_weight_trackjet_bTagSF_named_down[tagWP][name].resize(event.m_trackJets.size());
            }
          }
        } else m_tjet_tagWeightBin[tagWP].resize(event.m_trackJets.size());
      }
      for (const auto* const jetPtr : event.m_trackJets) {
        m_tjet_pt[i] = jetPtr->pt();
        m_tjet_eta[i] = jetPtr->eta();
        m_tjet_phi[i] = jetPtr->phi();
        m_tjet_e[i] = jetPtr->e();

        if (m_config->bTagAlgo_MV2c10_used_trkJet()) {
          const xAOD::BTagging* btag(nullptr);
          btag = jetPtr->btagging();
          double mvx = -999;
          if (btag) btag->MVx_discriminant("MV2c10", mvx);
          m_tjet_mv2c10[i] = mvx;
        }

        for (auto& tagWP : m_config->bTagWP_available_trkJet()) {
          if (tagWP.find("Continuous") == std::string::npos) {
            m_tjet_isbtagged[tagWP][i] = false;
            if (jetPtr->isAvailable<char>("isbtagged_" + tagWP)) m_tjet_isbtagged[tagWP][i] = jetPtr->auxdataConst<char>("isbtagged_" + tagWP);
            if (m_config->isMC() && m_config->storePerJetBtagSFs()) {
              if (std::find(m_config->bTagWP_calibrated_trkJet().begin(),
                m_config->bTagWP_calibrated_trkJet().end(), tagWP) == m_config->bTagWP_calibrated_trkJet().end()) continue;
              m_perjet_weight_trackjet_bTagSF[tagWP][i] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_nom");
              for (size_t ivar = 0; ivar < m_config->trkjet_btagging_num_B_eigenvars(tagWP); ++ivar) {
                m_perjet_weight_trackjet_bTagSF_eigen_B_up[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_B_" + std::to_string(ivar) + "__1up");
                m_perjet_weight_trackjet_bTagSF_eigen_B_down[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_B_" + std::to_string(ivar) + "__1down");
              }
              for (size_t ivar = 0; ivar < m_config->trkjet_btagging_num_C_eigenvars(tagWP); ++ivar) {
                m_perjet_weight_trackjet_bTagSF_eigen_C_up[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_C_" + std::to_string(ivar) + "__1up");
                m_perjet_weight_trackjet_bTagSF_eigen_C_down[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_C_" + std::to_string(ivar) + "__1down");
              }
              for (size_t ivar = 0; ivar < m_config->trkjet_btagging_num_Light_eigenvars(tagWP); ++ivar) {
                m_perjet_weight_trackjet_bTagSF_eigen_Light_up[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_Light_" + std::to_string(ivar) + "__1up");
                m_perjet_weight_trackjet_bTagSF_eigen_Light_down[tagWP][i][ivar] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_FT_EFF_Eigen_Light_" + std::to_string(ivar) + "__1down");
              }
              for (const std::string& name : m_config->trkjet_btagging_namedSysts(tagWP)) {
                m_perjet_weight_trackjet_bTagSF_named_up[tagWP][name][i] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_" + name + "__1up");
                m_perjet_weight_trackjet_bTagSF_named_down[tagWP][name][i] = jetPtr->auxdataConst<float>("btag_SF_" + tagWP + "_" + name + "__1down");
              }
            }
          } else {
            m_tjet_tagWeightBin[tagWP][i] = -2;// AT default value
            if (jetPtr->isAvailable<int>("tagWeightBin_" + tagWP)) m_tjet_tagWeightBin[tagWP][i] = jetPtr->auxdataConst<int>("tagWeightBin_" + tagWP);
          }
        }
        ++i;
      }

      // loop over selected DL1 algos and fill all track jet b-tagging information
      // the accessor uses decoration created in TopSystematicObjectMaker/JetObjectCollectionMaker
      // calculated by BtaggingSelectionTool
      for (const std::string& algo : m_config->bTagAlgo_available_trkJet()) {
        std::vector<float>& m_tjet_DLx_pick = m_tjet_DLx.at(algo);
        std::vector<float>& m_tjet_DLx_pb_pick = m_tjet_DLx_pb.at(algo);
        std::vector<float>& m_tjet_DLx_pc_pick = m_tjet_DLx_pc.at(algo);
        std::vector<float>& m_tjet_DLx_pu_pick = m_tjet_DLx_pu.at(algo);
        const SG::AuxElement::ConstAccessor<float>& DLx_acc = DLx.at(algo);
        i = 0;
        for (const auto* const jetPtr : event.m_trackJets) {
          m_tjet_DLx_pick[i] = DLx_acc(*jetPtr);

          const xAOD::BTagging* btag(nullptr);
          btag = jetPtr->btagging();
          if (btag) {
            double pu = -999;
            double pc = -999;
            double pb = -999;
            btag->pu(algo, pu);
            btag->pc(algo, pc);
            btag->pb(algo, pb);
            m_tjet_DLx_pb_pick[i] = pb;
            m_tjet_DLx_pc_pick[i] = pc;
            m_tjet_DLx_pu_pick[i] = pu;
          }
          ++i;
        }
      }
    }

    if (m_makeRCJets) {
      // re-clustered jet substructure
      static const SG::AuxElement::ConstAccessor<float> RCSplit12("Split12");
      static const SG::AuxElement::ConstAccessor<float> RCSplit23("Split23");



      // re-clustered jet substructure from clusters
      static const SG::AuxElement::ConstAccessor<float> Tau21_clstr("Tau21_clstr");
      static const SG::AuxElement::ConstAccessor<float> Tau32_clstr("Tau32_clstr");
      static const SG::AuxElement::ConstAccessor<float> Tau3_clstr("Tau3_clstr");
      static const SG::AuxElement::ConstAccessor<float> Tau2_clstr("Tau2_clstr");
      static const SG::AuxElement::ConstAccessor<float> Tau1_clstr("Tau1_clstr");
      static const SG::AuxElement::ConstAccessor<float> D2_clstr("D2_clstr");
      static const SG::AuxElement::ConstAccessor<float> ECF1_clstr("ECF1_clstr");
      static const SG::AuxElement::ConstAccessor<float> ECF2_clstr("ECF2_clstr");
      static const SG::AuxElement::ConstAccessor<float> ECF3_clstr("ECF3_clstr");
      static const SG::AuxElement::ConstAccessor<float> d12_clstr("d12_clstr");
      static const SG::AuxElement::ConstAccessor<float> d23_clstr("d23_clstr");
      static const SG::AuxElement::ConstAccessor<float> Qw_clstr("Qw_clstr");
      static const SG::AuxElement::ConstAccessor<float> nconstituent_clstr("nconstituent_clstr");

      static const SG::AuxElement::ConstAccessor<float> gECF332_clstr("gECF332_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF461_clstr("gECF461_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF322_clstr("gECF322_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF331_clstr("gECF331_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF422_clstr("gECF422_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF441_clstr("gECF441_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF212_clstr("gECF212_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF321_clstr("gECF321_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF311_clstr("gECF311_clstr");
      static const SG::AuxElement::ConstAccessor<float> L1_clstr("L1_clstr");
      static const SG::AuxElement::ConstAccessor<float> L2_clstr("L2_clstr");
      static const SG::AuxElement::ConstAccessor<float> L3_clstr("L3_clstr");
      static const SG::AuxElement::ConstAccessor<float> L4_clstr("L4_clstr");
      static const SG::AuxElement::ConstAccessor<float> L5_clstr("L5_clstr");
      // store also the jet that is rebuilt to calculate the JSS
      static const SG::AuxElement::ConstAccessor<float> RRCJet_pt("RRCJet_pt");
      static const SG::AuxElement::ConstAccessor<float> RRCJet_eta("RRCJet_eta");
      static const SG::AuxElement::ConstAccessor<float> RRCJet_phi("RRCJet_phi");
      static const SG::AuxElement::ConstAccessor<float> RRCJet_e("RRCJet_e");

      // Initialize the vectors to be saved as branches
      unsigned int sizeOfRCjets(event.m_RCJets.size());

      m_rcjet_pt.clear();
      m_rcjet_eta.clear();
      m_rcjet_phi.clear();
      m_rcjet_e.clear();
      m_rcjet_d12.clear();
      m_rcjet_d23.clear();
      m_rcjetsub_pt.clear();
      m_rcjetsub_eta.clear();
      m_rcjetsub_phi.clear();
      m_rcjetsub_e.clear();
      if (m_config->bTagAlgo_MV2c10_used()) {
        m_rcjetsub_mv2c10.clear();
      }
      m_rrcjet_pt.clear();
      m_rrcjet_eta.clear();
      m_rrcjet_phi.clear();
      m_rrcjet_e.clear();


      m_rcjet_tau32_clstr.clear();
      m_rcjet_tau21_clstr.clear();
      m_rcjet_tau3_clstr.clear();
      m_rcjet_tau2_clstr.clear();
      m_rcjet_tau1_clstr.clear();

      m_rcjet_D2_clstr.clear();
      m_rcjet_ECF1_clstr.clear();
      m_rcjet_ECF2_clstr.clear();
      m_rcjet_ECF3_clstr.clear();

      m_rcjet_d12_clstr.clear();
      m_rcjet_d23_clstr.clear();
      m_rcjet_Qw_clstr.clear();
      m_rcjet_nconstituent_clstr.clear();
      m_rcjet_gECF332_clstr.clear();
      m_rcjet_gECF461_clstr.clear();
      m_rcjet_gECF322_clstr.clear();
      m_rcjet_gECF331_clstr.clear();
      m_rcjet_gECF422_clstr.clear();
      m_rcjet_gECF441_clstr.clear();
      m_rcjet_gECF212_clstr.clear();
      m_rcjet_gECF321_clstr.clear();
      m_rcjet_gECF311_clstr.clear();
      m_rcjet_L1_clstr.clear();
      m_rcjet_L2_clstr.clear();
      m_rcjet_L3_clstr.clear();
      m_rcjet_L4_clstr.clear();
      m_rcjet_L5_clstr.clear();

      m_rcjet_pt.resize(sizeOfRCjets, -999.);
      m_rcjet_eta.resize(sizeOfRCjets, -999.);
      m_rcjet_phi.resize(sizeOfRCjets, -999.);
      m_rcjet_e.resize(sizeOfRCjets, -999.);
      m_rcjet_d12.resize(sizeOfRCjets, -999.);
      m_rcjet_d23.resize(sizeOfRCjets, -999.);
      m_rcjetsub_pt.resize(sizeOfRCjets, std::vector<float>());
      m_rcjetsub_eta.resize(sizeOfRCjets, std::vector<float>());
      m_rcjetsub_phi.resize(sizeOfRCjets, std::vector<float>());
      m_rcjetsub_e.resize(sizeOfRCjets, std::vector<float>());
      if (m_config->bTagAlgo_MV2c10_used()) {
        m_rcjetsub_mv2c10.resize(sizeOfRCjets, std::vector<float>());
      }

      if (m_useRCJSS || m_useRCAdditionalJSS) {
        m_rrcjet_pt.resize(sizeOfRCjets, -999.);
        m_rrcjet_eta.resize(sizeOfRCjets, -999.);
        m_rrcjet_phi.resize(sizeOfRCjets, -999.);
        m_rrcjet_e.resize(sizeOfRCjets, -999.);
      }
      if (m_useRCJSS) {
        m_rcjet_tau32_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_tau21_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_tau1_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_tau2_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_tau3_clstr.resize(sizeOfRCjets, -999.);

        m_rcjet_d12_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_d23_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_Qw_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_nconstituent_clstr.resize(sizeOfRCjets, -999.);
	
	m_rcjet_D2_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_ECF1_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_ECF2_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_ECF3_clstr.resize(sizeOfRCjets, -999.);
      }
      if (m_useRCAdditionalJSS) {

        m_rcjet_gECF332_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF461_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF322_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF331_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF422_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF441_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF212_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF321_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF311_clstr.resize(sizeOfRCjets, -999.);

        m_rcjet_L1_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_L2_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_L3_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_L4_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_L5_clstr.resize(sizeOfRCjets, -999.);
      }
      unsigned int i = 0;
      for (auto jet_itr = event.m_RCJets.begin(); jet_itr != event.m_RCJets.end(); ++jet_itr) {
        const xAOD::Jet* rc_jet = *jet_itr;


        m_rcjet_pt[i] = rc_jet->pt();
        m_rcjet_eta[i] = rc_jet->eta();
        m_rcjet_phi[i] = rc_jet->phi();
        m_rcjet_e[i] = rc_jet->e();

        m_rcjet_d12[i] = (RCSplit12.isAvailable(*rc_jet)) ? RCSplit12(*rc_jet) : -999.;
        m_rcjet_d23[i] = (RCSplit23.isAvailable(*rc_jet)) ? RCSplit23(*rc_jet) : -999.;

        if (m_useRCJSS || m_useRCAdditionalJSS) {
          m_rrcjet_pt[i] = (RRCJet_pt.isAvailable(*rc_jet))  ? RRCJet_pt(*rc_jet) : -999.;
          m_rrcjet_eta[i] = (RRCJet_eta.isAvailable(*rc_jet)) ? RRCJet_eta(*rc_jet) : -999.;
          m_rrcjet_phi[i] = (RRCJet_phi.isAvailable(*rc_jet)) ? RRCJet_phi(*rc_jet) : -999.;
          m_rrcjet_e[i] = (RRCJet_e.isAvailable(*rc_jet))   ? RRCJet_e(*rc_jet) : -999.;
        }
        if (m_useRCJSS) {
          m_rcjet_tau32_clstr[i] = (Tau32_clstr.isAvailable(*rc_jet)) ? Tau32_clstr(*rc_jet) : -999.;
          m_rcjet_tau21_clstr[i] = (Tau21_clstr.isAvailable(*rc_jet)) ? Tau21_clstr(*rc_jet) : -999.;
          m_rcjet_tau3_clstr[i] = (Tau3_clstr.isAvailable(*rc_jet)) ? Tau3_clstr(*rc_jet) : -999.;
          m_rcjet_tau2_clstr[i] = (Tau2_clstr.isAvailable(*rc_jet)) ? Tau2_clstr(*rc_jet) : -999.;
          m_rcjet_tau1_clstr[i] = (Tau1_clstr.isAvailable(*rc_jet)) ? Tau1_clstr(*rc_jet) : -999.;

          m_rcjet_d12_clstr[i] = (d12_clstr.isAvailable(*rc_jet)) ? d12_clstr(*rc_jet) : -999.;
          m_rcjet_d23_clstr[i] = (d23_clstr.isAvailable(*rc_jet)) ? d23_clstr(*rc_jet) : -999.;
          m_rcjet_Qw_clstr[i] = (Qw_clstr.isAvailable(*rc_jet)) ? Qw_clstr(*rc_jet) : -999.;
          m_rcjet_nconstituent_clstr[i] = (nconstituent_clstr.isAvailable(*rc_jet)) ? nconstituent_clstr(*rc_jet) : -999.;
	  
	  m_rcjet_D2_clstr[i] = (D2_clstr.isAvailable(*rc_jet)) ? D2_clstr(*rc_jet) : -999.;
          m_rcjet_ECF1_clstr[i] = (ECF1_clstr.isAvailable(*rc_jet)) ? ECF1_clstr(*rc_jet) : -999.;
          m_rcjet_ECF2_clstr[i] = (ECF2_clstr.isAvailable(*rc_jet)) ? ECF2_clstr(*rc_jet) : -999.;
          m_rcjet_ECF3_clstr[i] = (ECF3_clstr.isAvailable(*rc_jet)) ? ECF3_clstr(*rc_jet) : -999.;

        }
        if (m_useRCAdditionalJSS) {
          
          m_rcjet_gECF332_clstr[i] = (gECF332_clstr.isAvailable(*rc_jet)) ? gECF332_clstr(*rc_jet) : -999.;
          m_rcjet_gECF461_clstr[i] = (gECF461_clstr.isAvailable(*rc_jet)) ? gECF461_clstr(*rc_jet) : -999.;
          m_rcjet_gECF322_clstr[i] = (gECF322_clstr.isAvailable(*rc_jet)) ? gECF322_clstr(*rc_jet) : -999.;
          m_rcjet_gECF331_clstr[i] = (gECF331_clstr.isAvailable(*rc_jet)) ? gECF331_clstr(*rc_jet) : -999.;
          m_rcjet_gECF422_clstr[i] = (gECF422_clstr.isAvailable(*rc_jet)) ? gECF422_clstr(*rc_jet) : -999.;
          m_rcjet_gECF441_clstr[i] = (gECF441_clstr.isAvailable(*rc_jet)) ? gECF441_clstr(*rc_jet) : -999.;
          m_rcjet_gECF212_clstr[i] = (gECF212_clstr.isAvailable(*rc_jet)) ? gECF212_clstr(*rc_jet) : -999.;
          m_rcjet_gECF321_clstr[i] = (gECF321_clstr.isAvailable(*rc_jet)) ? gECF321_clstr(*rc_jet) : -999.;
          m_rcjet_gECF311_clstr[i] = (gECF311_clstr.isAvailable(*rc_jet)) ? gECF311_clstr(*rc_jet) : -999.;

          m_rcjet_L1_clstr[i] = (L1_clstr.isAvailable(*rc_jet)) ? L1_clstr(*rc_jet) : -999.;
          m_rcjet_L2_clstr[i] = (L2_clstr.isAvailable(*rc_jet)) ? L2_clstr(*rc_jet) : -999.;
          m_rcjet_L3_clstr[i] = (L3_clstr.isAvailable(*rc_jet)) ? L3_clstr(*rc_jet) : -999.;
          m_rcjet_L4_clstr[i] = (L4_clstr.isAvailable(*rc_jet)) ? L4_clstr(*rc_jet) : -999.;
          m_rcjet_L5_clstr[i] = (L5_clstr.isAvailable(*rc_jet)) ? L5_clstr(*rc_jet) : -999.;
        }

        // loop over subjets
        m_rcjetsub_pt[i].clear();     // clear the vector size (otherwise it grows out of control!)
        m_rcjetsub_eta[i].clear();
        m_rcjetsub_phi[i].clear();
        m_rcjetsub_e[i].clear();
        if (m_config->bTagAlgo_MV2c10_used()) {
          m_rcjetsub_mv2c10[i].clear();
        }

        const xAOD::Jet* subjet(nullptr);
        const xAOD::BTagging* btag(nullptr);
        for (auto rc_jet_subjet : rc_jet->getConstituents()) {
          subjet = static_cast<const xAOD::Jet*>(rc_jet_subjet->rawConstituent());

          if (m_config->bTagAlgo_MV2c10_used()) {
            btag = subjet->btagging();

            double mvx10(-999.);  // b-tagging mv2c10

            if (btag) {
              btag->MVx_discriminant("MV2c10", mvx10);
            } else {
              mvx10 = -999.;
            }
            m_rcjetsub_mv2c10[i].push_back(mvx10);
          }

          m_rcjetsub_pt[i].push_back(subjet->pt());
          m_rcjetsub_eta[i].push_back(subjet->eta());
          m_rcjetsub_phi[i].push_back(subjet->phi());
          m_rcjetsub_e[i].push_back(subjet->e());
        } // end for-loop over subjets
        ++i;
      } // end for-loop over re-clustered jets
    } // end if make rcjets
      // end re-clustered jets

    /**********************************/
    // VarRC jets
    if (m_makeVarRCJets) {
      std::string VarRC = "vrcjet";
      for (auto& rho : m_VarRCJetRho) {
        for (auto& mass_scale : m_VarRCJetMassScale) {
          std::replace(rho.begin(), rho.end(), '.', '_');
          std::string name = rho + mass_scale;

          // re-clustered jet substructure
          static const SG::AuxElement::ConstAccessor<float> VarRCSplit12("Split12");
          static const SG::AuxElement::ConstAccessor<float> VarRCSplit23("Split23");

          // re-clustered jet substructure from clusters
          static const SG::AuxElement::ConstAccessor<float> Tau21_clstr("Tau21_clstr");
          static const SG::AuxElement::ConstAccessor<float> Tau32_clstr("Tau32_clstr");
          static const SG::AuxElement::ConstAccessor<float> Tau3_clstr("Tau3_clstr");
          static const SG::AuxElement::ConstAccessor<float> Tau2_clstr("Tau2_clstr");
          static const SG::AuxElement::ConstAccessor<float> Tau1_clstr("Tau1_clstr");
          static const SG::AuxElement::ConstAccessor<float> D2_clstr("D2_clstr");
          static const SG::AuxElement::ConstAccessor<float> ECF1_clstr("ECF1_clstr");
          static const SG::AuxElement::ConstAccessor<float> ECF2_clstr("ECF2_clstr");
          static const SG::AuxElement::ConstAccessor<float> ECF3_clstr("ECF3_clstr");
          static const SG::AuxElement::ConstAccessor<float> d12_clstr("d12_clstr");
          static const SG::AuxElement::ConstAccessor<float> d23_clstr("d23_clstr");
          static const SG::AuxElement::ConstAccessor<float> Qw_clstr("Qw_clstr");

          static const SG::AuxElement::ConstAccessor<float> gECF332_clstr("gECF332_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF461_clstr("gECF461_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF322_clstr("gECF322_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF331_clstr("gECF331_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF422_clstr("gECF422_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF441_clstr("gECF441_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF212_clstr("gECF212_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF321_clstr("gECF321_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF311_clstr("gECF311_clstr");
          static const SG::AuxElement::ConstAccessor<float> L1_clstr("L1_clstr");
          static const SG::AuxElement::ConstAccessor<float> L2_clstr("L2_clstr");
          static const SG::AuxElement::ConstAccessor<float> L3_clstr("L3_clstr");
          static const SG::AuxElement::ConstAccessor<float> L4_clstr("L4_clstr");
          static const SG::AuxElement::ConstAccessor<float> L5_clstr("L5_clstr");
          // store also the jet that is rebuilt to calculate the JSS
          static const SG::AuxElement::ConstAccessor<float> RRCJet_pt("RRCJet_pt");
          static const SG::AuxElement::ConstAccessor<float> RRCJet_eta("RRCJet_eta");
          static const SG::AuxElement::ConstAccessor<float> RRCJet_phi("RRCJet_phi");
          static const SG::AuxElement::ConstAccessor<float> RRCJet_e("RRCJet_e");

          // Initialize the vectors to be saved as branches

          xAOD::JetContainer* vrc_jets = event.m_VarRCJets[name].get();
          unsigned int sizeOfRCjets = vrc_jets->size();
          m_VarRCjetBranches[VarRC + "_" + name + "_pt"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranches[VarRC + "_" + name + "_eta"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranches[VarRC + "_" + name + "_phi"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranches[VarRC + "_" + name + "_e"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranches[VarRC + "_" + name + "_d12"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranches[VarRC + "_" + name + "_d23"].resize(sizeOfRCjets, -999.);
          m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_pt"].resize(sizeOfRCjets, std::vector<float>());
          m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_eta"].resize(sizeOfRCjets, std::vector<float>());
          m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_phi"].resize(sizeOfRCjets, std::vector<float>());
          m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_e"].resize(sizeOfRCjets, std::vector<float>());
          if (m_config->bTagAlgo_MV2c10_used()) {
            m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_mv2c10"].resize(sizeOfRCjets, std::vector<float>());
          }

          if (m_useVarRCJSS || m_useVarRCAdditionalJSS) {
            m_VarRCjetBranches["vrrcjet_" + name + "_pt"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches["vrrcjet_" + name + "_eta"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches["vrrcjet_" + name + "_phi"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches["vrrcjet_" + name + "_e"].resize(sizeOfRCjets, -999.);
          }
          if (m_useVarRCJSS) {
            m_VarRCjetBranches[VarRC + "_" + name + "_tau32_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_tau21_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_tau1_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_tau2_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_tau3_clstr"].resize(sizeOfRCjets, -999.);

            m_VarRCjetBranches[VarRC + "_" + name + "_d12_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_d23_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_Qw_clstr"].resize(sizeOfRCjets, -999.);
	    
	    m_VarRCjetBranches[VarRC + "_" + name + "_D2_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_ECF1_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_ECF2_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_ECF3_clstr"].resize(sizeOfRCjets, -999.);
          }
          if (m_useVarRCAdditionalJSS) {
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF332_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF461_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF322_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF331_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF422_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF441_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF212_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF321_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_gECF311_clstr"].resize(sizeOfRCjets, -999.);

            m_VarRCjetBranches[VarRC + "_" + name + "_L1_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_L2_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_L3_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_L4_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranches[VarRC + "_" + name + "_L5_clstr"].resize(sizeOfRCjets, -999.);
          }


          unsigned int i = 0;

          for (auto jet_ptr : *vrc_jets) {
            const xAOD::Jet* rc_jet = jet_ptr;

            m_VarRCjetBranches[VarRC + "_" + name + "_pt"][i] = rc_jet->pt();
            m_VarRCjetBranches[VarRC + "_" + name + "_eta"][i] = rc_jet->eta();
            m_VarRCjetBranches[VarRC + "_" + name + "_phi"][i] = rc_jet->phi();
            m_VarRCjetBranches[VarRC + "_" + name + "_e"][i] = rc_jet->e();

            m_VarRCjetBranches[VarRC + "_" + name + "_d12"][i] = (VarRCSplit12.isAvailable(*rc_jet)) ? VarRCSplit12(*rc_jet) : -999.;
            m_VarRCjetBranches[VarRC + "_" + name + "_d23"][i] = (VarRCSplit23.isAvailable(*rc_jet)) ? VarRCSplit23(*rc_jet) : -999.;

            if (m_useVarRCJSS || m_useVarRCAdditionalJSS) {
              m_VarRCjetBranches["vrrcjet_" + name + "_pt"][i] = (RRCJet_pt.isAvailable(*rc_jet))  ? RRCJet_pt(*rc_jet) : -999.;
              m_VarRCjetBranches["vrrcjet_" + name + "_eta"][i] = (RRCJet_eta.isAvailable(*rc_jet)) ? RRCJet_eta(*rc_jet) : -999.;
              m_VarRCjetBranches["vrrcjet_" + name + "_phi"][i] = (RRCJet_phi.isAvailable(*rc_jet)) ? RRCJet_phi(*rc_jet) : -999.;
              m_VarRCjetBranches["vrrcjet_" + name + "_e"][i] = (RRCJet_e.isAvailable(*rc_jet))   ? RRCJet_e(*rc_jet) : -999.;
            }
            if (m_useVarRCJSS) {
              m_VarRCjetBranches[VarRC + "_" + name + "_tau32_clstr"][i] = (Tau32_clstr.isAvailable(*rc_jet)) ? Tau32_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_tau21_clstr"][i] = (Tau21_clstr.isAvailable(*rc_jet)) ? Tau21_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_tau3_clstr"][i] = (Tau3_clstr.isAvailable(*rc_jet)) ? Tau3_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_tau2_clstr"][i] = (Tau2_clstr.isAvailable(*rc_jet)) ? Tau2_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_tau1_clstr"][i] = (Tau1_clstr.isAvailable(*rc_jet)) ? Tau1_clstr(*rc_jet) : -999.;

              m_VarRCjetBranches[VarRC + "_" + name + "_d12_clstr"][i] = (d12_clstr.isAvailable(*rc_jet)) ? d12_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_d23_clstr"][i] = (d23_clstr.isAvailable(*rc_jet)) ? d23_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_Qw_clstr"][i] = (Qw_clstr.isAvailable(*rc_jet)) ? Qw_clstr(*rc_jet) : -999.;
	      
	      m_VarRCjetBranches[VarRC + "_" + name + "_D2_clstr"][i] = (D2_clstr.isAvailable(*rc_jet)) ? D2_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_ECF1_clstr"][i] = (ECF1_clstr.isAvailable(*rc_jet)) ? ECF1_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_ECF2_clstr"][i] = (ECF2_clstr.isAvailable(*rc_jet)) ? ECF2_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_ECF3_clstr"][i] = (ECF3_clstr.isAvailable(*rc_jet)) ? ECF3_clstr(*rc_jet) : -999.;
            }
            if (m_useVarRCAdditionalJSS) {
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF332_clstr"][i] = (gECF332_clstr.isAvailable(*rc_jet)) ? gECF332_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF461_clstr"][i] = (gECF461_clstr.isAvailable(*rc_jet)) ? gECF461_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF322_clstr"][i] = (gECF322_clstr.isAvailable(*rc_jet)) ? gECF322_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF331_clstr"][i] = (gECF331_clstr.isAvailable(*rc_jet)) ? gECF331_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF422_clstr"][i] = (gECF422_clstr.isAvailable(*rc_jet)) ? gECF422_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF441_clstr"][i] = (gECF441_clstr.isAvailable(*rc_jet)) ? gECF441_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF212_clstr"][i] = (gECF212_clstr.isAvailable(*rc_jet)) ? gECF212_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF321_clstr"][i] = (gECF321_clstr.isAvailable(*rc_jet)) ? gECF321_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_gECF311_clstr"][i] = (gECF311_clstr.isAvailable(*rc_jet)) ? gECF311_clstr(*rc_jet) : -999.;

              m_VarRCjetBranches[VarRC + "_" + name + "_L1_clstr"][i] = (L1_clstr.isAvailable(*rc_jet)) ? L1_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_L2_clstr"][i] = (L2_clstr.isAvailable(*rc_jet)) ? L2_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_L3_clstr"][i] = (L3_clstr.isAvailable(*rc_jet)) ? L3_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_L4_clstr"][i] = (L4_clstr.isAvailable(*rc_jet)) ? L4_clstr(*rc_jet) : -999.;
              m_VarRCjetBranches[VarRC + "_" + name + "_L5_clstr"][i] = (L5_clstr.isAvailable(*rc_jet)) ? L5_clstr(*rc_jet) : -999.;
            }


            // loop over subjets
            const xAOD::Jet* subjet(nullptr);
            const xAOD::BTagging* btag(nullptr);
            m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_pt"][i].clear();     // clear the vector size (otherwise it
                                                                                  // grows out of control!)
            m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_eta"][i].clear();
            m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_phi"][i].clear();
            m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_e"][i].clear();
            if (m_config->bTagAlgo_MV2c10_used()) {
              m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_mv2c10"][i].clear();
            }
            for (auto rc_jet_subjet : rc_jet->getConstituents()) {
              subjet = static_cast<const xAOD::Jet*>(rc_jet_subjet->rawConstituent());

              if (m_config->bTagAlgo_MV2c10_used()) {
                btag = subjet->btagging();

                double mvx10(-999.);  // b-tagging mv2c10

                if (btag) {
                  btag->MVx_discriminant("MV2c10", mvx10);
                } else {
                  mvx10 = -999.;
                }
                m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_mv2c10"][i].push_back(mvx10);
              }

              m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_pt"][i].push_back(subjet->pt());
              m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_eta"][i].push_back(subjet->eta());
              m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_phi"][i].push_back(subjet->phi());
              m_VarRCjetsubBranches[VarRC + "_" + name + "_sub_e"][i].push_back(subjet->e());
            } // end for-loop over subjets
            ++i;
          } // end for-loop over re-clustered jets
        } // end loop over mass parameters
      } // end loop over multipliers for mass scale
    } // end if make VarRC jets
      // end VarRC jets


    //met
    m_met_met = event.m_met->met();
    m_met_phi = event.m_met->phi();
    
    if(m_config->writeMETBuiltWithLooseObjects())
    {
      const xAOD::MissingETContainer* mets(nullptr);
      if(event.m_isLoose) top::check(evtStore()->retrieve(mets, m_config->sgKeyMissingEtLoose(event.m_hashValue)+"WithLooseObjects"), "Failed to retrieve MET");
      else top::check(evtStore()->retrieve(mets, m_config->sgKeyMissingEt(event.m_hashValue)+"WithLooseObjects"), "Failed to retrieve MET");
      
      m_met_met_withLooseObjects=(*mets)["FinalTrk"]->met();
      m_met_phi_withLooseObjects=(*mets)["FinalTrk"]->phi();
    }

    //KLFitter
    if (m_config->doKLFitter()) {
      unsigned int nPermutations(0), iPerm(0), bestPerm(0);
      bool validKLFitter(false);
      m_klfitter_selected = 0;
      if (event.m_KLFitterResults != nullptr) {
        validKLFitter = true;
        m_klfitter_selected = 1;
        nPermutations = event.m_KLFitterResults->size();
      }

      m_klfitter_selection.resize(nPermutations);
      m_klfitter_minuitDidNotConverge.resize(nPermutations);
      m_klfitter_fitAbortedDueToNaN.resize(nPermutations);
      m_klfitter_atLeastOneFitParameterAtItsLimit.resize(nPermutations);
      m_klfitter_invalidTransferFunctionAtConvergence.resize(nPermutations);

      /// Global result
      m_klfitter_bestPermutation.resize(nPermutations);
      m_klfitter_logLikelihood.resize(nPermutations);
      m_klfitter_eventProbability.resize(nPermutations);
      m_klfitter_parameters_size.resize(nPermutations);
      m_klfitter_parameters.resize(nPermutations);
      m_klfitter_parameterErrors.resize(nPermutations);

      if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles" || m_config->KLFitterLH() == "ttbar_BoostedLJets") {
        /// Model
        m_klfitter_model_bhad_pt.resize(nPermutations);
        m_klfitter_model_bhad_eta.resize(nPermutations);
        m_klfitter_model_bhad_phi.resize(nPermutations);
        m_klfitter_model_bhad_E.resize(nPermutations);
        m_klfitter_model_bhad_jetIndex.resize(nPermutations);

        m_klfitter_model_blep_pt.resize(nPermutations);
        m_klfitter_model_blep_eta.resize(nPermutations);
        m_klfitter_model_blep_phi.resize(nPermutations);
        m_klfitter_model_blep_E.resize(nPermutations);
        m_klfitter_model_blep_jetIndex.resize(nPermutations);

        m_klfitter_model_lq1_pt.resize(nPermutations);
        m_klfitter_model_lq1_eta.resize(nPermutations);
        m_klfitter_model_lq1_phi.resize(nPermutations);
        m_klfitter_model_lq1_E.resize(nPermutations);
        m_klfitter_model_lq1_jetIndex.resize(nPermutations);

        if (m_config->KLFitterLH() != "ttbar_BoostedLJets") {
          m_klfitter_model_lq2_pt.resize(nPermutations);
          m_klfitter_model_lq2_eta.resize(nPermutations);
          m_klfitter_model_lq2_phi.resize(nPermutations);
          m_klfitter_model_lq2_E.resize(nPermutations);
          m_klfitter_model_lq2_jetIndex.resize(nPermutations);

          if (m_config->KLFitterLH() == "ttH") {
            m_klfitter_model_Higgs_b1_pt.resize(nPermutations);
            m_klfitter_model_Higgs_b1_eta.resize(nPermutations);
            m_klfitter_model_Higgs_b1_phi.resize(nPermutations);
            m_klfitter_model_Higgs_b1_E.resize(nPermutations);
            m_klfitter_model_Higgs_b1_jetIndex.resize(nPermutations);

            m_klfitter_model_Higgs_b2_pt.resize(nPermutations);
            m_klfitter_model_Higgs_b2_eta.resize(nPermutations);
            m_klfitter_model_Higgs_b2_phi.resize(nPermutations);
            m_klfitter_model_Higgs_b2_E.resize(nPermutations);
            m_klfitter_model_Higgs_b2_jetIndex.resize(nPermutations);
          }
        }

        m_klfitter_model_lep_pt.resize(nPermutations);
        m_klfitter_model_lep_eta.resize(nPermutations);
        m_klfitter_model_lep_phi.resize(nPermutations);
        m_klfitter_model_lep_E.resize(nPermutations);
        m_klfitter_model_lep_index.resize(nPermutations);

        m_klfitter_model_lepZ1_pt.resize(nPermutations);
        m_klfitter_model_lepZ1_eta.resize(nPermutations);
        m_klfitter_model_lepZ1_phi.resize(nPermutations);
        m_klfitter_model_lepZ1_E.resize(nPermutations);
        m_klfitter_model_lepZ1_index.resize(nPermutations);

        m_klfitter_model_lepZ2_pt.resize(nPermutations);
        m_klfitter_model_lepZ2_eta.resize(nPermutations);
        m_klfitter_model_lepZ2_phi.resize(nPermutations);
        m_klfitter_model_lepZ2_E.resize(nPermutations);
        m_klfitter_model_lepZ2_index.resize(nPermutations);

        m_klfitter_model_nu_pt.resize(nPermutations);
        m_klfitter_model_nu_eta.resize(nPermutations);
        m_klfitter_model_nu_phi.resize(nPermutations);
        m_klfitter_model_nu_E.resize(nPermutations);
      } else if (m_config->KLFitterLH() == "ttbar_AllHadronic") {
        m_klfitter_model_b_from_top1_pt.resize(nPermutations);
        m_klfitter_model_b_from_top1_eta.resize(nPermutations);
        m_klfitter_model_b_from_top1_phi.resize(nPermutations);
        m_klfitter_model_b_from_top1_E.resize(nPermutations);
        m_klfitter_model_b_from_top1_jetIndex.resize(nPermutations);

        m_klfitter_model_b_from_top2_pt.resize(nPermutations);
        m_klfitter_model_b_from_top2_eta.resize(nPermutations);
        m_klfitter_model_b_from_top2_phi.resize(nPermutations);
        m_klfitter_model_b_from_top2_E.resize(nPermutations);
        m_klfitter_model_b_from_top2_jetIndex.resize(nPermutations);

        m_klfitter_model_lj1_from_top1_pt.resize(nPermutations);
        m_klfitter_model_lj1_from_top1_eta.resize(nPermutations);
        m_klfitter_model_lj1_from_top1_phi.resize(nPermutations);
        m_klfitter_model_lj1_from_top1_E.resize(nPermutations);
        m_klfitter_model_lj1_from_top1_jetIndex.resize(nPermutations);

        m_klfitter_model_lj2_from_top1_pt.resize(nPermutations);
        m_klfitter_model_lj2_from_top1_eta.resize(nPermutations);
        m_klfitter_model_lj2_from_top1_phi.resize(nPermutations);
        m_klfitter_model_lj2_from_top1_E.resize(nPermutations);
        m_klfitter_model_lj2_from_top1_jetIndex.resize(nPermutations);

        m_klfitter_model_lj1_from_top2_pt.resize(nPermutations);
        m_klfitter_model_lj1_from_top2_eta.resize(nPermutations);
        m_klfitter_model_lj1_from_top2_phi.resize(nPermutations);
        m_klfitter_model_lj1_from_top2_E.resize(nPermutations);
        m_klfitter_model_lj1_from_top2_jetIndex.resize(nPermutations);

        m_klfitter_model_lj2_from_top2_pt.resize(nPermutations);
        m_klfitter_model_lj2_from_top2_eta.resize(nPermutations);
        m_klfitter_model_lj2_from_top2_phi.resize(nPermutations);
        m_klfitter_model_lj2_from_top2_E.resize(nPermutations);
        m_klfitter_model_lj2_from_top2_jetIndex.resize(nPermutations);
      }

      if (validKLFitter) {
        for (const auto* const klPtr : *event.m_KLFitterResults) {
          m_klfitter_selection[iPerm] = "unknown";
          std::hash<std::string> st_hash;
          for (unsigned int s = 0; s < m_config->allSelectionNames()->size(); ++s) {
            if (st_hash(m_config->allSelectionNames()->at(s)) == klPtr->selectionCode()) {
              m_klfitter_selection[iPerm] = m_config->allSelectionNames()->at(s);
              break;
            }
          }
          m_klfitter_minuitDidNotConverge[iPerm] = klPtr->minuitDidNotConverge();
          m_klfitter_fitAbortedDueToNaN[iPerm] = klPtr->fitAbortedDueToNaN();
          m_klfitter_atLeastOneFitParameterAtItsLimit[iPerm] = klPtr->atLeastOneFitParameterAtItsLimit();
          m_klfitter_invalidTransferFunctionAtConvergence[iPerm] = klPtr->invalidTransferFunctionAtConvergence();

          /// Global result
          m_klfitter_bestPermutation[iPerm] = klPtr->bestPermutation();
          if (klPtr->bestPermutation() == 1) {
            bestPerm = iPerm;
          }
          m_klfitter_logLikelihood[iPerm] = klPtr->logLikelihood();
          m_klfitter_eventProbability[iPerm] = klPtr->eventProbability();
          m_klfitter_parameters_size[iPerm] = klPtr->parameters().size();
          m_klfitter_parameters[iPerm] = klPtr->parameters();
          m_klfitter_parameterErrors[iPerm] = klPtr->parameterErrors();

          /// Model
          if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles" || m_config->KLFitterLH() == "ttbar_BoostedLJets") {
            m_klfitter_model_bhad_pt[iPerm] = klPtr->model_bhad_pt();
            m_klfitter_model_bhad_eta[iPerm] = klPtr->model_bhad_eta();
            m_klfitter_model_bhad_phi[iPerm] = klPtr->model_bhad_phi();
            m_klfitter_model_bhad_E[iPerm] = klPtr->model_bhad_E();
            m_klfitter_model_bhad_jetIndex[iPerm] = klPtr->model_bhad_jetIndex();

            m_klfitter_model_blep_pt[iPerm] = klPtr->model_blep_pt();
            m_klfitter_model_blep_eta[iPerm] = klPtr->model_blep_eta();
            m_klfitter_model_blep_phi[iPerm] = klPtr->model_blep_phi();
            m_klfitter_model_blep_E[iPerm] = klPtr->model_blep_E();
            m_klfitter_model_blep_jetIndex[iPerm] = klPtr->model_blep_jetIndex();

            m_klfitter_model_lq1_pt[iPerm] = klPtr->model_lq1_pt();
            m_klfitter_model_lq1_eta[iPerm] = klPtr->model_lq1_eta();
            m_klfitter_model_lq1_phi[iPerm] = klPtr->model_lq1_phi();
            m_klfitter_model_lq1_E[iPerm] = klPtr->model_lq1_E();
            m_klfitter_model_lq1_jetIndex[iPerm] = klPtr->model_lq1_jetIndex();

            if (m_config->KLFitterLH() != "ttbar_BoostedLJets") {
              m_klfitter_model_lq2_pt[iPerm] = klPtr->model_lq2_pt();
              m_klfitter_model_lq2_eta[iPerm] = klPtr->model_lq2_eta();
              m_klfitter_model_lq2_phi[iPerm] = klPtr->model_lq2_phi();
              m_klfitter_model_lq2_E[iPerm] = klPtr->model_lq2_E();
              m_klfitter_model_lq2_jetIndex[iPerm] = klPtr->model_lq2_jetIndex();

              if (m_config->KLFitterLH() == "ttH") {
                m_klfitter_model_Higgs_b1_pt[iPerm] = klPtr->model_Higgs_b1_pt();
                m_klfitter_model_Higgs_b1_eta[iPerm] = klPtr->model_Higgs_b1_eta();
                m_klfitter_model_Higgs_b1_phi[iPerm] = klPtr->model_Higgs_b1_phi();
                m_klfitter_model_Higgs_b1_E[iPerm] = klPtr->model_Higgs_b1_E();
                m_klfitter_model_Higgs_b1_jetIndex[iPerm] = klPtr->model_Higgs_b1_jetIndex();

                m_klfitter_model_Higgs_b2_pt[iPerm] = klPtr->model_Higgs_b2_pt();
                m_klfitter_model_Higgs_b2_eta[iPerm] = klPtr->model_Higgs_b2_eta();
                m_klfitter_model_Higgs_b2_phi[iPerm] = klPtr->model_Higgs_b2_phi();
                m_klfitter_model_Higgs_b2_E[iPerm] = klPtr->model_Higgs_b2_E();
                m_klfitter_model_Higgs_b2_jetIndex[iPerm] = klPtr->model_Higgs_b2_jetIndex();
              }
            }
            m_klfitter_model_lep_pt[iPerm] = klPtr->model_lep_pt();
            m_klfitter_model_lep_eta[iPerm] = klPtr->model_lep_eta();
            m_klfitter_model_lep_phi[iPerm] = klPtr->model_lep_phi();
            m_klfitter_model_lep_E[iPerm] = klPtr->model_lep_E();
            m_klfitter_model_lep_index[iPerm] = klPtr->model_lep_index();

            m_klfitter_model_lepZ1_pt[iPerm] = klPtr->model_lepZ1_pt();
            m_klfitter_model_lepZ1_eta[iPerm] = klPtr->model_lepZ1_eta();
            m_klfitter_model_lepZ1_phi[iPerm] = klPtr->model_lepZ1_phi();
            m_klfitter_model_lepZ1_E[iPerm] = klPtr->model_lepZ1_E();
            m_klfitter_model_lepZ1_index[iPerm] = klPtr->model_lepZ1_index();

            m_klfitter_model_lepZ2_pt[iPerm] = klPtr->model_lepZ2_pt();
            m_klfitter_model_lepZ2_eta[iPerm] = klPtr->model_lepZ2_eta();
            m_klfitter_model_lepZ2_phi[iPerm] = klPtr->model_lepZ2_phi();
            m_klfitter_model_lepZ2_E[iPerm] = klPtr->model_lepZ2_E();
            m_klfitter_model_lepZ2_index[iPerm] = klPtr->model_lepZ2_index();

            m_klfitter_model_nu_pt[iPerm] = klPtr->model_nu_pt();
            m_klfitter_model_nu_eta[iPerm] = klPtr->model_nu_eta();
            m_klfitter_model_nu_phi[iPerm] = klPtr->model_nu_phi();
            m_klfitter_model_nu_E[iPerm] = klPtr->model_nu_E();
          } else if (m_config->KLFitterLH() == "ttbar_AllHadronic") {
            m_klfitter_model_b_from_top1_pt[iPerm] = klPtr->model_b_from_top1_pt();
            m_klfitter_model_b_from_top1_eta[iPerm] = klPtr->model_b_from_top1_eta();
            m_klfitter_model_b_from_top1_phi[iPerm] = klPtr->model_b_from_top1_phi();
            m_klfitter_model_b_from_top1_E[iPerm] = klPtr->model_b_from_top1_E();
            m_klfitter_model_b_from_top1_jetIndex[iPerm] = klPtr->model_b_from_top1_jetIndex();

            m_klfitter_model_b_from_top2_pt[iPerm] = klPtr->model_b_from_top2_pt();
            m_klfitter_model_b_from_top2_eta[iPerm] = klPtr->model_b_from_top2_eta();
            m_klfitter_model_b_from_top2_phi[iPerm] = klPtr->model_b_from_top2_phi();
            m_klfitter_model_b_from_top2_E[iPerm] = klPtr->model_b_from_top2_E();
            m_klfitter_model_b_from_top2_jetIndex[iPerm] = klPtr->model_b_from_top2_jetIndex();

            m_klfitter_model_lj1_from_top1_pt[iPerm] = klPtr->model_lj1_from_top1_pt();
            m_klfitter_model_lj1_from_top1_eta[iPerm] = klPtr->model_lj1_from_top1_eta();
            m_klfitter_model_lj1_from_top1_phi[iPerm] = klPtr->model_lj1_from_top1_phi();
            m_klfitter_model_lj1_from_top1_E[iPerm] = klPtr->model_lj1_from_top1_E();
            m_klfitter_model_lj1_from_top1_jetIndex[iPerm] = klPtr->model_lj1_from_top1_jetIndex();

            m_klfitter_model_lj2_from_top1_pt[iPerm] = klPtr->model_lj2_from_top1_pt();
            m_klfitter_model_lj2_from_top1_eta[iPerm] = klPtr->model_lj2_from_top1_eta();
            m_klfitter_model_lj2_from_top1_phi[iPerm] = klPtr->model_lj2_from_top1_phi();
            m_klfitter_model_lj2_from_top1_E[iPerm] = klPtr->model_lj2_from_top1_E();
            m_klfitter_model_lj2_from_top1_jetIndex[iPerm] = klPtr->model_lj2_from_top1_jetIndex();

            m_klfitter_model_lj1_from_top2_pt[iPerm] = klPtr->model_lj1_from_top2_pt();
            m_klfitter_model_lj1_from_top2_eta[iPerm] = klPtr->model_lj1_from_top2_eta();
            m_klfitter_model_lj1_from_top2_phi[iPerm] = klPtr->model_lj1_from_top2_phi();
            m_klfitter_model_lj1_from_top2_E[iPerm] = klPtr->model_lj1_from_top2_E();
            m_klfitter_model_lj1_from_top2_jetIndex[iPerm] = klPtr->model_lj1_from_top2_jetIndex();

            m_klfitter_model_lj2_from_top2_pt[iPerm] = klPtr->model_lj2_from_top2_pt();
            m_klfitter_model_lj2_from_top2_eta[iPerm] = klPtr->model_lj2_from_top2_eta();
            m_klfitter_model_lj2_from_top2_phi[iPerm] = klPtr->model_lj2_from_top2_phi();
            m_klfitter_model_lj2_from_top2_E[iPerm] = klPtr->model_lj2_from_top2_E();
            m_klfitter_model_lj2_from_top2_jetIndex[iPerm] = klPtr->model_lj2_from_top2_jetIndex();
          }


          ++iPerm;
        }

        // now take the best permutation and build the tops and the ttbar system!
        if (m_config->KLFitterLH() == "ttbar" || m_config->KLFitterLH() == "ttZTrilepton" || m_config->KLFitterLH() == "ttH" || m_config->KLFitterLH() == "ttbar_JetAngles") {
          if (nPermutations != 0) {
            TLorentzVector bhad, blep, lq1, lq2, lep, nu, top_had, top_lep, ttbar;

            bhad.SetPtEtaPhiE(m_klfitter_model_bhad_pt[bestPerm], m_klfitter_model_bhad_eta[bestPerm], m_klfitter_model_bhad_phi[bestPerm], m_klfitter_model_bhad_E[bestPerm]);
            blep.SetPtEtaPhiE(m_klfitter_model_blep_pt[bestPerm], m_klfitter_model_blep_eta[bestPerm], m_klfitter_model_blep_phi[bestPerm], m_klfitter_model_blep_E[bestPerm]);
            lq1.SetPtEtaPhiE(m_klfitter_model_lq1_pt[bestPerm], m_klfitter_model_lq1_eta[bestPerm], m_klfitter_model_lq1_phi[bestPerm], m_klfitter_model_lq1_E[bestPerm]);
            lq2.SetPtEtaPhiE(m_klfitter_model_lq2_pt[bestPerm], m_klfitter_model_lq2_eta[bestPerm], m_klfitter_model_lq2_phi[bestPerm], m_klfitter_model_lq2_E[bestPerm]);
            lep.SetPtEtaPhiE(m_klfitter_model_lep_pt[bestPerm], m_klfitter_model_lep_eta[bestPerm], m_klfitter_model_lep_phi[bestPerm], m_klfitter_model_lep_E[bestPerm]);
            nu.SetPtEtaPhiE(m_klfitter_model_nu_pt[bestPerm], m_klfitter_model_nu_eta[bestPerm], m_klfitter_model_nu_phi[bestPerm], m_klfitter_model_nu_E[bestPerm]);

            top_had = bhad + lq1 + lq2;
            top_lep = blep + lep + nu;
            ttbar = top_had + top_lep;

            m_klfitter_bestPerm_topLep_pt = top_lep.Pt();
            m_klfitter_bestPerm_topLep_eta = top_lep.Eta();
            m_klfitter_bestPerm_topLep_phi = top_lep.Phi();
            m_klfitter_bestPerm_topLep_E = top_lep.E();
            m_klfitter_bestPerm_topLep_m = top_lep.M();

            m_klfitter_bestPerm_topHad_pt = top_had.Pt();
            m_klfitter_bestPerm_topHad_eta = top_had.Eta();
            m_klfitter_bestPerm_topHad_phi = top_had.Phi();
            m_klfitter_bestPerm_topHad_E = top_had.E();
            m_klfitter_bestPerm_topHad_m = top_had.M();

            m_klfitter_bestPerm_ttbar_pt = ttbar.Pt();
            m_klfitter_bestPerm_ttbar_eta = ttbar.Eta();
            m_klfitter_bestPerm_ttbar_phi = ttbar.Phi();
            m_klfitter_bestPerm_ttbar_E = ttbar.E();
            m_klfitter_bestPerm_ttbar_m = ttbar.M();
          }
        }
      }
    }

    if (m_config->doPseudoTop()) {
      const xAOD::PseudoTopResultContainer* pseudoTopResultContainer(nullptr);
      const xAOD::PseudoTopResult* pseudoTopResult(nullptr);

      if ((!event.m_isLoose && evtStore()->contains<xAOD::PseudoTopResultContainer>(topConfig()->sgKeyPseudoTop(event.m_hashValue))) ||
          (event.m_isLoose && evtStore()->contains<xAOD::PseudoTopResultContainer>(topConfig()->sgKeyPseudoTopLoose(event.m_hashValue)))) {
        if (!event.m_isLoose) {
          top::check(evtStore()->retrieve(pseudoTopResultContainer, topConfig()->sgKeyPseudoTop(event.m_hashValue)), "Failed to retrieve PseudoTop");
        } else {
          top::check(evtStore()->retrieve(pseudoTopResultContainer, topConfig()->sgKeyPseudoTopLoose(event.m_hashValue)), "Failed to retrieve PseudoTop");
        }

        pseudoTopResult = pseudoTopResultContainer->at(0);

        m_PseudoTop_Reco_ttbar_pt = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_ttbar_pt");
        m_PseudoTop_Reco_ttbar_eta = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_ttbar_eta");
        m_PseudoTop_Reco_ttbar_phi = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_ttbar_phi");
        m_PseudoTop_Reco_ttbar_m = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_ttbar_m");

        m_PseudoTop_Reco_top_lep_pt = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_top_lep_pt");
        m_PseudoTop_Reco_top_lep_eta = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_top_lep_eta");
        m_PseudoTop_Reco_top_lep_phi = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_top_lep_phi");
        m_PseudoTop_Reco_top_lep_m = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_top_lep_m");

        m_PseudoTop_Reco_top_had_pt = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_top_had_pt");
        m_PseudoTop_Reco_top_had_eta = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_top_had_eta");
        m_PseudoTop_Reco_top_had_phi = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_top_had_phi");
        m_PseudoTop_Reco_top_had_m = pseudoTopResult->auxdecor<float>("PseudoTop_Reco_top_had_m");
      }
    }
  }

  void EventSaverFlatNtuple::fillEvent(const top::Event& event) {
    //do it!
    m_treeManagers[event.m_ttreeIndex]->fill();
  }

  void EventSaverFlatNtuple::saveTruthEvent() {
    this->cleanTruthEvent();
    this->calculateTruthEvent();
    this->fillTruthEvent();
  }

  void EventSaverFlatNtuple::cleanTruthEvent() {
    /* Doing nothing for now, but we should put here the (re-)initialisation to dummy values
     * of all variables later calculated in cleanTruthEvent()
     * For example all weights set to 1, all vectors cleaned, all kinematic or angular variables set to -99999.
     */
  }

  void EventSaverFlatNtuple::calculateTruthEvent() {
    
    const xAOD::EventInfo* eventInfo(nullptr);

    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");

    m_weight_mc = eventInfo->auxdataConst<float>("AnalysisTop_eventWeight");
    m_eventNumber = eventInfo->eventNumber();
    m_runNumber = eventInfo->runNumber();
    m_mcChannelNumber = eventInfo->mcChannelNumber();
    m_mu = eventInfo->averageInteractionsPerCrossing();
    m_mu_actual = eventInfo->actualInteractionsPerCrossing();

    if (m_config->doPileupReweighting() && !m_config->isTruthDxAOD()) {
      m_weight_pileup = eventInfo->auxdataConst<float>("PileupWeight");
      m_randomRunNumber = eventInfo->auxdataConst<unsigned int>("RandomRunNumber");
    } else {
      m_weight_pileup = NAN;
      m_randomRunNumber = 0;
    }

    //MC particle
    if (m_config->useTruthParticles() && m_config->doTruthBlockInfo()) {
      const xAOD::TruthParticleContainer* truth(nullptr);
      top::check(evtStore()->retrieve(truth, m_config->sgKeyMCParticle()), "Failed to retrieve TruthParticleContainer");

      if (truth != nullptr) {
        unsigned int i(0);
        unsigned int truthSize = truth->size();
        m_mc_pt.resize(truthSize);
        m_mc_eta.resize(truthSize);
        m_mc_phi.resize(truthSize);
        m_mc_e.resize(truthSize);
        m_mc_pdgId.resize(truthSize);
        m_mc_charge.resize(truthSize);
        m_mc_status.resize(truthSize);
        m_mc_barcode.resize(truthSize);

        for (const auto* const mcPtr : *truth) {
          // Fix for
          // Warning in <TVector3::PseudoRapidity>: transvers momentum = 0! return +/- 10e10

          float eta(-999.9);

          if (!(mcPtr->p4().CosTheta() * mcPtr->p4().CosTheta() >= 1 && mcPtr->p4().Pz() != 0)) {
            eta = mcPtr->eta();
          }

          m_mc_pt[i] = mcPtr->pt();
          m_mc_eta[i] = eta;
          m_mc_phi[i] = mcPtr->phi();
          m_mc_e[i] = mcPtr->e();
          m_mc_pdgId[i] = mcPtr->pdgId();
          m_mc_charge[i] = mcPtr->charge();
          m_mc_status[i] = mcPtr->status();
          m_mc_barcode[i] = mcPtr->barcode();

          ++i;
        }
      }
    }

    if (m_config->doTopPartonHistory()) {
      const xAOD::PartonHistoryContainer* partonHistoryContainer(nullptr);
      const xAOD::PartonHistory* partonHistory(nullptr);
      if (evtStore()->contains<xAOD::PartonHistoryContainer>(m_config->sgKeyTopPartonHistory())) {
        top::check(evtStore()->retrieve(partonHistoryContainer, m_config->sgKeyTopPartonHistory()), "Failed to retrieve Top Parton History");
        if (partonHistoryContainer->size() == 1) {
          partonHistory = partonHistoryContainer->at(0);
        }
      }

      if (partonHistory != nullptr) {
        if (!m_config->isTopPartonHistoryRegisteredInNtuple()) {
          registerObjectIntoTruthTree(*(partonHistory));
          m_config->setTopPartonHistoryRegisteredInNtuple();
        }
        saveObjectIntoTruthTree(*(partonHistory));
      }
    }
  }

  void EventSaverFlatNtuple::fillTruthEvent() {
    if(m_config->doTopPartonLevel()) m_truthTreeManager->fill();
  }

  void EventSaverFlatNtuple::saveParticleLevelEvent(const top::ParticleLevelEvent& plEvent) {
    // Quick return if particle level is disabled.
    if (not m_config->doTopParticleLevel()) {
      return;
    }
    // No need to attempt to write out anything for non-MC data.
    if (!m_config->isMC()) {
      return;
    }
    this->cleanParticleLevelEvent();
    this->calculateParticleLevelEvent(plEvent);
    this->fillParticleLevelEvent();
  }

  void EventSaverFlatNtuple::cleanParticleLevelEvent() {
    /* Doing nothing for now, but we should put here the (re-)initialisation to dummy values
     * of all variables later calculated in calculateParticleLevelEvent(const top::ParticleLevelEvent& plEvent)
     * For example all weights set to 1, all vectors cleaned, all kinematic or angular variables set to -99999.
     */
  }

  void EventSaverFlatNtuple::calculateParticleLevelEvent(const top::ParticleLevelEvent& plEvent) {
    for (auto& selectionDecision : m_particleLevel_SelectionDecisions) {
      selectionDecision.second = plEvent.m_selectionDecisions[ selectionDecision.first ];
    }

    m_weight_mc = plEvent.m_info->auxdataConst<float>("AnalysisTop_eventWeight");

    m_eventNumber = plEvent.m_info->eventNumber();
    m_runNumber = plEvent.m_info->runNumber();
    m_mcChannelNumber = plEvent.m_info->mcChannelNumber();

    m_mu = plEvent.m_info->averageInteractionsPerCrossing();
    m_mu_actual = plEvent.m_info->actualInteractionsPerCrossing();

    if (m_config->doPileupReweighting() && !m_config->isTruthDxAOD()) {
      m_weight_pileup = plEvent.m_info->auxdataConst<float>("PileupWeight");
      m_randomRunNumber = plEvent.m_info->auxdataConst<unsigned int>("RandomRunNumber");
    } else {
      m_weight_pileup = NAN;
      m_randomRunNumber = 0;
    }

    //electrons
    if (m_config->useTruthElectrons()) {
      unsigned int i = 0;

      m_el_pt.resize(plEvent.m_electrons->size());
      m_el_eta.resize(plEvent.m_electrons->size());
      m_el_phi.resize(plEvent.m_electrons->size());
      m_el_e.resize(plEvent.m_electrons->size());
      m_el_charge.resize(plEvent.m_electrons->size());

      m_el_pt_bare.resize(plEvent.m_electrons->size());
      m_el_eta_bare.resize(plEvent.m_electrons->size());
      m_el_phi_bare.resize(plEvent.m_electrons->size());
      m_el_e_bare.resize(plEvent.m_electrons->size());
      
      m_el_true_type.resize(plEvent.m_electrons->size());
      m_el_true_origin.resize(plEvent.m_electrons->size());

      for (const auto& elPtr : *plEvent.m_electrons) {
        m_el_pt[i] = elPtr->pt();
        m_el_eta[i] = elPtr->eta();
        m_el_phi[i] = elPtr->phi();
        m_el_e[i] = elPtr->e();
        m_el_charge[i] = elPtr->charge();

        m_el_pt_bare[i] = elPtr->auxdata<float>("pt_bare");
        m_el_eta_bare[i] = elPtr->auxdata<float>("eta_bare");
        m_el_phi_bare[i] = elPtr->auxdata<float>("phi_bare");
        m_el_e_bare[i] = elPtr->auxdata<float>("e_bare");
        
        if(elPtr->isAvailable<unsigned int>("particleType")) m_el_true_type[i] = elPtr->auxdata<unsigned int>("particleType");
        else if(elPtr->isAvailable<unsigned int>("classifierParticleType")) m_el_true_type[i] = elPtr->auxdata<unsigned int>("classifierParticleType");
        else m_el_true_type[i] = 0;
        
        if(elPtr->isAvailable<unsigned int>("particleOrigin")) m_el_true_origin[i] = elPtr->auxdata<unsigned int>("particleOrigin");
        else if(elPtr->isAvailable<unsigned int>("classifierParticleOrigin")) m_el_true_origin[i] = elPtr->auxdata<unsigned int>("classifierParticleOrigin");
        else m_el_true_origin[i] = 0;

        ++i;
      }
    }

    //muons
    if (m_config->useTruthMuons()) {
      unsigned int i = 0;

      m_mu_pt.resize(plEvent.m_muons->size());
      m_mu_eta.resize(plEvent.m_muons->size());
      m_mu_phi.resize(plEvent.m_muons->size());
      m_mu_e.resize(plEvent.m_muons->size());
      m_mu_charge.resize(plEvent.m_muons->size());

      m_mu_pt_bare.resize(plEvent.m_muons->size());
      m_mu_eta_bare.resize(plEvent.m_muons->size());
      m_mu_phi_bare.resize(plEvent.m_muons->size());
      m_mu_e_bare.resize(plEvent.m_muons->size());
      
      m_mu_true_type.resize(plEvent.m_muons->size());
      m_mu_true_origin.resize(plEvent.m_muons->size());

      for (const auto& muPtr : *plEvent.m_muons) {
        m_mu_pt[i] = muPtr->pt();
        m_mu_eta[i] = muPtr->eta();
        m_mu_phi[i] = muPtr->phi();
        m_mu_e[i] = muPtr->e();
        m_mu_charge[i] = muPtr->charge();

        m_mu_pt_bare[i] = muPtr->auxdata<float>("pt_bare");
        m_mu_eta_bare[i] = muPtr->auxdata<float>("eta_bare");
        m_mu_phi_bare[i] = muPtr->auxdata<float>("phi_bare");
        m_mu_e_bare[i] = muPtr->auxdata<float>("e_bare");
        
        if(muPtr->isAvailable<unsigned int>("particleType")) m_mu_true_type[i] = muPtr->auxdata<unsigned int>("particleType");
        else if(muPtr->isAvailable<unsigned int>("classifierParticleType")) m_mu_true_type[i] = muPtr->auxdata<unsigned int>("classifierParticleType");
        else m_mu_true_type[i] = 0;
        
        if(muPtr->isAvailable<unsigned int>("particleOrigin")) m_mu_true_origin[i] = muPtr->auxdata<unsigned int>("particleOrigin");
        else if(muPtr->isAvailable<unsigned int>("classifierParticleOrigin")) m_mu_true_origin[i] = muPtr->auxdata<unsigned int>("classifierParticleOrigin");
        else m_mu_true_origin[i] = 0;

        ++i;
      }
      
      i=0;
      if(m_config->useSoftMuons()) {
        
        m_softmu_pt.resize(plEvent.m_softmuons->size());
        m_softmu_eta.resize(plEvent.m_softmuons->size());
        m_softmu_phi.resize(plEvent.m_softmuons->size());
        m_softmu_e.resize(plEvent.m_softmuons->size());
        m_softmu_charge.resize(plEvent.m_softmuons->size());
        
        m_softmu_true_type.resize(plEvent.m_softmuons->size());
        m_softmu_true_origin.resize(plEvent.m_softmuons->size());
        
        if(m_config->softmuonAdditionalTruthInfo())
        {
          m_softmu_parton_origin_flag.resize(plEvent.m_softmuons->size());
          m_softmu_particle_origin_flag.resize(plEvent.m_softmuons->size());
          m_softmu_parent_pdgid.resize(plEvent.m_softmuons->size());
          m_softmu_b_hadron_parent_pdgid.resize(plEvent.m_softmuons->size());
          m_softmu_c_hadron_parent_pdgid.resize(plEvent.m_softmuons->size());
        }
        
        for (const auto& muPtr : *plEvent.m_softmuons) {
          m_softmu_pt[i] = muPtr->pt();
          m_softmu_eta[i] = muPtr->eta();
          m_softmu_phi[i] = muPtr->phi();
          m_softmu_e[i] = muPtr->e();
          m_softmu_charge[i] = muPtr->charge();
          
          if(muPtr->isAvailable<unsigned int>("particleType")) m_softmu_true_type[i] = muPtr->auxdata<unsigned int>("particleType");
          else if(muPtr->isAvailable<unsigned int>("classifierParticleType")) m_softmu_true_type[i] = muPtr->auxdata<unsigned int>("classifierParticleType");
          else m_softmu_true_type[i] = 0;
          
          if(muPtr->isAvailable<unsigned int>("particleOrigin")) m_softmu_true_origin[i] = muPtr->auxdata<unsigned int>("particleOrigin");
          else if(muPtr->isAvailable<unsigned int>("classifierParticleOrigin")) m_softmu_true_origin[i] = muPtr->auxdata<unsigned int>("classifierParticleOrigin");
          else m_softmu_true_origin[i] = 0;
          
          if(m_config->softmuonAdditionalTruthInfo())
          {
            
            m_softmu_parton_origin_flag[i]=0;
            if(m_config->softmuonAdditionalTruthInfoCheckPartonOrigin())
            {
              static const SG::AuxElement::Accessor<top::LepPartonOriginFlag> leppartonoriginflag("LepPartonOriginFlag");
              if(leppartonoriginflag.isAvailable(*muPtr)) m_softmu_parton_origin_flag[i]=static_cast<int>(leppartonoriginflag(*muPtr));
            }
            m_softmu_particle_origin_flag[i]=0;
            m_softmu_parent_pdgid[i]=0;
            m_softmu_b_hadron_parent_pdgid[i]=0;
            m_softmu_c_hadron_parent_pdgid[i]=0;
            static const SG::AuxElement::Accessor<top::LepParticleOriginFlag> lepparticleoriginflag("LepParticleOriginFlag");
            if(lepparticleoriginflag.isAvailable(*muPtr)) m_softmu_particle_origin_flag[i]=static_cast<int>(lepparticleoriginflag(*muPtr));
            
            static const SG::AuxElement::Accessor<const xAOD::TruthParticle*> Mother("truthMotherLink");
            const xAOD::TruthParticle* mother = 0;
            if(Mother.isAvailable(*muPtr)) mother=Mother(*muPtr);
            if(mother) m_softmu_parent_pdgid[i]=mother->pdgId();
            
            static const SG::AuxElement::Accessor<const xAOD::TruthParticle*> BMother("truthBMotherLink");
            const xAOD::TruthParticle* Bmother = 0;
            if(BMother.isAvailable(*muPtr)) Bmother=BMother(*muPtr);
            if(Bmother) m_softmu_b_hadron_parent_pdgid[i]=Bmother->pdgId();
            
            static const SG::AuxElement::Accessor<const xAOD::TruthParticle*> CMother("truthCMotherLink");
            const xAOD::TruthParticle* Cmother = 0;
            if(CMother.isAvailable(*muPtr)) Cmother=CMother(*muPtr);
            if(Cmother) m_softmu_c_hadron_parent_pdgid[i]=Cmother->pdgId();
            
            if(m_config->softmuonAdditionalTruthInfoDoVerbose()) asg::msgUserCode::ATH_MSG_INFO("writing truth soft muon with pt="<<m_softmu_pt[i] <<" parton_origin_flag="<<m_softmu_parton_origin_flag[i]<<" particle_origin_flag="<<m_softmu_particle_origin_flag[i]<<" parent_pdg_id="<<m_softmu_parent_pdgid[i]<<" b_hadron_parent_pdg_id="<<m_softmu_b_hadron_parent_pdgid[i]<<" c_hadron_parent_pdg_id="<<m_softmu_c_hadron_parent_pdgid[i]);
          }
          
          ++i;
        } 
      }//end of soft muons part
    }//end of muons part

    //photons
    if (m_config->useTruthPhotons()) {
      unsigned int i(0);
      m_ph_pt.resize(plEvent.m_photons->size());
      m_ph_eta.resize(plEvent.m_photons->size());
      m_ph_phi.resize(plEvent.m_photons->size());
      m_ph_e.resize(plEvent.m_photons->size());
      for (const auto* const phPtr : *plEvent.m_photons) {
        m_ph_pt[i] = phPtr->pt();
        m_ph_eta[i] = phPtr->eta();
        m_ph_phi[i] = phPtr->phi();
        m_ph_e[i] = phPtr->e();

        ++i;
      }
    }

    static const SG::AuxElement::ConstAccessor<int> ghostBHadronsFinalCount("GhostBHadronsFinalCount");
    static const SG::AuxElement::ConstAccessor<int> ghostCHadronsFinalCount("GhostCHadronsFinalCount");

    //jets
    if (m_config->useTruthJets()) {
      unsigned int i = 0;

      m_jet_pt.resize(plEvent.m_jets->size());
      m_jet_eta.resize(plEvent.m_jets->size());
      m_jet_phi.resize(plEvent.m_jets->size());
      m_jet_e.resize(plEvent.m_jets->size());
      m_jet_Ghosts_BHadron_Final_Count.resize(plEvent.m_jets->size());
      m_jet_Ghosts_CHadron_Final_Count.resize(plEvent.m_jets->size());
      for (const auto& jetPtr : *plEvent.m_jets) {
        m_jet_pt[i] = jetPtr->pt();
        m_jet_eta[i] = jetPtr->eta();
        m_jet_phi[i] = jetPtr->phi();
        m_jet_e[i] = jetPtr->e();

	if(ghostBHadronsFinalCount.isAvailable(*jetPtr)) {
	  m_jet_Ghosts_BHadron_Final_Count[i] = ghostBHadronsFinalCount(*jetPtr);
	}
	else {
	  ATH_MSG_DEBUG("Found a jet with no GhostBHadronFinalCount auxdata");
	  m_jet_Ghosts_BHadron_Final_Count[i] = 0;
	}
	
	if(ghostCHadronsFinalCount.isAvailable(*jetPtr)) {
	  m_jet_Ghosts_CHadron_Final_Count[i] = ghostCHadronsFinalCount(*jetPtr);
	}
	else {
	  ATH_MSG_DEBUG("Found a jet with no GhostCHadronFinalCount auxdata");
	  m_jet_Ghosts_CHadron_Final_Count[i] = 0;
	}

        ++i;
      }
    }

    //large R jets
    if (m_config->useTruthLargeRJets()) {
      unsigned int i = 0;

      m_ljet_pt.resize(plEvent.m_largeRJets->size());
      m_ljet_eta.resize(plEvent.m_largeRJets->size());
      m_ljet_phi.resize(plEvent.m_largeRJets->size());
      m_ljet_e.resize(plEvent.m_largeRJets->size());
      m_ljet_Ghosts_BHadron_Final_Count.resize(plEvent.m_largeRJets->size());
      m_ljet_Ghosts_CHadron_Final_Count.resize(plEvent.m_largeRJets->size());
      for (const auto& jetPtr : *plEvent.m_largeRJets) {
        m_ljet_pt[i] = jetPtr->pt();
        m_ljet_eta[i] = jetPtr->eta();
        m_ljet_phi[i] = jetPtr->phi();
        m_ljet_e[i] = jetPtr->e();
	
	if(ghostBHadronsFinalCount.isAvailable(*jetPtr)) {
	  m_ljet_Ghosts_BHadron_Final_Count[i] = ghostBHadronsFinalCount(*jetPtr);
	}
	else {
	  ATH_MSG_DEBUG("Found a jet with no GhostBHadronFinalCount auxdata");
	  m_ljet_Ghosts_BHadron_Final_Count[i] = 0;
	}
	
	if(ghostCHadronsFinalCount.isAvailable(*jetPtr)) {
	  m_ljet_Ghosts_CHadron_Final_Count[i] = ghostCHadronsFinalCount(*jetPtr);
	}
	else {
	  ATH_MSG_DEBUG("Found a jet with no GhostCHadronFinalCount auxdata");
	  m_ljet_Ghosts_CHadron_Final_Count[i] = 0;
	}

        ++i;
      }
    }

    //Taus
    if (m_config->useTruthTaus()) {
      unsigned int i = 0;

      m_tau_pt.resize(plEvent.m_taus->size());
      m_tau_eta.resize(plEvent.m_taus->size());
      m_tau_phi.resize(plEvent.m_taus->size());
      m_tau_e.resize(plEvent.m_taus->size());
      m_tau_charge.resize(plEvent.m_taus->size());
      m_tau_isHadronic.resize(plEvent.m_taus->size());

      for (const auto& tauPtr : *plEvent.m_taus) {
        m_tau_pt[i] = tauPtr->pt();
        m_tau_eta[i] = tauPtr->eta();
        m_tau_phi[i] = tauPtr->phi();
        m_tau_e[i] = tauPtr->e();
        m_tau_charge[i] = tauPtr->charge();
        if (tauPtr->isAvailable<char>("IsHadronicTau")) {
          m_tau_isHadronic[i] = tauPtr->auxdata<char>("IsHadronicTau");
        }

        ++i;
      }
    }

    if (m_makeRCJets) {
      // re-clustered jet substructure
      static const SG::AuxElement::ConstAccessor<float> RCSplit12("Split12");
      static const SG::AuxElement::ConstAccessor<float> RCSplit23("Split23");

      // re-clustered jet substructure from clusters
      static const SG::AuxElement::ConstAccessor<float> Tau21_clstr("Tau21_clstr");
      static const SG::AuxElement::ConstAccessor<float> Tau32_clstr("Tau32_clstr");
      static const SG::AuxElement::ConstAccessor<float> Tau3_clstr("Tau3_clstr");
      static const SG::AuxElement::ConstAccessor<float> Tau2_clstr("Tau2_clstr");
      static const SG::AuxElement::ConstAccessor<float> Tau1_clstr("Tau1_clstr");
      static const SG::AuxElement::ConstAccessor<float> D2_clstr("D2_clstr");
      static const SG::AuxElement::ConstAccessor<float> ECF1_clstr("ECF1_clstr");
      static const SG::AuxElement::ConstAccessor<float> ECF2_clstr("ECF2_clstr");
      static const SG::AuxElement::ConstAccessor<float> ECF3_clstr("ECF3_clstr");
      static const SG::AuxElement::ConstAccessor<float> d12_clstr("d12_clstr");
      static const SG::AuxElement::ConstAccessor<float> d23_clstr("d23_clstr");
      static const SG::AuxElement::ConstAccessor<float> Qw_clstr("Qw_clstr");
      static const SG::AuxElement::ConstAccessor<float> nconstituent_clstr("nconstituent_clstr");

      static const SG::AuxElement::ConstAccessor<float> gECF332_clstr("gECF332_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF461_clstr("gECF461_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF322_clstr("gECF322_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF331_clstr("gECF331_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF422_clstr("gECF422_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF441_clstr("gECF441_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF212_clstr("gECF212_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF321_clstr("gECF321_clstr");
      static const SG::AuxElement::ConstAccessor<float> gECF311_clstr("gECF311_clstr");
      static const SG::AuxElement::ConstAccessor<float> L1_clstr("L1_clstr");
      static const SG::AuxElement::ConstAccessor<float> L2_clstr("L2_clstr");
      static const SG::AuxElement::ConstAccessor<float> L3_clstr("L3_clstr");
      static const SG::AuxElement::ConstAccessor<float> L4_clstr("L4_clstr");
      static const SG::AuxElement::ConstAccessor<float> L5_clstr("L5_clstr");
      // store also the jet that is rebuilt to calculate the JSS
      static const SG::AuxElement::ConstAccessor<float> RRCJet_pt("RRCJet_pt");
      static const SG::AuxElement::ConstAccessor<float> RRCJet_eta("RRCJet_eta");
      static const SG::AuxElement::ConstAccessor<float> RRCJet_phi("RRCJet_phi");
      static const SG::AuxElement::ConstAccessor<float> RRCJet_e("RRCJet_e");


      // Initialize the vectors to be saved as branches
      unsigned int sizeOfRCjets(plEvent.m_RCJets.size());

      m_rcjet_pt.clear();
      m_rcjet_eta.clear();
      m_rcjet_phi.clear();
      m_rcjet_e.clear();
      m_rcjet_d12.clear();
      m_rcjet_d23.clear();
      m_rcjetsub_pt.clear();
      m_rcjetsub_eta.clear();
      m_rcjetsub_phi.clear();
      m_rcjetsub_e.clear();
      m_rcjetsub_Ghosts_BHadron_Final_Count.clear();
      m_rcjetsub_Ghosts_CHadron_Final_Count.clear();


      m_rcjet_tau32_clstr.clear();
      m_rcjet_tau21_clstr.clear();
      m_rcjet_tau3_clstr.clear();
      m_rcjet_tau2_clstr.clear();
      m_rcjet_tau1_clstr.clear();
      m_rcjet_D2_clstr.clear();
      m_rcjet_ECF1_clstr.clear();
      m_rcjet_ECF2_clstr.clear();
      m_rcjet_ECF3_clstr.clear();
      m_rcjet_d12_clstr.clear();
      m_rcjet_d23_clstr.clear();
      m_rcjet_Qw_clstr.clear();
      m_rcjet_nconstituent_clstr.clear();

      m_rcjet_gECF332_clstr.clear();
      m_rcjet_gECF461_clstr.clear();
      m_rcjet_gECF322_clstr.clear();
      m_rcjet_gECF331_clstr.clear();
      m_rcjet_gECF422_clstr.clear();
      m_rcjet_gECF441_clstr.clear();
      m_rcjet_gECF212_clstr.clear();
      m_rcjet_gECF321_clstr.clear();
      m_rcjet_gECF311_clstr.clear();
      m_rcjet_L1_clstr.clear();
      m_rcjet_L2_clstr.clear();
      m_rcjet_L3_clstr.clear();
      m_rcjet_L4_clstr.clear();
      m_rcjet_L5_clstr.clear();

      m_rcjet_pt.resize(sizeOfRCjets, -999.);
      m_rcjet_eta.resize(sizeOfRCjets, -999.);
      m_rcjet_phi.resize(sizeOfRCjets, -999.);
      m_rcjet_e.resize(sizeOfRCjets, -999.);
      m_rcjet_d12.resize(sizeOfRCjets, -999.);
      m_rcjet_d23.resize(sizeOfRCjets, -999.);
      m_rcjetsub_pt.resize(sizeOfRCjets, std::vector<float>());
      m_rcjetsub_eta.resize(sizeOfRCjets, std::vector<float>());
      m_rcjetsub_phi.resize(sizeOfRCjets, std::vector<float>());
      m_rcjetsub_e.resize(sizeOfRCjets, std::vector<float>());
      m_rcjetsub_Ghosts_BHadron_Final_Count.resize(sizeOfRCjets, std::vector<int>());
      m_rcjetsub_Ghosts_CHadron_Final_Count.resize(sizeOfRCjets, std::vector<int>());

      if (m_useRCJSS || m_useRCAdditionalJSS) {
        m_rrcjet_pt.resize(sizeOfRCjets, -999.);
        m_rrcjet_eta.resize(sizeOfRCjets, -999.);
        m_rrcjet_phi.resize(sizeOfRCjets, -999.);
        m_rrcjet_e.resize(sizeOfRCjets, -999.);
      }
      if (m_useRCJSS) {
        m_rcjet_tau32_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_tau21_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_tau1_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_tau2_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_tau3_clstr.resize(sizeOfRCjets, -999.);

        m_rcjet_d12_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_d23_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_Qw_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_nconstituent_clstr.resize(sizeOfRCjets, -999.);
	
	m_rcjet_D2_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_ECF1_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_ECF2_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_ECF3_clstr.resize(sizeOfRCjets, -999.);
      }
      if (m_useRCAdditionalJSS) {
        m_rcjet_gECF332_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF461_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF322_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF331_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF422_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF441_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF212_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF321_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_gECF311_clstr.resize(sizeOfRCjets, -999.);

        m_rcjet_L1_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_L2_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_L3_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_L4_clstr.resize(sizeOfRCjets, -999.);
        m_rcjet_L5_clstr.resize(sizeOfRCjets, -999.);
      }
      unsigned int i = 0;
      for (auto rc_jet : plEvent.m_RCJets) {
        m_rcjet_pt[i] = rc_jet->pt();
        m_rcjet_eta[i] = rc_jet->eta();
        m_rcjet_phi[i] = rc_jet->phi();
        m_rcjet_e[i] = rc_jet->e();

        m_rcjet_d12[i] = (RCSplit12.isAvailable(*rc_jet)) ? RCSplit12(*rc_jet) : -999.;
        m_rcjet_d23[i] = (RCSplit23.isAvailable(*rc_jet)) ? RCSplit23(*rc_jet) : -999.;

        if (m_useRCJSS || m_useRCAdditionalJSS) {
          m_rrcjet_pt[i] = (RRCJet_pt.isAvailable(*rc_jet))  ? RRCJet_pt(*rc_jet) : -999.;
          m_rrcjet_eta[i] = (RRCJet_eta.isAvailable(*rc_jet)) ? RRCJet_eta(*rc_jet) : -999.;
          m_rrcjet_phi[i] = (RRCJet_phi.isAvailable(*rc_jet)) ? RRCJet_phi(*rc_jet) : -999.;
          m_rrcjet_e[i] = (RRCJet_e.isAvailable(*rc_jet))   ? RRCJet_e(*rc_jet) : -999.;
        }
        if (m_useRCJSS) {
          m_rcjet_tau32_clstr[i] = (Tau32_clstr.isAvailable(*rc_jet)) ? Tau32_clstr(*rc_jet) : -999.;
          m_rcjet_tau21_clstr[i] = (Tau21_clstr.isAvailable(*rc_jet)) ? Tau21_clstr(*rc_jet) : -999.;
          m_rcjet_tau3_clstr[i] = (Tau3_clstr.isAvailable(*rc_jet)) ? Tau3_clstr(*rc_jet) : -999.;
          m_rcjet_tau2_clstr[i] = (Tau2_clstr.isAvailable(*rc_jet)) ? Tau2_clstr(*rc_jet) : -999.;
          m_rcjet_tau1_clstr[i] = (Tau1_clstr.isAvailable(*rc_jet)) ? Tau1_clstr(*rc_jet) : -999.;

          m_rcjet_d12_clstr[i] = (d12_clstr.isAvailable(*rc_jet)) ? d12_clstr(*rc_jet) : -999.;
          m_rcjet_d23_clstr[i] = (d23_clstr.isAvailable(*rc_jet)) ? d23_clstr(*rc_jet) : -999.;
          m_rcjet_Qw_clstr[i] = (Qw_clstr.isAvailable(*rc_jet)) ? Qw_clstr(*rc_jet) : -999.;
          m_rcjet_nconstituent_clstr[i] = (nconstituent_clstr.isAvailable(*rc_jet)) ? nconstituent_clstr(*rc_jet) : -999.;
	  
	  m_rcjet_D2_clstr[i] = (D2_clstr.isAvailable(*rc_jet)) ? D2_clstr(*rc_jet) : -999.;
          m_rcjet_ECF1_clstr[i] = (ECF1_clstr.isAvailable(*rc_jet)) ? ECF1_clstr(*rc_jet) : -999.;
          m_rcjet_ECF2_clstr[i] = (ECF2_clstr.isAvailable(*rc_jet)) ? ECF2_clstr(*rc_jet) : -999.;
          m_rcjet_ECF3_clstr[i] = (ECF3_clstr.isAvailable(*rc_jet)) ? ECF3_clstr(*rc_jet) : -999.;
        }
        if (m_useRCAdditionalJSS) {
          m_rcjet_gECF332_clstr[i] = (gECF332_clstr.isAvailable(*rc_jet)) ? gECF332_clstr(*rc_jet) : -999.;
          m_rcjet_gECF461_clstr[i] = (gECF461_clstr.isAvailable(*rc_jet)) ? gECF461_clstr(*rc_jet) : -999.;
          m_rcjet_gECF322_clstr[i] = (gECF322_clstr.isAvailable(*rc_jet)) ? gECF322_clstr(*rc_jet) : -999.;
          m_rcjet_gECF331_clstr[i] = (gECF331_clstr.isAvailable(*rc_jet)) ? gECF331_clstr(*rc_jet) : -999.;
          m_rcjet_gECF422_clstr[i] = (gECF422_clstr.isAvailable(*rc_jet)) ? gECF422_clstr(*rc_jet) : -999.;
          m_rcjet_gECF441_clstr[i] = (gECF441_clstr.isAvailable(*rc_jet)) ? gECF441_clstr(*rc_jet) : -999.;
          m_rcjet_gECF212_clstr[i] = (gECF212_clstr.isAvailable(*rc_jet)) ? gECF212_clstr(*rc_jet) : -999.;
          m_rcjet_gECF321_clstr[i] = (gECF321_clstr.isAvailable(*rc_jet)) ? gECF321_clstr(*rc_jet) : -999.;
          m_rcjet_gECF311_clstr[i] = (gECF311_clstr.isAvailable(*rc_jet)) ? gECF311_clstr(*rc_jet) : -999.;

          m_rcjet_L1_clstr[i] = (L1_clstr.isAvailable(*rc_jet)) ? L1_clstr(*rc_jet) : -999.;
          m_rcjet_L2_clstr[i] = (L2_clstr.isAvailable(*rc_jet)) ? L2_clstr(*rc_jet) : -999.;
          m_rcjet_L3_clstr[i] = (L3_clstr.isAvailable(*rc_jet)) ? L3_clstr(*rc_jet) : -999.;
          m_rcjet_L4_clstr[i] = (L4_clstr.isAvailable(*rc_jet)) ? L4_clstr(*rc_jet) : -999.;
          m_rcjet_L5_clstr[i] = (L5_clstr.isAvailable(*rc_jet)) ? L5_clstr(*rc_jet) : -999.;
        }

        // loop over subjets
        m_rcjetsub_pt[i].clear();     // clear the vector size (otherwise it grows out of control!)
        m_rcjetsub_eta[i].clear();
        m_rcjetsub_phi[i].clear();
        m_rcjetsub_e[i].clear();

        const xAOD::Jet* subjet(nullptr);
        for (auto rc_jet_subjet : rc_jet->getConstituents()) {
          subjet = static_cast<const xAOD::Jet*>(rc_jet_subjet->rawConstituent());
          m_rcjetsub_pt[i].push_back(subjet->pt());
          m_rcjetsub_eta[i].push_back(subjet->eta());
          m_rcjetsub_phi[i].push_back(subjet->phi());
          m_rcjetsub_e[i].push_back(subjet->e());
	  
	  if(ghostBHadronsFinalCount.isAvailable(*subjet)) {
	    m_rcjetsub_Ghosts_BHadron_Final_Count[i].push_back(ghostBHadronsFinalCount(*subjet));
	  }
	  else {
	    ATH_MSG_DEBUG("Found a jet with no GhostBHadronFinalCount auxdata");
	    m_rcjetsub_Ghosts_BHadron_Final_Count[i].push_back(0);
	  }
	  
	  if(ghostCHadronsFinalCount.isAvailable(*subjet)) {
	    m_rcjetsub_Ghosts_CHadron_Final_Count[i].push_back(ghostCHadronsFinalCount(*subjet));
	  }
	  else {
	    ATH_MSG_DEBUG("Found a jet with no GhostCHadronFinalCount auxdata");
	    m_rcjetsub_Ghosts_CHadron_Final_Count[i].push_back(0);
	  }
	  
        } // end for-loop over subjets
        ++i;
      } // end for-loop over re-clustered jets
    }

    // VarRC jets
    if (m_makeVarRCJets) {
      std::string VarRC = "vrcjet";
      for (auto& rho : m_VarRCJetRho) {
        for (auto& mass_scale : m_VarRCJetMassScale) {
          std::replace(rho.begin(), rho.end(), '.', '_');
          std::string name = rho + mass_scale;

          // re-clustered jet substructure
          static const SG::AuxElement::ConstAccessor<float> VarRCSplit12("Split12");
          static const SG::AuxElement::ConstAccessor<float> VarRCSplit23("Split23");
          // re-clustered jet substructure from clusters
          static const SG::AuxElement::ConstAccessor<float> Tau21_clstr("Tau21_clstr");
          static const SG::AuxElement::ConstAccessor<float> Tau32_clstr("Tau32_clstr");
          static const SG::AuxElement::ConstAccessor<float> Tau3_clstr("Tau3_clstr");
          static const SG::AuxElement::ConstAccessor<float> Tau2_clstr("Tau2_clstr");
          static const SG::AuxElement::ConstAccessor<float> Tau1_clstr("Tau1_clstr");
          static const SG::AuxElement::ConstAccessor<float> D2_clstr("D2_clstr");
          static const SG::AuxElement::ConstAccessor<float> ECF1_clstr("ECF1_clstr");
          static const SG::AuxElement::ConstAccessor<float> ECF2_clstr("ECF2_clstr");
          static const SG::AuxElement::ConstAccessor<float> ECF3_clstr("ECF3_clstr");
          static const SG::AuxElement::ConstAccessor<float> d12_clstr("d12_clstr");
          static const SG::AuxElement::ConstAccessor<float> d23_clstr("d23_clstr");
          static const SG::AuxElement::ConstAccessor<float> Qw_clstr("Qw_clstr");

          static const SG::AuxElement::ConstAccessor<float> gECF332_clstr("gECF332_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF461_clstr("gECF461_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF322_clstr("gECF322_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF331_clstr("gECF331_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF422_clstr("gECF422_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF441_clstr("gECF441_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF212_clstr("gECF212_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF321_clstr("gECF321_clstr");
          static const SG::AuxElement::ConstAccessor<float> gECF311_clstr("gECF311_clstr");
          static const SG::AuxElement::ConstAccessor<float> L1_clstr("L1_clstr");
          static const SG::AuxElement::ConstAccessor<float> L2_clstr("L2_clstr");
          static const SG::AuxElement::ConstAccessor<float> L3_clstr("L3_clstr");
          static const SG::AuxElement::ConstAccessor<float> L4_clstr("L4_clstr");
          static const SG::AuxElement::ConstAccessor<float> L5_clstr("L5_clstr");
          // store also the jet that is rebuilt to calculate the JSS
          static const SG::AuxElement::ConstAccessor<float> RRCJet_pt("RRCJet_pt");
          static const SG::AuxElement::ConstAccessor<float> RRCJet_eta("RRCJet_eta");
          static const SG::AuxElement::ConstAccessor<float> RRCJet_phi("RRCJet_phi");
          static const SG::AuxElement::ConstAccessor<float> RRCJet_e("RRCJet_e");

          // Initialize the vectors to be saved as branches

          xAOD::JetContainer* vrc_jets = plEvent.m_VarRCJets[name].get();
          unsigned int sizeOfRCjets = vrc_jets->size();
          m_VarRCjetBranchesParticle[VarRC + "_" + name + "_pt"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranchesParticle[VarRC + "_" + name + "_eta"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranchesParticle[VarRC + "_" + name + "_phi"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranchesParticle[VarRC + "_" + name + "_e"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d12"].resize(sizeOfRCjets, -999.);
          m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d23"].resize(sizeOfRCjets, -999.);
          m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_pt"].resize(sizeOfRCjets, std::vector<float>());
          m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_eta"].resize(sizeOfRCjets, std::vector<float>());
          m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_phi"].resize(sizeOfRCjets, std::vector<float>());
          m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_e"].resize(sizeOfRCjets, std::vector<float>());
          if (m_useVarRCJSS || m_useVarRCAdditionalJSS) {
            m_VarRCjetBranchesParticle["vrrcjet_" + name + "_pt"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle["vrrcjet_" + name + "_eta"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle["vrrcjet_" + name + "_phi"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle["vrrcjet_" + name + "_e"].resize(sizeOfRCjets, -999.);
          }
          if (m_useVarRCJSS) {
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau32_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau21_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau1_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau2_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau3_clstr"].resize(sizeOfRCjets, -999.);

            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d12_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d23_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_Qw_clstr"].resize(sizeOfRCjets, -999.);
	    
	    m_VarRCjetBranchesParticle[VarRC + "_" + name + "_D2_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_ECF1_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_ECF2_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_ECF3_clstr"].resize(sizeOfRCjets, -999.);
          }
          if (m_useVarRCAdditionalJSS) {
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF332_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF461_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF322_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF331_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF422_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF441_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF212_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF321_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF311_clstr"].resize(sizeOfRCjets, -999.);

            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L1_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L2_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L3_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L4_clstr"].resize(sizeOfRCjets, -999.);
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L5_clstr"].resize(sizeOfRCjets, -999.);
          }

          unsigned int i = 0;

          for (auto jet_ptr : *vrc_jets) {
            const xAOD::Jet* rc_jet = jet_ptr;

            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_pt"][i] = rc_jet->pt();
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_eta"][i] = rc_jet->eta();
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_phi"][i] = rc_jet->phi();
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_e"][i] = rc_jet->e();

            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d12"][i] = (VarRCSplit12.isAvailable(*rc_jet)) ? VarRCSplit12(*rc_jet) : -999.;
            m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d23"][i] = (VarRCSplit23.isAvailable(*rc_jet)) ? VarRCSplit23(*rc_jet) : -999.;

            if (m_useVarRCJSS || m_useVarRCAdditionalJSS) {
              m_VarRCjetBranchesParticle["vrrcjet_" + name + "_pt"][i] = (RRCJet_pt.isAvailable(*rc_jet))  ? RRCJet_pt(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle["vrrcjet_" + name + "_eta"][i] = (RRCJet_eta.isAvailable(*rc_jet)) ? RRCJet_eta(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle["vrrcjet_" + name + "_phi"][i] = (RRCJet_phi.isAvailable(*rc_jet)) ? RRCJet_phi(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle["vrrcjet_" + name + "_e"][i] = (RRCJet_e.isAvailable(*rc_jet))   ? RRCJet_e(*rc_jet) : -999.;
            }
            if (m_useVarRCJSS) {
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau32_clstr"][i] = (Tau32_clstr.isAvailable(*rc_jet)) ? Tau32_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau21_clstr"][i] = (Tau21_clstr.isAvailable(*rc_jet)) ? Tau21_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau3_clstr"][i] = (Tau3_clstr.isAvailable(*rc_jet)) ? Tau3_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau2_clstr"][i] = (Tau2_clstr.isAvailable(*rc_jet)) ? Tau2_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_tau1_clstr"][i] = (Tau1_clstr.isAvailable(*rc_jet)) ? Tau1_clstr(*rc_jet) : -999.;

              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d12_clstr"][i] = (d12_clstr.isAvailable(*rc_jet)) ? d12_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_d23_clstr"][i] = (d23_clstr.isAvailable(*rc_jet)) ? d23_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_Qw_clstr"][i] = (Qw_clstr.isAvailable(*rc_jet)) ? Qw_clstr(*rc_jet) : -999.;
	      
	      m_VarRCjetBranchesParticle[VarRC + "_" + name + "_D2_clstr"][i] = (D2_clstr.isAvailable(*rc_jet)) ? D2_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_ECF1_clstr"][i] = (ECF1_clstr.isAvailable(*rc_jet)) ? ECF1_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_ECF2_clstr"][i] = (ECF2_clstr.isAvailable(*rc_jet)) ? ECF2_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_ECF3_clstr"][i] = (ECF3_clstr.isAvailable(*rc_jet)) ? ECF3_clstr(*rc_jet) : -999.;
            }
            if (m_useVarRCAdditionalJSS) {
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF332_clstr"][i] = (gECF332_clstr.isAvailable(*rc_jet)) ? gECF332_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF461_clstr"][i] = (gECF461_clstr.isAvailable(*rc_jet)) ? gECF461_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF322_clstr"][i] = (gECF322_clstr.isAvailable(*rc_jet)) ? gECF322_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF331_clstr"][i] = (gECF331_clstr.isAvailable(*rc_jet)) ? gECF331_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF422_clstr"][i] = (gECF422_clstr.isAvailable(*rc_jet)) ? gECF422_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF441_clstr"][i] = (gECF441_clstr.isAvailable(*rc_jet)) ? gECF441_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF212_clstr"][i] = (gECF212_clstr.isAvailable(*rc_jet)) ? gECF212_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF321_clstr"][i] = (gECF321_clstr.isAvailable(*rc_jet)) ? gECF321_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_gECF311_clstr"][i] = (gECF311_clstr.isAvailable(*rc_jet)) ? gECF311_clstr(*rc_jet) : -999.;

              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L1_clstr"][i] = (L1_clstr.isAvailable(*rc_jet)) ? L1_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L2_clstr"][i] = (L2_clstr.isAvailable(*rc_jet)) ? L2_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L3_clstr"][i] = (L3_clstr.isAvailable(*rc_jet)) ? L3_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L4_clstr"][i] = (L4_clstr.isAvailable(*rc_jet)) ? L4_clstr(*rc_jet) : -999.;
              m_VarRCjetBranchesParticle[VarRC + "_" + name + "_L5_clstr"][i] = (L5_clstr.isAvailable(*rc_jet)) ? L5_clstr(*rc_jet) : -999.;
            }

            // loop over subjets
            const xAOD::Jet* subjet(nullptr);
            m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_pt"][i].clear();     // clear the vector size
                                                                                          // (otherwise it grows out of
                                                                                          // control!)
            m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_eta"][i].clear();
            m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_phi"][i].clear();
            m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_e"][i].clear();
            for (auto rc_jet_subjet : rc_jet->getConstituents()) {
              subjet = static_cast<const xAOD::Jet*>(rc_jet_subjet->rawConstituent());

              m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_pt"][i].push_back(subjet->pt());
              m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_eta"][i].push_back(subjet->eta());
              m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_phi"][i].push_back(subjet->phi());
              m_VarRCjetsubBranchesParticle[VarRC + "_" + name + "_sub_e"][i].push_back(subjet->e());
            } // end for-loop over subjets
            ++i;
          } // end for-loop over re-clustered jets
        } // end loop over mass parameters
      } // end loop over multipliers for mass scale
    } // end if make VarRC jets
      // end VarRC jets



    //met
    if (m_config->useTruthMET()) {
      m_met_met = plEvent.m_met->met();
      m_met_phi = plEvent.m_met->phi();
    }

    if (m_config->doPseudoTop()) {
      const xAOD::PseudoTopResultContainer* pseudoTopResultContainer(nullptr);
      const xAOD::PseudoTopResult* pseudoTopResult(nullptr);

      if (evtStore()->contains<xAOD::PseudoTopResultContainer>(m_config->sgKeyPseudoTop(0))) {
        top::check(evtStore()->retrieve(pseudoTopResultContainer, m_config->sgKeyPseudoTop(0)), "Failed to retrieve PseudoTop");

        pseudoTopResult = pseudoTopResultContainer->at(0);

        m_PseudoTop_Particle_ttbar_pt = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_ttbar_pt");
        m_PseudoTop_Particle_ttbar_eta = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_ttbar_eta");
        m_PseudoTop_Particle_ttbar_phi = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_ttbar_phi");
        m_PseudoTop_Particle_ttbar_m = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_ttbar_m");

        m_PseudoTop_Particle_top_lep_pt = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_top_lep_pt");
        m_PseudoTop_Particle_top_lep_eta = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_top_lep_eta");
        m_PseudoTop_Particle_top_lep_phi = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_top_lep_phi");
        m_PseudoTop_Particle_top_lep_m = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_top_lep_m");

        m_PseudoTop_Particle_top_had_pt = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_top_had_pt");
        m_PseudoTop_Particle_top_had_eta = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_top_had_eta");
        m_PseudoTop_Particle_top_had_phi = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_top_had_phi");
        m_PseudoTop_Particle_top_had_m = pseudoTopResult->auxdecor<float>("PseudoTop_Particle_top_had_m");
      }
    }
  }

  void EventSaverFlatNtuple::fillParticleLevelEvent() {
    //do it!
    m_particleLevelTreeManager->fill();
  }

  void EventSaverFlatNtuple::saveUpgradeEvent(const top::ParticleLevelEvent& upgradeEvent) {
    // Quick return if upgrade is disabled.
    if (not m_config->HLLHC()) {
      return;
    }
    // No need to attempt to write out anything for non-MC data.
    if (!m_config->isMC()) {
      return;
    }
    this->cleanUpgradeEvent();
    this->calculateUpgradeEvent(upgradeEvent);
    this->fillUpgradeEvent();
  }

  void EventSaverFlatNtuple::cleanUpgradeEvent() {
    /* Doing nothing for now, but we should put here the (re-)initialisation to dummy values
     * of all variables later calculated in calculateParticleLevelEvent(const top::ParticleLevelEvent& plEvent)
     * For example all weights set to 1, all vectors cleaned, all kinematic or angular variables set to -99999.
     */
  }

  void EventSaverFlatNtuple::calculateUpgradeEvent(const top::ParticleLevelEvent& upgradeEvent) {
    for (auto& selectionDecision : m_upgrade_SelectionDecisions) {
      selectionDecision.second = upgradeEvent.m_selectionDecisions[ selectionDecision.first ];
    }

    m_weight_mc = upgradeEvent.m_info->auxdataConst<float>("AnalysisTop_eventWeight");

    m_eventNumber = upgradeEvent.m_info->eventNumber();
    m_runNumber = upgradeEvent.m_info->runNumber();
    m_mcChannelNumber = upgradeEvent.m_info->mcChannelNumber();

    m_mu = upgradeEvent.m_info->averageInteractionsPerCrossing();
    m_mu_actual = upgradeEvent.m_info->actualInteractionsPerCrossing();

    // save electrons
    unsigned int i = 0;
    m_el_pt.resize(upgradeEvent.m_electrons->size());
    m_el_eta.resize(upgradeEvent.m_electrons->size());
    m_el_phi.resize(upgradeEvent.m_electrons->size());
    m_el_e.resize(upgradeEvent.m_electrons->size());
    m_el_etcone20.resize(upgradeEvent.m_electrons->size());
    m_el_ptcone30.resize(upgradeEvent.m_electrons->size());
    m_el_charge.resize(upgradeEvent.m_electrons->size());
    m_el_true_type.resize(upgradeEvent.m_electrons->size());
    m_el_true_origin.resize(upgradeEvent.m_electrons->size());
    //m_el_true_firstEgMotherTruthOrigin.resize(upgradeEvent.m_electrons->size());
    //m_el_true_firstEgMotherTruthType.resize(upgradeEvent.m_electrons->size());
    //m_el_true_isPrompt.resize(upgradeEvent.m_electrons->size());
    if (m_config->HLLHCFakes()) m_el_faketype.resize(upgradeEvent.m_electrons->size());

    for (const auto elPtr : *upgradeEvent.m_electrons) {
      m_el_pt[i] = elPtr->pt();
      m_el_eta[i] = elPtr->eta();
      m_el_phi[i] = elPtr->phi();
      m_el_e[i] = elPtr->e();
      m_el_charge[i] = elPtr->charge();

      m_el_true_type[i] = 0;
      m_el_true_origin[i] = 0;
      //m_el_true_firstEgMotherTruthType[i] = 0;
      //m_el_true_firstEgMotherTruthOrigin[i] = 0;
      //m_el_true_isPrompt[i] = 0;

      if (elPtr->isAvailable<float>("etcone20")) {
        m_el_etcone20[i] = elPtr->auxdata<float>("etcone20");
      } else {
        m_el_etcone20[i] = -999;
      }
      if (elPtr->isAvailable<float>("ptcone30")) {
        m_el_ptcone30[i] = elPtr->auxdata<float>("ptcone30");
      } else {
        m_el_ptcone30[i] = -999;
      }

      if (!m_config->HLLHCFakes()) {
        if (elPtr->isAvailable<unsigned int>("particleType")) {
          m_el_true_type[i] = elPtr->auxdata<unsigned int>("particleType");
        } else if (elPtr->isAvailable<unsigned int>("classifierParticleType")) {
          m_el_true_type[i] = elPtr->auxdata<unsigned int>("classifierParticleType");
        } else {
          top::check(false, "Could not obtain truth Type decoration for electron!");
        }
        if (elPtr->isAvailable<unsigned int>("particleOrigin")) {
          m_el_true_origin[i] = elPtr->auxdata<unsigned int>("particleOrigin");
        } else if (elPtr->isAvailable<unsigned int>("classifierParticleOrigin")) {
          m_el_true_origin[i] = elPtr->auxdata<unsigned int>("classifierParticleOrigin");
        } else {
          top::check(false, "Could not obtain truth Origin decoration for electron!");
        }
        //if (elPtr->isAvailable<unsigned int>("firstEgMotherTruthType")) {
        //    m_el_true_firstEgMotherTruthType[i] = elPtr->auxdata<unsigned int>("firstEgMotherTruthType");
        //} else {
        //    top::check(false, "Could not obtain mother's truth Type decoration for electron!");
        //}
        //if (elPtr->isAvailable<unsigned int>("firstEgMotherTruthOrigin")) {
        //    m_el_true_firstEgMotherTruthOrigin[i] = elPtr->auxdata<unsigned int>("firstEgMotherTruthOrigin");
        //} else {
        //    top::check(false, "Could not obtain mother's truth Origin decoration for electron!");
        //}
        //m_el_true_isPrompt[i] = isPromptElectron(m_el_true_type[i], m_el_true_origin[i],
        // m_el_true_firstEgMotherTruthType[i], m_el_true_firstEgMotherTruthOrigin[i]);
      } else {
        if (elPtr->isAvailable<int>("FakeType")) {
          m_el_faketype[i] = elPtr->auxdata<int>("FakeType");
        } else {
          top::check(false, "Could not obtain FakeType decoration for electron!");
        }
        if (m_el_faketype[i] == 0) { // truthType/Origin only available for true electron (also for electron fake, but
                                     // we are not interested in its truth info)
          if (elPtr->isAvailable<unsigned int>("particleType")) {
            m_el_true_type[i] = elPtr->auxdata<unsigned int>("particleType");
          } else if (elPtr->isAvailable<unsigned int>("classifierParticleType")) {
            m_el_true_type[i] = elPtr->auxdata<unsigned int>("classifierParticleType");
          } else {
            top::check(false, "Could not obtain truth Type decoration for electron!");
          }
          if (elPtr->isAvailable<unsigned int>("particleOrigin")) {
            m_el_true_origin[i] = elPtr->auxdata<unsigned int>("particleOrigin");
          } else if (elPtr->isAvailable<unsigned int>("classifierParticleOrigin")) {
            m_el_true_origin[i] = elPtr->auxdata<unsigned int>("classifierParticleOrigin");
          } else {
            top::check(false, "Could not obtain truth Origin decoration for electron!");
          }
          //if (elPtr->isAvailable<unsigned int>("firstEgMotherTruthType")) {
          //    m_el_true_firstEgMotherTruthType[i] = elPtr->auxdata<unsigned int>("firstEgMotherTruthType");
          //} else {
          //    top::check(false, "Could not obtain mother's truth Type decoration for electron!");
          //}
          //if (elPtr->isAvailable<unsigned int>("firstEgMotherTruthOrigin")) {
          //    m_el_true_firstEgMotherTruthOrigin[i] = elPtr->auxdata<unsigned int>("firstEgMotherTruthOrigin");
          //} else {
          //    top::check(false, "Could not obtain mother's truth Origin decoration for electron!");
          //}
          //m_el_true_isPrompt[i] = isPromptElectron(m_el_true_type[i], m_el_true_origin[i],
          // m_el_true_firstEgMotherTruthType[i], m_el_true_firstEgMotherTruthOrigin[i]);
        }
      }

      ++i;
    }

    // save muons
    i = 0;
    m_mu_pt.resize(upgradeEvent.m_muons->size());
    m_mu_eta.resize(upgradeEvent.m_muons->size());
    m_mu_phi.resize(upgradeEvent.m_muons->size());
    m_mu_e.resize(upgradeEvent.m_muons->size());
    m_mu_etcone20.resize(upgradeEvent.m_muons->size());
    m_mu_ptcone30.resize(upgradeEvent.m_muons->size());
    m_mu_charge.resize(upgradeEvent.m_muons->size());
    m_mu_true_type.resize(upgradeEvent.m_muons->size());
    m_mu_true_origin.resize(upgradeEvent.m_muons->size());
    m_mu_true_isPrompt.resize(upgradeEvent.m_muons->size());
    m_mu_prodVtx_z.resize(upgradeEvent.m_muons->size());
    m_mu_prodVtx_perp.resize(upgradeEvent.m_muons->size());
    m_mu_prodVtx_phi.resize(upgradeEvent.m_muons->size());

    for (const auto muPtr : *upgradeEvent.m_muons) {
      m_mu_pt[i] = muPtr->pt();
      m_mu_eta[i] = muPtr->eta();
      m_mu_phi[i] = muPtr->phi();
      m_mu_e[i] = muPtr->e();
      m_mu_charge[i] = muPtr->charge();

      m_mu_true_type[i] = 0;
      m_mu_true_origin[i] = 0;
      m_mu_true_isPrompt[i] = 0;
      m_mu_prodVtx_z[i] = -999;
      m_mu_prodVtx_perp[i] = -999;
      m_mu_prodVtx_phi[i] = -999;
      if (muPtr->isAvailable<unsigned int>("particleType")) {
        m_mu_true_type[i] = muPtr->auxdata<unsigned int>("particleType");
      } else if (muPtr->isAvailable<unsigned int>("classifierParticleType")) {
        m_mu_true_type[i] = muPtr->auxdata<unsigned int>("classifierParticleType");
      } else {
        top::check(false, "Could not obtain truth Type decoration for muon!");
      }
      if (muPtr->isAvailable<unsigned int>("particleOrigin")) {
        m_mu_true_origin[i] = muPtr->auxdata<unsigned int>("particleOrigin");
      } else if (muPtr->isAvailable<unsigned int>("classifierParticleOrigin")) {
        m_mu_true_origin[i] = muPtr->auxdata<unsigned int>("classifierParticleOrigin");
      } else {
        top::check(false, "Could not obtain truth Origin decoration for muon!");
      }
      m_mu_true_isPrompt[i] = isPromptMuon(m_mu_true_type[i], m_mu_true_origin[i]);
      if (muPtr->isAvailable<float>("prodVtx_z")) {
        m_mu_prodVtx_z[i] = muPtr->auxdata<float>("prodVtx_z");
      }
      if (muPtr->isAvailable<float>("prodVtx_perp")) {
        m_mu_prodVtx_perp[i] = muPtr->auxdata<float>("prodVtx_perp");
      }
      if (muPtr->isAvailable<float>("etcone20")) {
        m_mu_etcone20[i] = muPtr->auxdata<float>("etcone20");
      } else {
        m_mu_etcone20[i] = -999;
      }
      if (muPtr->isAvailable<float>("ptcone30")) {
        m_mu_ptcone30[i] = muPtr->auxdata<float>("ptcone30");
      } else {
        m_mu_ptcone30[i] = -999;
      }
      if (muPtr->isAvailable<float>("prodVtx_phi")) {
        m_mu_prodVtx_phi[i] = muPtr->auxdata<float>("prodVtx_phi");
      }

      ++i;
    }

    static const SG::AuxElement::ConstAccessor<int> ghostBHadronsFinalCount("GhostBHadronsFinalCount");
    static const SG::AuxElement::ConstAccessor<int> ghostCHadronsFinalCount("GhostCHadronsFinalCount");

    // save jets
    i = 0;
    m_jet_pt.resize(upgradeEvent.m_jets->size());
    m_jet_eta.resize(upgradeEvent.m_jets->size());
    m_jet_phi.resize(upgradeEvent.m_jets->size());
    m_jet_e.resize(upgradeEvent.m_jets->size());
    m_jet_mv1eff.resize(upgradeEvent.m_jets->size());
    m_jet_isPileup.resize(upgradeEvent.m_jets->size());
    m_jet_Ghosts_BHadron_Final_Count.resize(upgradeEvent.m_jets->size());
    m_jet_Ghosts_CHadron_Final_Count.resize(upgradeEvent.m_jets->size());
    for (const auto& jetPtr : *upgradeEvent.m_jets) {
      m_jet_pt[i] = jetPtr->pt();
      m_jet_eta[i] = jetPtr->eta();
      m_jet_phi[i] = jetPtr->phi();
      m_jet_e[i] = jetPtr->e();

      m_jet_mv1eff[i] = jetPtr->auxdata<float>("mv1TagEff");

      if (jetPtr->auxdata<int>("pileUp") == 0) {
        m_jet_isPileup[i] = 0;
        
	if(ghostBHadronsFinalCount.isAvailable(*jetPtr)) {
	  m_jet_Ghosts_BHadron_Final_Count[i] = ghostBHadronsFinalCount(*jetPtr);
	}
	else {
	  ATH_MSG_DEBUG("Found a jet with no GhostBHadronFinalCount auxdata");
	  m_jet_Ghosts_BHadron_Final_Count[i] = 0;
	}
	
	if(ghostCHadronsFinalCount.isAvailable(*jetPtr)) {
	  m_jet_Ghosts_CHadron_Final_Count[i] = ghostCHadronsFinalCount(*jetPtr);
	}
	else {
	  ATH_MSG_DEBUG("Found a jet with no GhostCHadronFinalCount auxdata");
	  m_jet_Ghosts_CHadron_Final_Count[i] = 0;
	}
	
      } else {
        m_jet_isPileup[i] = 1;
        m_jet_Ghosts_BHadron_Final_Count[i] = 0;
        m_jet_Ghosts_CHadron_Final_Count[i] = 0;
      }

      ++i;
    }

    //large R jets
    if (m_config->useTruthLargeRJets()) {
      unsigned int i = 0;

      m_ljet_pt.resize(upgradeEvent.m_largeRJets->size());
      m_ljet_eta.resize(upgradeEvent.m_largeRJets->size());
      m_ljet_phi.resize(upgradeEvent.m_largeRJets->size());
      m_ljet_e.resize(upgradeEvent.m_largeRJets->size());
      m_ljet_Ghosts_BHadron_Final_Count.resize(upgradeEvent.m_largeRJets->size());
      m_ljet_Ghosts_CHadron_Final_Count.resize(upgradeEvent.m_largeRJets->size());
      for (const auto& jetPtr : *upgradeEvent.m_largeRJets) {
        m_ljet_pt[i] = jetPtr->pt();
        m_ljet_eta[i] = jetPtr->eta();
        m_ljet_phi[i] = jetPtr->phi();
        m_ljet_e[i] = jetPtr->e();

        if(ghostBHadronsFinalCount.isAvailable(*jetPtr)) {
	  m_ljet_Ghosts_BHadron_Final_Count[i] = ghostBHadronsFinalCount(*jetPtr);
	}
	else {
	  ATH_MSG_DEBUG("Found a jet with no GhostBHadronFinalCount auxdata");
	  m_ljet_Ghosts_BHadron_Final_Count[i] = 0;
	}
	
	if(ghostCHadronsFinalCount.isAvailable(*jetPtr)) {
	  m_ljet_Ghosts_CHadron_Final_Count[i] = ghostCHadronsFinalCount(*jetPtr);
	}
	else {
	  ATH_MSG_DEBUG("Found a jet with no GhostCHadronFinalCount auxdata");
	  m_ljet_Ghosts_CHadron_Final_Count[i] = 0;
	}

        ++i;
      }
    }

    //photons
    if (m_config->useTruthPhotons()) {
      unsigned int i(0);
      m_ph_pt.resize(upgradeEvent.m_photons->size());
      m_ph_eta.resize(upgradeEvent.m_photons->size());
      m_ph_phi.resize(upgradeEvent.m_photons->size());
      m_ph_e.resize(upgradeEvent.m_photons->size());
      m_ph_true_type.resize(upgradeEvent.m_photons->size());
      m_ph_true_origin.resize(upgradeEvent.m_photons->size());
      if (m_config->HLLHCFakes()) m_ph_faketype.resize(upgradeEvent.m_photons->size());

      for (const auto* const phPtr : *upgradeEvent.m_photons) {
        m_ph_pt[i] = phPtr->pt();
        m_ph_eta[i] = phPtr->eta();
        m_ph_phi[i] = phPtr->phi();
        m_ph_e[i] = phPtr->e();
        if (!m_config->HLLHCFakes()) {
          if (phPtr->isAvailable<unsigned int>("particleType")) {
            m_ph_true_type[i] = phPtr->auxdata<unsigned int>("particleType");
          } else if (phPtr->isAvailable<unsigned int>("classifierParticleType")) {
            m_ph_true_type[i] = phPtr->auxdata<unsigned int>("classifierParticleType");
          } else {
            top::check(false, "Could not obtain truth Type decoration for photon!");
          }
          if (phPtr->isAvailable<unsigned int>("particleOrigin")) {
            m_ph_true_origin[i] = phPtr->auxdata<unsigned int>("particleOrigin");
          } else if (phPtr->isAvailable<unsigned int>("classifierParticleOrigin")) {
            m_ph_true_origin[i] = phPtr->auxdata<unsigned int>("classifierParticleOrigin");
          } else {
            top::check(false, "Could not obtain truth Origin decoration for photon!");
          }
        } else {
          if (phPtr->isAvailable<int>("FakeType")) {
            m_ph_faketype[i] = phPtr->auxdata<int>("FakeType");
          } else {
            top::check(false, "Could not obtain FakeType decoration for photon!");
          }
          if (m_ph_faketype[i] == 0) { // truthType/Origin only available for true photon (also for electron fake, but
                                       // we are not interested in its truth info)
            if (phPtr->isAvailable<unsigned int>("particleType")) {
              m_ph_true_type[i] = phPtr->auxdata<unsigned int>("particleType");
            } else if (phPtr->isAvailable<unsigned int>("classifierParticleType")) {
              m_ph_true_type[i] = phPtr->auxdata<unsigned int>("classifierParticleType");
            } else {
              top::check(false, "Could not obtain truth Type decoration for photon!");
            }
            if (phPtr->isAvailable<unsigned int>("particleOrigin")) {
              m_ph_true_origin[i] = phPtr->auxdata<unsigned int>("particleOrigin");
            } else if (phPtr->isAvailable<unsigned int>("classifierParticleOrigin")) {
              m_ph_true_origin[i] = phPtr->auxdata<unsigned int>("classifierParticleOrigin");
            } else {
              top::check(false, "Could not obtain truth Origin decoration for photon!");
            }
          }
        }

        ++i;
      }
    }

    // MET
    m_met_met = upgradeEvent.m_met->met();
    m_met_phi = upgradeEvent.m_met->phi();

    if (m_config->doMCGeneratorWeights()) {
      // delegate to helper function.
      loadMCGeneratorWeights();
    }
  }

  void EventSaverFlatNtuple::fillUpgradeEvent() {
    // fill the tree
    m_upgradeTreeManager->fill();
  }//saveUpgradeEvent

  void EventSaverFlatNtuple::finalize() {
    m_outputFile->Write();
  }

  void EventSaverFlatNtuple::loadPdfInfo() {
    const xAOD::TruthEventContainer* truthEvent(nullptr);

    top::check(evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()), "Failed to retrieve truth PDF info");

    if (truthEvent != nullptr) {
      unsigned int i(0);
      unsigned int truthEventSize = truthEvent->size();
      m_PDFinfo_X1.resize(truthEventSize);
      m_PDFinfo_X2.resize(truthEventSize);
      m_PDFinfo_PDGID1.resize(truthEventSize);
      m_PDFinfo_PDGID2.resize(truthEventSize);
      m_PDFinfo_Q.resize(truthEventSize);
      m_PDFinfo_XF1.resize(truthEventSize);
      m_PDFinfo_XF2.resize(truthEventSize);

      for (const auto* const tePtr : *truthEvent) {
        // LMTODO: ASG tools/pdfInfo() can't be used yet, since scale Q is not available
        // xAOD::TruthEvent::PdfInfo _pdfInfo=tePtr->pdfInfo();
        // if _pdfInfo.valid()  { m_PDFinfo_X1[i] = _pdfInfo.x1; //etc. }
        std::string PDFinfoVarName = "X1";
        if (truthEvent->isAvailable<float>(PDFinfoVarName)) m_PDFinfo_X1[i] = (*tePtr).auxdataConst< float >(PDFinfoVarName);
        PDFinfoVarName = "X2";
        if (truthEvent->isAvailable<float>(PDFinfoVarName)) m_PDFinfo_X2[i] = (*tePtr).auxdataConst< float >(PDFinfoVarName);
        PDFinfoVarName = "PDGID1";
        if (truthEvent->isAvailable<int>(PDFinfoVarName)) m_PDFinfo_PDGID1[i] = (*tePtr).auxdataConst< int >(PDFinfoVarName);
        PDFinfoVarName = "PDGID2";
        if (truthEvent->isAvailable<int>(PDFinfoVarName)) m_PDFinfo_PDGID2[i] = (*tePtr).auxdataConst< int >(PDFinfoVarName);
        PDFinfoVarName = "XF1";
        if (truthEvent->isAvailable<float>(PDFinfoVarName)) m_PDFinfo_XF1[i] = (*tePtr).auxdataConst< float >(PDFinfoVarName);
        PDFinfoVarName = "XF2";
        if (truthEvent->isAvailable<float>(PDFinfoVarName)) m_PDFinfo_XF2[i] = (*tePtr).auxdataConst< float >(PDFinfoVarName);
        PDFinfoVarName = "Q";
        if (truthEvent->isAvailable<float>(PDFinfoVarName)) m_PDFinfo_Q[i] = (*tePtr).auxdataConst< float >(PDFinfoVarName);
        ++i;
      }
    }
  }

  void EventSaverFlatNtuple::loadPdfWeights() {
    const xAOD::TruthEventContainer* truthEvent(nullptr);

    top::check(evtStore()->retrieve(truthEvent, m_config->sgKeyTruthEvent()), "Failed to retrieve truth PDF info");

    for (auto tePtr : *truthEvent) {
      for (const std::string& pdf_name : m_config->LHAPDFSets()) {
        if (tePtr->isAvailable<std::vector<float> >("AnalysisTop_" + pdf_name + "_Weights")) {
          m_PDF_eventWeights[ pdf_name ] = tePtr->auxdata<std::vector<float> >("AnalysisTop_" + pdf_name + "_Weights");
        }
      }
    }
  }

  void EventSaverFlatNtuple::loadMCGeneratorWeights() {
    const xAOD::EventInfo* eventInfo(nullptr);
    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo for loading of MCGenWeights!");
    m_mc_generator_weights = eventInfo->mcEventWeights();
  }

  // This bit is new
  // It is not optimal and can do with a lot of improvement
  // Consider it a first pass
  //
  // We can save xAOD objects into flat ntuple by looking at their SG::AuxElement data
  //
  // inspired by:
  // https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODCore/tags/xAODCore-00-00-93/Root/PrintHelpers.cxx

  void EventSaverFlatNtuple::registerObjectIntoTruthTree(const SG::AuxElement& obj) {
    // If the container doesn't have an auxiliary store, bail now:
    if (!obj.container()->getConstStore()) return;

    // Get a convenience pointer to the aux type registry:
    SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

    // Get the auxiliary IDs of the object:
    const SG::auxid_set_t& auxids =
      (obj.container()->getConstStore() ?
       obj.container()->getConstStore()->getAuxIDs() :
       obj.getConstStore()->getAuxIDs());

    // find the variables and fill them
    for (SG::auxid_t auxid : auxids) {
      // get the type of the variable
      const std::type_info* ti = reg.getType(auxid);
      // get the name of the variable
      const std::string name = reg.getName(auxid);

      // Add variable
      if (*ti == typeid(int)) {
        m_extraTruthVars_int.insert(std::make_pair(name, new int));
      } else if (*ti == typeid(float)) {
        m_extraTruthVars_float.insert(std::make_pair(name, new float));
      } else {
        ATH_MSG_INFO("insertObjectIntoTruthTree - you need another variable map for more types!");
      }
    }

    // add the variables to the truthTreeManager
    for (std::unordered_map<std::string, int*>::const_iterator i = m_extraTruthVars_int.begin(); i != m_extraTruthVars_int.end(); ++i) {
      m_truthTreeManager->makeOutputVariable(*(*i).second, (*i).first);
    }
    for (std::unordered_map<std::string, float*>::const_iterator i = m_extraTruthVars_float.begin(); i != m_extraTruthVars_float.end(); ++i) {
      m_truthTreeManager->makeOutputVariable(*(*i).second, (*i).first);
    }
  }

  void EventSaverFlatNtuple::saveObjectIntoTruthTree(const SG::AuxElement& obj) {
    // If the container doesn't have an auxiliary store, bail now:
    if (!obj.container()->getConstStore()) return;

    // Get a convenience pointer to the aux type registry:
    SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
    // Get the auxiliary IDs of the object:
    const SG::auxid_set_t& auxids =
      (obj.container()->getConstStore() ?
       obj.container()->getConstStore()->getAuxIDs() :
       obj.getConstStore()->getAuxIDs());

    // setup new variables
    for (SG::auxid_t auxid : auxids) {
      // get the type of the variable
      const std::type_info* ti = reg.getType(auxid);
      // get the name of the variable
      const std::string name = reg.getName(auxid);

      // Add variable
      if (*ti == typeid(int)) {
        std::unordered_map<std::string, int*>::iterator itr = m_extraTruthVars_int.find(name);
        if (itr != m_extraTruthVars_int.end()) {
          *((*itr).second) = obj.auxdataConst<int> (name);
        }
      } else if (*ti == typeid(float)) {
        std::unordered_map<std::string, float*>::iterator itr = m_extraTruthVars_float.find(name);
        if (itr != m_extraTruthVars_float.end()) {
          *((*itr).second) = obj.auxdataConst<float> (name);
        }
      } else {
        ATH_MSG_INFO("saveObjectIntoTruthTree - you need another variable map for more types!");
      }
    }
  }

  // get short name for FixedCutBEff_* WPs
  std::string EventSaverFlatNtuple::shortBtagWP(std::string const& WP) const {
    std::string str = "FixedCutBEff_";
    std::string out = WP;
    if (out.find(str) != std::string::npos) {
      out.replace(out.find(str), str.length(), "");
    }
    return out;
  }

  // remove "FT_EFF_", spaces, and "-" in named systematics
  std::string EventSaverFlatNtuple::betterBtagNamedSyst(const std::string name) {
    std::string str = "FT_EFF_";
    std::string out = name;
    if (out.find(str) != std::string::npos) {
      out.replace(out.find(str), str.length(), "");
    }
    str = " ";
    while (out.find(str) != std::string::npos) {
      out.replace(out.find(str), str.length(), "_");
    }
    str = "-";
    while (out.find(str) != std::string::npos) {
      out.replace(out.find(str), str.length(), "_");
    }
    return out;
  }

  //new prompt lepton classification below based on
  // https://twiki.cern.ch/twiki/pub/AtlasProtected/IsolationFakeForum/MakeTruthClassification.hxx
  //these represent the latest IFF recommendations
  std::pair<bool, bool> EventSaverFlatNtuple::isPromptElectron(int type, int origin, int egMotherType, int egMotherOrigin, int egMotherPdgId, int RecoCharge) {
    // 43 is "diboson" origin, but is needed due to buggy origin flags in Sherpa ttbar
    bool isprompt = (type == 2 ||
                     (type == 4 && origin == 5 && fabs(egMotherPdgId) == 11) ||
                     // bkg electrons from ElMagDecay with origin top, W or Z, higgs, diBoson
                     (type == 4 && origin == 7 && egMotherType == 2 && (egMotherOrigin == 10 || egMotherOrigin == 12 || egMotherOrigin == 13 || egMotherOrigin == 14 || egMotherOrigin == 43) && fabs(egMotherPdgId) == 11) ||
                     // unknown electrons from multi-boson (sherpa 222, di-boson)
                     (type == 1 && egMotherType == 2 && egMotherOrigin == 47 && fabs(egMotherPdgId) == 11));

    bool isChargeFl = false;

    if (egMotherPdgId * RecoCharge > 0) isChargeFl = true;
    if (isprompt) return std::make_pair(true, isChargeFl); //charge flipped electrons are also considered prompt

    // bkg photons from photon conv from FSR (must check!!)
    if (type == 4 && origin == 5 && egMotherOrigin == 40) return std::make_pair(true, false);

    // non-iso photons from FSR for the moment but we must check!! (must check!!)
    if (type == 15 && origin == 40) return std::make_pair(true, false);

    // mainly in Sherpa Zee, but some also in Zmumu
    if (type == 4 && origin == 7 && egMotherType == 15 && egMotherOrigin == 40) return std::make_pair(true, false);

    return std::make_pair(false, false);
  }

  bool EventSaverFlatNtuple::isPromptMuon(int type, int origin) {
    // 43 is "diboson" origin, but is needed due to buggy origin flags in Sherpa ttbar
    bool prompt = (type == 6 &&
                   (origin == 10 || origin == 12 || origin == 13 || origin == 14 || origin == 15 || origin == 22 || origin == 43));

    return prompt;
  }

  int EventSaverFlatNtuple::filterBranches(const top::TreeManager* treeManager, const std::string& variable) {
    std::vector<std::string> filteredBranches;
    if(treeManager->name() == "truth") filteredBranches= m_config->filterPartonLevelBranches();
    else if(treeManager->name() == "particleLevel") filteredBranches= m_config->filterParticleLevelBranches();
    else if(treeManager->name() == "nominal_Loose") filteredBranches= m_config->filterNominalLooseBranches();
    else  filteredBranches= m_config->filterBranches();

    // lambda to test a wildcard on the variable
    auto matches_wildcard = [&variable] (const std::string& wildcard) {
                              std::smatch match;
                              std::regex_search(variable, match, std::regex(wildcard));
                              if (!match.empty()) return true;

                              return false;
                            };

    for (std::string filter : filteredBranches) {
      // replace "*" with ".*"
      filter = regex_replace(filter, std::regex("\\*"), ".*");
      // check for a match
      if (matches_wildcard(("^" + filter).c_str())) return 0;
    }

    return -1;
  }
} // namespace
