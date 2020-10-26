/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/JetFtagEffPlots.h"

#include <cmath>
#include <array>
#include <string>

#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "TopCorrections/ScaleFactorRetriever.h"

#include "TH1.h"
#include "TH2D.h"
#include "TFile.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

#include "TopEventSelectionTools/MsgCategory.h"
using namespace TopEventSelectionTools;

namespace top {
  const double JetFtagEffPlots::toGeV = 0.001;


  JetFtagEffPlots::JetFtagEffPlots(const std::string& name,
                                   TFile* outputFile, const std::string& params, std::shared_ptr<top::TopConfig> config,
                                   EL::Worker* wk) :
    m_nominalHashValue(0),
    m_CDIfile("xAODBTaggingEfficiency/13TeV/2020-21-13TeV-MC16-CDI-2020-03-11_v3.root"),
    m_fill_total_hists(false),
    m_histogram_suffix(""),
    m_dont_use_event_weight(false),
    m_use_track_jets(false),
    m_doNominal(false),
    m_doMuRup(false),
    m_doMuRdown(false),
    m_doMuFup(false),
    m_doMuFdown(false),
    m_doVar3cup(false),
    m_doVar3cdown(false),
    m_doFSRup(false),
    m_doFSRdown(false),
    // default pT and eta binning
    m_ptBins("15:20:30:45:60:80:110:160:210:260:310:400:500:600:800:1000:1200:1500:1800:2500"),
    m_etaBins("0.:0.3:0.8:1.2:2.1:2.8:3.6:4.5"),
    m_max_pT(-1),
    m_min_pT(-1),
    m_N_pT_bins(-1),
    m_max_Eta(-1),
    m_min_Eta(-1),
    m_N_Eta_bins(-1),
    m_jetCollection(""),
    m_WP("FixedCutBEff_70"),
    m_tagger("DL1"),
    m_config(config),
    m_selection_tool("BTaggingSelectionTool/selTool"),
    m_PMGTruthWeights(nullptr),
    m_sfRetriever(nullptr) {
    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();


    const std::string truthWeightToolName = "PMGTruthWeightTool";
    if (asg::ToolStore::contains<PMGTools::PMGTruthWeightTool>(truthWeightToolName)) m_PMGTruthWeights =
        asg::ToolStore::get<PMGTools::PMGTruthWeightTool>(truthWeightToolName);
    else {
      m_PMGTruthWeights = new PMGTools::PMGTruthWeightTool(truthWeightToolName);
      top::check(m_PMGTruthWeights->initialize(), "Failed to initialize " + truthWeightToolName);
    }
    //decoding arguments
    std::istringstream stream(params);
    std::string s;
    while (stream >> s) {
      if (s.substr(0, 3) == "WP:") m_WP = s.substr(3, s.size() - 3);
      else if (s.substr(0, 7) == "tagger:") m_tagger = s.substr(7, s.size() - 7);
      else if (s.substr(0, 8) == "CDIfile:") m_CDIfile = s.substr(8, s.size() - 8);
      else if (s.substr(0, 3) == "pt:" || s.substr(0, 3) == "pT:") m_ptBins = s.substr(3, s.size() - 3);
      else if (s.substr(0, 7) == "abseta:") m_etaBins = s.substr(7, s.size() - 7);
      else if (s.substr(0, 7) == "max_pT:") m_max_pT = std::stof(s.substr(7, s.size() - 7));
      else if (s.substr(0, 7) == "min_pT:") m_min_pT = std::stof(s.substr(7, s.size() - 7));
      else if (s.substr(0, 5) == "N_pT:") m_N_pT_bins = std::stoi(s.substr(5, s.size() - 5));
      else if (s.substr(0, 8) == "max_eta:") m_max_Eta = std::stof(s.substr(8, s.size() - 8));
      else if (s.substr(0, 8) == "min_eta:") m_min_Eta = std::stof(s.substr(8, s.size() - 8));
      else if (s.substr(0, 6) == "N_eta:") m_N_Eta_bins = std::stoi(s.substr(6, s.size() - 6));
      else if (s.substr(0, 7) == "suffix:") m_histogram_suffix = s.substr(7, s.size() - 7);
      else if (s == "fill_total_hist") m_fill_total_hists = true;
      else if (s == "dont_use_event_weight") m_dont_use_event_weight = true;
      else if (s == "use_track_jets") m_use_track_jets = true;
      else if (s == "NOMINAL") m_doNominal = true;
      else if (s == "MURUP") m_doMuRup = true;
      else if (s == "MURDOWN") m_doMuRdown = true;
      else if (s == "MUFUP") m_doMuFup = true;
      else if (s == "MUFDOWN") m_doMuFdown = true;
      else if (s == "VAR3CUP") m_doVar3cup = true;
      else if (s == "VAR3CDOWN") m_doVar3cdown = true;
      else if (s == "FSRUP") m_doFSRup = true;
      else if (s == "FSRDOWN") m_doFSRdown = true;
      else {
        throw std::runtime_error {
                "ERROR: Can't understand argument " + s + " for JETFTAGEFFPLOTS."
        };
      }
    }

    // retrieve jet collection
    if (!m_use_track_jets) {
      m_jetCollection = m_config->sgKeyJets();
    } else {
      m_jetCollection = m_config->sgKeyTrackJets();
    }

    m_sfRetriever = asg::ToolStore::get<ScaleFactorRetriever>("top::ScaleFactorRetriever");

    // No option were set, run only nominal
    if (!m_doNominal && !m_doMuRup && !m_doMuRdown && !m_doMuFup
        && !m_doMuFdown && !m_doVar3cup && !m_doVar3cdown && !m_doFSRup && !m_doFSRdown) {
      ATH_MSG_INFO("No variation has been set. Assuming nominal.");
      m_doNominal = true;
    }

    if (m_doNominal) m_hists = std::make_shared<PlotManager>(name + "/JetFtagEffPlots", outputFile, wk);
    if (m_doMuRup) m_hists_muRup = std::make_shared<PlotManager>(name + "/JetFtagEffPlots_MuRup", outputFile, wk);
    if (m_doMuRdown) m_hists_muRdown = std::make_shared<PlotManager>(name + "/JetFtagEffPlots_MuRdown", outputFile, wk);

    if (m_doMuFup) m_hists_muFup = std::make_shared<PlotManager>(name + "/JetFtagEffPlots_MuFup", outputFile, wk);
    if (m_doMuFdown) m_hists_muFdown = std::make_shared<PlotManager>(name + "/JetFtagEffPlots_MuFdown", outputFile, wk);

    if (m_doVar3cup) m_hists_Var3cup = std::make_shared<PlotManager>(name + "/JetFtagEffPlots_Var3cup", outputFile, wk);

    if (m_doVar3cdown) m_hists_Var3cdown = std::make_shared<PlotManager>(name + "/JetFtagEffPlots_Var3cdown", outputFile, wk);
    if (m_doFSRup) m_hists_FSRup = std::make_shared<PlotManager>(name + "/JetFtagEffPlots_FSRup", outputFile, wk);
    if (m_doFSRup) m_hists_FSRdown = std::make_shared<PlotManager>(name + "/JetFtagEffPlots_FSRdown", outputFile, wk);

    //handle binning and book histograms
    std::vector<double> ptBins;
    std::vector<double> etaBins;

    if (m_N_pT_bins < 1) {
      formatBinning(m_ptBins, ptBins);
      formatBinning(m_etaBins, etaBins);
    }

    for (std::string flavour:{"B", "C", "L", "T"}) {
      if (m_fill_total_hists) {
        std::string total_hist_name = flavour + "_total_" + m_jetCollection + m_histogram_suffix;
        if (m_N_pT_bins < 1) {
          if (m_hists) m_hists->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                        ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
          if (m_hists_muRup) m_hists_muRup->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                    ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
          if (m_hists_muRdown) m_hists_muRdown->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                        ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
          if (m_hists_muFup) m_hists_muFup->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                    ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
          if (m_hists_muFdown) m_hists_muFdown->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                        ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
          if (m_hists_Var3cup) m_hists_Var3cup->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                        ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
          if (m_hists_Var3cdown) m_hists_Var3cdown->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                            ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
          if (m_hists_FSRup) m_hists_FSRup->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                    ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
          if (m_hists_FSRdown) m_hists_FSRdown->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                        ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        } else {
          if (m_hists) m_hists->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                        m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
          if (m_hists_muRup) m_hists_muRup->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                    m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
          if (m_hists_muRdown) m_hists_muRdown->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                        m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
          if (m_hists_muFup) m_hists_muFup->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                    m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
          if (m_hists_muFdown) m_hists_muFdown->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                        m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
          if (m_hists_Var3cup) m_hists_Var3cup->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                        m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
          if (m_hists_Var3cdown) m_hists_Var3cdown->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                            m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
          if (m_hists_FSRup) m_hists_FSRup->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                    m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
          if (m_hists_FSRdown) m_hists_FSRdown->addHist(total_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                        m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        }
      }

      const std::string pass_hist_name = m_tagger + "_" + m_WP + "_" + m_jetCollection + "_" + flavour + "_pass" + m_histogram_suffix;
      if (m_N_pT_bins < 1) {
        if (m_hists) m_hists->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                      ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        if (m_hists_muRup) m_hists_muRup->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                  ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        if (m_hists_muRdown) m_hists_muRdown->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                      ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        if (m_hists_muFup) m_hists_muFup->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                  ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        if (m_hists_muFdown) m_hists_muFdown->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                      ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        if (m_hists_Var3cup) m_hists_Var3cup->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                      ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        if (m_hists_Var3cdown) m_hists_Var3cdown->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                          ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        if (m_hists_FSRup) m_hists_FSRup->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                  ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        if (m_hists_FSRdown) m_hists_FSRdown->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                      ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
      } else {
        if (m_hists) m_hists->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                      m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        if (m_hists_muRup) m_hists_muRup->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                  m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        if (m_hists_muRdown) m_hists_muRdown->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                      m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        if (m_hists_muFup) m_hists_muFup->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                  m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        if (m_hists_muFdown) m_hists_muFdown->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                      m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        if (m_hists_Var3cup) m_hists_Var3cup->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                      m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        if (m_hists_Var3cdown) m_hists_Var3cdown->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                          m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        if (m_hists_FSRup) m_hists_FSRup->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                  m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
        if (m_hists_FSRdown) m_hists_FSRdown->addHist(pass_hist_name, ";pT [GeV];|#eta^{jet}|;Events",
                                                      m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);
      }
    }

    m_selection_tool = asg::AnaToolHandle<IBTaggingSelectionTool>("BTaggingSelectionTool/BTagSelec_" + m_tagger + "_" + m_WP + "_" + m_jetCollection);
    top::check(m_selection_tool.setProperty("FlvTagCutDefinitionsFileName", m_CDIfile), "failed to initialize BtagSelectionTool " + m_tagger + "_" + m_WP + "_" + m_jetCollection);
    top::check(m_selection_tool.setProperty("TaggerName", m_tagger), "failed to initialize BtagSelectionTool " + m_tagger + "_" + m_WP + "_" + m_jetCollection);
    top::check(m_selection_tool.setProperty("OperatingPoint", m_WP), "failed to initialize BtagSelectionTool " + m_tagger + "_" + m_WP + "_" + m_jetCollection);
    top::check(m_selection_tool.setProperty("JetAuthor", m_jetCollection), "failed to initialize BtagSelectionTool " + m_tagger + "_" + m_WP + "_" + m_jetCollection);
    top::check(m_selection_tool.setProperty("MinPt", m_min_pT*1e3), "failed to initialize BtagSelectionTool, failed in min jet pT");
    top::check(m_selection_tool.setProperty("MaxEta", m_max_Eta), "failed to initialize BtagSelectionTool, failed in max jet eta");

    top::check(m_selection_tool.initialize(), "failed to initialize BtagSelectionTool " + m_tagger + "_" + m_WP + "_" + m_jetCollection);
  }

  bool JetFtagEffPlots::apply(const top::Event& event) const {
    //only MC
    if (!top::isSimulation(event)) return true;

    // only nominal
    if (event.m_hashValue != m_nominalHashValue) return true;

    // do loose or tight events only if requested
    if (event.m_isLoose && !m_config->doLooseEvents()) return true;

    if (!event.m_isLoose && !m_config->doTightEvents()) return true;

    float eventWeightNominal = 1.0;
    float eventWeightMuRup = 1.0;
    float eventWeightMuRdown = 1.0;
    float eventWeightMuFup = 1.0;
    float eventWeightMuFdown = 1.0;
    float eventWeightVar3cUp = 1.0;
    float eventWeightVar3cDown = 1.0;
    float eventWeightFSRUp = 1.0;
    float eventWeightFSRDown = 1.0;
    float muRup = GetPMGTruthWeight(WEIGHTTYPE::MURUP);
    float muRdown = GetPMGTruthWeight(WEIGHTTYPE::MURDOWN);
    float muFup = GetPMGTruthWeight(WEIGHTTYPE::MUFUP);
    float muFdown = GetPMGTruthWeight(WEIGHTTYPE::MUFDOWN);
    float Var3cUp = GetPMGTruthWeight(WEIGHTTYPE::VAR3CUP);
    float Var3cDown = GetPMGTruthWeight(WEIGHTTYPE::VAR3CDOWN);
    float FSRUp = GetPMGTruthWeight(WEIGHTTYPE::FSRUP);
    float FSRDown = GetPMGTruthWeight(WEIGHTTYPE::FSRDOWN);

    float jvt_sf = event.m_jvtSF;
    float pu_weight = 1.0;
    if (top::ScaleFactorRetriever::hasPileupSF(event)) {
      pu_weight = top::ScaleFactorRetriever::pileupSF(event);
    }
    float lepton_sf = m_sfRetriever->leptonSF(event, nominal);

    if (m_doNominal) {
      eventWeightNominal = event.m_info->auxdataConst<float>("AnalysisTop_eventWeight") * jvt_sf * pu_weight * lepton_sf;
    }
    if (m_doMuRup) eventWeightMuRup = muRup * jvt_sf * pu_weight * lepton_sf;
    if (m_doMuRdown) eventWeightMuRdown = muRdown * jvt_sf * pu_weight * lepton_sf;
    if (m_doMuFup) eventWeightMuFup = muFup * jvt_sf * pu_weight * lepton_sf;
    if (m_doMuFdown) eventWeightMuFdown = muFdown * jvt_sf * pu_weight * lepton_sf;
    if (m_doVar3cup) eventWeightVar3cUp = Var3cUp * jvt_sf * pu_weight * lepton_sf;
    if (m_doVar3cdown) eventWeightVar3cDown = Var3cDown * jvt_sf * pu_weight * lepton_sf;
    if (m_doFSRup) eventWeightFSRUp = FSRUp * jvt_sf * pu_weight * lepton_sf;
    if (m_doFSRdown) eventWeightFSRDown = FSRDown * jvt_sf * pu_weight * lepton_sf;

    if (m_dont_use_event_weight) {
      eventWeightNominal = 1.0;
    }

    if (m_hists) FillHistograms(m_hists, eventWeightNominal, event);
    if (m_hists_muRup) FillHistograms(m_hists_muRup, eventWeightMuRup, event);
    if (m_hists_muRdown) FillHistograms(m_hists_muRdown, eventWeightMuRdown, event);
    if (m_hists_muFup) FillHistograms(m_hists_muFup, eventWeightMuFup, event);
    if (m_hists_muFdown) FillHistograms(m_hists_muFdown, eventWeightMuFdown, event);
    if (m_hists_Var3cup) FillHistograms(m_hists_Var3cup, eventWeightVar3cUp, event);
    if (m_hists_Var3cdown) FillHistograms(m_hists_Var3cdown, eventWeightVar3cDown, event);
    if (m_hists_FSRup) FillHistograms(m_hists_FSRup, eventWeightFSRUp, event);
    if (m_hists_FSRdown) FillHistograms(m_hists_FSRdown, eventWeightFSRDown, event);

    return true;
  }

  void JetFtagEffPlots::FillHistograms(std::shared_ptr<PlotManager> h_ptr, double w_event, const top::Event& event) const {
    const xAOD::JetContainer* jets;

    if (!m_use_track_jets) {
      jets = &event.m_jets;
    } else {
      jets = &event.m_trackJets;
    }

    for (unsigned long jet_i = 0; jet_i < jets->size(); jet_i++) {
      const xAOD::Jet* jetPtr = jets->at(jet_i);

      int jet_flavor = -99;
      bool status = jetPtr->getAttribute<int>("HadronConeExclTruthLabelID", jet_flavor);

      if (!status) continue;

      std::string flav_name;

      if (jet_flavor == 5) flav_name = "B";

      else if (jet_flavor == 4) flav_name = "C";

      else if (jet_flavor == 0) flav_name = "L";

      else if (jet_flavor == 15) flav_name = "T";

      if (m_fill_total_hists) {
        static_cast<TH2D*>(h_ptr->hist(flav_name + "_total_" + m_jetCollection + m_histogram_suffix))->Fill(jetPtr->pt() * toGeV, fabs(jetPtr->eta()), w_event);
      }

      bool pass_cut = static_cast<bool>(m_selection_tool->accept(jetPtr));

      if (pass_cut) {
        std::string hist_name = m_tagger + "_" + m_WP + "_" + m_jetCollection + "_" + flav_name + "_pass" + m_histogram_suffix;
        static_cast<TH2D*>(h_ptr->hist(hist_name))->Fill(jetPtr->pt() * toGeV, fabs(jetPtr->eta()), w_event);
      }
    }
  }

// function to translate the binnings into vector of bin edges
  void JetFtagEffPlots::formatBinning(const std::string& str, std::vector<double>& binEdges) {
    std::stringstream ss(str);
    std::string tok;
    char separator = ':';
    while (std::getline(ss, tok, separator)) {
      binEdges.push_back(std::atof(tok.c_str()));
    }
  }

  float JetFtagEffPlots::GetPMGTruthWeight(WEIGHTTYPE type) const {
    float result = 1;

    if (type == WEIGHTTYPE::MURUP) {
      if (m_PMGTruthWeights->hasWeight(" muR = 2.0, muF = 1.0 ")) result = m_PMGTruthWeights->getWeight(" muR = 2.0, muF = 1.0 ");
      else if (m_PMGTruthWeights->hasWeight(" muR = 2.00, muF = 1.00 ")) result = m_PMGTruthWeights->getWeight(" muR = 2.00, muF = 1.00 ");
    } else if (type == WEIGHTTYPE::MURDOWN) {
      if (m_PMGTruthWeights->hasWeight(" muR = 0.5, muF = 1.0 ")) result = m_PMGTruthWeights->getWeight(" muR = 0.5, muF = 1.0 ");
      else if (m_PMGTruthWeights->hasWeight(" muR = 0.50, muF = 1.00 ")) result = m_PMGTruthWeights->getWeight(" muR = 0.50, muF = 1.00 ");
    } else if (type == WEIGHTTYPE::MUFUP) {
      if (m_PMGTruthWeights->hasWeight(" muR = 1.0, muF = 2.0 ")) result = m_PMGTruthWeights->getWeight(" muR = 1.0, muF = 2.0 ");
      else if (m_PMGTruthWeights->hasWeight(" muR = 1.00, muF = 2.00 ")) result = m_PMGTruthWeights->getWeight(" muR = 1.00, muF = 2.00 ");
    } else if (type == WEIGHTTYPE::MUFDOWN) {
      if (m_PMGTruthWeights->hasWeight(" muR = 1.0, muF = 0.5 ")) result = m_PMGTruthWeights->getWeight(" muR = 1.0, muF = 0.5 ");
      else if (m_PMGTruthWeights->hasWeight(" muR = 1.00, muF = 0.50 ")) result = m_PMGTruthWeights->getWeight(" muR = 1.00, muF = 0.50 ");
    } else if (type == WEIGHTTYPE::VAR3CUP) {
      if (m_PMGTruthWeights->hasWeight("Var3cUp")) result = m_PMGTruthWeights->getWeight("Var3cUp");
    } else if (type == WEIGHTTYPE::VAR3CDOWN) {
      if (m_PMGTruthWeights->hasWeight("Var3cDown")) result = m_PMGTruthWeights->getWeight("Var3cDown");
    } else if (type == WEIGHTTYPE::FSRUP) {
      if (m_PMGTruthWeights->hasWeight("isr:muRfac=1.0_fsr:muRfac=2.0")) result = m_PMGTruthWeights->getWeight("isr:muRfac=1.0_fsr:muRfac=2.0");
      else if (m_PMGTruthWeights->hasWeight("isr:muRfac=1.00_fsr:muRfac=2.00")) result = m_PMGTruthWeights->getWeight("isr:muRfac=1.00_fsr:muRfac=2.00");
    } else if (type == WEIGHTTYPE::FSRDOWN) {
      if (m_PMGTruthWeights->hasWeight("isr:muRfac=1.0_fsr:muRfac=0.5")) result = m_PMGTruthWeights->getWeight("isr:muRfac=1.0_fsr:muRfac=0.5");
      else if (m_PMGTruthWeights->hasWeight("isr:muRfac=1.00_fsr:muRfac=0.50")) result = m_PMGTruthWeights->getWeight("isr:muRfac=1.00_fsr:muRfac=0.50");
    }

    return result;
  }

  std::string JetFtagEffPlots::name() const {
    return "JETFTAGEFFPLOTS";
  }
}  // namespace top
