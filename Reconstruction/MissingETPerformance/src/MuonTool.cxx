/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

#include "MissingETPerformance/MuonTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// Accessing data:
//#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "muonEvent/MuonContainer.h"
#include "JetEvent/JetCollection.h"

#include "TH1.h"

//HEPMC stuff
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenNonInteracting.h"
#include "TruthHelper/GenAccessIO.h"
//#include "McParticleEvent/TruthParticleContainer.h"
//#include "McParticleEvent/TruthParticle.h"

#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"

#include <sstream>
using CLHEP::HepLorentzVector;
using CLHEP::GeV;
using namespace Analysis;
using namespace Rec;
using namespace TruthHelper;
//------------------------------------------------------------------------------
MuonTool::MuonTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<MuonTool>( this );

  declareProperty("JetCollectionKey",           _JetCollectionKey          = "Cone4H1TopoJets" );
  declareProperty("CaloMuonCollectionKey",      _CaloMuonCollectionKey     = "CaloMuonCollection" );

  declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode MuonTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endreq;
     return sc;
  }

  double _pi = 2*acos(0);
  std::string locationPrefix = "/AANT/" + m_folderName + "Muons/Truth/";
  std::string location;

  h_MC_eta_dist_Truth = new TH1D("h_MC_eta_dist_Truth","Truth MC Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_MC_eta_dist_Truth";
  sc = m_thistSvc->regHist(location, h_MC_eta_dist_Truth);
  h_MC_phi_dist_Truth = new TH1D("h_MC_phi_dist_Truth","Truth MC Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_phi_dist_Truth";
  sc = m_thistSvc->regHist(location, h_MC_phi_dist_Truth);
  h_MC_Pt_dist_Truth = new TH1D("h_MC_Pt_dist_Truth","Truth MC Pt Distribution",100,0,200);
  location = locationPrefix + "h_MC_Pt_dist_Truth";
  sc = m_thistSvc->regHist(location, h_MC_Pt_dist_Truth);
  h_MC_2D_phiVsEta_Truth = new TH2D("h_MC_2D_phiVsEta_Truth","Truth MC Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_2D_phiVsEta_Truth";
  sc = m_thistSvc->regHist(location, h_MC_2D_phiVsEta_Truth);

  locationPrefix = "/AANT/" + m_folderName + "Muons/MuonBoy/";

  h_eta_dist_allMuonBoy = new TH1D("h_eta_dist_all_MuonBoy","MuonBoy Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_eta_dist_all_MuonBoy";
  sc = m_thistSvc->regHist(location, h_eta_dist_allMuonBoy);
  h_phi_dist_allMuonBoy = new TH1D("h_phi_dist_all_MuonBoy","MuonBoy Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_phi_dist_all_MuonBoy";
  sc = m_thistSvc->regHist(location, h_phi_dist_allMuonBoy);
  h_Pt_dist_allMuonBoy = new TH1D("h_Pt_dist_all_MuonBoy","MuonBoy Pt Distribution",100,0,200);
  location = locationPrefix + "h_Pt_dist_all_MuonBoy";
  sc = m_thistSvc->regHist(location, h_Pt_dist_allMuonBoy);
  h_2D_phiVsEta_allMuonBoy = new TH2D("h_2D_phiVsEta_MuonBoy","MuonBoy Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_2D_phiVsEta_MuonBoy";
  sc = m_thistSvc->regHist(location, h_2D_phiVsEta_allMuonBoy);

  locationPrefix = "/AANT/" + m_folderName + "Muons/MuonBoy/JetMatched/";

  h_2D_jetMatched_IsolationVsPt_LogLogMuonBoy = new TH2D("h_2D_jetMatched_IsolationVsPt_LogLog_MuonBoy","Jet Matched Muonboys, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_2D_jetMatched_IsolationVsPt_LogLog_MuonBoy";
  sc = m_thistSvc->regHist(location, h_2D_jetMatched_IsolationVsPt_LogLogMuonBoy);
  h_2D_jetMatched_IsolationVsPtMuonBoy = new TH2D("h_2D_jetMatched_IsolationVsPt_MuonBoy","Jet Matched Muonboys, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_2D_jetMatched_IsolationVsPt_MuonBoy";
  sc = m_thistSvc->regHist(location, h_2D_jetMatched_IsolationVsPtMuonBoy);

  h_eta_dist_jetMatchedMuonBoy = new TH1D("h_eta_dist_jetMatched_MuonBoy","Jet Matched (dR < .2) MuonBoy Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_eta_dist_jetMatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_eta_dist_jetMatchedMuonBoy);
  h_phi_dist_jetMatchedMuonBoy = new TH1D("h_phi_dist_jetMatched_MuonBoy","Jet Matched (dR < .2) MuonBoy Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_phi_dist_jetMatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_phi_dist_jetMatchedMuonBoy);
  h_Pt_dist_jetMatchedMuonBoy = new TH1D("h_Pt_dist_jetMatched_MuonBoy","Jet Matched (dR < .2) MuonBoy Pt Distribution",100,0,200);
  location = locationPrefix + "h_Pt_dist_jetMatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_Pt_dist_jetMatchedMuonBoy);
  h_2D_phiVsEta_jetMatchedMuonBoy = new TH2D("h_2D_phiVsEta_MuonBoy","Jet Matched (dR < .2) MuonBoy Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_2D_phiVsEta_MuonBoy";
  sc = m_thistSvc->regHist(location, h_2D_phiVsEta_jetMatchedMuonBoy);

  locationPrefix = "/AANT/" + m_folderName + "Muons/MuonBoy/MC/TruthMatched/";
  
  h_MC_2D_truthMatched_IsolationVsPt_LogLogMuonBoy = new TH2D("h_MC_2D_truthMatched_IsolationVsPt_LogLog_MuonBoy","Truth Matched Muonboys, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthMatched_IsolationVsPt_LogLog_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthMatched_IsolationVsPt_LogLogMuonBoy);
  h_MC_2D_truthMatched_IsolationVsPtMuonBoy = new TH2D("h_MC_2D_truthMatched_IsolationVsPt_MuonBoy","Truth Matched Muonboys, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthMatched_IsolationVsPt_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthMatched_IsolationVsPtMuonBoy);

  h_MC_eta_dist_truthMatchedMuonBoy = new TH1D("h_MC_eta_dist_truthMatched_MuonBoy","Truth Matched (dR < .2) MuonBoy Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_MC_eta_dist_truthMatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_eta_dist_truthMatchedMuonBoy);
  h_MC_phi_dist_truthMatchedMuonBoy = new TH1D("h_MC_phi_dist_truthMatched_MuonBoy","Truth Matched (dR < .2) MuonBoy Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_phi_dist_truthMatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_phi_dist_truthMatchedMuonBoy);
  h_MC_Pt_dist_truthMatchedMuonBoy = new TH1D("h_MC_Pt_dist_truthMatched_MuonBoy","Truth Matched (dR < .2) MuonBoy Pt Distribution",100,0,200);
  location = locationPrefix + "h_MC_Pt_dist_truthMatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_Pt_dist_truthMatchedMuonBoy);
  h_MC_2D_phiVsEta_truthMatchedMuonBoy = new TH2D("h_MC_2D_phiVsEta_MuonBoy","Truth Matched (dR < .2) MuonBoy Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_2D_phiVsEta_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_2D_phiVsEta_truthMatchedMuonBoy);

  locationPrefix = "/AANT/" + m_folderName + "Muons/MuonBoy/MC/TruthUnmatched/";
 
  h_MC_2D_truthUnmatched_IsolationVsPt_LogLogMuonBoy = new TH2D("h_MC_2D_truthUnmatched_IsolationVsPt_LogLog_MuonBoy","Truth Unmatched Muonboys, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthUnmatched_IsolationVsPt_LogLog_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthUnmatched_IsolationVsPt_LogLogMuonBoy);
  h_MC_2D_truthUnmatched_IsolationVsPtMuonBoy = new TH2D("h_MC_2D_truthUnmatched_IsolationVsPt_MuonBoy","Truth Unmatched Muonboys, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthUnmatched_IsolationVsPt_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthUnmatched_IsolationVsPtMuonBoy);
  
  h_MC_eta_dist_truthUnmatchedMuonBoy = new TH1D("h_MC_eta_dist_truthUnmatched_MuonBoy","Truth Unmatched (dR < .2) MuonBoy Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_MC_eta_dist_truthUnmatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_eta_dist_truthUnmatchedMuonBoy);
  h_MC_phi_dist_truthUnmatchedMuonBoy = new TH1D("h_MC_phi_dist_truthUnmatched_MuonBoy","Truth Unmatched (dR < .2) MuonBoy Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_phi_dist_truthUnmatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_phi_dist_truthUnmatchedMuonBoy);
  h_MC_Pt_dist_truthUnmatchedMuonBoy = new TH1D("h_MC_Pt_dist_truthUnmatched_MuonBoy","Truth Unmatched (dR < .2) MuonBoy Pt Distribution",100,0,200);
  location = locationPrefix + "h_MC_Pt_dist_truthUnmatched_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_Pt_dist_truthUnmatchedMuonBoy);
  h_MC_2D_phiVsEta_truthUnmatchedMuonBoy = new TH2D("h_MC_2D_phiVsEta_MuonBoy","Truth Unmatched (dR < .2) MuonBoy Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_2D_phiVsEta_MuonBoy";
  sc = m_thistSvc->regHist(location, h_MC_2D_phiVsEta_truthUnmatchedMuonBoy);

  locationPrefix = "/AANT/" + m_folderName + "Muons/Mutag/";

  h_eta_dist_allMutag = new TH1D("h_eta_dist_all_Mutag","Mutag Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_eta_dist_all_Mutag";
  sc = m_thistSvc->regHist(location, h_eta_dist_allMutag);
  h_phi_dist_allMutag = new TH1D("h_phi_dist_all_Mutag","Mutag Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_phi_dist_all_Mutag";
  sc = m_thistSvc->regHist(location, h_phi_dist_allMutag);
  h_Pt_dist_allMutag = new TH1D("h_Pt_dist_all_Mutag","Mutag Pt Distribution",100,0,200);
  location = locationPrefix + "h_Pt_dist_all_Mutag";
  sc = m_thistSvc->regHist(location, h_Pt_dist_allMutag);
  h_2D_phiVsEta_allMutag = new TH2D("h_2D_phiVsEta_Mutag","Mutag Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_2D_phiVsEta_Mutag";
  sc = m_thistSvc->regHist(location, h_2D_phiVsEta_allMutag);

  locationPrefix = "/AANT/" + m_folderName + "Muons/Mutag/JetMatched/";

  h_2D_jetMatched_IsolationVsPt_LogLogMutag = new TH2D("h_2D_jetMatched_IsolationVsPt_LogLog_Mutag","Jet Matched Mutag, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_2D_jetMatched_IsolationVsPt_LogLog_Mutag";
  sc = m_thistSvc->regHist(location, h_2D_jetMatched_IsolationVsPt_LogLogMutag);
  h_2D_jetMatched_IsolationVsPtMutag = new TH2D("h_2D_jetMatched_IsolationVsPt_Mutag","Jet Matched Mutag, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_2D_jetMatched_IsolationVsPt_Mutag";
  sc = m_thistSvc->regHist(location, h_2D_jetMatched_IsolationVsPtMutag);

  h_eta_dist_jetMatchedMutag = new TH1D("h_eta_dist_jetMatched_Mutag","Jet Matched (dR < .2) Mutag Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_eta_dist_jetMatched_Mutag";
  sc = m_thistSvc->regHist(location, h_eta_dist_jetMatchedMutag);
  h_phi_dist_jetMatchedMutag = new TH1D("h_phi_dist_jetMatched_Mutag","Jet Matched (dR < .2) Mutag Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_phi_dist_jetMatched_Mutag";
  sc = m_thistSvc->regHist(location, h_phi_dist_jetMatchedMutag);
  h_Pt_dist_jetMatchedMutag = new TH1D("h_Pt_dist_jetMatched_Mutag","Jet Matched (dR < .2) Mutag Pt Distribution",100,0,200);
  location = locationPrefix + "h_Pt_dist_jetMatched_Mutag";
  sc = m_thistSvc->regHist(location, h_Pt_dist_jetMatchedMutag);
  h_2D_phiVsEta_jetMatchedMutag = new TH2D("h_2D_phiVsEta_Mutag","Jet Matched (dR < .2) Mutag Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_2D_phiVsEta_Mutag";
  sc = m_thistSvc->regHist(location, h_2D_phiVsEta_jetMatchedMutag);

  locationPrefix = "/AANT/" + m_folderName + "Muons/Mutag/MC/TruthMatched/";

  h_MC_2D_truthMatched_IsolationVsPt_LogLogMutag = new TH2D("h_MC_2D_truthMatched_IsolationVsPt_LogLog_Mutag","Truth Matched Mutag, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthMatched_IsolationVsPt_LogLog_Mutag";
  sc = m_thistSvc->regHist(location,h_MC_2D_truthMatched_IsolationVsPt_LogLogMutag);
  h_MC_2D_truthMatched_IsolationVsPtMutag = new TH2D("h_MC_2D_truthMatched_IsolationVsPt_Mutag","Truth Matched Mutag, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthMatched_IsolationVsPt_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthMatched_IsolationVsPtMutag);

  h_MC_eta_dist_truthMatchedMutag = new TH1D("h_MC_eta_dist_truthMatched_Mutag","Truth Matched (dR < .2) Mutag Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_MC_eta_dist_truthMatched_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_eta_dist_truthMatchedMutag);
  h_MC_phi_dist_truthMatchedMutag = new TH1D("h_MC_phi_dist_truthMatched_Mutag","Truth Matched (dR < .2) Mutag Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_phi_dist_truthMatched_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_phi_dist_truthMatchedMutag);
  h_MC_Pt_dist_truthMatchedMutag = new TH1D("h_MC_Pt_dist_truthMatched_Mutag","Truth Matched (dR < .2) Mutag Pt Distribution",100,0,200);
  location = locationPrefix + "h_MC_Pt_dist_truthMatched_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_Pt_dist_truthMatchedMutag);
  h_MC_2D_phiVsEta_truthMatchedMutag = new TH2D("h_MC_2D_phiVsEta_Mutag","Truth Matched (dR < .2) Mutag Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_2D_phiVsEta_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_2D_phiVsEta_truthMatchedMutag);

  locationPrefix = "/AANT/" + m_folderName + "Muons/Mutag/MC/TruthUnmatched/";

  h_MC_2D_truthUnmatched_IsolationVsPt_LogLogMutag = new TH2D("h_MC_2D_truthUnmatched_IsolationVsPt_LogLog_Mutag","Truth Unmatched Mutag, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthUnmatched_IsolationVsPt_LogLog_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthUnmatched_IsolationVsPt_LogLogMutag);
  h_MC_2D_truthUnmatched_IsolationVsPtMutag = new TH2D("h_MC_2D_truthUnmatched_IsolationVsPt_Mutag","Truth Unmatched Mutag, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthUnmatched_IsolationVsPt_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthUnmatched_IsolationVsPtMutag);

  h_MC_eta_dist_truthUnmatchedMutag = new TH1D("h_MC_eta_dist_truthUnmatched_Mutag","Truth Unmatched (dR < .2) Mutag Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_MC_eta_dist_truthUnmatched_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_eta_dist_truthUnmatchedMutag);
  h_MC_phi_dist_truthUnmatchedMutag = new TH1D("h_MC_phi_dist_truthUnmatched_Mutag","Truth Unmatched (dR < .2) Mutag Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_phi_dist_truthUnmatched_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_phi_dist_truthUnmatchedMutag);
  h_MC_Pt_dist_truthUnmatchedMutag = new TH1D("h_MC_Pt_dist_truthUnmatched_Mutag","Truth Unmatched (dR < .2) Mutag Pt Distribution",100,0,200);
  location = locationPrefix + "h_MC_Pt_dist_truthUnmatched_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_Pt_dist_truthUnmatchedMutag);
  h_MC_2D_phiVsEta_truthUnmatchedMutag = new TH2D("h_MC_2D_phiVsEta_Mutag","Truth Unmatched (dR < .2) Mutag Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_2D_phiVsEta_Mutag";
  sc = m_thistSvc->regHist(location, h_MC_2D_phiVsEta_truthUnmatchedMutag);

  locationPrefix = "/AANT/" + m_folderName + "Muons/CaloMuon/";

  h_eta_dist_allCaloMuons = new TH1D("h_eta_dist_all_CaloMuons","CaloMuons Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_eta_dist_all_CaloMuons";
  sc = m_thistSvc->regHist(location, h_eta_dist_allCaloMuons);
  h_phi_dist_allCaloMuons = new TH1D("h_phi_dist_all_CaloMuons","CaloMuon Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_phi_dist_all_CaloMuons";
  sc = m_thistSvc->regHist(location, h_phi_dist_allCaloMuons);
  h_Pt_dist_allCaloMuons = new TH1D("h_Pt_dist_all_CaloMuons","CaloMuons Pt Distribution",100,0,200);
  location = locationPrefix + "h_Pt_dist_all_CaloMuons";
  sc = m_thistSvc->regHist(location, h_Pt_dist_allCaloMuons);
  h_2D_phiVsEta_allCaloMuons = new TH2D("h_2D_phiVsEta_CaloMuons","CaloMuons Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_2D_phiVsEta_CaloMuons";
  sc = m_thistSvc->regHist(location, h_2D_phiVsEta_allCaloMuons);

  locationPrefix = "/AANT/" + m_folderName + "Muons/CaloMuon/JetMatched/";

  h_2D_jetMatched_IsolationVsPt_LogLogCaloMuons = new TH2D("h_2D_jetMatched_IsolationVsPt_LogLog_CaloMuons","Jet Matched CaloMuons, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_2D_jetMatched_IsolationVsPt_LogLog_CaloMuons";
  sc = m_thistSvc->regHist(location, h_2D_jetMatched_IsolationVsPt_LogLogCaloMuons);
  h_2D_jetMatched_IsolationVsPtCaloMuons = new TH2D("h_2D_jetMatched_IsolationVsPt_CaloMuons","Jet Matched CaloMuons, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_2D_jetMatched_IsolationVsPt_CaloMuons";
  sc = m_thistSvc->regHist(location, h_2D_jetMatched_IsolationVsPtCaloMuons);

  h_eta_dist_jetMatchedCaloMuons = new TH1D("h_eta_dist_jetMatched_CaloMuons","Jet Matched (dR < .2) CaloMuons Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_eta_dist_jetMatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_eta_dist_jetMatchedCaloMuons);
  h_phi_dist_jetMatchedCaloMuons = new TH1D("h_phi_dist_jetMatched_CaloMuons","Jet Matched (dR < .2) CaloMuon Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_phi_dist_jetMatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_phi_dist_jetMatchedCaloMuons);
  h_Pt_dist_jetMatchedCaloMuons = new TH1D("h_Pt_dist_jetMatched_CaloMuons","Jet Matched (dR < .2) CaloMuons Pt Distribution",100,0,200);
  location = locationPrefix + "h_Pt_dist_jetMatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_Pt_dist_jetMatchedCaloMuons);
  h_2D_phiVsEta_jetMatchedCaloMuons = new TH2D("h_2D_phiVsEta_CaloMuons","Jet Matched (dR < .2) CaloMuons Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_2D_phiVsEta_CaloMuons";
  sc = m_thistSvc->regHist(location, h_2D_phiVsEta_jetMatchedCaloMuons);

  locationPrefix = "/AANT/" + m_folderName + "Muons/CaloMuon/MC/TruthMatched/";

  h_MC_2D_truthMatched_IsolationVsPt_LogLogCaloMuons = new TH2D("h_MC_2D_truthMatched_IsolationVsPt_LogLog_CaloMuons","Truth Matched CaloMuons, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthMatched_IsolationVsPt_LogLog_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthMatched_IsolationVsPt_LogLogCaloMuons);
  h_MC_2D_truthMatched_IsolationVsPtCaloMuons = new TH2D("h_MC_2D_truthMatched_IsolationVsPt_CaloMuons","Truth Matched CaloMuons, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthMatched_IsolationVsPt_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthMatched_IsolationVsPtCaloMuons);

  h_MC_eta_dist_truthMatchedCaloMuons = new TH1D("h_MC_eta_dist_truthMatched_CaloMuons","Truth Matched (dR < .2) CaloMuons Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_MC_eta_dist_truthMatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_eta_dist_truthMatchedCaloMuons);
  h_MC_phi_dist_truthMatchedCaloMuons = new TH1D("h_MC_phi_dist_truthMatched_CaloMuons","Truth Matched (dR < .2) CaloMuon Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_phi_dist_truthMatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_phi_dist_truthMatchedCaloMuons);
  h_MC_Pt_dist_truthMatchedCaloMuons = new TH1D("h_MC_Pt_dist_truthMatched_CaloMuons","Truth Matched (dR < .2) CaloMuons Pt Distribution",100,0,200);
  location = locationPrefix + "h_MC_Pt_dist_truthMatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_Pt_dist_truthMatchedCaloMuons);
  h_MC_2D_phiVsEta_truthMatchedCaloMuons = new TH2D("h_MC_2D_phiVsEta_CaloMuons","Truth Matched (dR < .2) CaloMuons Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_2D_phiVsEta_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_2D_phiVsEta_truthMatchedCaloMuons);

  locationPrefix = "/AANT/" + m_folderName + "Muons/CaloMuon/MC/TruthUnmatched/";

  h_MC_2D_truthUnmatched_IsolationVsPt_LogLogCaloMuons = new TH2D("h_MC_2D_truthUnmatched_IsolationVsPt_LogLog_CaloMuons","Truth Unmatched CaloMuons, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthUnmatched_IsolationVsPt_LogLog_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_2D_truthUnmatched_IsolationVsPt_LogLogCaloMuons);
  h_MC_2D_truthUnmatched_IsolationVsPtCaloMuons = new TH2D("h_MC_2D_truthUnmatched_IsolationVsPt_CaloMuons","Truth Unmatched CaloMuons, Iso vs Pt",100, 0.0, 500.0, 100, 0.0, 50.0);
  location = locationPrefix + "h_MC_2D_truthUnmatched_IsolationVsPt_CaloMuons";
  sc = m_thistSvc->regHist(location,h_MC_2D_truthUnmatched_IsolationVsPtCaloMuons);

  h_MC_eta_dist_truthUnmatchedCaloMuons = new TH1D("h_MC_eta_dist_truthUnmatched_CaloMuons","Truth Unmatched (dR < .2) CaloMuons Eta Distribution",100, -3, 3);
  location = locationPrefix + "h_MC_eta_dist_truthUnmatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_eta_dist_truthUnmatchedCaloMuons);
  h_MC_phi_dist_truthUnmatchedCaloMuons = new TH1D("h_MC_phi_dist_truthUnmatched_CaloMuons","Truth Unmatched (dR < .2) CaloMuon Phi Distribution",100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_phi_dist_truthUnmatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_phi_dist_truthUnmatchedCaloMuons);
  h_MC_Pt_dist_truthUnmatchedCaloMuons = new TH1D("h_MC_Pt_dist_truthUnmatched_CaloMuons","Truth Unmatched (dR < .2) CaloMuons Pt Distribution",100,0,200);
  location = locationPrefix + "h_MC_Pt_dist_truthUnmatched_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_Pt_dist_truthUnmatchedCaloMuons);
  h_MC_2D_phiVsEta_truthUnmatchedCaloMuons = new TH2D("h_MC_2D_phiVsEta_CaloMuons","Truth Unmatched (dR < .2) CaloMuons Phi vs Eta Distribution",100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_MC_2D_phiVsEta_CaloMuons";
  sc = m_thistSvc->regHist(location, h_MC_2D_phiVsEta_truthUnmatchedCaloMuons);

  locationPrefix = "/AANT/" + m_folderName + "Jets/Cone4Topo/";

  h_eta_dist_Jets = new TH1D("h_eta_dist_Jets","h_eta_dist_Jets", 100, -3, 3);
  location = locationPrefix + "h_eta_dist_Jets";
  sc = m_thistSvc->regHist(location, h_eta_dist_Jets);
  h_phi_dist_Jets = new TH1D("h_phi_dist_Jets","h_phi_dist_Jets", 100, -1*_pi, _pi);
  location = locationPrefix + "h_phi_dist_Jets";
  sc = m_thistSvc->regHist(location, h_phi_dist_Jets);
  h_Pt_dist_Jets = new TH1D("h_Pt_dist_Jets","h_Pt_dist_Jets", 100, 0, 200);
  location = locationPrefix + "h_Pt_dist_Jets";
  sc = m_thistSvc->regHist(location, h_Pt_dist_Jets);
  h_2D_phiVsEta_Jets = new TH2D("h_2D_phiVsEta_Jets","h_2D_phiVsEta_Jets", 100, -3, 3, 100, -1*_pi, _pi);
  location = locationPrefix + "h_2D_phiVsEta_Jets";
  sc = m_thistSvc->regHist(location, h_2D_phiVsEta_Jets);

  return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
StatusCode MuonTool::initialize() {
  return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
StatusCode MuonTool::execute(MissingETMuonData *muondata ) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  sc = muonSetup(muondata);
  if( sc.isFailure() ) {
    msg() << MSG::WARNING
          << "Muon Setup failed in MuonTool"
          << endreq;
    return StatusCode::SUCCESS;
  }
  return sc;
}
//------------------------------------------------------------------------------

StatusCode MuonTool::muonSetup(MissingETMuonData *muondata) {

  msg() << MSG::DEBUG << "in resolution() " << endreq;

  StatusCode sc = StatusCode::SUCCESS; 

  const std::vector<const HepMC::GenParticle*> &truth_muons      = muondata->used_truth_muons();
  std::vector<const Analysis::Muon*> spectroMuons               = muondata->used_MuonBoy_Spectro_Muons();
  std::vector<const Analysis::Muon*> trackMuons                 = muondata->used_MuonBoy_Track_Muons();

  for (std::vector<const Analysis::Muon*>::iterator trItr = spectroMuons.begin(); trItr != spectroMuons.end(); ++trItr ) {
    trackMuons.push_back(*trItr);
  }

  const MuonContainer* calomuonTES;
  sc=evtStore()->retrieve( calomuonTES, _CaloMuonCollectionKey);
  if( sc.isFailure()  ||  !calomuonTES ) {
    msg() << MSG::WARNING
          << "No CaloMuonCollection AOD muon container " << _CaloMuonCollectionKey << " found in TDS"
          << endreq;
    return StatusCode::SUCCESS;
  }

 const JetCollection* jetTEStopo4 = 0;
 sc=evtStore()->retrieve( jetTEStopo4, _JetCollectionKey);
  if( sc.isFailure()  ||  !jetTEStopo4 ) {
    msg() << MSG::WARNING
          << "No AOD Cone4TopoParticleJets jet container " << _CaloMuonCollectionKey << " found in TDS"
          << endreq; 
    return StatusCode::SUCCESS;
  }

  bool truthMatched = false;
  bool jetMatched = false;

  for (std::vector<const Analysis::Muon*>::const_iterator muonBoyItr = trackMuons.begin(); muonBoyItr != trackMuons.end(); ++muonBoyItr ) {
    HepLorentzVector muonHLV = (*muonBoyItr)->hlv();
    truthMatched = false;
    jetMatched = false;
    for (std::vector<const HepMC::GenParticle*>::const_iterator it = truth_muons.begin(); it != truth_muons.end(); ++it ) {
      HepLorentzVector t1((*it)->momentum().px(),(*it)->momentum().py(),(*it)->momentum().pz(),(*it)->momentum().e());
      if(t1.deltaR(muonHLV) < .02) truthMatched = true;
    }//end of truth loop

    for(JetCollection::const_iterator jetItr = jetTEStopo4->begin(); jetItr != jetTEStopo4->end(); ++jetItr)
      {
	HepLorentzVector j1  = (*jetItr)->hlv();
	if(j1.deltaR(muonHLV) < .2) jetMatched = true;
      }//end of jet loop  

    if(fabs((*muonBoyItr)->eta()) < 2.5)
      {
	h_eta_dist_allMuonBoy->Fill((*muonBoyItr)->eta());	
	h_phi_dist_allMuonBoy->Fill((*muonBoyItr)->phi());
	h_Pt_dist_allMuonBoy->Fill((*muonBoyItr)->pt()/GeV);
	h_2D_phiVsEta_allMuonBoy->Fill((*muonBoyItr)->eta(), (*muonBoyItr)->phi());
	if(truthMatched)
	  {
	    h_MC_2D_truthMatched_IsolationVsPt_LogLogMuonBoy->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	    h_MC_2D_truthMatched_IsolationVsPtMuonBoy->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	 
	    h_MC_eta_dist_truthMatchedMuonBoy->Fill((*muonBoyItr)->eta());
	    h_MC_phi_dist_truthMatchedMuonBoy->Fill((*muonBoyItr)->phi());
	    h_MC_Pt_dist_truthMatchedMuonBoy->Fill((*muonBoyItr)->pt()/GeV);
	    h_MC_2D_phiVsEta_truthMatchedMuonBoy->Fill((*muonBoyItr)->eta(), (*muonBoyItr)->phi());
	  }
	if(!truthMatched)
	  {
	    h_MC_2D_truthUnmatched_IsolationVsPt_LogLogMuonBoy->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	    h_MC_2D_truthUnmatched_IsolationVsPtMuonBoy->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	 
	    h_MC_eta_dist_truthUnmatchedMuonBoy->Fill((*muonBoyItr)->eta());
	    h_MC_phi_dist_truthUnmatchedMuonBoy->Fill((*muonBoyItr)->phi());
	    h_MC_Pt_dist_truthUnmatchedMuonBoy->Fill((*muonBoyItr)->pt()/GeV);
	    h_MC_2D_phiVsEta_truthUnmatchedMuonBoy->Fill((*muonBoyItr)->eta(), (*muonBoyItr)->phi());
	  }

	if(jetMatched)
	  {
	    h_2D_jetMatched_IsolationVsPt_LogLogMuonBoy->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	    h_2D_jetMatched_IsolationVsPtMuonBoy->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	 
	    h_eta_dist_jetMatchedMuonBoy->Fill((*muonBoyItr)->eta());
	    h_phi_dist_jetMatchedMuonBoy->Fill((*muonBoyItr)->phi());
	    h_Pt_dist_jetMatchedMuonBoy->Fill((*muonBoyItr)->pt()/GeV);
	    h_2D_phiVsEta_jetMatchedMuonBoy->Fill((*muonBoyItr)->eta(), (*muonBoyItr)->phi());
	  }
      }

//     if((*muonBoyItr)->isLowPt() == 1 && (*muonBoyItr)->isHighPt() == 0 && fabs((*muonBoyItr)->eta()) > 1.0 && fabs((*muonBoyItr)->eta()) < 1.3) 
    if((*muonBoyItr)->isSegmentTaggedMuon() && (!(*muonBoyItr)->isTight()) && fabs((*muonBoyItr)->eta()) > 1.0 && fabs((*muonBoyItr)->eta()) < 1.3) 
      {
	h_eta_dist_allMutag->Fill((*muonBoyItr)->eta());
	h_phi_dist_allMutag->Fill((*muonBoyItr)->phi());
	h_Pt_dist_allMutag->Fill((*muonBoyItr)->pt()/GeV);
	h_2D_phiVsEta_allMutag->Fill((*muonBoyItr)->eta(), (*muonBoyItr)->phi());

	if(truthMatched)
	  {
	    h_MC_2D_truthMatched_IsolationVsPt_LogLogMutag->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	    h_MC_2D_truthMatched_IsolationVsPtMutag->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	 
	    h_MC_eta_dist_truthMatchedMutag->Fill((*muonBoyItr)->eta());
	    h_MC_phi_dist_truthMatchedMutag->Fill((*muonBoyItr)->phi());
	    h_MC_Pt_dist_truthMatchedMutag->Fill((*muonBoyItr)->pt()/GeV);
	    h_MC_2D_phiVsEta_truthMatchedMutag->Fill((*muonBoyItr)->eta(), (*muonBoyItr)->phi());
	  }
	if(!truthMatched)
	  {
	    h_MC_2D_truthUnmatched_IsolationVsPt_LogLogMutag->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	    h_MC_2D_truthUnmatched_IsolationVsPtMutag->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	 
	    h_MC_eta_dist_truthUnmatchedMutag->Fill((*muonBoyItr)->eta());
	    h_MC_phi_dist_truthUnmatchedMutag->Fill((*muonBoyItr)->phi());
	    h_MC_Pt_dist_truthUnmatchedMutag->Fill((*muonBoyItr)->pt()/GeV);
	    h_MC_2D_phiVsEta_truthUnmatchedMutag->Fill((*muonBoyItr)->eta(), (*muonBoyItr)->phi());
	  }
	if(jetMatched)
	  {
	    h_2D_jetMatched_IsolationVsPt_LogLogMutag->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	    h_2D_jetMatched_IsolationVsPtMutag->Fill((*muonBoyItr)->pt()/GeV, (*muonBoyItr)->parameter(MuonParameters::etcone40)/GeV);
	 
	    h_eta_dist_jetMatchedMutag->Fill((*muonBoyItr)->eta());
	    h_phi_dist_jetMatchedMutag->Fill((*muonBoyItr)->phi());
	    h_Pt_dist_jetMatchedMutag->Fill((*muonBoyItr)->pt()/GeV);
	    h_2D_phiVsEta_jetMatchedMutag->Fill((*muonBoyItr)->eta(), (*muonBoyItr)->phi());
	  }
      }
  }//end of muonboy loop

  for(MuonContainer::const_iterator caloMuonItr = calomuonTES->begin(); caloMuonItr != calomuonTES->end(); ++caloMuonItr)
    {
      if((*caloMuonItr)->caloMuonAlsoFoundByMuonReco() == 0 && fabs((*caloMuonItr)->eta()) < .1)
	{
	  HepLorentzVector caloMuonHLV = (*caloMuonItr)->hlv();//caloMuonHLVIsoVector->hlv(i);
	  truthMatched = false;
	  jetMatched = false;

	  for (std::vector<const HepMC::GenParticle*>::const_iterator it = truth_muons.begin(); it != truth_muons.end(); ++it ) {
	    HepLorentzVector t1((*it)->momentum().px(),(*it)->momentum().py(),(*it)->momentum().pz(),(*it)->momentum().e());
	    if(t1.deltaR(caloMuonHLV) < .02) truthMatched = true;
	  }//end of truth loop

	  for(JetCollection::const_iterator jetItr = jetTEStopo4->begin(); jetItr != jetTEStopo4->end(); ++jetItr)
	    {
	      HepLorentzVector j1  = (*jetItr)->hlv();
	      if(j1.deltaR(caloMuonHLV) < .2) jetMatched = true;
	    }//end of jet loop 

	  h_eta_dist_allCaloMuons->Fill((*caloMuonItr)->eta());
	  h_phi_dist_allCaloMuons->Fill((*caloMuonItr)->phi());
	  h_Pt_dist_allCaloMuons->Fill((*caloMuonItr)->pt()/GeV);
	  h_2D_phiVsEta_allCaloMuons->Fill((*caloMuonItr)->eta(), (*caloMuonItr)->phi());
	  if(truthMatched)
	    {
	      h_MC_2D_truthMatched_IsolationVsPt_LogLogCaloMuons->Fill((*caloMuonItr)->pt()/GeV, (*caloMuonItr)->parameter(MuonParameters::etcone40)/GeV);
	      h_MC_2D_truthMatched_IsolationVsPtCaloMuons->Fill((*caloMuonItr)->pt()/GeV, (*caloMuonItr)->parameter(MuonParameters::etcone40)/GeV);
	      
	      h_MC_eta_dist_truthMatchedCaloMuons->Fill((*caloMuonItr)->eta());
	      h_MC_phi_dist_truthMatchedCaloMuons->Fill((*caloMuonItr)->phi());
	      h_MC_Pt_dist_truthMatchedCaloMuons->Fill((*caloMuonItr)->pt()/GeV);
	      h_MC_2D_phiVsEta_truthMatchedCaloMuons->Fill((*caloMuonItr)->eta(), (*caloMuonItr)->phi());	      
	    }
	  if(!truthMatched)
	    {
	      h_MC_2D_truthUnmatched_IsolationVsPt_LogLogCaloMuons->Fill((*caloMuonItr)->pt()/GeV, (*caloMuonItr)->parameter(MuonParameters::etcone40)/GeV);
	      h_MC_2D_truthUnmatched_IsolationVsPtCaloMuons->Fill((*caloMuonItr)->pt()/GeV, (*caloMuonItr)->parameter(MuonParameters::etcone40)/GeV);
	      
	      h_MC_eta_dist_truthUnmatchedCaloMuons->Fill((*caloMuonItr)->eta());
	      h_MC_phi_dist_truthUnmatchedCaloMuons->Fill((*caloMuonItr)->phi());
	      h_MC_Pt_dist_truthUnmatchedCaloMuons->Fill((*caloMuonItr)->pt()/GeV);
	      h_MC_2D_phiVsEta_truthUnmatchedCaloMuons->Fill((*caloMuonItr)->eta(), (*caloMuonItr)->phi());	      
	    }
	  
	  if(jetMatched)
	    {
	      h_2D_jetMatched_IsolationVsPt_LogLogCaloMuons->Fill((*caloMuonItr)->pt()/GeV, (*caloMuonItr)->parameter(MuonParameters::etcone40)/GeV);
	      h_2D_jetMatched_IsolationVsPtCaloMuons->Fill((*caloMuonItr)->pt()/GeV, (*caloMuonItr)->parameter(MuonParameters::etcone40)/GeV);
	      
	      h_eta_dist_jetMatchedCaloMuons->Fill((*caloMuonItr)->eta());
	      h_phi_dist_jetMatchedCaloMuons->Fill((*caloMuonItr)->phi());
	      h_Pt_dist_jetMatchedCaloMuons->Fill((*caloMuonItr)->pt()/GeV);
	      h_2D_phiVsEta_jetMatchedCaloMuons->Fill((*caloMuonItr)->eta(), (*caloMuonItr)->phi());
	    }
	}
    }//end of calomuon loop

  for(JetCollection::const_iterator jetItr2 = jetTEStopo4->begin(); jetItr2 != jetTEStopo4->end(); ++jetItr2) {
    if(fabs((*jetItr2)->eta()) < 2.5){
      h_eta_dist_Jets->Fill((*jetItr2)->eta());
      h_phi_dist_Jets->Fill((*jetItr2)->phi());
      h_Pt_dist_Jets->Fill((*jetItr2)->pt()/GeV);
      h_2D_phiVsEta_Jets->Fill((*jetItr2)->eta(), (*jetItr2)->phi());
    }
  }//end of the jet loop
 
  for (std::vector<const HepMC::GenParticle*>::const_iterator it = truth_muons.begin(); it != truth_muons.end(); ++it ) {
    if(fabs((*it)->momentum().eta()) < 2.5 && sqrt(pow((*it)->momentum().px(),2)+pow((*it)->momentum().py(),2))/GeV > 2.0) {
      h_MC_eta_dist_Truth->Fill((*it)->momentum().eta());
      h_MC_phi_dist_Truth->Fill((*it)->momentum().phi());
      h_MC_Pt_dist_Truth->Fill(sqrt(pow((*it)->momentum().px(),2)+pow((*it)->momentum().py(),2))/GeV);
      h_MC_2D_phiVsEta_Truth->Fill((*it)->momentum().eta(), (*it)->momentum().phi()); 
    }
  }//end of truth loop

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode MuonTool::finalize() {
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
MuonTool::~MuonTool() {}
