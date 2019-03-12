/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrackCaloClusterRecValidationTool.h"
//
#include "xAODTruth/TruthParticle.h"
#include "xAODJet/JetContainer.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"

#include "JetCalibTools/JetCalibrationTool.h"

#include "TCCPlots.h"
//
#include <algorithm>
#include <limits>
#include <cmath> // to get std::isnan(), std::abs etc.
#include <utility>
#include <cstdlib> // to getenv
#include <vector>

using CLHEP::GeV;

///Parametrized constructor
TrackCaloClusterRecValidationTool::TrackCaloClusterRecValidationTool(const std::string& type, const std::string& name,
								     const IInterface* parent) :
  ManagedMonitorToolBase(type, name, parent),
  m_jetCalibrationTools()
  {
  declareProperty("JetCalibrationTools"         , m_jetCalibrationTools);
  declareProperty("ApplyCalibration"            , m_applyCalibration = false);
  declareProperty("CollectionsToCalibrate"      , m_jetCalibrationCollections);
  declareProperty("SaveJetInfo"                 , m_saveJetInfo = true);
  declareProperty("JetTruthContainerName"       , m_truthJetContainerName);
  declareProperty("JetTruthTrimmedContainerName", m_truthTrimmedJetContainerName);
  declareProperty("JetContainerNames"           , m_jetContainerNames);
  declareProperty("PrimaryVertexContainerName"  , m_vertexContainerName = "PrimaryVertices");
  declareProperty("TopoJetReferenceName"        , m_topoJetReferenceName = "AntiKt10LCTopoJets");
  declareProperty("TopoTrimmedJetReferenceName" , m_topoTrimmedJetReferenceName = "AntiKt10LCTopoTrimmedJets");
  declareProperty("maxTrkJetDR"                 , m_maxJetDR = 0.75);
  declareProperty("maxEta"                      , m_maxEta   = 2.0);
  declareProperty("minPt"                       , m_minPt    = 200*GeV);
  declareProperty("minMass"                     , m_minMass  =  50.*GeV);
  declareProperty("maxMass"                     , m_maxMass  = 150.*GeV);
  declareProperty("DirName"                     , m_dirName = "TCCValidation/");
  declareProperty("SubFolder"                   , m_folder);
  declareProperty("SaveTrackInfo"               , m_saveTrackInfo = false);
  declareProperty("SaveMatchingInfo"            , m_saveMatchingInfo = false);
  declareProperty("TrackCollectionName"         , m_trackParticleCollectionName = "InDetTrackParticles");
  declareProperty("TrackPtMin"                  , m_trackPtMin = 20.*GeV);
  declareProperty("JetPtBins"                   , m_jetPtBins           );
  declareProperty("JetMassOverPtBins"           , m_jetMassOverPtBins   );
  declareProperty("TrackPtBins"                 , m_trackPtBins         );
  declareProperty("TrackProdRadiusBins"         , m_trackProdRadiusBins );
  declareProperty("SaveClusterInfo"             , m_saveClusterInfo = false);
  declareProperty("ClusterCollectionName"       , m_caloClusterCollectionName = "TimedCaloCalTopoClusters");
  declareProperty("ClusterEtaMax"               , m_caloClusterEtaMax = 2.5);
  declareProperty("SaveTrackCaloClusterInfo"    , m_saveTCCInfo = false);
  declareProperty("TCCCombinedCollectionNames"  , m_TCCCombinedCollectionNames);
  declareProperty("TCCptMin"                    , m_tccPtMin = 10.*GeV);
  declareProperty("TCCetaMax"                   , m_tccEtaMax = 2.5);
  
  }

TrackCaloClusterRecValidationTool::~TrackCaloClusterRecValidationTool() {
}

StatusCode
TrackCaloClusterRecValidationTool::initialize() {
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  
  // retrieve the jet calibration tool
  if (m_applyCalibration) {
    if (m_jetCalibrationCollections.size() != m_jetCalibrationTools.size()) {
      ATH_MSG_WARNING("Number of collections to calibrate differs from the number of calibration tools... switching off calibration!");
      m_applyCalibration=false;
    }
    CHECK( m_jetCalibrationTools.retrieve() );
  }
  
  if (m_saveJetInfo) {
    for (auto name : m_jetContainerNames) {
      ATH_MSG_INFO("Saving Plots for " << name << "...");
      std::string myname = name;
      if (name.find("AntiKt10LCTopo")!= std::string::npos and name.find("My")== std::string::npos)
	myname = "My"+name;
      
      if (name == "AntiKt10TrackCaloClustersChargedJets")
	myname = "AntiKt10TrackCaloClustersCombinedJets";
      if (name == "AntiKt10TrackCaloClustersChargedTrimmedJets")
	myname = "AntiKt10TrackCaloClustersCombinedTrimmedJets";
      
      m_tccPlots.insert(std::pair<std::string, TCCPlots*>(name, new TCCPlots(0, m_dirName + myname, "jets")));
      m_tccPlots.at(name)->setJetPtBinning(m_jetPtBins);
      m_tccPlots.at(name)->setJetMassOverPtBinning(m_jetMassOverPtBins);
    }
  }
  
  if (m_saveTrackInfo) {
   ATH_MSG_INFO("Saving Plots for " << m_trackParticleCollectionName << "...");
    m_tccPlots.insert(std::pair<std::string, TCCPlots*>(m_trackParticleCollectionName, new TCCPlots(0, m_dirName + m_trackParticleCollectionName, "tracks")));
    m_tccPlots.at(m_trackParticleCollectionName)->setTrackPtBinning(m_trackPtBins);
    m_tccPlots.at(m_trackParticleCollectionName)->setTrackProdRadiusBinning(m_trackProdRadiusBins);
  }
  
  if (m_saveClusterInfo) {
    ATH_MSG_INFO("Saving Plots for " << m_caloClusterCollectionName << "...");
    m_tccPlots.insert(std::pair<std::string, TCCPlots*>(m_caloClusterCollectionName, new TCCPlots(0, m_dirName + m_caloClusterCollectionName, "clusters")));    
  }
  
  if (m_saveTCCInfo) {
    for (auto name : m_TCCCombinedCollectionNames) {
      ATH_MSG_INFO("Saving Plots for " << name << "...");
      m_tccPlots.insert(std::pair<std::string, TCCPlots*>(name, new TCCPlots(0, m_dirName + name, "tccs")));
      m_tccPlots.at(name)->setTrackPtBinning(m_trackPtBins);
    }
  }
  
  ATH_CHECK( m_evt.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode
TrackCaloClusterRecValidationTool::fillHistograms() {

  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  if(!evt.isValid()) {
    ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  } 
  float mcEventWeight = evt->mcEventWeight();
  
  if (m_saveJetInfo) {
    ATH_MSG_DEBUG("Filling hists " << name() << "...");
        
    const auto vertices = getContainer<xAOD::VertexContainer>(m_vertexContainerName);
        
    // retrieve jet container
    for (auto name : m_jetContainerNames) {
      
      m_tccPlots.at(name)->setEventWeight(mcEventWeight);
      ATH_MSG_DEBUG("Using Container " << name << "...");
      ATH_MSG_DEBUG("-- weight = " << mcEventWeight << "...");
    
      const auto jets_beforeCalib = getContainer<xAOD::JetContainer>(name);
      if (not jets_beforeCalib) return StatusCode::FAILURE;
      
      const xAOD::JetContainer* jets = jets_beforeCalib;
              
      if (m_applyCalibration and std::find(m_jetCalibrationCollections.begin(), m_jetCalibrationCollections.end(), name)!=m_jetCalibrationCollections.end()) {
	/** Calibrate and record a shallow copy of the jet container */
	jets = calibrateAndRecordShallowCopyJetCollection(jets_beforeCalib, name);
	if(!jets){
	  ATH_MSG_WARNING(  "Unable to create calibrated jet shallow copy container" );
	  return StatusCode::SUCCESS;
	}
      }
    
      // Getting the collections for the pseudo response
      const auto caloclusters = (name.find("Trimmed")== std::string::npos) ?
                                 getContainer<xAOD::JetContainer>(m_topoJetReferenceName) :
                                 getContainer<xAOD::JetContainer>(m_topoTrimmedJetReferenceName);
      const auto truths = (name.find("Trimmed")== std::string::npos) ?
                          getContainer<xAOD::JetContainer>(m_truthJetContainerName) :
                          getContainer<xAOD::JetContainer>(m_truthTrimmedJetContainerName);
      
      if (not truths) return StatusCode::FAILURE;
    
      if (not caloclusters) return StatusCode::FAILURE;
      
      m_tccPlots.at(name)->fill(*jets);
      
      for (const auto& jet: *jets) {
	// conditions to be satisfied to select jets
	if (fabs(jet->eta())>m_maxEta) continue;
      
	// get the truth matched
	const xAOD::Jet* truth_matched_nocuts = ClusterMatched(jet,truths);
	// if truth_matched exists, fill the response w/o pt and mass cuts
	if (truth_matched_nocuts)
	  m_tccPlots.at(name)->fillResponseNoPtNoMassCuts(*jet,*truth_matched_nocuts);
	
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
	
	if (vertices)
	  m_tccPlots.at(name)->fillResponseNPV(*jet,*truth_matched,vertices->size());
	
	// get the calo matched
	const xAOD::Jet* calo_matched = ClusterMatched(jet,caloclusters);
	// if calo_matched exists, fill the jet histograms + calo matched
	if (calo_matched)
	  m_tccPlots.at(name)->fillPseudoResponse(*jet,*calo_matched);
      }
    
    
      ATH_MSG_DEBUG("All jets histograms filled! ...");
    
      // evaluate the leadings in mass of the leadings is pt
      std::vector<const xAOD::Jet*> leadings = {nullptr, nullptr};
      std::vector<const xAOD::Jet*> leadings_nocuts = {nullptr, nullptr};
    
      std::vector<const xAOD::Jet*> tmp_leadings;
      if (jets->size()>0)
	tmp_leadings.push_back(jets->at(0));
      if (jets->size()>1)
	tmp_leadings.push_back(jets->at(1));
    
      if (tmp_leadings.size()>1 and tmp_leadings.at(0)->m()<tmp_leadings.at(1)->m()) std::swap(tmp_leadings.at(0), tmp_leadings.at(1));
    
      // fill the leadings jets if they satisfy the eta requirement
      if (tmp_leadings.size()>0 
	and fabs(tmp_leadings.at(0)->eta())<m_maxEta)
	leadings_nocuts.at(0) = tmp_leadings.at(0);
    
      if (tmp_leadings.size()>1 
	and fabs(tmp_leadings.at(1)->eta())<m_maxEta)
	leadings_nocuts.at(1) = tmp_leadings.at(1);
    
      std::vector<const xAOD::Jet*> truth_matches_nocuts  = {nullptr, nullptr};
      unsigned int pos = 0;
      for (const auto& jet: leadings_nocuts) {
	pos++;
	if (not jet) continue;
	const xAOD::Jet* truth_matched_nocuts = ClusterMatched(jet,truths);
	if (truth_matched_nocuts)
	  truth_matches_nocuts.at(pos-1) = truth_matched_nocuts;
      }
    
      if (leadings_nocuts.at(0)) {
	ATH_MSG_DEBUG(" ---> fillLeading w/o cuts ...");
	if (truth_matches_nocuts.at(0)) 
	  m_tccPlots.at(name)->fillResponseNoPtNoMassCutsLeading(*leadings_nocuts.at(0),*truth_matches_nocuts.at(0));
      ATH_MSG_DEBUG("Leading jet w/o cuts histograms filled! ...");
    }
    
    if (leadings_nocuts.at(1)) {
      ATH_MSG_DEBUG(" ---> fillSubLeading w/o cuts ...");
      if (truth_matches_nocuts.at(1)) 
	m_tccPlots.at(name)->fillResponseNoPtNoMassCutsSubLeading(*leadings_nocuts.at(1),*truth_matches_nocuts.at(1));
	ATH_MSG_DEBUG("SubLeading jet w/o cuts histograms filled! ...");
    }  
    
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
       
      std::vector<const xAOD::Jet*> truth_matches         = {nullptr, nullptr};
      std::vector<const xAOD::Jet*>  calo_matches         = {nullptr, nullptr};
      pos = 0;
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
	  if (vertices)
	    m_tccPlots.at(name)->fillResponseLeadingNPV(*leadings.at(0),*truth_matches.at(0),vertices->size());
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
	  if (vertices)
	    m_tccPlots.at(name)->fillResponseSubLeadingNPV(*leadings.at(1),*truth_matches.at(1),vertices->size());
	  if (calo_matches.at(1))
	    m_tccPlots.at(name)->fillPseudoResponseSubLeading(*leadings.at(1),*calo_matches.at(1));
	}
	ATH_MSG_DEBUG("SubLeading jet histograms filled! ...");
      }     
    }
  }
  
  // Getting the collections for TrackParticles
  if (m_saveTrackInfo) {
    const auto tracks = getContainer<xAOD::TrackParticleContainer>(m_trackParticleCollectionName);
    if (not tracks) return StatusCode::FAILURE;
    for (const auto& track: *tracks) {
      if (m_saveMatchingInfo)
	m_tccPlots.at(m_trackParticleCollectionName)->fillMatching(*track);
      m_tccPlots.at(m_trackParticleCollectionName)->fillTrackParametersAllPt(*track);
      m_tccPlots.at(m_trackParticleCollectionName)->fillCaloEntryInfoAllPt(*track);
      m_tccPlots.at(m_trackParticleCollectionName)->fillPerigeeInfoAllPt(*track);
      m_tccPlots.at(m_trackParticleCollectionName)->fillPerigeeVsCaloEntryAllPt(*track);
      if (track->pt()<m_trackPtMin) continue;
      m_tccPlots.at(m_trackParticleCollectionName)->fillTrackParameters(*track);
      m_tccPlots.at(m_trackParticleCollectionName)->fillCaloEntryInfo(*track);
      m_tccPlots.at(m_trackParticleCollectionName)->fillPerigeeInfo(*track);
      m_tccPlots.at(m_trackParticleCollectionName)->fillPerigeeVsCaloEntry(*track);
    }    
  }
  
  // Getting the collections for the CaloClusters
  if (m_saveClusterInfo) {
   const auto clusters = getContainer<xAOD::CaloClusterContainer>(m_caloClusterCollectionName);
   if (not clusters) return StatusCode::FAILURE;
   for (const auto& cluster: *clusters) {
     m_tccPlots.at(m_caloClusterCollectionName)->fillCluster(*cluster);
     if (fabs(cluster->eta())<m_caloClusterEtaMax)
       m_tccPlots.at(m_caloClusterCollectionName)->fillClusterEtaCut(*cluster);
   }    
  }
  
  // Getting the collections for the TrackCaloClusters
  if (m_saveTCCInfo) {
    for (auto name : m_TCCCombinedCollectionNames) {
      const auto tccs = getContainer<xAOD::TrackCaloClusterContainer>(name);
      if (not tccs) return StatusCode::FAILURE;
      // fill the map with all the tracks creating tcc (means from PV0)
      std::vector<const xAOD::TrackParticle*> allpv0tracks;
      for (const auto& tcc: *tccs) {
	allpv0tracks.push_back(*tcc->trackParticleLink());
      }
      
      for (const auto& tcc: *tccs) {
	m_tccPlots.at(name)->fillTCC(*tcc, allpv0tracks);
	if (tcc->pt()>m_tccPtMin)
	  m_tccPlots.at(name)->fillTCCptCut(*tcc);
	if (fabs(tcc->eta())<m_tccEtaMax)
	  m_tccPlots.at(name)->fillTCCetaCut(*tcc);
      }
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

/**Calibrate and record a shallow copy of a given jet container */
const xAOD::JetContainer* TrackCaloClusterRecValidationTool::calibrateAndRecordShallowCopyJetCollection(const xAOD::JetContainer * jetContainer, const std::string name) {
  
  // create a shallow copy of the jet container
  std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* >  shallowCopy = xAOD::shallowCopyContainer(*jetContainer);
  xAOD::JetContainer *jetContainerShallowCopy           = shallowCopy.first;
  xAOD::ShallowAuxContainer *jetAuxContainerShallowCopy = shallowCopy.second;
 
  if( evtStore()->record(jetContainerShallowCopy, name+"_Calib").isFailure() ){
    ATH_MSG_WARNING("Unable to record JetCalibratedContainer: " << name+"_Calib");
    return 0;
  }
  if( evtStore()->record(jetAuxContainerShallowCopy, name+"_Calib"+"Aux.").isFailure() ){
    ATH_MSG_WARNING("Unable to record JetCalibratedAuxContainer: " << name+"_Calib"+"Aux.");
    return 0;
  }
  
  static SG::AuxElement::Accessor< xAOD::IParticleLink > accSetOriginLink ("originalObjectLink");
  static SG::AuxElement::Decorator< float > decJvt("JvtUpdate");

  int pos = std::find(m_jetCalibrationCollections.begin(), m_jetCalibrationCollections.end(), name) - m_jetCalibrationCollections.begin();
  for ( xAOD::Jet *shallowCopyJet : * jetContainerShallowCopy ) {
    
    if( m_jetCalibrationTools[pos]->applyCalibration(*shallowCopyJet).isFailure() ){
      ATH_MSG_WARNING( "Failed to apply calibration to the jet container"); 
      return 0;
    }
    const xAOD::IParticleLink originLink( *jetContainer, shallowCopyJet->index() );
    accSetOriginLink(*shallowCopyJet) = originLink;
  }

  if( evtStore()->setConst(jetContainerShallowCopy ).isFailure() ){
    ATH_MSG_WARNING( "Failed to set jetcalibCollection (" << name+"_Calib"+"Aux." << ")const in StoreGate!"); 
    return 0;
  }
  if( evtStore()->setConst(jetAuxContainerShallowCopy ).isFailure() ){
    ATH_MSG_WARNING( "Failed to set jetcalibCollection (" << name+"_Calib"+"Aux." << ")const in StoreGate!"); 
    return 0;
  }
  
  return jetContainerShallowCopy; 
}
