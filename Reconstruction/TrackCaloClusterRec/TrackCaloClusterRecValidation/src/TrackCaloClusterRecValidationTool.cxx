#include "TrackCaloClusterRecValidation/TrackCaloClusterRecValidationTool.h"
//
#include "xAODTruth/TruthParticle.h"
#include "xAODJet/JetContainer.h"

#include "TCCPlots.h"
//
#include <algorithm>
#include <limits>
#include <cmath> // to get std::isnan(), std::abs etc.
// #include <functional> // to get std::plus
#include <utility>
#include <cstdlib> // to getenv
#include <vector>


///Parametrized constructor
TrackCaloClusterRecValidationTool::TrackCaloClusterRecValidationTool(const std::string& type, const std::string& name,
								     const IInterface* parent) :
  ManagedMonitorToolBase(type, name, parent)
  {
  declareProperty("JetTruthContainerName"       , m_truthJetContainerName);
  declareProperty("JetContainerNames"           , m_jetContainerNames);
  declareProperty("TopoJetReferenceName"        , m_topoJetReferenceName = "AntiKt10LCTopoJets");
  declareProperty("TopoTrimmedJetReferenceName" , m_topoTrimmedJetReferenceName = "AntiKt10LCTopoTrimmedJets");
  declareProperty("maxTrkJetDR"                 , m_maxJetDR = 0.75);
  declareProperty("maxEta"                      , m_maxEta = 2.0);
  declareProperty("DirName"                     , m_dirName = "TCCValidation/");
  declareProperty("SubFolder"                   , m_folder);
  }

TrackCaloClusterRecValidationTool::~TrackCaloClusterRecValidationTool() {
}

StatusCode
TrackCaloClusterRecValidationTool::initialize() {
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  
  for (auto name : m_jetContainerNames) {
    ATH_MSG_INFO("Saving Plots for " << name << "...");
    m_tccPlots.insert(std::pair<std::string, TCCPlots*>(name, new TCCPlots(0, m_dirName + name)));
  }

  return StatusCode::SUCCESS;
}

StatusCode
TrackCaloClusterRecValidationTool::fillHistograms() {
  ATH_MSG_DEBUG("Filling hists " << name() << "...");
  
  const auto truths = getContainer<xAOD::JetContainer>(m_truthJetContainerName);
  if (not truths) return StatusCode::FAILURE;
    
  // retrieve jet container
  for (auto name : m_jetContainerNames) {
    ATH_MSG_DEBUG("Using Container " << name << "...");
    
    const auto jets = getContainer<xAOD::JetContainer>(name);
    if (not jets) return StatusCode::FAILURE;
    
    // Getting the collections for the pseudo response
    const auto caloclusters = (name.find("Trimmed")== std::string::npos) ?
				 getContainer<xAOD::JetContainer>(m_topoJetReferenceName) :
				 getContainer<xAOD::JetContainer>(m_topoTrimmedJetReferenceName);
    if (not caloclusters) return StatusCode::FAILURE;
        
    m_tccPlots.at(name)->fill(*jets);
        
    for (const auto& jet: *jets) {
      if (fabs(jet->eta())>m_maxEta) continue;
      // fill all jets histograms
      m_tccPlots.at(name)->fill(*jet);
      m_tccPlots.at(name)->fillMoments(*jet);
      
      // fill all jets histograms + truth
      for (const auto& truth: *truths)
	m_tccPlots.at(name)->fill(*jet,*truth);
            
      // get the truth matched
      const xAOD::Jet* truth_matched = ClusterMatched(jet,truths);
      // if truth_matched exists, fill the jet histograms + truth matched
      if (truth_matched) 
	m_tccPlots.at(name)->fillResponse(*jet,*truth_matched);
      
      // get the calo matched
      const xAOD::Jet* calo_matched = ClusterMatched(jet,caloclusters);
      // if calo_matched exists, fill the jet histograms + calo matched
      if (calo_matched) {
	m_tccPlots.at(name)->fillMoments(*jet,*calo_matched);
	m_tccPlots.at(name)->fillPseudoResponse(*jet,*calo_matched);
      }
    }
    ATH_MSG_DEBUG("All jets histograms filled! ...");
    
    // evaluate the leadings in mass of the leadings is pt
    std::vector<const xAOD::Jet*> leadings ={};
    if (jets->size()>0)
      leadings.push_back(jets->at(0));
    if (jets->size()>1)
      leadings.push_back(jets->at(1));
        
    if (leadings.size()>1 and leadings.at(0)->m()<leadings.at(1)->m()) std::swap(leadings.at(0), leadings.at(1));
       
    std::vector<const xAOD::Jet*> truth_matches  = {nullptr, nullptr};
    std::vector<const xAOD::Jet*>  calo_matches  = {nullptr, nullptr};
    unsigned int pos = 0;
    for (const auto& jet: leadings) {
      pos++;
      if (fabs(jet->eta())>m_maxEta) {
	jet = nullptr;
	continue;
      }
      const xAOD::Jet* truth_matched = ClusterMatched(jet,truths);
      if (truth_matched)
	truth_matches.at(pos) = truth_matched;
      const xAOD::Jet* calo_matched  = ClusterMatched(jet,caloclusters);
      if (calo_matched)
	calo_matches.at(pos) = calo_matched;
    }
    m_tccPlots.at(name)->fill2Leadings(leadings,truth_matches);
    ATH_MSG_DEBUG(" ---> fill2Leadings(leadings,truth_matches) ...");

    m_tccPlots.at(name)->fillMoments2Leadings(leadings,calo_matches);
    ATH_MSG_DEBUG(" ---> fillMoments2Leadings(leadings,calo_matches) ...");

    m_tccPlots.at(name)->fillPseudoResponse2Leadings(leadings,calo_matches);
    
    ATH_MSG_DEBUG("Leading jets histograms filled! ...");
  }
  
  return StatusCode::SUCCESS;
}

const xAOD::Jet* TrackCaloClusterRecValidationTool::ClusterMatched(const xAOD::Jet* jet, const xAOD::JetContainer* jets) {
  std::vector<const xAOD::Jet*> myjets = {};
  for (const auto& tomatch: *jets) myjets.push_back(tomatch);  
  return ClusterMatched(jet, myjets);
}

const xAOD::Jet* TrackCaloClusterRecValidationTool::ClusterMatched(const xAOD::Jet* jet, std::vector<const xAOD::Jet*> jets) {
  double minDeltaR = m_maxJetDR;
  const xAOD::Jet* matched = 0;
  for (const auto& tomatch: jets) {
    if (jet->p4().DeltaR(tomatch->p4()) < minDeltaR) {
      minDeltaR = jet->p4().DeltaR(tomatch->p4());
      matched = tomatch;
    }
  }
  return matched;
}

StatusCode
TrackCaloClusterRecValidationTool::bookHistograms() {
  ATH_MSG_INFO("Booking hists " << name() << "...");
  for (auto& plots : m_tccPlots) {
    plots.second->setDetailLevel(100); // DEBUG, enable expert histograms
    plots.second->initialize();
    std::vector<HistData> hists = plots.second->retrieveBookedHistograms();
    for (auto hist : hists) {
      ATH_CHECK(regHist(hist.first, hist.second, all)); 
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode
TrackCaloClusterRecValidationTool::procHistograms() {
  ATH_MSG_INFO("Finalising hists " << name() << "...");
  
  if (endOfRunFlag()) {
    for (auto& plots : m_tccPlots) 
      plots.second->finalize();
  }
  
  ATH_MSG_INFO("Successfully finalized hists");
  return StatusCode::SUCCESS;
}
