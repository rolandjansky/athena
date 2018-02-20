/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

namespace top {

EventSelection::EventSelection(const std::string& name, const std::vector<std::string>& cutNames, TFile* outputFile, const std::vector<std::unique_ptr<top::ToolLoaderBase>>& toolLoaders,std::shared_ptr<top::TopConfig> config, EL::Worker* wk) :
            m_cutflow(nullptr),
            m_cutflow_Loose(nullptr),
            m_cutflowMCWeights(nullptr),
            m_cutflowMCWeights_Loose(nullptr),
            m_cutflowPUWeights(nullptr),
            m_cutflowPUWeights_Loose(nullptr),
            m_cutflowZVtxWeights(nullptr),
            m_cutflowZVtxWeights_Loose(nullptr),
            m_cutflowMCPUWeights(nullptr),
            m_cutflowMCPUWeights_Loose(nullptr),
            m_cutflowMCPUZVtxWeights(nullptr),
            m_cutflowMCPUZVtxWeights_Loose(nullptr),
            m_cutflowScaleFactors(nullptr),
	    m_cutflowScaleFactors_Loose(nullptr),
            m_cutflowBScaleFactors(nullptr),
            m_cutflowBScaleFactors_Loose(nullptr),
	    m_cutflowParticleLevel(nullptr),
	    m_cutflowParticleLevelMCWeights(nullptr),
            m_cutflowUpgradeLevel(nullptr),
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
            m_toBeSaved(false)
{

    // We use the nominal hash value so we don't do
    // string comparisions during the event loop
    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();
    
    // need that later for the sf in the cutflow, not especially elegant but works for now
    m_config = config;
    m_isMC   = config->isMC();

    if(m_isMC)
      m_sfRetriever = std::unique_ptr<top::ScaleFactorRetriever> ( new top::ScaleFactorRetriever( config) );

    TDirectory* plotDir = outputFile->mkdir(m_name.c_str());
    plotDir->cd();
    if (config->doTightEvents()) {
      m_cutflow = new TH1D("cutflow", (name + " cutflow").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCWeights = new TH1D("cutflow_mc", (name + " cutflow MC weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowPUWeights = new TH1D("cutflow_pu", (name + " cutflow PU weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowZVtxWeights = new TH1D("cutflow_zvtx", (name + " cutflow ZVtx weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCPUWeights = new TH1D("cutflow_mc_pu", (name + " cutflow MC*PU weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCPUZVtxWeights = new TH1D("cutflow_mc_pu_zvtx", (name + " cutflow MC*PU*ZVtx weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowScaleFactors = new TH1D("cutflow_scale_factors", (name + " cutflow ScaleFactors").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowBScaleFactors = new TH1D("cutflow_btag_scale_factors", (name + " cutflow b-tag ScaleFactors").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
    }
    if (config->doLooseEvents()) {
      m_cutflow_Loose = new TH1D("cutflow_Loose", (name + " cutflow_Loose").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCWeights_Loose = new TH1D("cutflow_mc_Loose", (name + " cutflow_Loose MC weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowPUWeights_Loose = new TH1D("cutflow_pu_Loose", (name + " cutflow_Loose PU weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowZVtxWeights_Loose = new TH1D("cutflow_zvtx_Loose", (name + " cutflow_Loose ZVtx weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCPUWeights_Loose = new TH1D("cutflow_mc_pu_Loose", (name + " cutflow_Loose MC*PU weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowMCPUZVtxWeights_Loose = new TH1D("cutflow_mc_pu_zvtx_Loose", (name + " cutflow_Loose MC*PU*ZVtx weights").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowScaleFactors_Loose = new TH1D("cutflow_scale_factors_Loose", (name + " cutflow_Loose ScaleFactors").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
      m_cutflowBScaleFactors_Loose = new TH1D("cutflow_btag_scale_factors_Loose", (name + " cutflow_Loose b-tag ScaleFactors").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
    }

    if ( config->doTopParticleLevel() ){
	m_cutflowParticleLevel = new TH1D("cutflow_particle_level", (name + " cutflow_particle_level").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
	m_cutflowParticleLevelMCWeights = new TH1D("cutflow_particle_level_mc", (name + " cutflow_particle_level_mc").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
    }

    if ( config->HLLHC() ){
       m_cutflowUpgradeLevel = new TH1D("cutflow_upgrade_level", (name + " cutflow_upgrade_level").c_str(), cutNames.size(), -0.5, cutNames.size() - 0.5);
    }

    unsigned int i(0);
    for (const auto& currentCutName : cutNames) {

        //look through all the libraries and load any tools
        top::EventSelectorBase* tool = nullptr;
        for (auto& libraryPtr : toolLoaders) {
            tool = libraryPtr->initTool(m_name, currentCutName, outputFile, config, wk);
            if (tool)
                break;
        }

        if (tool != nullptr) {
            m_allCuts.emplace_back(std::move(tool));
        } else {
            std::cout << "Could not find " << currentCutName << std::endl;
            exit(1);
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
          m_cutflowZVtxWeights->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowMCPUWeights->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowMCPUZVtxWeights->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowScaleFactors->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowBScaleFactors->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        }
        if (config->doLooseEvents()) {
          m_cutflow_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowMCWeights_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowPUWeights_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowZVtxWeights_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowMCPUWeights_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowMCPUZVtxWeights_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowScaleFactors_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
          m_cutflowBScaleFactors_Loose->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
        }

	if ( m_cutflowParticleLevel ){
	    m_cutflowParticleLevel->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
	}

	if ( m_cutflowParticleLevelMCWeights ){
	    m_cutflowParticleLevelMCWeights->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
	}

        if ( m_cutflowUpgradeLevel ){
            m_cutflowUpgradeLevel->GetXaxis()->SetBinLabel(i + 1, m_allCuts[i]->name().c_str());
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
            m_cutflowZVtxWeights(std::move(other.m_cutflowZVtxWeights)),
            m_cutflowZVtxWeights_Loose(std::move(other.m_cutflowZVtxWeights_Loose)),
            m_cutflowMCPUWeights(std::move(other.m_cutflowMCPUWeights)),
            m_cutflowMCPUWeights_Loose(std::move(other.m_cutflowMCPUWeights_Loose)),
            m_cutflowMCPUZVtxWeights(std::move(other.m_cutflowMCPUZVtxWeights)),
            m_cutflowMCPUZVtxWeights_Loose(std::move(other.m_cutflowMCPUZVtxWeights_Loose)),
            m_cutflowScaleFactors(std::move(other.m_cutflowScaleFactors)),
	    m_cutflowScaleFactors_Loose(std::move(other.m_cutflowScaleFactors_Loose)),
            m_cutflowBScaleFactors(std::move(other.m_cutflowBScaleFactors)),
	    m_cutflowBScaleFactors_Loose(std::move(other.m_cutflowBScaleFactors_Loose)),
	    m_cutflowParticleLevel(std::move(other.m_cutflowParticleLevel)),
	    m_cutflowParticleLevelMCWeights(std::move(other.m_cutflowParticleLevelMCWeights)),
            m_cutflowUpgradeLevel(std::move(other.m_cutflowUpgradeLevel)),
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
            m_toBeSaved(std::move(other.m_toBeSaved))
{

  // need this to calculate the lepton SF for the cutflow
  if(m_isMC)
    m_sfRetriever = std::unique_ptr<top::ScaleFactorRetriever> ( new top::ScaleFactorRetriever( m_config ) );

}

void EventSelection::countInitial(const float mcEventWeight,const float pileupWeight,const float zvtxWeight) const
{
  if (m_containsInitial) {
    if (m_config->doTightEvents()) {
      m_cutflow->Fill(m_positionInitial);
      m_cutflowMCWeights->Fill(m_positionInitial,mcEventWeight);
      m_cutflowPUWeights->Fill(m_positionInitial,pileupWeight);
      m_cutflowZVtxWeights->Fill(m_positionInitial,zvtxWeight);
      m_cutflowMCPUWeights->Fill(m_positionInitial,mcEventWeight*pileupWeight);
      m_cutflowMCPUZVtxWeights->Fill(m_positionInitial,mcEventWeight*pileupWeight*zvtxWeight);
    }
    if (m_config->doLooseEvents()) {
      m_cutflow_Loose->Fill(m_positionInitial);
      m_cutflowMCWeights_Loose->Fill(m_positionInitial,mcEventWeight);
      m_cutflowPUWeights_Loose->Fill(m_positionInitial,pileupWeight);
      m_cutflowZVtxWeights_Loose->Fill(m_positionInitial,zvtxWeight);
      m_cutflowMCPUWeights_Loose->Fill(m_positionInitial,mcEventWeight*pileupWeight);
      m_cutflowMCPUZVtxWeights_Loose->Fill(m_positionInitial,mcEventWeight*pileupWeight*zvtxWeight);
    }
  }
}

void EventSelection::countGRL(const float mcEventWeight,const float pileupWeight,const float zvtxWeight) const
{
  if (m_containsGRL) {
    if (m_config->doTightEvents()) {
      m_cutflow->Fill(m_positionGRL);
      m_cutflowMCWeights->Fill(m_positionGRL,mcEventWeight);
      m_cutflowPUWeights->Fill(m_positionGRL,pileupWeight);
      m_cutflowZVtxWeights->Fill(m_positionGRL,zvtxWeight);
      m_cutflowMCPUWeights->Fill(m_positionGRL,mcEventWeight*pileupWeight);
      m_cutflowMCPUZVtxWeights->Fill(m_positionGRL,mcEventWeight*pileupWeight*zvtxWeight);
    }
    if (m_config->doLooseEvents()) {
      m_cutflow_Loose->Fill(m_positionGRL);
      m_cutflowMCWeights_Loose->Fill(m_positionGRL,mcEventWeight);
      m_cutflowPUWeights_Loose->Fill(m_positionGRL,pileupWeight);
      m_cutflowZVtxWeights_Loose->Fill(m_positionGRL,zvtxWeight);
      m_cutflowMCPUWeights_Loose->Fill(m_positionGRL,mcEventWeight*pileupWeight);
      m_cutflowMCPUZVtxWeights_Loose->Fill(m_positionGRL,mcEventWeight*pileupWeight*zvtxWeight);
    }
  }
}

void EventSelection::countGoodCalo(const float mcEventWeight,const float pileupWeight,const float zvtxWeight) const
{
  if (m_containsGoodCalo) {
    if (m_config->doTightEvents()) {
      m_cutflow->Fill(m_positionGoodCalo);
      m_cutflowMCWeights->Fill(m_positionGoodCalo,mcEventWeight);
      m_cutflowPUWeights->Fill(m_positionGoodCalo,pileupWeight);
      m_cutflowZVtxWeights->Fill(m_positionGoodCalo,zvtxWeight);
      m_cutflowMCPUWeights->Fill(m_positionGoodCalo,mcEventWeight*pileupWeight);
      m_cutflowMCPUZVtxWeights->Fill(m_positionGoodCalo,mcEventWeight*pileupWeight*zvtxWeight);
    }
    if (m_config->doLooseEvents()) {
      m_cutflow_Loose->Fill(m_positionGoodCalo);
      m_cutflowMCWeights_Loose->Fill(m_positionGoodCalo,mcEventWeight);
      m_cutflowPUWeights_Loose->Fill(m_positionGoodCalo,pileupWeight);
      m_cutflowZVtxWeights_Loose->Fill(m_positionGoodCalo,zvtxWeight);
      m_cutflowMCPUWeights_Loose->Fill(m_positionGoodCalo,mcEventWeight*pileupWeight);
      m_cutflowMCPUZVtxWeights_Loose->Fill(m_positionGoodCalo,mcEventWeight*pileupWeight*zvtxWeight);
    }
  }
}

void EventSelection::countPrimaryVertex(const float mcEventWeight,const float pileupWeight,const float zvtxWeight) const
{
  if (m_containsPrimaryVertex) {
    if (m_config->doTightEvents()) {
      m_cutflow->Fill(m_positionPrimaryVertex);
      m_cutflowMCWeights->Fill(m_positionPrimaryVertex,mcEventWeight);
      m_cutflowPUWeights->Fill(m_positionPrimaryVertex,pileupWeight);
      m_cutflowZVtxWeights->Fill(m_positionPrimaryVertex,zvtxWeight);
      m_cutflowMCPUWeights->Fill(m_positionPrimaryVertex,mcEventWeight*pileupWeight);
      m_cutflowMCPUZVtxWeights->Fill(m_positionPrimaryVertex,mcEventWeight*pileupWeight*zvtxWeight);
    }
    if (m_config->doLooseEvents()) {
      m_cutflow_Loose->Fill(m_positionPrimaryVertex);
      m_cutflowMCWeights_Loose->Fill(m_positionPrimaryVertex,mcEventWeight);
      m_cutflowPUWeights_Loose->Fill(m_positionPrimaryVertex,pileupWeight);
      m_cutflowZVtxWeights_Loose->Fill(m_positionPrimaryVertex,zvtxWeight);
      m_cutflowMCPUWeights_Loose->Fill(m_positionPrimaryVertex,mcEventWeight*pileupWeight);
      m_cutflowMCPUZVtxWeights_Loose->Fill(m_positionPrimaryVertex,mcEventWeight*pileupWeight*zvtxWeight);
    }
  }
}

bool EventSelection::apply(const top::Event& event) const {
    unsigned int i(0);
    bool passEvent(false);

    for (const auto& currentCut : m_allCuts) {
        const bool passed = currentCut->apply(event);
        //std::cout << (*it)->name() << " " << passed << std::endl;

        if (!passed)
            break;

        double mcweight        = 1.;
        double puweight        = 1.;
        double zvtxweight      = 1.;
	double leptonSF        = 1.;
 	double btagSF          = 1.;

        if (m_isMC) {
//             mcweight = event.m_info->mcEventWeight();
            mcweight = event.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix

            if (top::ScaleFactorRetriever::hasPileupSF(event))
                puweight = top::ScaleFactorRetriever::pileupSF(event);

	    leptonSF = m_sfRetriever -> leptonSF(event, top::topSFSyst::nominal);

	    for (auto& tag : ( m_config->useTrackJets() ? m_config->bTagWP_available_trkJet() : m_config->bTagWP_available())){
	      btagSF = m_sfRetriever -> btagSF(event, top::topSFSyst::nominal, tag, m_config->useTrackJets());
	    }

        }

        //add cutflow information for the nominal (not systematic) selection
        //For data we have (sometimes) the same event with loose and tight
        //The cutflow is just the "analysis" -> tight to avoid double counting
        if (event.m_hashValue == m_nominalHashValue) {
          bool countThisCut(true);
          if ( m_containsInitial && i == m_positionInitial) {countThisCut = false;}
          if ( m_containsGRL && i == m_positionGRL) {countThisCut = false;}
          if ( m_containsGoodCalo && i == m_positionGoodCalo) {countThisCut = false;}
          if ( m_containsPrimaryVertex && i == m_positionPrimaryVertex) {countThisCut = false;}
          if (countThisCut) {
            if (m_config->doTightEvents() && !event.m_isLoose) {
              m_cutflow->Fill(i);
              m_cutflowMCWeights->Fill(i, mcweight);
              m_cutflowPUWeights->Fill(i, puweight);
              m_cutflowZVtxWeights->Fill(i, zvtxweight);
              m_cutflowMCPUWeights->Fill(i, mcweight * puweight);
              m_cutflowMCPUZVtxWeights->Fill(i, mcweight * puweight * zvtxweight);
              m_cutflowScaleFactors->Fill(i, leptonSF);
              m_cutflowBScaleFactors->Fill(i, btagSF);
            }
            if (m_config->doLooseEvents() && event.m_isLoose) {
              m_cutflow_Loose->Fill(i);
              m_cutflowMCWeights_Loose->Fill(i, mcweight);
              m_cutflowPUWeights_Loose->Fill(i, puweight);
              m_cutflowZVtxWeights_Loose->Fill(i, zvtxweight);
              m_cutflowMCPUWeights_Loose->Fill(i, mcweight * puweight);
              m_cutflowMCPUZVtxWeights_Loose->Fill(i, mcweight * puweight * zvtxweight);
              m_cutflowScaleFactors_Loose->Fill(i, leptonSF);
              m_cutflowBScaleFactors_Loose->Fill(i, btagSF);
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
    if ( not m_cutflowParticleLevel ){ return false; }
    if ( not m_cutflowParticleLevelMCWeights ){ return false; }

    unsigned int i(0);
    bool passEvent(false);

    for (const auto& currentCut : m_allCuts) {
	const bool passed = currentCut->applyParticleLevel( plEvent );
	//std::cout << (*it)->name() << " " << passed << std::endl;

	if (!passed)
	    break;

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

bool EventSelection::applyUpgradeLevel(const top::ParticleLevelEvent& upgradeEvent) const {
    // In principle, this function should never be called for non-active
    // upgrade level. However, for code safetly, include a null pointer
    // check. Return false here because "do-not-do-particle-level" is equivalent
    // to "do-no-save-particle-level".
    if ( not m_cutflowUpgradeLevel ){ return false; }

    unsigned int i(0);
    bool passEvent(false);

    for (const auto& currentCut : m_allCuts) {
      // we use applyParticleLevel, because in the end upgrade events are smeared
      // truth level, so we can just re-use this function
      const bool passed = currentCut->applyParticleLevel( upgradeEvent );
      //std::cout << (*it)->name() << " " << passed << std::endl;

      if (!passed)
       break;

      m_cutflowUpgradeLevel->Fill(i);

      passEvent |= (currentCut->name() == "SAVE");
      ++i;
    }

    // If "SAVE" wasn't found but event passes all cuts, event passes the selection
    passEvent |= (i == m_allCuts.size());

    return passEvent;
}

void EventSelection::finalise() const {
    //2dp, neater output for numbers
    std::cout << std::right;
    if (m_isMC)
        std::cout << std::fixed << std::setprecision(2);

  if (m_config->doTightEvents()) {
    //channel name
    std::cout << " - " << m_name << " cutflow:\n";

    //some headings
    std::cout << std::setw(7) << "" <<
            std::setw(30) << "cut" <<
            std::setw(15) << "events";

    if (m_isMC)
        std::cout << std::setw(15) << "mc weights" <<
	  std::setw(15) << "mc*pu weights" <<
	  std::setw(15) << "lepton SF" <<
	  std::setw(15) << "b-tag SF" ;

    if( m_cutflowParticleLevel ) {
	std::cout << std::setw(15) << "particle level";
    }

    if( m_cutflowParticleLevelMCWeights ) {
	std::cout << std::setw(15) << "particle level mc";
    }
 
    if( m_cutflowUpgradeLevel ) {
       std::cout << std::setw(15) << "upgrade level";
    }

    std::cout << "\n";

    //cutflow table content
    for (int i = 1; i <= m_cutflow->GetNbinsX(); ++i) {
        std::cout << "    " << std::setw(3) << i
                << std::setw(30) << m_cutflow->GetXaxis()->GetBinLabel(i)
                << std::setw(15) << m_cutflow->GetBinContent(i);

        if (m_isMC)
            std::cout << std::setw(15) << m_cutflowMCWeights->GetBinContent(i)
		      << std::setw(15) << m_cutflowMCPUWeights->GetBinContent(i)
		      << std::setw(15) << m_cutflowScaleFactors->GetBinContent(i)
		      << std::setw(15) << m_cutflowBScaleFactors->GetBinContent(i);

	if ( m_cutflowParticleLevel ){
	    std::cout << std::setw(15) << m_cutflowParticleLevel->GetBinContent(i);
	}

	if ( m_cutflowParticleLevelMCWeights ){
	    std::cout << std::setw(15) << m_cutflowParticleLevelMCWeights->GetBinContent(i);
	}
 
        if ( m_cutflowUpgradeLevel ){
            std::cout << std::setw(15) << m_cutflowUpgradeLevel->GetBinContent(i);
        }

        std::cout << "\n";
    }
  }
  if (m_config->doLooseEvents()) {
    //channel name
    std::cout << " - " << m_name << " cutflow (Loose):\n";

    //some headings
    std::cout << std::setw(7) << "" <<
            std::setw(30) << "cut" <<
            std::setw(15) << "events";

    if (m_isMC)
        std::cout << std::setw(15) << "mc weights" <<
          std::setw(15) << "mc*pu weights" <<
          std::setw(15) << "lepton SF" <<
          std::setw(15) << "b-tag SF" ;

    if( m_cutflowParticleLevel ) {
        std::cout << std::setw(15) << "particle level";
    }

    if( m_cutflowParticleLevelMCWeights ) {
        std::cout << std::setw(15) << "particle level mc";
    }

    if( m_cutflowUpgradeLevel ) {
        std::cout << std::setw(15) << "upgrade level";
    }


    std::cout << "\n";

    //cutflow table content
    for (int i = 1; i <= m_cutflow_Loose->GetNbinsX(); ++i) {
        std::cout << "    " << std::setw(3) << i
                << std::setw(30) << m_cutflow_Loose->GetXaxis()->GetBinLabel(i)
                << std::setw(15) << m_cutflow_Loose->GetBinContent(i);

        if (m_isMC)
            std::cout << std::setw(15) << m_cutflowMCWeights_Loose->GetBinContent(i)
                      << std::setw(15) << m_cutflowMCPUWeights_Loose->GetBinContent(i)
                      << std::setw(15) << m_cutflowScaleFactors_Loose->GetBinContent(i)
                      << std::setw(15) << m_cutflowBScaleFactors_Loose->GetBinContent(i);

        if ( m_cutflowParticleLevel ){
            std::cout << std::setw(15) << m_cutflowParticleLevel->GetBinContent(i);
        }

        if ( m_cutflowParticleLevelMCWeights ){
            std::cout << std::setw(15) << m_cutflowParticleLevelMCWeights->GetBinContent(i);
        }

        if ( m_cutflowUpgradeLevel ){
            std::cout << std::setw(15) << m_cutflowUpgradeLevel->GetBinContent(i);
        }

        std::cout << "\n";
    }
  }
    std::cout << "\n";
}

const std::string EventSelection::name() const {
    return m_name;
}

std::vector<std::string> EventSelection::GetFakesMMConfigs() const {
    std::vector<std::string> configs;
    for (const auto& currentCutName : m_allCuts) {
      if (currentCutName->name()!="FAKESMMCONFIGS") continue;
      else {
	FakesMMConfigs* conf = dynamic_cast<FakesMMConfigs*>(currentCutName.get());
	for (std::string s : conf->configurations())
	  configs.push_back(s);
      }
    }
    return configs;
}

void EventSelection::printCuts() {
    std::cout << " - " << m_name << ":\n";
    if (m_config->doTightEvents()) {
      for (int i = 1; i <= m_cutflow->GetNbinsX(); ++i) {
          std::cout << "    " << std::setw(3) << i
                  << std::setw(30) << m_cutflow->GetXaxis()->GetBinLabel(i)
                  << "\n";
      }
    }
    else if (m_config->doLooseEvents()) {
      for (int i = 1; i <= m_cutflow_Loose->GetNbinsX(); ++i) {
          std::cout << "    " << std::setw(3) << i
                  << std::setw(30) << m_cutflow_Loose->GetXaxis()->GetBinLabel(i)
                  << "\n";
      }
    }

    std::cout << "\n";
}

}
