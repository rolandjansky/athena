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

using CLHEP::GeV;

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
  declareProperty("maxEta"                      , m_maxEta   = 2.0);
  declareProperty("minPt"                       , m_minPt    = 200*GeV);
  declareProperty("minMass"                     , m_minMass  =  50.*GeV);
  declareProperty("maxMass"                     , m_maxMass  = 150.*GeV);
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
    std::string myname = name;
    if (name.find("AntiKt10LCTopo")!= std::string::npos and name.find("My")== std::string::npos)
      myname = "My"+name;
    m_tccPlots.insert(std::pair<std::string, TCCPlots*>(name, new TCCPlots(0, m_dirName + myname)));
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
      // conditions to be satisfied to select jets
      if (fabs(jet->eta())>m_maxEta) continue;
      if (fabs(jet->pt())<m_minPt) continue;
            
      // fill all jets histograms
      m_tccPlots.at(name)->fill(*jet);
      m_tccPlots.at(name)->fillMoments(*jet);
      
      // fill all jets histograms + truth
      for (const auto& truth: *truths)
	m_tccPlots.at(name)->fill(*jet,*truth);
            
      // get the truth matched
      const xAOD::Jet* truth_matched = ClusterMatched(jet,truths);
      
      // apply mass requirement on the truth jet once you have matched
      if (not truth_matched or (truth_matched->m()<m_minMass or truth_matched->m()>m_maxMass))
	continue;
      
      // if truth_matched exists, fill the jet histograms + truth matched
      m_tccPlots.at(name)->fillResponse(*jet,*truth_matched);
      m_tccPlots.at(name)->fillMomentsWithMassCut(*jet);
            
      // get the calo matched
      const xAOD::Jet* calo_matched = ClusterMatched(jet,caloclusters);
      // if calo_matched exists, fill the jet histograms + calo matched
      if (calo_matched)
	m_tccPlots.at(name)->fillPseudoResponse(*jet,*calo_matched);
    }
    ATH_MSG_DEBUG("All jets histograms filled! ...");
    
    // evaluate the leadings in mass of the leadings is pt
    std::vector<const xAOD::Jet*> leadings = {nullptr, nullptr};
    
    std::vector<const xAOD::Jet*> tmp_leadings;
    if (jets->size()>0)
      tmp_leadings.push_back(jets->at(0));
    if (jets->size()>1)
      tmp_leadings.push_back(jets->at(1));
    
    if (tmp_leadings.size()>1 and tmp_leadings.at(0)->m()<tmp_leadings.at(1)->m()) std::swap(tmp_leadings.at(0), tmp_leadings.at(1));
    
    // fill the leadings jets if they satisfy the eta and pt requirements
    if (tmp_leadings.size()>0 
      and fabs(tmp_leadings.at(0)->eta())<m_maxEta 
      and tmp_leadings.at(0)->pt()>m_minPt)
      leadings.at(0) = tmp_leadings.at(0);
    
    if (tmp_leadings.size()>1 
      and fabs(tmp_leadings.at(1)->eta())<m_maxEta 
      and tmp_leadings.at(1)->pt()>m_minPt)
      leadings.at(1) = tmp_leadings.at(1);
    
    if (leadings.at(0)) {
      m_tccPlots.at(name)->fillLeading(*leadings.at(0));
      m_tccPlots.at(name)->fillMomentsLeading(*leadings.at(0));
    }
    
    if (leadings.at(1)) {
      m_tccPlots.at(name)->fillSubLeading(*leadings.at(1));
      m_tccPlots.at(name)->fillMomentsSubLeading(*leadings.at(1));
    }
       
    std::vector<const xAOD::Jet*> truth_matches  = {nullptr, nullptr};
    std::vector<const xAOD::Jet*>  calo_matches  = {nullptr, nullptr};
    unsigned int pos = 0;
    for (const auto& jet: leadings) {
      pos++;
      if (not jet) continue;
      const xAOD::Jet* truth_matched = ClusterMatched(jet,truths);
      if (truth_matched)
	truth_matches.at(pos-1) = truth_matched;
      const xAOD::Jet* calo_matched  = ClusterMatched(jet,caloclusters);
      if (calo_matched)
	calo_matches.at(pos-1) = calo_matched;
    }
    
    if (leadings.at(0)) {
      ATH_MSG_DEBUG(" ---> fillLeading ...");
      if (truth_matches.at(0) and (truth_matches.at(0)->m()>m_minMass and truth_matches.at(0)->m()<m_maxMass)) {
	m_tccPlots.at(name)->fillMomentsLeadingWithMassCut(*leadings.at(0));
	m_tccPlots.at(name)->fillResponseLeading(*leadings.at(0),*truth_matches.at(0));
	if (calo_matches.at(0))
	  m_tccPlots.at(name)->fillPseudoResponseLeading(*leadings.at(0),*calo_matches.at(0));
      }
      ATH_MSG_DEBUG("Leading jet histograms filled! ...");
    }
    
    if (leadings.at(1)) {
      ATH_MSG_DEBUG(" ---> fillSubLeading ...");
      if (truth_matches.at(1) and (truth_matches.at(1)->m()>m_minMass and truth_matches.at(1)->m()<m_maxMass)) {
	m_tccPlots.at(name)->fillMomentsSubLeadingWithMassCut(*leadings.at(1));
	m_tccPlots.at(name)->fillResponseSubLeading(*leadings.at(1),*truth_matches.at(1));
	if (calo_matches.at(1))
	  m_tccPlots.at(name)->fillPseudoResponseSubLeading(*leadings.at(1),*calo_matches.at(1));
      }
      ATH_MSG_DEBUG("SubLeading jet histograms filled! ...");
    }
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
