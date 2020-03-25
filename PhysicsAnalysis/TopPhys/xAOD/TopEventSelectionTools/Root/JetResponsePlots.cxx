/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/JetResponsePlots.h"

#include "AsgTools/AsgTool.h"
#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "TopCorrections/ScaleFactorRetriever.h"
#include "PATInterfaces/SystematicSet.h"
#include "xAODJet/JetContainer.h"
#include "PMGTools/PMGTruthWeightTool.h"

#include "TH1.h"
#include "TH2D.h"
#include "TFile.h"

//using namespace TopEventSelectionTools;
namespace top {

  JetResponsePlots::JetResponsePlots(const std::string& name,
                                     TFile* outputFile,
                                     const std::string& params,
                                     std::shared_ptr<top::TopConfig> config,
                                     EL::Worker* wk) :
    m_nominalHashValue(0),
    m_jetCollection(""),
    m_deltaR(0.3),
    m_bins(30),
    m_min(-3),
    m_max(3),
    m_config(config),
    m_PMGTruthWeights(nullptr) {

    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();

    // retrieve jet collection and remove the "Jets" at the end of it
    m_jetCollection = m_config->sgKeyTruthJets();
    //retrieve PMGTruthWeights
    static const std::string truthWeightToolName = "PMGTruthWeightTool";
    if (asg::ToolStore::contains<PMGTools::IPMGTruthWeightTool>(truthWeightToolName)) m_PMGTruthWeights =
        asg::ToolStore::get<PMGTools::IPMGTruthWeightTool>(truthWeightToolName);
    else {
      std::unique_ptr<PMGTools::PMGTruthWeightTool> pmgtruthtool = std::make_unique<PMGTools::PMGTruthWeightTool>(truthWeightToolName);
      top::check(pmgtruthtool->initialize(), "Failed to initialize " + truthWeightToolName);
      m_PMGTruthWeights = pmgtruthtool.release();
    }
    //decoding arguments
    std::istringstream stream(params);
    std::string s;
    while (stream >> s) {
      if (s.substr(0,7) == "deltaR:") {
        m_deltaR = std::stof(s.substr(7, s.size() - 7));
        m_config->setJetResponseMatchingDeltaR(m_deltaR);
      } else if (s.substr(0,5) == "bins:") {
        m_bins = std::stoi(s.substr(5, s.size() - 5));
      } else if (s.substr(0,4) == "min:") {
        m_min = std::stof(s.substr(4, s.size() - 4));
      } else if (s.substr(0,4) == "max:") {
        m_max = std::stof(s.substr(4, s.size() - 4));
      } else {
        throw std::runtime_error("ERROR: Can't understand argument " + s + "For JETRESPONSEPLOTS");
      }
    }

    m_hists = std::make_shared<PlotManager>(name + "/JetResponsePlots", outputFile, wk);
    m_hists->addHist("JetResponse", ";p_{T}^{truth}-p_{T}^{reco}/p_{T}^{truth};Events", m_bins, m_min, m_max); 
  }

  bool JetResponsePlots::apply(const top::Event& event) const {
    //only MC
    if (!top::isSimulation(event)) return true;

    // only nominal
    if (event.m_hashValue != m_nominalHashValue) return true;

    // do loose or tight events only if requested
    if (event.m_isLoose && !m_config->doLooseEvents()) return true;

    if (!event.m_isLoose && !m_config->doTightEvents()) return true;

    const double nominalWeight = event.m_info->auxdata<float>("AnalysisTop_eventWeight");

    FillHistograms(m_hists, nominalWeight, event);
      
    return true;
  }

  void JetResponsePlots::FillHistograms(std::shared_ptr<PlotManager> h_ptr,
                                        const double w_event,
                                        const top::Event& event) const {

    for (const auto* const jetPtr : event.m_jets) {
      int jet_flavor = -99;
      bool status = jetPtr->getAttribute<int>("PartonTruthLabelID", jet_flavor);
      if (!status) continue;

      float matchedPt = -9999;
      status = jetPtr->getAttribute<float>("AnalysisTop_MatchedTruthJetPt", matchedPt);
      if (!status) continue;
      
      // protection against division by zero
      if (matchedPt < 1e-6) continue;

      const float response = (matchedPt - jetPtr->pt()) / matchedPt;

      static_cast<TH1D*>(h_ptr->hist("JetResponse"))->Fill(response,
                                                           w_event);
    }

  }

  std::string JetResponsePlots::name() const {
    return "JETRESPONSEPLOTS";
  }
}  // namespace top
