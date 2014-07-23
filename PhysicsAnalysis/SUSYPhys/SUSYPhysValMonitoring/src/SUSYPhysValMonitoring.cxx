///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SUSYPhysValMonitoring.cxx
// Implementation file for class SUSYPhysValMonitoring
// Author:
///////////////////////////////////////////////////////////////////

// SUSYPhysValMonitoring includes
#include "../SUSYPhysValMonitoring/SUSYPhysValMonitoring.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"

namespace SUSYPhysVal {

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////

SUSYPhysValMonitoring::SUSYPhysValMonitoring( const std::string& type,
    const std::string& name,
    const IInterface* parent ) :
  ManagedMonitorToolBase( type, name, parent ) {
    
  declareProperty( "JetContainerName",      m_c_jetName    = "AntiKt4LCTopoJets" );
  declareProperty( "ElectronContainerName", m_c_elecName   = "ElectronCollection" );
  declareProperty( "PhotonContainerName",   m_c_photonName = "PhotonCollection" );
  declareProperty( "MuonContainerName",     m_c_muonName   = "Muons" );
  declareProperty( "TauContainerName",      m_c_tauName    = "TauRecContainer" );
  declareProperty( "METContainerName",      m_c_metContainerName  = "MET_RefFinal" );
  declareProperty( "METName",               m_c_metName           = "Final" );
  declareProperty( "PvxContainerName",      m_c_pvxName    = "PrimaryVertices" );
  declareProperty( "TruthContainerName",    m_c_truthName  = "TruthParticle" );
  
  declareProperty( "HistBaseDirectory",     m_c_histBaseDirectory = "SUSY/ETmiss/");
  declareProperty( "UseTruthInformation",   m_c_use_truth_information = false);
  
}


// Destructor
///////////////
SUSYPhysValMonitoring::~SUSYPhysValMonitoring() {}


// Athena algtool's Hooks
////////////////////////////
StatusCode SUSYPhysValMonitoring::initialize() {
  
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  
  ATH_MSG_INFO ("Detail level is set to " << m_detailLevel);
  if (m_detailLevel < 10)
    ATH_MSG_INFO ("  Any detail level below 10 will not create output histograms.");
  ATH_MSG_INFO ("Container names:");
  ATH_MSG_INFO ("  Jet        : " << m_c_jetName);
  ATH_MSG_INFO ("  Electron   : " << m_c_elecName);
  ATH_MSG_INFO ("  Photon     : " << m_c_photonName);
  ATH_MSG_INFO ("  Muon       : " << m_c_muonName);
  ATH_MSG_INFO ("  Tau        : " << m_c_tauName);
  ATH_MSG_INFO ("  MET        : " << m_c_metContainerName << " (" << m_c_metName << ")");
  ATH_MSG_INFO ("Root directory for histograms: " << m_c_histBaseDirectory);

  return StatusCode::SUCCESS;
}


StatusCode SUSYPhysValMonitoring::bookHistograms() {
  
  // all histograms go into a subdirectory of [m_c_histBaseDirectory]
  ATH_MSG_INFO ("Booking hists " << name() << "...");

  // Physics validation plots are level 10
  if (m_detailLevel >= 10) {
    
    StatusCode sc(StatusCode::SUCCESS);
    
    std::string dir_jet(m_c_histBaseDirectory + "Jet/" + m_c_jetName + "/FourVec/");
    sc = sc & regHist(h_jet_Pt = new TH1D("Pt",(m_c_jetName + "Jet p_{T};p_{T};# jets").c_str(),100,0,200.), dir_jet, all);
    sc = sc & regHist(h_jet_eta = new TH1D("eta",(m_c_jetName + "Jet #eta;#eta;# jets").c_str(),100,-5.,5.), dir_jet, all);

    std::string dir_elec(m_c_histBaseDirectory + "Electron/" + m_c_elecName + "/FourVec/");
    sc = sc & regHist(h_elec_Pt = new TH1D("Pt",(m_c_elecName + "Electron p_{T};p_{T};# electrons").c_str(),100,0,200.), dir_elec, all);
    sc = sc & regHist(h_elec_eta = new TH1D("eta",(m_c_elecName + "Electron #eta;#eta;# electrons").c_str(),100,-5.,5.), dir_elec, all);

    std::string dir_phot(m_c_histBaseDirectory + "Photon/" + m_c_photonName + "/FourVec/");
    sc = sc & regHist(h_photon_Pt = new TH1D("Pt",(m_c_photonName + "Photon p_{T};p_{T};# photons").c_str(),100,0,200.), dir_phot, all);

    std::string dir_muon(m_c_histBaseDirectory + "Muon/" + m_c_muonName + "/FourVec/");
    sc = sc & regHist(h_muon_Pt = new TH1D("Pt",(m_c_muonName + "Muon p_{T};p_{T};# muons").c_str(),100,0,200.), dir_muon, all);
    sc = sc & regHist(h_muon_eta = new TH1D("eta",(m_c_muonName + "Muon #eta;#eta;# muons").c_str(),100,-5.,5.), dir_muon, all);

    std::string dir_tau(m_c_histBaseDirectory + "Tau/" + m_c_tauName + "/FourVec/");
    sc = sc & regHist(h_tau_Pt = new TH1D("Pt",(m_c_tauName + "Tau p_{T};p_{T};# taus").c_str(),100,0,200.), dir_tau, all);
    sc = sc & regHist(h_tau_eta = new TH1D("eta",(m_c_tauName + "Tau #eta;#eta;# taus").c_str(),100,-5.,5.), dir_tau, all);
    
    std::string dir_MET(m_c_histBaseDirectory + "MET/" + m_c_metContainerName + "/");
    sc = sc & regHist(h_met_Et = new TH1D("Et", (m_c_metContainerName + " (" + m_c_metName + ") E_{T}^{miss};E_{T}^{miss} [GeV]; events / 4 GeV").c_str(),100, 0, 400.), dir_MET, all);
    sc = sc & regHist(h_met_phi = new TH1D("phi", (m_c_metContainerName + " (" + m_c_metName + ") E_{T}^{miss};#phi;events / (2#pi/50)").c_str(), 50, -M_PI, M_PI), dir_MET, all);

    std::string dir_truth(m_c_histBaseDirectory + "Truth/" );
    sc = sc & regHist(h_SUSY_particle_parentIDs = new TH1D("h_SUSY_particle_parentIDs", "SUSY parent IDs; abs(PDG ID); #", 70, 0.5, 70.5), dir_truth, all);
    sc = sc & regHist(h_SUSY_particle_IDs = new TH1D("h_SUSY_particle_IDs", "SUSY particle IDs; Particle; #", 70, 0.5, 70.5), dir_truth, all);
    if (m_c_use_truth_information) {
      // label bins of h_SUSY_particle_IDs
      TDatabasePDG* pdg = new TDatabasePDG();
      for (uint i = 1; i < 70; ++i) {
        uint id = i + 1000000 + (1000000-40) * (i > 40);
        TParticlePDG* p = pdg->GetParticle(id);
        if (p)
          h_SUSY_particle_IDs->GetXaxis()->SetBinLabel(i, p->GetName());
      }
      delete pdg;
    }
    
    // check status code
    ATH_CHECK(sc);
    
  }

  return StatusCode::SUCCESS;
  
}


StatusCode SUSYPhysValMonitoring::fillHistograms() {
  
  /// This is the "ExecuteEvent()" method in the validation framework.

  // do nothing if detail level below 10
  if (m_detailLevel < 10) 
    return StatusCode::SUCCESS;
   
  // get and select objects
  if (!retrieveObjects().isSuccess())
    return StatusCode::FAILURE;
  
  // fill histograms
  if (!fillBasicHistograms().isSuccess())
    return StatusCode::FAILURE;

  if (m_c_use_truth_information)
    if (!fillTruthHistograms().isSuccess())
      return StatusCode::FAILURE;
    
  return StatusCode::SUCCESS;
}
  
  
StatusCode SUSYPhysValMonitoring::retrieveObjects() {    
  
  // Retrieve jet container:
  m_jets = evtStore()->retrieve< const xAOD::JetContainer >( m_c_jetName );

  if (!m_jets) {
    ATH_MSG_ERROR ("Couldn't retrieve jet container with key: " << m_c_jetName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Number of jets: " << m_jets->size() );
  
  // Retrieve elec container:
  m_elecs = evtStore()->retrieve< const xAOD::ElectronContainer >( m_c_elecName );

  if (!m_elecs) {
    ATH_MSG_ERROR ("Couldn't retrieve electron container with key: " << m_c_elecName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Number of electrons: " << m_elecs->size() );
  
  // Retrieve photon container:
  m_photons = evtStore()->retrieve< const xAOD::PhotonContainer >( m_c_photonName );

  if (!m_photons) {
    ATH_MSG_ERROR ("Couldn't retrieve photon container with key: " << m_c_photonName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Number of photons: " << m_photons->size() );

  // Retrieve muon container:
  m_muons = evtStore()->retrieve< const xAOD::MuonContainer >( m_c_muonName );

  if (!m_muons) {
    ATH_MSG_ERROR ("Couldn't retrieve muon container with key: " << m_c_muonName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Number of muons: " << m_muons->size() );
  
  // Retrieve tau container:
  m_taus = evtStore()->retrieve< const xAOD::TauJetContainer >( m_c_tauName );

  if (!m_taus) {
    ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_c_tauName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Number of taus: " << m_taus->size() );
  
//   // retrieve primary vertices
//   m_pvx = evtStore()->retrieve< const xAOD::VertexContainer >(m_c_pvxName);
//   
//   if (!m_pvx) {
//     ATH_MSG_ERROR ("Couldn't retrieve primary-vertex container with key: " << m_c_pvxName);
//     return StatusCode::FAILURE;
//   }
// 
//   ATH_MSG_DEBUG( "Number of primary vertices: " << m_pvx->size() );

  if (m_c_use_truth_information) {
    // retrieve truth particles
    m_truth = evtStore()->retrieve< const xAOD::TruthParticleContainer >(m_c_truthName);
    
    if (!m_truth) {
      ATH_MSG_ERROR ("Couldn't retrieve truth-particle container with key: " << m_c_truthName);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG( "Number of truth particles: " << m_truth->size() );
  }
  
  return StatusCode::SUCCESS;
}


StatusCode SUSYPhysValMonitoring::fillBasicHistograms() {  
  
  // jets
  int i(0);
  for (auto jet : *m_jets) {
    if (m_detailLevel >= 10) {
      h_jet_Pt->Fill(jet->pt()/1000., 1.);
      h_jet_eta->Fill(jet->eta(), 1.);
    }

    ATH_MSG_VERBOSE( "  4-momentum: px = " << jet->px()
                     << ", py = " << jet->py()
                     << ", pz = " << jet->pz()
                     << ", e = " << jet->e() );
    ++i;
  }


  // electrons
  i = 0;
  for (const xAOD::Electron* elec : *m_elecs) {
    if (m_detailLevel >= 10) {
      h_elec_Pt ->Fill(elec->pt()/1000., 1.);
      h_elec_eta->Fill(elec->eta(), 1.);
    }

    ATH_MSG_VERBOSE( "Investigating elec #" << i );
    ATH_MSG_VERBOSE( "  4-momentum: px = " << elec->p4().Px()
                     << ", py = " << elec->p4().Py()
                     << ", pz = " << elec->p4().Pz()
                     << ", e = " << elec->e() );
    ++i;
    
  }


  // photons
  i = 0;
  for (auto photon : *m_photons) {
    if (m_detailLevel >= 10) h_photon_Pt->Fill(photon->pt()/1000., 1.);

    ATH_MSG_VERBOSE( "Investigating photon #" << i );
    ATH_MSG_VERBOSE( "  4-momentum: px = " << photon->p4().Px()
                     << ", py = " << photon->p4().Py()
                     << ", pz = " << photon->p4().Pz()
                     << ", e = " << photon->e() );
    ++i;
  }

  // muons
  i = 0;
  for (const xAOD::Muon* muon: *m_muons) {
    if (m_detailLevel >= 10) {
      h_muon_Pt->Fill(muon->pt()/1000., 1.);
      h_muon_eta->Fill(muon->eta(), 1.);
    }

    ATH_MSG_VERBOSE( "Investigating muon #" << i );
    ATH_MSG_VERBOSE( "  4-momentum: px = " << muon->p4().Px()
                      << ", py = " << muon->p4().Py()
                      << ", pz = " << muon->p4().Pz()
                      << ", e = " << muon->p4().E() );
    ++i;
  }

  // taus
  i = 0;
  for (auto tau : *m_taus) {
    if (m_detailLevel >= 10) {
      h_tau_Pt->Fill(tau->pt()/1000., 1.);
      h_tau_eta->Fill(tau->eta(), 1.);
    }

    ATH_MSG_VERBOSE( "Investigating tau #" << i );
    ATH_MSG_VERBOSE( "  taus: pt = " << tau->pt()
                  << ", eta = " << tau->eta()
                  << ", phi = " << tau->phi()
                  << ", e = " << tau->e() );
    ++i;
  }

  // Retrieve MET container:
  if (!m_c_metContainerName.empty()) {
    
    const xAOD::MissingETContainer* met_container (0);
    ATH_CHECK(evtStore()->retrieve(met_container, m_c_metContainerName));
    
    const xAOD::MissingET* met = (*met_container)[m_c_metName];

    if (!met) {
      ATH_MSG_ERROR ("Couldn't retrieve MET with key " << m_c_metName << " from container");
      return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE( "  Missing Et = " << met->met());
    if (m_detailLevel >= 10) {
      h_met_Et ->Fill(met->met()/1000., 1.);
      h_met_phi->Fill(met->phi(), 1.);
    }
    
  }
  
  

  return StatusCode::SUCCESS;
}


double SUSYPhysValMonitoring::GetEventWeight() {
  
  /// return weight of current event
  
  return 1.;
  
}


StatusCode SUSYPhysValMonitoring::fillTruthHistograms() {  
  
  /// fill histograms from truth information
  for (const xAOD::TruthParticle* tp : *m_truth) {
    
    int susyid = 0;
    uint abspdgid = abs(tp->pdgId());
    if ( (abspdgid > 1000000) && (abspdgid < 1000040) )
      susyid = abspdgid - 1000000;
    else if ( (abspdgid > 2000000) && (abspdgid < 2000040) )
      susyid = abspdgid - 2000000 + 40;
    if (susyid) {
      h_SUSY_particle_IDs->Fill(susyid, GetEventWeight());
      // TODO: lookup not working on test xAOD: ERROR Attempt to retrieve nonexistent aux data item`::prodVtxLink' (11).
      if (tp->hasProdVtx()) { 
        for (uint pidx = 0; pidx < tp->prodVtx()->nIncomingParticles(); ++pidx) 
          h_SUSY_particle_parentIDs->Fill(tp->prodVtx()->incomingParticle(pidx)->pdgId(), GetEventWeight());
      }
    }
    
  }
  
  return StatusCode::SUCCESS;
}



StatusCode SUSYPhysValMonitoring::procHistograms() {
  
  ATH_MSG_INFO ("Finalising hists " << name() << "...");
  return StatusCode::SUCCESS;
  
}



}


