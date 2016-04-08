///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TopPhysValMonitoring.cxx
// Implementation file for class TopPhysValMonitoring
// Author:
///////////////////////////////////////////////////////////////////

// TopPhysValMonitoring includes
#include "../TopPhysValMonitoring/TopPhysValMonitoring.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"

namespace TopPhysVal {

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////

TopPhysValMonitoring::TopPhysValMonitoring( const std::string& type,
    const std::string& name,
    const IInterface* parent ) :
  ManagedMonitorToolBase( type, name, parent ),
  m_jets(nullptr),
  m_elecs(nullptr),
  m_photons(nullptr),
  m_muons(nullptr),
  m_taus(nullptr),
  h_jetPt(nullptr),
  h_elecPt(nullptr),
  h_photonPt(nullptr),
  h_muonPt(nullptr),
  h_tauPt(nullptr),
  h_tauEta(nullptr),
  h_metEt(nullptr),
  h_NJets(nullptr) {
    
  declareProperty( "JetContainerName",      m_c_jetName    = "AntiKt4LCTopoJets" );
  declareProperty( "ElectronContainerName", m_c_elecName   = "ElectronCollection" );
  declareProperty( "PhotonContainerName",   m_c_photonName = "PhotonCollection" );
  declareProperty( "MuonContainerName",     m_c_muonName   = "Muons" );
  declareProperty( "TauContainerName",      m_c_tauName    = "TauRecContainer" );
  declareProperty( "METContainerName",      m_c_metContainerName  = "MET_RefFinal" );
  declareProperty( "METName",               m_c_metName           = "Final" );
  declareProperty( "HistBaseDirectory",     m_c_histBaseDirectory = "Top/");
  
}


// Destructor
///////////////
TopPhysValMonitoring::~TopPhysValMonitoring() {}


// Athena algtool's Hooks
////////////////////////////
StatusCode TopPhysValMonitoring::initialize() {
  
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


StatusCode TopPhysValMonitoring::bookHistograms() {
  
  ATH_MSG_INFO ("Booking hists " << name() << "...");

  // Physics validation plots are level 10
  if (m_detailLevel >= 10) {
    
    // all histograms go into a subdirectory of [m_c_histBaseDirectory]
    
    std::string dir_jet(m_c_histBaseDirectory + "Jet/" + m_c_jetName + "/FourVec/");
    ATH_CHECK(regHist(h_jetPt = new TH1D("Pt",(m_c_jetName + "Jet p_{T}; p_{T}; # Jets").c_str(),100,0,200.), dir_jet, all));
    ATH_CHECK(regHist(h_NJets = new TH1D("N",(m_c_jetName + "Number of Jets; Number; # Jets").c_str(),100,0,200.), dir_jet, all));

    std::string dir_elec(m_c_histBaseDirectory + "Electron/" + m_c_elecName + "/FourVec/");
    ATH_CHECK(regHist(h_elecPt = new TH1D("Pt",(m_c_elecName + "Elec p_{T}; p_{T}; # Elecs").c_str(),100,0,200.), dir_elec, all));

    std::string dir_phot(m_c_histBaseDirectory + "Photon/" + m_c_photonName + "/FourVec/");
    ATH_CHECK(regHist(h_photonPt = new TH1D("Pt",(m_c_photonName + "Photon p_{T}; p_{T}; # Photons").c_str(),100,0,200.), dir_phot, all));

    std::string dir_muon(m_c_histBaseDirectory + "Muon/" + m_c_muonName + "/FourVec/");
    ATH_CHECK(regHist(h_muonPt = new TH1D("Pt",(m_c_muonName + "Muon p_{T}; p_{T}; # Muons").c_str(),100,0,200.), dir_muon, all));

    std::string dir_tau(m_c_histBaseDirectory + "Tau/" + m_c_tauName + "/FourVec/");
    ATH_CHECK(regHist(h_tauPt = new TH1D("Pt",(m_c_tauName + "Tau p_{T}; p_{T}; # Taus").c_str(),100,0,200.), dir_tau, all));
    ATH_CHECK(regHist(h_tauEta = new TH1D("Eta",(m_c_tauName + "Tau #eta; #eta; # Taus").c_str(),100,-5.,5.), dir_tau, all));
    
    std::string dir_MET(m_c_histBaseDirectory + "MET/" + m_c_metContainerName + "/");
    ATH_CHECK(regHist(h_metEt = new TH1D("MET", (m_c_metContainerName + " (" + m_c_metName + ") E_{T}^{miss}; E_{T}^{miss} [GeV]; events / 4 GeV").c_str(),100, 0, 400.), dir_MET, all));

  }

  return StatusCode::SUCCESS;
  
}


StatusCode TopPhysValMonitoring::fillHistograms() {
  
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
    
  return StatusCode::SUCCESS;
}
  
  
StatusCode TopPhysValMonitoring::retrieveObjects() {    
  
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
  
  return StatusCode::SUCCESS;
}


StatusCode TopPhysValMonitoring::fillBasicHistograms() {  
  
  // jets
  int i(0);
  for (auto jet : *m_jets) {
    if (m_detailLevel >= 10) h_jetPt->Fill(jet->pt()/1000., 1.);

    ATH_MSG_VERBOSE( "  4-momentum: px = " << jet->px()
                     << ", py = " << jet->py()
                     << ", pz = " << jet->pz()
                     << ", e = " << jet->e() );
    ++i;
  }
	h_NJets->Fill((double)i);


  // electrons
  i = 0;
  for (auto elec : *m_elecs) {
    if (m_detailLevel >= 10) h_elecPt->Fill(elec->pt()/1000., 1.);

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
    if (m_detailLevel >= 10) h_photonPt->Fill(photon->pt()/1000., 1.);

    ATH_MSG_VERBOSE( "Investigating photon #" << i );
    ATH_MSG_VERBOSE( "  4-momentum: px = " << photon->p4().Px()
                     << ", py = " << photon->p4().Py()
                     << ", pz = " << photon->p4().Pz()
                     << ", e = " << photon->e() );
    ++i;
  }

  // muons
  i = 0;
  for (auto muon : *m_muons) {
    if (m_detailLevel >= 10) h_muonPt->Fill(muon->p4().Perp()/1000., 1.);

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
      h_tauPt->Fill(tau->pt()/1000., 1.);
      h_tauEta->Fill(tau->eta(), 1.);
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
    if (m_detailLevel >= 10) 
      h_metEt->Fill(met->met()/1000., 1.);
    
  }

  return StatusCode::SUCCESS;
}


StatusCode TopPhysValMonitoring::procHistograms() {
  
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

///////////////////////////////////////////////////////////////////
// Const methods:
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Non-const methods:
///////////////////////////////////////////////////////////////////


}

//  LocalWords:  str
