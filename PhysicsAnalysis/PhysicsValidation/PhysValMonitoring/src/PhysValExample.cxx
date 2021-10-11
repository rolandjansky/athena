///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// PhysValExample.cxx 
// Implementation file for class PhysValExample
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PhysVal includes
#include "PhysValExample.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h" 
#include "xAODMuon/MuonContainer.h" 
#include "xAODTau/TauJetContainer.h" 
#include "xAODMissingET/MissingET.h" 
#include "xAODMissingET/MissingETContainer.h" 
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/Vertex.h" 
#include "xAODEventInfo/EventInfo.h" 
#include "xAODBTagging/BTagging.h" 
#include "xAODBTagging/BTaggingUtilities.h"

#include "RecEvent/RecoTimingObj.h"

#include "AthenaBaseComps/AthCheckMacros.h"

namespace PhysVal {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

  PhysValExample::PhysValExample( const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent ) : 
    ManagedMonitorToolBase( type, name, parent ),
    m_jetPlots(nullptr, "Summary/Jet/", "Jet"),
    m_elecPlots(nullptr, "Summary/Electron/", "Electron"),
    m_photonPlots(nullptr, "Summary/Photon/", "Photon"),
    m_muonPlots(nullptr, "Summary/Muon/", "Muon"),
    m_tauPlots(nullptr, "Summary/Tau/", "Tau"),
    m_trkvtxPlots(nullptr, "Summary/TrackAndVertex/"),
    m_metPlots(nullptr, "Summary/MET/", "RefFinal"),
    m_btagPlots(nullptr, "Summary/BTag/", "IP3D")
  {
    
    declareProperty( "JetContainerName", m_jetName = "AntiKt4EMTopoJets" );
    declareProperty( "ElectronContainerName", m_elecName = "ElectronCollection" ); 
    declareProperty( "PhotonContainerName", m_photonName = "PhotonCollection" ); 
    declareProperty( "MuonContainerName", m_muonName = "Muons" ); 
    declareProperty( "TauContainerName", m_tauName = "TauRecContainer" );
    declareProperty( "TrackContainerName", m_trackName = "InDetTrackParticles" );  
    declareProperty( "VertexContainerName", m_vertexName = "PrimaryVertices" );  
    declareProperty( "METContainerName", m_metName = "MET_RefFinal" ); 
    declareProperty( "DoExMET", m_doExMET = true ); 
    declareProperty( "DoExJet", m_doExJet = true ); 
    declareProperty( "DoExBtag", m_doExBtag = true ); 
  }
  
  // Destructor
  ///////////////
  PhysValExample::~PhysValExample()
  {}
  
  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode PhysValExample::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");    
    ATH_CHECK(ManagedMonitorToolBase::initialize());

    return StatusCode::SUCCESS;
  }
  
  StatusCode PhysValExample::book(PlotBase& plots)
  {
    plots.initialize();
    std::vector<HistData> hists = plots.retrieveBookedHistograms();
    
    for (auto& hist : hists){
      ATH_MSG_INFO ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
      ATH_CHECK(regHist(hist.first,hist.second,all));
    }
    return StatusCode::SUCCESS;      
  }

  StatusCode PhysValExample::bookHistograms()
 {
      ATH_MSG_INFO ("Booking hists " << name() << "...");      

      if (m_detailLevel >= 10) {
	ATH_CHECK(book(m_jetPlots));
	ATH_CHECK(book(m_btagPlots));
 	ATH_CHECK(book(m_elecPlots));
 	ATH_CHECK(book(m_photonPlots));
 	ATH_CHECK(book(m_muonPlots));
 	ATH_CHECK(book(m_tauPlots));
 	ATH_CHECK(book(m_trkvtxPlots));
 	ATH_CHECK(book(m_metPlots));
      }

      for (const auto& name : m_timingNames) {
	if (name == "EVNTtoHITS") {
	  m_timingPlots.push_back(new TH1F(("Timing" + name).c_str(), ("Timing" + name).c_str(), 10000, 0, 10000));
	} else {
	  m_timingPlots.push_back(new TH1F(("Timing" + name).c_str(), ("Timing" + name).c_str(), 10000, 0, 100));
	}
	ATH_CHECK(regHist(m_timingPlots.back(), "Summary/Timing/" + name,all));
      }
      
      return StatusCode::SUCCESS;      
 }
  
  StatusCode PhysValExample::fillHistograms()
  {
    ATH_MSG_INFO ("Filling hists " << name() << "...");
    
    if (m_detailLevel < 10) return StatusCode::SUCCESS;

    // event Info
    const xAOD::EventInfo* event(nullptr);
    ATH_CHECK(evtStore()->retrieve(event, "EventInfo"));
    
    // Jets
    int nbtag(0);
    if (m_doExJet){
      m_jetPlots.initializeEvent();
      const xAOD::JetContainer* jets(nullptr);
      ATH_CHECK(evtStore()->retrieve(jets, m_jetName));
      for (auto jet : *jets) {
	m_jetPlots.fill(jet,event);
	if (m_doExBtag){
	  const xAOD::BTagging* btag = xAOD::BTaggingUtilities::getBTagging( *jet );
	  if (btag && btag->IP3D_loglikelihoodratio() > 1.2) ++nbtag;
	}
      }
      m_jetPlots.fill(event);
    }
    m_btagPlots.fill(nbtag,event);
      
    // Electrons
    m_elecPlots.initializeEvent();
    const xAOD::ElectronContainer* electrons(nullptr);
    ATH_CHECK(evtStore()->retrieve(electrons, m_elecName));
    for (auto elec : *electrons) m_elecPlots.fill(elec,event);
    m_elecPlots.fill(event);

    // Photons
    m_photonPlots.initializeEvent();
    const xAOD::PhotonContainer* photons(nullptr);
    ATH_CHECK(evtStore()->retrieve(photons, m_photonName));
    for (auto photon : *photons) m_photonPlots.fill(photon,event);
    m_photonPlots.fill(event);

    // Muons
    m_muonPlots.initializeEvent();
    const xAOD::MuonContainer* muons(nullptr);
    ATH_CHECK(evtStore()->retrieve(muons, m_muonName));
    for (auto muon : *muons) m_muonPlots.fill(muon,event);
    m_muonPlots.fill(event);

    // Taus
    m_tauPlots.initializeEvent();
    const xAOD::TauJetContainer* taus(nullptr);
    ATH_CHECK(evtStore()->retrieve(taus, m_tauName));
    for (auto tau : *taus) m_tauPlots.fill(tau,event);
    m_tauPlots.fill(event);

    // Tracks/Vertices
    const xAOD::TrackParticleContainer* trks(nullptr);
    ATH_CHECK(evtStore()->retrieve(trks, m_trackName));

    const xAOD::VertexContainer* vtxs(nullptr);
    ATH_CHECK(evtStore()->retrieve(vtxs, m_vertexName));
    for (auto vtx : *vtxs) m_trkvtxPlots.fill(vtx,event);


    m_trkvtxPlots.fill(trks->size(), vtxs->size(), event->averageInteractionsPerCrossing(),event);



    if (m_doExMET){
      const xAOD::MissingETContainer* met_container (nullptr);
      ATH_CHECK(evtStore()->retrieve(met_container, m_metName));
      
      const xAOD::MissingET* met = (*met_container)["FinalClus"];
      if (!met) {
	ATH_MSG_WARNING ("Couldn't retrieve MET Final");
	return StatusCode::SUCCESS;
      }
      m_metPlots.fill(met,event);
    }
    int i(0);
    for (const auto& name : m_timingNames) {
      float time;
      if (getTiming(name, time).isSuccess()) {
	m_timingPlots[i]->Fill(time,event->beamSpotWeight());
      }
      ++i;
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode PhysValExample::procHistograms()
  {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    return StatusCode::SUCCESS;
  }
  
  /////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

  StatusCode PhysValExample::getTiming(const std::string& name, float& recoTime) {
    // Code form
    // m_recoInclPers
    
    const RecoTimingObj* recTiming(nullptr);
    recoTime = 0;
    if (evtStore()->contains<RecoTimingObj>(name + "_timings")) {
      if (evtStore()->retrieve( recTiming, name + "_timings" ).isFailure()) {
	ATH_MSG_VERBOSE("Cannot get RecoTimingObj with name " << name + "_timings");
	return StatusCode::SUCCESS;
      }
      
      bool recoInclPers(true);
      if (recoInclPers) {
	if (!(*recTiming).empty()) 
	  recoTime=*((*recTiming).rbegin());
      } else {
	if ((*recTiming).size() > 1)
	  recoTime=(*recTiming)[(*recTiming).size()-2];
      }
    }
    
    ATH_MSG_VERBOSE("Filling RecoTiming <" << recoTime << ">.");
    
    return StatusCode::SUCCESS;
  }
  
  /////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


}

//  LocalWords:  str 
