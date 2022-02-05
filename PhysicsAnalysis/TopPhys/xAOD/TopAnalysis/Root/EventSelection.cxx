/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopAnalysis/EventSelection.h"
#include "TopAnalysis/Tools.h"

#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include "TopCorrections/ScaleFactorRetriever.h"

#include "PATInterfaces/SystematicSet.h"

#include <iomanip>
#include <sstream>
#include <cassert>
#include <utility>

#include "TH1D.h"
#include "TFile.h"
#include "TClass.h"
#include "TROOT.h"

#include "TopParticleLevel/ParticleLevelEvent.h"

#include "TopEventSelectionTools/FakesMMConfigs.h"
#include "TopEventSelectionTools/NJetBtagSelector.h"

#include "TopAnalysis/MsgCategory.h"
using namespace TopAnalysis;

namespace top {
  EventSelection::EventSelection(const std::string& name, const std::vector<std::string>& cutNames, TFile* outputFile,
                                 const std::vector<std::unique_ptr<top::ToolLoaderBase> >& toolLoaders,
                                 std::shared_ptr<top::TopConfig> config, EL::Worker* wk) :
    m_cutflow(nullptr),
    m_cutflow_Loose(nullptr),
    m_cutflowMCWeights(nullptr),
    m_cutflowMCWeights_Loose(nullptr),
    m_cutflowPUWeights(nullptr),
    m_cutflowPUWeights_Loose(nullptr),
    m_cutflowMCPUWeights(nullptr),
    m_cutflowMCPUWeights_Loose(nullptr),
    m_cutflowScaleFactors(nullptr),
    m_cutflowScaleFactors_Loose(nullptr),
    m_cutflowParticleLevel(nullptr),
    m_cutflowParticleLevelMCWeights(nullptr),
    m_name(name),
    m_isMC(false),
    m_sfRetriever(nullptr),
    m_nominalHashValue(0),
    m_containsInitial(false),
    m_containsGRL(false),
    m_containsGoodCalo(false),
    m_containsPrimaryVertex(false),
    m_positionInitial(0),
    m_positionGRL(0),
    m_positionGoodCalo(0),
    m_positionPrimaryVertex(0),
    m_toBeSaved(false) {
    // We use the nominal hash value so we don't do
    // string comparisions during the event loop
    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();

    // need that later for the sf in the cutflow, not especially elegant but works for now
    m_config = config;
    m_isMC = config->isMC();

    if (m_isMC) {
      this->initialiseTopScaleFactorRetriever(m_config);
    }

    TDirectory* plotDir = outputFile->mkdir(m_name.c_str());
    plotDir->cd();
    if (config->doTightEvents()) {
      m_cutflow = new TH1D("cutflow", (name + " cutflow").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCWeights = new TH1D("cutflow_mc", (name + " cutflow MC weights").c_str(),
                                    cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowPUWeights = new TH1D("cutflow_pu", (name + " cutflow PU weights").c_str(),
                                    cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCPUWeights = new TH1D("cutflow_mc_pu", (name + " cutflow MC*PU weights").c_str(),
                                      cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowScaleFactors = new TH1D("cutflow_scale_factors", (name + " cutflow ScaleFactors").c_str(),
                                       cutNames.size(), -0.5, cutNames.size() - 0.5);
    }
    if (config->doLooseEvents()) {
      m_cutflow_Loose = new TH1D("cutflow_Loose", (name + " cutflow_Loose").c_str(),
                                 cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCWeights_Loose = new TH1D("cutflow_mc_Loose",
                                          (name + " cutflow_Loose MC weights").c_str(),
                                          cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowPUWeights_Loose = new TH1D("cutflow_pu_Loose",
                                          (name + " cutflow_Loose PU weights").c_str(),
                                          cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCPUWeights_Loose = new TH1D("cutflow_mc_pu_Loose",
                                            (name + " cutflow_Loose MC*PU weights").c_str(),
                                            cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowScaleFactors_Loose = new TH1D("cutflow_scale_factors_Loose",
                                             (name + " cutflow_Loose ScaleFactors").c_str(),
                                             cutNames.size(), -0.5, cutNames.size() - 0.5);
    }

    if (config->doTopParticleLevel()) {
      m_cutflowParticleLevel = new TH1D("cutflow_particle_level",
                                        (name + " cutflow_particle_level").c_str(),
                                        cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowParticleLevelMCWeights = new TH1D("cutflow_particle_level_mc",
                                                 (name + " cutflow_particle_level_mc").c_str(),
                                                 cutNames.size(), -0.5, cutNames.size() - 0.5);
    }

    unsigned int i(0);
    for (const auto& currentCutName : cutNames) {
      //look through all the libraries and load any tools
      top::EventSelectorBase* tool = nullptr;
      for (auto& libraryPtr : toolLoaders) {
        tool = libraryPtr->initTool(m_name, currentCutName, outputFile, config, wk);
        if (tool) break;
      }

      if (tool != nullptr) {
        m_allCuts.emplace_back(std::move(tool));
      } else {
        throw std::runtime_error("Could not find " + currentCutName);
      }

      // Initlal and GRL
      if (currentCutName == "INITIAL") {
        m_containsInitial = true;
        m_positionInitial = i;
      }

      if (currentCutName == "GRL") {
        m_containsGRL = true;
        m_positionGRL = i;
      }

      if (currentCutName == "GOODCALO") {
        m_containsGoodCalo = true;
        m_positionGoodCalo = i;
      }

      if (currentCutName == "PRIVTX") {
        m_containsPrimaryVertex = true;
        m_positionPrimaryVertex = i;
      }

      //some cutflow histograms
      if (config->doTightEvents()) {
        m_cutflow->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        m_cutflowMCWeights->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        m_cutflowPUWeights->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        m_cutflowMCPUWeights->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        m_cutflowScaleFactors->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
      }
      if (config->doLooseEvents()) {
        m_cutflow_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        m_cutflowMCWeights_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        m_cutflowPUWeights_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        m_cutflowMCPUWeights_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        m_cutflowScaleFactors_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
      }

      if (m_cutflowParticleLevel) {
        m_cutflowParticleLevel->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
      }

      if (m_cutflowParticleLevelMCWeights) {
        m_cutflowParticleLevelMCWeights->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
      }

      if (currentCutName == "SAVE") {
        m_toBeSaved = true;
      }

      ++i;
    }

    //be nice to the user and tell them what they enabled
    //at the very start of the run.  It allows them to quit the program
    //if they notice a problem.
    printCuts();
  }

  EventSelection::EventSelection(EventSelection&& other) :
    m_allCuts(std::move(other.m_allCuts)),
    m_cutflow(std::move(other.m_cutflow)),
    m_cutflow_Loose(std::move(other.m_cutflow_Loose)),
    m_cutflowMCWeights(std::move(other.m_cutflowMCWeights)),
    m_cutflowMCWeights_Loose(std::move(other.m_cutflowMCWeights_Loose)),
    m_cutflowPUWeights(std::move(other.m_cutflowPUWeights)),
    m_cutflowPUWeights_Loose(std::move(other.m_cutflowPUWeights_Loose)),
    m_cutflowMCPUWeights(std::move(other.m_cutflowMCPUWeights)),
    m_cutflowMCPUWeights_Loose(std::move(other.m_cutflowMCPUWeights_Loose)),
    m_cutflowScaleFactors(std::move(other.m_cutflowScaleFactors)),
    m_cutflowScaleFactors_Loose(std::move(other.m_cutflowScaleFactors_Loose)),
    m_cutflowParticleLevel(std::move(other.m_cutflowParticleLevel)),
    m_cutflowParticleLevelMCWeights(std::move(other.m_cutflowParticleLevelMCWeights)),
    m_name(std::move(other.m_name)),
    m_isMC(std::move(other.m_isMC)),
    m_config(std::move(other.m_config)),
    m_sfRetriever(nullptr),
    m_nominalHashValue(std::move(other.m_nominalHashValue)),
    m_containsInitial(std::move(other.m_containsInitial)),
    m_containsGRL(std::move(other.m_containsGRL)),
    m_containsGoodCalo(std::move(other.m_containsGoodCalo)),
    m_containsPrimaryVertex(std::move(other.m_containsPrimaryVertex)),
    m_positionInitial(std::move(other.m_positionInitial)),
    m_positionGRL(std::move(other.m_positionGRL)),
    m_positionGoodCalo(std::move(other.m_positionGoodCalo)),
    m_positionPrimaryVertex(std::move(other.m_positionPrimaryVertex)),
    m_toBeSaved(std::move(other.m_toBeSaved)) {
    // need this to calculate the lepton SF for the cutflow
    if (m_isMC) {
      this->initialiseTopScaleFactorRetriever(m_config);
    }
  }

  void EventSelection::initialiseTopScaleFactorRetriever(std::shared_ptr<TopConfig> config) {
    // Define this once, and then use it when needed
    if (asg::ToolStore::contains<ScaleFactorRetriever>("top::ScaleFactorRetriever")) {
      m_sfRetriever = asg::ToolStore::get<ScaleFactorRetriever>("top::ScaleFactorRetriever");
    } else {
      top::ScaleFactorRetriever* topSFR = new top::ScaleFactorRetriever("top::ScaleFactorRetriever");
      top::check(asg::setProperty(topSFR, "config", config), "Failed to set config");
      top::check(topSFR->initialize(), "Failed to initalialise");
      m_sfRetriever = topSFR;
    }
  }

  void EventSelection::countInitial(const float mcEventWeight, const float pileupWeight) const {
    if (m_containsInitial) {
      if (m_config->doTightEvents()) {
        m_cutflow->Fill(m_positionInitial);
        m_cutflowMCWeights->Fill(m_positionInitial, mcEventWeight);
        m_cutflowPUWeights->Fill(m_positionInitial, pileupWeight);
        m_cutflowMCPUWeights->Fill(m_positionInitial, mcEventWeight * pileupWeight);
      }
      if (m_config->doLooseEvents()) {
        m_cutflow_Loose->Fill(m_positionInitial);
        m_cutflowMCWeights_Loose->Fill(m_positionInitial, mcEventWeight);
        m_cutflowPUWeights_Loose->Fill(m_positionInitial, pileupWeight);
        m_cutflowMCPUWeights_Loose->Fill(m_positionInitial, mcEventWeight * pileupWeight);
      }
    }
  }

  void EventSelection::countGRL(const float mcEventWeight, const float pileupWeight) const {
    if (m_containsGRL) {
      if (m_config->doTightEvents()) {
        m_cutflow->Fill(m_positionGRL);
        m_cutflowMCWeights->Fill(m_positionGRL, mcEventWeight);
        m_cutflowPUWeights->Fill(m_positionGRL, pileupWeight);
        m_cutflowMCPUWeights->Fill(m_positionGRL, mcEventWeight * pileupWeight);
      }
      if (m_config->doLooseEvents()) {
        m_cutflow_Loose->Fill(m_positionGRL);
        m_cutflowMCWeights_Loose->Fill(m_positionGRL, mcEventWeight);
        m_cutflowPUWeights_Loose->Fill(m_positionGRL, pileupWeight);
        m_cutflowMCPUWeights_Loose->Fill(m_positionGRL, mcEventWeight * pileupWeight);
      }
    }
  }

  void EventSelection::countGoodCalo(const float mcEventWeight, const float pileupWeight) const {
    if (m_containsGoodCalo) {
      if (m_config->doTightEvents()) {
        m_cutflow->Fill(m_positionGoodCalo);
        m_cutflowMCWeights->Fill(m_positionGoodCalo, mcEventWeight);
        m_cutflowPUWeights->Fill(m_positionGoodCalo, pileupWeight);
        m_cutflowMCPUWeights->Fill(m_positionGoodCalo, mcEventWeight * pileupWeight);
      }
      if (m_config->doLooseEvents()) {
        m_cutflow_Loose->Fill(m_positionGoodCalo);
        m_cutflowMCWeights_Loose->Fill(m_positionGoodCalo, mcEventWeight);
        m_cutflowPUWeights_Loose->Fill(m_positionGoodCalo, pileupWeight);
        m_cutflowMCPUWeights_Loose->Fill(m_positionGoodCalo, mcEventWeight * pileupWeight);
      }
    }
  }

  void EventSelection::countPrimaryVertex(const float mcEventWeight, const float pileupWeight) const {
    if (m_containsPrimaryVertex) {
      if (m_config->doTightEvents()) {
        m_cutflow->Fill(m_positionPrimaryVertex);
        m_cutflowMCWeights->Fill(m_positionPrimaryVertex, mcEventWeight);
        m_cutflowPUWeights->Fill(m_positionPrimaryVertex, pileupWeight);
        m_cutflowMCPUWeights->Fill(m_positionPrimaryVertex, mcEventWeight * pileupWeight);
      }
      if (m_config->doLooseEvents()) {
        m_cutflow_Loose->Fill(m_positionPrimaryVertex);
        m_cutflowMCWeights_Loose->Fill(m_positionPrimaryVertex, mcEventWeight);
        m_cutflowPUWeights_Loose->Fill(m_positionPrimaryVertex, pileupWeight);
        m_cutflowMCPUWeights_Loose->Fill(m_positionPrimaryVertex, mcEventWeight * pileupWeight);
      }
    }
  }

  bool EventSelection::apply(const top::Event& event) const {
    unsigned int i(0);
    bool passEvent(false);

    for (const auto& currentCut : m_allCuts) {
      const bool passed = currentCut->apply(event);

      if (!passed) break;

      double mcweight = 1.;
      double puweight = 1.;
      double leptonSF = 1.;

      if (m_isMC) {
          mcweight = event.m_info->auxdataConst<float>("AnalysisTop_eventWeight");

          if (top::ScaleFactorRetriever::hasPileupSF(event)) puweight = top::ScaleFactorRetriever::pileupSF(event);

          leptonSF = m_sfRetriever->leptonSF(event, top::topSFSyst::nominal);
      }

      //add cutflow information for the nominal (not systematic) selection
      //For data we have (sometimes) the same event with loose and tight
      //The cutflow is just the "analysis" -> tight to avoid double counting
      if (event.m_hashValue == m_nominalHashValue) {
        bool countThisCut(true);
        if (m_containsInitial && i == m_positionInitial) {
          countThisCut = false;
        }
        if (m_containsGRL && i == m_positionGRL) {
          countThisCut = false;
        }
        if (m_containsGoodCalo && i == m_positionGoodCalo) {
          countThisCut = false;
        }
        if (m_containsPrimaryVertex && i == m_positionPrimaryVertex) {
          countThisCut = false;
        }
        if (countThisCut) {
          if (m_config->doTightEvents() && !event.m_isLoose) {
            m_cutflow->Fill(i);
            m_cutflowMCWeights->Fill(i, mcweight);
            m_cutflowPUWeights->Fill(i, puweight);
            m_cutflowMCPUWeights->Fill(i, mcweight * puweight);
            m_cutflowScaleFactors->Fill(i, leptonSF);
          }
          if (m_config->doLooseEvents() && event.m_isLoose) {
            m_cutflow_Loose->Fill(i);
            m_cutflowMCWeights_Loose->Fill(i, mcweight);
            m_cutflowPUWeights_Loose->Fill(i, puweight);
            m_cutflowMCPUWeights_Loose->Fill(i, mcweight * puweight);
            m_cutflowScaleFactors_Loose->Fill(i, leptonSF);
          }
        }
      }

      passEvent |= (currentCut->name() == "SAVE");
      ++i;
    }

    // If "SAVE" wasn't found but event passes all cuts, event passes the selection
    passEvent |= (i == m_allCuts.size());

    return passEvent;
  }

  bool EventSelection::applyParticleLevel(const top::ParticleLevelEvent& plEvent) const {
    // In principle, this function should never be called for non-active
    // particle level. However, for code safetly, include a null pointer
    // check. Return false here because "do-not-do-particle-level" is equivalent
    // to "do-no-save-particle-level".
    if (not m_cutflowParticleLevel) {
      return false;
    }
    if (not m_cutflowParticleLevelMCWeights) {
      return false;
    }

    unsigned int i(0);
    bool passEvent(false);

    for (const auto& currentCut : m_allCuts) {
      const bool passed = currentCut->applyParticleLevel(plEvent);

      if (!passed) break;

      m_cutflowParticleLevel->Fill(i);

      double mcweight = plEvent.m_info->mcEventWeight(0);
      m_cutflowParticleLevelMCWeights->Fill(i, mcweight);

      passEvent |= (currentCut->name() == "SAVE");
      ++i;
    }

    // If "SAVE" wasn't found but event passes all cuts, event passes the selection
    passEvent |= (i == m_allCuts.size());

    return passEvent;
  }

  void EventSelection::finalise() const {
    //2dp, neater output for numbers
    std::ostream& msgInfo = msg(MSG::Level::INFO);
    msgInfo << std::right;
    if (m_isMC) msgInfo << std::fixed << std::setprecision(2);

    if (m_config->doTightEvents()) {
      //channel name
      msgInfo << " - " << m_name << " cutflow:\n";

      //some headings
      msgInfo << std::setw(7) << "" <<
        std::setw(30) << "cut" <<
        std::setw(15) << "events";

      if (m_isMC)
        msgInfo << std::setw(15) << "mc weights" <<
          std::setw(15) << "mc*pu weights" <<
          std::setw(15) << "lepton SF" <<
          std::setw(15) << "b-tag SF";

      if (m_cutflowParticleLevel) {
        msgInfo << std::setw(15) << "particle level";
      }

      if (m_cutflowParticleLevelMCWeights) {
        msgInfo << std::setw(15) << "particle level mc";
      }

      msgInfo << "\n";

      //cutflow table content
      for (int i = 1; i <= m_cutflow->GetNbinsX(); ++i) {
        msgInfo << "    " << std::setw(3) << i
          << std::setw(30) << m_cutflow->GetXaxis()->GetBinLabel(i)
          << std::setw(15) << m_cutflow->GetBinContent(i);

        if (m_isMC)
          msgInfo << std::setw(15) << m_cutflowMCWeights->GetBinContent(i)
            << std::setw(15) << m_cutflowMCPUWeights->GetBinContent(i)
            << std::setw(15) << m_cutflowScaleFactors->GetBinContent(i);

        if (m_cutflowParticleLevel) {
          msgInfo << std::setw(15) << m_cutflowParticleLevel->GetBinContent(i);
        }

        if (m_cutflowParticleLevelMCWeights) {
          msgInfo << std::setw(15) << m_cutflowParticleLevelMCWeights->GetBinContent(i);
        }

        msgInfo << "\n";
      }
    }
    if (m_config->doLooseEvents()) {
      //channel name
      msgInfo << " - " << m_name << " cutflow (Loose):\n";

      //some headings
      msgInfo << std::setw(7) << "" <<
        std::setw(30) << "cut" <<
        std::setw(15) << "events";

      if (m_isMC)
        msgInfo << std::setw(15) << "mc weights" <<
          std::setw(15) << "mc*pu weights" <<
          std::setw(15) << "lepton SF" <<
          std::setw(15) << "b-tag SF";

      if (m_cutflowParticleLevel) {
        msgInfo << std::setw(15) << "particle level";
      }

      if (m_cutflowParticleLevelMCWeights) {
        msgInfo << std::setw(15) << "particle level mc";
      }

      msgInfo << "\n";

      //cutflow table content
      for (int i = 1; i <= m_cutflow_Loose->GetNbinsX(); ++i) {
        msgInfo << "    " << std::setw(3) << i
          << std::setw(30) << m_cutflow_Loose->GetXaxis()->GetBinLabel(i)
          << std::setw(15) << m_cutflow_Loose->GetBinContent(i);

        if (m_isMC)
          msgInfo << std::setw(15) << m_cutflowMCWeights_Loose->GetBinContent(i)
            << std::setw(15) << m_cutflowMCPUWeights_Loose->GetBinContent(i)
            << std::setw(15) << m_cutflowScaleFactors_Loose->GetBinContent(i);

        if (m_cutflowParticleLevel) {
          msgInfo << std::setw(15) << m_cutflowParticleLevel->GetBinContent(i);
        }

        if (m_cutflowParticleLevelMCWeights) {
          msgInfo << std::setw(15) << m_cutflowParticleLevelMCWeights->GetBinContent(i);
        }

        msgInfo << "\n";
      }
    }
    msgInfo << "\n";
  }

  const std::string EventSelection::name() const {
    return m_name;
  }

  std::vector<std::string> EventSelection::GetFakesMMConfigs() const {
    std::vector<std::string> configs;
    for (const auto& currentCutName : m_allCuts) {
      if (currentCutName->name() != "FAKESMMCONFIGS") continue;
      else {
        FakesMMConfigs* conf = dynamic_cast<FakesMMConfigs*>(currentCutName.get());
        for (std::string s : conf->configurations())
          configs.push_back(s);
      }
    }
    return configs;
  }

  void EventSelection::printCuts() {
    std::ostream& msgInfo = msg(MSG::Level::INFO);
    msgInfo << "\n - " << m_name << ":\n";
    if (m_config->doTightEvents()) {
      for (int i = 1; i <= m_cutflow->GetNbinsX(); ++i) {
        msgInfo << "    " << std::setw(3) << i
          << std::setw(30) << m_cutflow->GetXaxis()->GetBinLabel(i)
          << "\n";
      }
    } else if (m_config->doLooseEvents()) {
      for (int i = 1; i <= m_cutflow_Loose->GetNbinsX(); ++i) {
        msgInfo << "    " << std::setw(3) << i
          << std::setw(30) << m_cutflow_Loose->GetXaxis()->GetBinLabel(i)
          << "\n";
      }
    }
    msgInfo << std::endl;
  }
}
