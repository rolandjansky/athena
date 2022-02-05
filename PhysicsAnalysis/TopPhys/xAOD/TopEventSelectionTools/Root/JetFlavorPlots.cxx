/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/JetFlavorPlots.h"

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
  const double JetFlavorPlots::toGeV = 0.001;


  JetFlavorPlots::JetFlavorPlots(const std::string& name,
                                 TFile* outputFile, const std::string& params, std::shared_ptr<top::TopConfig> config,
                                 EL::Worker* wk) :
    m_nominalHashValue(0),
    m_detailed(false),
    m_doNominal(false),
    m_doRadHigh(false),
    m_doRadLow(false),
    m_doRadHighNoVar3c(false),
    m_doRadLowNoVar3c(false),
    // default pT and eta binning, and default max number of Jets
    m_ptBins("15:20:30:45:60:80:110:160:210:260:310:400:500:600:800:1000:1200:1500:1800:2500"),
    m_etaBins("0.:0.3:0.8:1.2:2.1:2.8:3.6:4.5"),
    m_nJetsMax(15),
    m_jetCollection(""),
    m_config(config),
    m_PMGTruthWeights(nullptr),
    m_throwwarningPMG(true) {
    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();

    // retrieve jet collection and remove the "Jets" at the end of it
    m_jetCollection = m_config->sgKeyJets();
    int pos = m_jetCollection.find("Jets");
    m_jetCollection.erase(pos);
    //FIXME: If no metadata is available, the PMGTool will crash. Providing here a "manual" workaround.
    //FIXME PMG: comment from here
    //retrieve PMGTruthWeights
    const std::string truthWeightToolName = "PMGTruthWeightTool";
    if (asg::ToolStore::contains<PMGTools::PMGTruthWeightTool>(truthWeightToolName)) m_PMGTruthWeights =
        asg::ToolStore::get<PMGTools::PMGTruthWeightTool>(truthWeightToolName);
    else {
      m_PMGTruthWeights = new PMGTools::PMGTruthWeightTool(truthWeightToolName);
      top::check(m_PMGTruthWeights->initialize(), "Failed to initialize " + truthWeightToolName);
    }
    //FIXME PMG: comment till here
    //decoding arguments
    std::istringstream stream(params);
    std::string s;
    while (stream >> s) {
      if (s == "detailed") m_detailed = true;
      else if (s.substr(0, 3) == "pt:" || s.substr(0, 3) == "pT:") m_ptBins = s.substr(3, s.size() - 3);
      else if (s.substr(0, 7) == "abseta:") m_etaBins = s.substr(7, s.size() - 7);
      else if (s.substr(0, 9) == "nJetsMax:") m_nJetsMax = std::atoi(s.substr(9, s.size() - 9).c_str());
      else if (s == "nominal" || s == "NOMINAL" || s == "Nominal") m_doNominal = true;
      else if (s == "radiationhigh" || s == "RADIATIONHIGH" || s == "RadiationHigh" ||
               s == "radiationHigh") m_doRadHigh = true;
      else if (s == "radiationlow" || s == "RADIATIONLOW" || s == "RadiationLow" ||
               s == "radiationLow") m_doRadLow = true;
      else if (s == "radiationhighnovar3c" || s == "RADIATIONHIGHNOVAR3C" || s == "RadiationHighNoVar3c" ||
               s == "radiationHighNoVar3c") m_doRadHighNoVar3c = true;
      else if (s == "radiationlownovar3c" || s == "RADIATIONLOWNOVAR3C" || s == "RadiationLowNoVar3c" ||
               s == "radiationLowNoVar3c") m_doRadLowNoVar3c = true;
      else {
        throw std::runtime_error("ERROR: Can't understand argument " + s + "For JETFLAVORPLOTS");
      }
    }
    //If neither nominal or radiation has been selected, assume it's nominal
    if ((m_doNominal && m_doRadHigh && m_doRadLow && m_doRadHighNoVar3c && m_doRadLowNoVar3c) == false) m_doNominal = true;
    // create the JetFlavorPlots and JetFlavorPlots_Loose directories only if needed
    if (m_config->doTightEvents()) {
      if (m_doNominal) m_hists = std::make_shared<PlotManager>(name + "/JetFlavorPlots", outputFile, wk);
      if (m_doRadHigh) m_hists_RadHigh = std::make_shared<PlotManager>(name + "/JetFlavorPlots_RadHigh", outputFile, wk);
      if (m_doRadLow) m_hists_RadLow = std::make_shared<PlotManager>(name + "/JetFlavorPlots_RadLow", outputFile, wk);
      if (m_doRadHighNoVar3c) m_hists_RadHighNoVar3c = std::make_shared<PlotManager>(name + "/JetFlavorPlots_RadHighNoVar3c", outputFile, wk);
      if (m_doRadLowNoVar3c) m_hists_RadLowNoVar3c = std::make_shared<PlotManager>(name + "/JetFlavorPlots_RadLowNoVar3c", outputFile, wk);
    }
    if (m_config->doLooseEvents()) {
      if (m_doNominal) m_hists_Loose = std::make_shared<PlotManager>(name + "/JetFlavorPlots_Loose", outputFile, wk);
      if (m_doRadHigh) m_hists_RadHigh_Loose = std::make_shared<PlotManager>(name + "/JetFlavorPlots_Loose_RadHigh", outputFile, wk);
      if (m_doRadLow) m_hists_RadLow_Loose = std::make_shared<PlotManager>(name + "/JetFlavorPlots_Loose_RadLow", outputFile, wk);
      if (m_doRadHighNoVar3c) m_hists_RadHighNoVar3c_Loose = std::make_shared<PlotManager>(name + "/JetFlavorPlots_Loose_RadHighNoVar3c", outputFile, wk);
      if (m_doRadLowNoVar3c) m_hists_RadLowNoVar3c_Loose = std::make_shared<PlotManager>(name + "/JetFlavorPlots_Loose_RadLowNoVar3c", outputFile, wk);
    }
    //handle binning
    std::vector<double> ptBinning;
    std::vector<double> etaBinning;
    formatBinning(m_ptBins, ptBinning);
    formatBinning(m_etaBins, etaBinning);
    std::ostream& msgInfo = msg(MSG::Level::INFO);
    msgInfo << "Here is the binning used for JETFLAVORPLOTS in selection " << name << ":\n";
    msgInfo << "pt: ";
    for (auto pt:ptBinning) msgInfo << pt << " ";
    msgInfo << "\n";
    msgInfo << "abseta: ";
    for (auto eta:etaBinning) msgInfo << eta << " ";
    msgInfo << std::endl;

    if (m_config->doTightEvents()) {
      if (m_doNominal) BookHistograms(m_hists, ptBinning, etaBinning);
      if (m_doRadHigh) BookHistograms(m_hists_RadHigh, ptBinning, etaBinning);
      if (m_doRadLow) BookHistograms(m_hists_RadLow, ptBinning, etaBinning);
      if (m_doRadHighNoVar3c) BookHistograms(m_hists_RadHighNoVar3c, ptBinning, etaBinning);
      if (m_doRadLowNoVar3c) BookHistograms(m_hists_RadLowNoVar3c, ptBinning, etaBinning);
    }
    if (m_config->doLooseEvents()) {
      if (m_doNominal) BookHistograms(m_hists_Loose, ptBinning, etaBinning);
      if (m_doRadHigh) BookHistograms(m_hists_RadHigh_Loose, ptBinning, etaBinning);
      if (m_doRadLow) BookHistograms(m_hists_RadLow_Loose, ptBinning, etaBinning);
      if (m_doRadHighNoVar3c) BookHistograms(m_hists_RadHighNoVar3c_Loose, ptBinning, etaBinning);
      if (m_doRadLowNoVar3c) BookHistograms(m_hists_RadLowNoVar3c_Loose, ptBinning, etaBinning);
    }
  }

  void JetFlavorPlots::BookHistograms(std::shared_ptr<PlotManager> h_ptr, std::vector<double> ptBins,
                                      std::vector<double> etaBins) {
    if (m_detailed) {
      for (std::string flavour:{"gluon", "lightquark", "cquark", "bquark", "other"}) {
        // vs. nJets and flavour
        for (int i = 0; i <= m_nJetsMax; ++i) {
          h_ptr->addHist(flavour + "_jets_njet" + std::to_string(
            i) + "_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                         ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
        }
        // vs. flavour only
        h_ptr->addHist(flavour + "_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                       ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
      }
      // if "detailed", also making the quark_jets one
      h_ptr->addHist("quark_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                     ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
    } else {
      // the simplest case, one for gluon, one for quarks
      h_ptr->addHist("quark_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                     ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
      h_ptr->addHist("gluon_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                     ptBins.size() - 1, ptBins.data(), etaBins.size() - 1, etaBins.data());
    }
  }

  bool JetFlavorPlots::apply(const top::Event& event) const {
    //only MC
    if (!top::isSimulation(event)) return true;

    // only nominal
    if (event.m_hashValue != m_nominalHashValue) return true;

    // do loose or tight events only if requested
    if (event.m_isLoose && !m_config->doLooseEvents()) return true;

    if (!event.m_isLoose && !m_config->doTightEvents()) return true;

    double nominalWeight = event.m_info->auxdata<float>("AnalysisTop_eventWeight");

    if (m_doNominal) {
      double eventWeight = nominalWeight;

      if (event.m_isLoose) FillHistograms(m_hists_Loose, eventWeight, event);
      else FillHistograms(m_hists, eventWeight, event);
    }
    if (m_doRadHigh || m_doRadHighNoVar3c) {
      // 2 different names are acceptable
      double scaleWeight = 1.;
      if (m_PMGTruthWeights->hasWeight(" muR = 0.5, muF = 0.5 ")) scaleWeight = m_PMGTruthWeights->getWeight(" muR = 0.5, muF = 0.5 ");
      else if (m_PMGTruthWeights->hasWeight(" muR = 0.50, muF = 0.50 ")) scaleWeight = m_PMGTruthWeights->getWeight(" muR = 0.50, muF = 0.50 ");
      else if (m_PMGTruthWeights->hasWeight("MUR0.5_MUF0.5_PDF261000")) scaleWeight = m_PMGTruthWeights->getWeight("MUR0.5_MUF0.5_PDF261000"); // for e.g. Sherpa Z+jets
      else if (m_PMGTruthWeights->hasWeight(" muR=0.50000E+00 muF=0.50000E+00 ")) scaleWeight = m_PMGTruthWeights->getWeight(" muR=0.50000E+00 muF=0.50000E+00 "); // for e.g. ttZ DSID 410218
      else if (m_PMGTruthWeights->hasWeight(" dyn=   0 muR=0.50000E+00 muF=0.50000E+00 ")) scaleWeight = m_PMGTruthWeights->getWeight(" dyn=   0 muR=0.50000E+00 muF=0.50000E+00 "); // for e.g. tWZ 412118
      else if (m_PMGTruthWeights->hasWeight("1009")) scaleWeight = m_PMGTruthWeights->getWeight("1009"); // for e.g. tZ 412063
      else if (m_PMGTruthWeights->hasWeight(" dyn=  -1 muR=0.50000E+00 muF=0.50000E+00 ")) scaleWeight = m_PMGTruthWeights->getWeight(" dyn=  -1 muR=0.50000E+00 muF=0.50000E+00 "); // for e.g. ttZ 504330
      else if (m_PMGTruthWeights->hasWeight("muR=05,muF=05")) scaleWeight = m_PMGTruthWeights->getWeight("muR=05,muF=05"); // for some other generator setups
      else top::check(m_PMGTruthWeights->hasWeight(" muR = 0.5, muF = 0.5 "), "JetFlavorPlots::apply(): Weight \" muR = 0.5, muF = 0.5 \" not found. Please report this message!");
      double eventWeight = scaleWeight;
      if (!m_doRadHighNoVar3c) {
        top::check(m_PMGTruthWeights->hasWeight("Var3cUp"), "JetFlavorPlots::apply(): Weight \"Var3cUp\" not found. Please report this message!");
        eventWeight *= m_PMGTruthWeights->getWeight("Var3cUp") / nominalWeight;
        if (event.m_isLoose) FillHistograms(m_hists_RadHigh_Loose, eventWeight, event);
        else FillHistograms(m_hists_RadHigh, eventWeight, event);
      } // finish if (!m_doRadHighNoVar3c) 
      else { // m_doRadHighVar3c is true
        if (event.m_isLoose) FillHistograms(m_hists_RadHighNoVar3c_Loose, eventWeight, event);
        else FillHistograms(m_hists_RadHighNoVar3c, eventWeight, event);
      } // finish else 
    }
    if (m_doRadLow || m_doRadLowNoVar3c) {
      //2 different names are acceptable
      double scaleWeight = 1.;
      if (m_PMGTruthWeights->hasWeight(" muR = 2.0, muF = 2.0 ")) scaleWeight = m_PMGTruthWeights->getWeight(" muR = 2.0, muF = 2.0 ");
      else if (m_PMGTruthWeights->hasWeight(" muR = 2.00, muF = 2.00 ")) scaleWeight = m_PMGTruthWeights->getWeight(" muR = 2.00, muF = 2.00 ");
      else if (m_PMGTruthWeights->hasWeight("MUR2_MUF2_PDF261000")) scaleWeight = m_PMGTruthWeights->getWeight("MUR2_MUF2_PDF261000"); // for e.g. Sherpa Z+jets
      else if (m_PMGTruthWeights->hasWeight(" muR=0.20000E+01 muF=0.20000E+01 ")) scaleWeight = m_PMGTruthWeights->getWeight(" muR=0.20000E+01 muF=0.20000E+01 "); // for e.g. ttZ DSID 410218
      else if (m_PMGTruthWeights->hasWeight(" dyn=   0 muR=0.20000E+01 muF=0.20000E+01 ")) scaleWeight = m_PMGTruthWeights->getWeight(" dyn=   0 muR=0.20000E+01 muF=0.20000E+01 "); // for e.g. tWZ 412118
      else if (m_PMGTruthWeights->hasWeight("1005")) scaleWeight = m_PMGTruthWeights->getWeight("1005"); // for e.g. tZ 412063
      else if (m_PMGTruthWeights->hasWeight(" dyn=  -1 muR=0.20000E+01 muF=0.20000E+01 ")) scaleWeight = m_PMGTruthWeights->getWeight(" dyn=  -1 muR=0.20000E+01 muF=0.20000E+01 "); // for e.g. ttZ 504330
      else if (m_PMGTruthWeights->hasWeight("muR=20,muF=20")) scaleWeight = m_PMGTruthWeights->getWeight("muR=20,muF=20"); // for some other generator setups
      else top::check(m_PMGTruthWeights->hasWeight(" muR = 2.0, muF = 2.0 "), "JetFlavorPlots::apply(): Weight \" muR = 2.0, muF = 2.0 \" not found. Please report this message!");
      double eventWeight = scaleWeight;
      if (!m_doRadLowNoVar3c) {
        top::check(m_PMGTruthWeights->hasWeight("Var3cDown"), "JetFlavorPlots::apply(): Weight \"Var3cDown\" not found. Please report this message!");
        eventWeight *= m_PMGTruthWeights->getWeight("Var3cDown") / nominalWeight;
        if (event.m_isLoose) FillHistograms(m_hists_RadLow_Loose, eventWeight, event);
        else FillHistograms(m_hists_RadLow, eventWeight, event);
      } // finish if (!m_doRadLowNoVar3c) {
      else { // m_doRadLowNoVar3c is true
        if (event.m_isLoose) FillHistograms(m_hists_RadLowNoVar3c_Loose, eventWeight, event);
        else FillHistograms(m_hists_RadLowNoVar3c, eventWeight, event);
      } // finish else 
      
    }
    return true;
  }

  void JetFlavorPlots::FillHistograms(std::shared_ptr<PlotManager> h_ptr, double w_event, const top::Event& event) const {
    bool throwWarning = false;

    for (const auto* const jetPtr : event.m_jets) {
      int jet_flavor = -99;
      bool status = jetPtr->getAttribute<int>("PartonTruthLabelID", jet_flavor);
      if (!status) continue;

      if (m_detailed) {
        if ((unsigned int) m_nJetsMax >= event.m_jets.size()) {
          // vs. nJets and flavour
          char name[200];
          // PDG ID for gluon is 21
          if (jet_flavor == 21) sprintf(name, "gluon_jets_njet%lu_%s", event.m_jets.size(), m_jetCollection.c_str());
          // PDG ID for d,u,s is 1,2,3
          else if (jet_flavor >= 1 && jet_flavor <= 3) sprintf(name, "lightquark_jets_njet%lu_%s", event.m_jets.size(), m_jetCollection.c_str());
          // PDG ID for c is 4
          else if (jet_flavor == 4) sprintf(name, "cquark_jets_njet%lu_%s", event.m_jets.size(), m_jetCollection.c_str());
          // PDG ID for b is 5
          else if (jet_flavor == 5) sprintf(name, "bquark_jets_njet%lu_%s", event.m_jets.size(), m_jetCollection.c_str());
          else sprintf(name, "other_jets_njet%lu_%s", event.m_jets.size(), m_jetCollection.c_str());

          static_cast<TH2D*>(h_ptr->hist(name))->Fill(jetPtr->pt() * toGeV,
                                                      jetPtr->eta(),
                                                      w_event);

          // if "detailed", also making the quark_jets one
          if (jet_flavor >= 1 && jet_flavor <= 4) {
            sprintf(name, "quark_jets_%s", m_jetCollection.c_str());

            static_cast<TH2D*>(h_ptr->hist(name))->Fill(jetPtr->pt() * toGeV,
                                                        jetPtr->eta(),
                                                        w_event);
          }
        } else throwWarning = true;
        // vs. flavour only
        char shortname[200];
        // PDG ID for gluon is 21
        if (jet_flavor == 21) sprintf(shortname, "gluon_jets_%s", m_jetCollection.c_str());
        // PDG ID for d,u,s is 1,2,3
        else if (jet_flavor >= 1 && jet_flavor <= 3) sprintf(shortname, "lightquark_jets_%s", m_jetCollection.c_str());
        // PDG ID for c is 4
        else if (jet_flavor == 4) sprintf(shortname, "cquark_jets_%s", m_jetCollection.c_str());
        // PDG ID for b is 5
        else if (jet_flavor == 5) sprintf(shortname, "bquark_jets_%s", m_jetCollection.c_str());
        else sprintf(shortname, "other_jets_%s", m_jetCollection.c_str());

        static_cast<TH2D*>(h_ptr->hist(shortname))->Fill(jetPtr->pt() * toGeV,
                                                         jetPtr->eta(),
                                                         w_event);
      } else {
        // the simplest case, one for gluon, one for quarks
        char name[200];
        // PDG ID for gluon is 21
        if (jet_flavor == 21) sprintf(name, "gluon_jets_%s", m_jetCollection.c_str());
        // PDG ID for d,u,s is 1,2,3
        else if (jet_flavor >= 1 && jet_flavor <= 4) sprintf(name, "quark_jets_%s", m_jetCollection.c_str());
        // We are not interested in other PDG IDs
        else continue;

        static_cast<TH2D*>(h_ptr->hist(name))->Fill(jetPtr->pt() * toGeV,
                                                    jetPtr->eta(),
                                                    w_event);
      }
    }

    if (throwWarning){
      ATH_MSG_WARNING("Warning! Number of jets in the event is " << event.m_jets.size()
          << ", but histograms have been booked up to " << m_nJetsMax
          << ". Exclusive histograms in number of jets have not been filled.\n");
    }
  }

// function to translate the binnings into vector of bin edges
  void JetFlavorPlots::formatBinning(const std::string& str, std::vector<double>& binEdges) {
    std::stringstream ss(str);
    std::string tok;
    char separator = ':';
    while (std::getline(ss, tok, separator)) {
      binEdges.push_back(std::atof(tok.c_str()));
    }
  }

  std::string JetFlavorPlots::name() const {
    return "JETFLAVORPLOTS";
  }
}  // namespace top
