///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    m_jetPlots(0, "Summary/Jet/", "Jet"),
    m_elecPlots(0, "Summary/Electron/", "Electron"),
    m_photonPlots(0, "Summary/Photon/", "Photon"),
    m_muonPlots(0, "Summary/Muon/", "Muon"),
    m_tauPlots(0, "Summary/Tau/", "Tau"),
    m_trkvtxPlots(0, "Summary/TrackAndVertex/"),
    m_metPlots(0, "Summary/MET/", "RefFinal"),
    m_btagPlots(0, "Summary/BTag/", "IP3D")
  {
    
    declareProperty( "JetContainerName", m_jetName = "AntiKt4EMTopoJets" );
    declareProperty( "ElectronContainerName", m_elecName = "ElectronCollection" ); 
    declareProperty( "PhotonContainerName", m_photonName = "PhotonCollection" ); 
    declareProperty( "MuonContainerName", m_muonName = "Muons" ); 
    declareProperty( "TauContainerName", m_tauName = "TauRecContainer" );
    declareProperty( "TrackContainerName", m_trackName = "InDetTrackParticles" );  
    declareProperty( "VertexContainerName", m_vertexName = "PrimaryVertices" );  
    declareProperty( "METContainerName", m_metName = "MET_RefFinal" ); 
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

      for (auto name : m_timingNames) {
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
    
    // Jets
    int nbtag(0);
    m_jetPlots.initializeEvent();
    const xAOD::JetContainer* jets(0);
    ATH_CHECK(evtStore()->retrieve(jets, m_jetName));
    for (auto jet : *jets) {
      m_jetPlots.fill(jet);
      const xAOD::BTagging* btag = jet->btagging();
      if (btag && btag->IP3D_loglikelihoodratio() > 1.2) ++nbtag;
    }
    m_jetPlots.fill();
    m_btagPlots.fill(nbtag);

    // Electrons
    m_elecPlots.initializeEvent();
    const xAOD::ElectronContainer* electrons(0);
    ATH_CHECK(evtStore()->retrieve(electrons, m_elecName));
    for (auto elec : *electrons) m_elecPlots.fill(elec);
    m_elecPlots.fill();

    // Photons
    m_photonPlots.initializeEvent();
    const xAOD::PhotonContainer* photons(0);
    ATH_CHECK(evtStore()->retrieve(photons, m_photonName));
    for (auto photon : *photons) m_photonPlots.fill(photon);
    m_photonPlots.fill();

    // Muons
    m_muonPlots.initializeEvent();
    const xAOD::MuonContainer* muons(0);
    ATH_CHECK(evtStore()->retrieve(muons, m_muonName));
    for (auto muon : *muons) m_muonPlots.fill(muon);
    m_muonPlots.fill();

    // Taus
    m_tauPlots.initializeEvent();
    const xAOD::TauJetContainer* taus(0);
    ATH_CHECK(evtStore()->retrieve(taus, m_tauName));
    for (auto tau : *taus) m_tauPlots.fill(tau);
    m_tauPlots.fill();

    // Tracks/Vertices
    const xAOD::TrackParticleContainer* trks(0);
    ATH_CHECK(evtStore()->retrieve(trks, m_trackName));

    const xAOD::VertexContainer* vtxs(0);
    ATH_CHECK(evtStore()->retrieve(vtxs, m_vertexName));
    for (auto vtx : *vtxs) m_trkvtxPlots.fill(vtx);

    const xAOD::EventInfo* event(0);
    ATH_CHECK(evtStore()->retrieve(event, "EventInfo"));

    m_trkvtxPlots.fill(trks->size(), vtxs->size(), event->averageInteractionsPerCrossing());

    const xAOD::MissingETContainer* met_container (0);
    ATH_CHECK(evtStore()->retrieve(met_container, m_metName));
    const xAOD::MissingET* met = (*met_container)["FinalClus"];
    if (!met) {
      ATH_MSG_ERROR ("Couldn't retrieve MET Final");
      return StatusCode::SUCCESS;
    }
    m_metPlots.fill(met);
    
    int i(0);
    for (auto name : m_timingNames) {
      float time;
      if (getTiming(name, time).isSuccess()) {
	m_timingPlots[i]->Fill(time);
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

  StatusCode PhysValExample::getTiming(std::string name, float& recoTime) {
    // Code form
    // m_recoInclPers
    
    const RecoTimingObj* recTiming(0);
    recoTime = 0;
    if (evtStore()->contains<RecoTimingObj>(name + "_timings")) {
      if (evtStore()->retrieve( recTiming, name + "_timings" ).isFailure()) {
	ATH_MSG_VERBOSE("Cannot get RecoTimingObj with name " << name + "_timings");
	return StatusCode::SUCCESS;
      }
      
      bool m_recoInclPers(true);
      if (m_recoInclPers) {
	if ((*recTiming).size() > 0) 
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
