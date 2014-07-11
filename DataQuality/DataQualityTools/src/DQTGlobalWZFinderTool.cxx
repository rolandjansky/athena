/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTGlobalWZFinderTool.cxx
// PACKAGE:  DataQualityTools  
// 
// AUTHORS:   Jahred Adelman (jahred.adelman@cern.ch)
//            Simon Viel (svielcern.ch)
//            Sam King (samking@physics.ubc.ca)
//            Koos van Nieuwkoop (jvannieu@cern.ch)
//
// ********************************************************************

#include "DataQualityTools/DQTGlobalWZFinderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

//#include "egammaEvent/ElectronContainer.h"
//#include "egammaEvent/EMTrackMatch.h"
//#include "muonEvent/Muon.h"
//#include "muonEvent/MuonContainer.h"
//#include "MissingETEvent/MissingET.h"

//#include "VxVertex/VxContainer.h"
//#include "VxVertex/VxCandidate.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
// #include "xAODJet/JetContainer.h"
//#include "xAODEgamma/ElectronContainer.h"
//#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
//#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEventInfo/EventInfo.h"
//#include "xAODBTagging/BTagging.h"

//#include "TrkTrack/Track.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "TrkVertexAnalysisUtils/V0Tools.h"
//EP #include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

#include <vector>

#include "xAODEventInfo/EventInfo.h"

using xAOD::Jet;
using xAOD::JetContainer;

//----------------------------------------------------------------------------------
DQTGlobalWZFinderTool::DQTGlobalWZFinderTool(const std::string & type, 
		   const std::string & name,
		   const IInterface* parent)
   : DataQualityFatherMonTool(type, name, parent),
     m_electronContainerName("ElectronCollection"),
     m_egDetailContainerName("egDetailAOD"),
     //m_VxPrimContainerName("VxPrimaryCandidate"), //Kshort
     m_VxPrimContainerName("PrimaryVertices"),
     m_VxContainerName("SecVertices"), //Kshort
      m_METName("MET_LocHadTopo"),
     m_muonContainerName("Muons"),
     m_jetCollectionName("AntiKt4EMTopoJets"),
     m_tracksName("InDetTrackParticles"), //Kshort
     m_electronEtCut(20000),
     m_metCut(25000),
     m_doEtaCuts(0),
     m_zCutLow(86.0),
     m_zCutHigh(96.0)//,
     //EP m_xaodConverter("Trk::VxCandidateXAODVertex")
  
//  declareProperty( "JetContainerName", m_jetName = "AntiKt4EMTopoJets" );
//  declareProperty( "ElectronContainerName", m_elecName = "ElectronCollection" ); 
//  declareProperty( "PhotonContainerName", m_photonName = "PhotonCollection" ); 
//  declareProperty( "MuonContainerName", m_muonName = "Muons" ); 
//  declareProperty( "TauContainerName", m_tauName = "TauRecContainer" );
//  declareProperty( "TrackContainerName", m_trackName = "InDetTrackParticles" );  
//  declareProperty( "VertexContainerName", m_vertexName = "PrimaryVertices" );  
//  declareProperty( "METContainerName", m_metName = "MET_RefFinal" ); 
    
//----------------------------------------------------------------------------------

{
  declareInterface<IMonitorToolBase>(this);
  m_path = "GLOBAL/DQTGlobalWZFinder";
  declareProperty("ElectronContainerName", m_electronContainerName);
  declareProperty("ElectronDetailContainerName", m_egDetailContainerName);
  declareProperty("MetName", m_METName);
  declareProperty("MuonContainerName",m_muonContainerName);
  declareProperty("JetCollectionName", m_jetCollectionName);
  declareProperty("ElectronEtCut",m_electronEtCut);
  declareProperty("MetCut",m_metCut);
  declareProperty("doRunCosmics", m_doRunCosmics = 1);
  declareProperty("doRunBeam", m_doRunBeam = 1);
  declareProperty("doOfflineHists", m_doOfflineHists = 1);
  declareProperty("doOnlineHists", m_doOnlineHists = 1);
  declareProperty("doEtaCuts", m_doEtaCuts = 1);
  declareProperty("ZCutLow", m_zCutLow = 86.0);
  declareProperty("ZCutHigh", m_zCutHigh = 96.0);
  declareProperty("doTrigger", m_doTrigger = 1);    
  //EP declareProperty("XAODConverter",m_xaodConverter);

  m_W_mt_ele = 0;
  m_W_mt_mu = 0;
  m_W_pt_v_met_ele = 0;
  m_W_pt_v_met_mu = 0;
  m_Z_mass_opsele = 0;
  m_Z_mass_opsmu = 0;
  m_Z_mass_ssele = 0;
  m_Z_mass_ssmu = 0;
  m_Z_Q_ele = 0;
  m_Z_Q_mu = 0;
  m_metHist = 0;
  m_metPhiHist = 0;
  
  m_graph_Wenu = 0;
  m_graph_Wmunu = 0;
  m_graph_Zee_ops = 0;
  m_graph_Zmumu_ops = 0;
  m_graph_Zee_ss = 0;
  m_graph_Zmumu_ss = 0;
}

//----------------------------------------------------------------------------------
DQTGlobalWZFinderTool::~DQTGlobalWZFinderTool()
//----------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::bookHistograms( )
//StatusCode DQTGlobalWZFinderTool::bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
//----------------------------------------------------------------------------------
{
  bool failure(false);

  MsgStream log(msgSvc(), name());
  printedErrorEleContainer = false;
  printedErrorMuColl = false;
  printedErrorMet = false;
  printedErrorTrackContainer = false;

  //Initialize counters for rate monitors
  for (int index =0 ; index < 2 ; index++) {
    JPsiCounterSBG[index]=0;
    JPsiCounter2BG[index]=0;
    JPsiCounterSignal[index]=0;

    UpsilonCounterSBG[index]=0;
    UpsilonCounterBG[index]=0;
    UpsilonCounterSignal[index]=0;

    KshortCounterSBG[index]=0;
    KshortCounterBG[index]=0;
    KshortCounterSignal[index]=0;

    ZBosonCounterSBG_El[index]=0;
    ZBosonCounterSBG_Mu[index]=0;
  }
  
  //if (newRun) {
     log << MSG::DEBUG << "in bookHistograms() and m_doRunCosmics = " << m_doRunCosmics << " and m_doRunBeam = " << m_doRunBeam << endreq;
     log << MSG::DEBUG << "Using base path " << m_path << endreq;
     
     failure = bookDQTGlobalWZFinderTool();
  //}
  //else if (newEventsBlock || newLumiBlock) {
  //  return StatusCode::SUCCESS;
  //}
  if (failure) {return  StatusCode::FAILURE;}
  else {return StatusCode::SUCCESS;}
}	

		

//----------------------------------------------------------------------------------
bool DQTGlobalWZFinderTool::bookDQTGlobalWZFinderTool()
//----------------------------------------------------------------------------------
{
  bool failure(false);
  //  if (isNewEventsBlock || isNewLumiBlock || isNewRun) {
  MsgStream log(msgSvc(), name());
 
  std::string  fullPathDQTGlobalWZFinder=m_path+"/DQTGlobalWZFinder";
  
  if (m_doRunBeam) {
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_mt_ele = TH1F_LW::create("m_W_mt_ele","W#rightarrowe#nu M_{T} ",40,20*1000,400*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_mt_mu = TH1F_LW::create("m_W_mt_mu","W#rightarrow#mu#nu M_{T} ",40,20*1000,400*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_pt_v_met_ele = TH2F_LW::create("m_W_pt_v_met_ele","W#rightarrowe#nu e E_{T} vs met ",100,5*1000,200*1000, 100, 5*1000, 200*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_pt_v_met_mu = TH2F_LW::create("m_W_pt_v_met_mu","W#rightarrow#mu#nu #mu p_{T} vs met ",100,5*1000,200*1000, 100, 5*1000, 200*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_metHist = TH1F_LW::create("m_metHist","MET",50,0*1000,500*1000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_metPhiHist = TH1F_LW::create("m_metPhiHist","MET phi",10,-1.6,1.6)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_opsele = TH1F_LW::create("m_Z_mass_opsele","Z#rightarrowee (op. sign) Mass ",40, 70000, 110000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_opsmu = TH1F_LW::create("m_Z_mass_opsmu","Z#rightarrow#mu#mu (op. sign) Mass ",40, 70000, 110000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_JPsi_mass_opsmu = TH1F_LW::create("m_JPsi_mass_opsmu","JPsi#rightarrow#mu#mu (op. sign) Mass ",40, 2600, 3600)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Upsilon_mass_opsmu = TH1F_LW::create("m_Upsilon_mass_opsmu", "Upsilon#rightarrow#mu#mu", 40, 4500, 14500)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Kshort_mass_pipi = TH1F_LW::create("m_Kshort_mass_pipi","K^{0}_{S}#rightarrow#pi#pi (op. sign) Mass ",40, 450, 550)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_ssele = TH1F_LW::create("m_Z_mass_ssele","Z#rightarrowee (same sign) Mass ",40, 70000, 110000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_ssmu = TH1F_LW::create("m_Z_mass_ssmu","Z#rightarrow#mu#mu (same sign) Mass ",40, 70000, 110000)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_Q_ele = TH1F_LW::create("m_Z_Q_ele","Z#rightarrowee Charge ",7, -3, 3)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_Q_mu = TH1F_LW::create("m_Z_Q_mu","Z#rightarrow#mu#mu Charge ",7, -3, 3)).isFailure();


     //Resonance particle rate monitoring
     minLumiBlock  = 0.0;
     maxLumiBlock = 1200.0;
     numBins = 30.0;
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_El = TH1F_LW::create("m_Z_Counter_el","Z to e e Count per Lumi Block",numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_Mu = TH1F_LW::create("m_Z_Counter_mu","Z to mu mu Count per Lumi Block", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_JPsiCounter_Mu   = TH1F_LW::create("m_JPsi_Counter_mu", "JPsi to mu mu Count per Lumi Block", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_UpsilonCounter_Mu   = TH1F_LW::create("m_Upsilon_Counter_mu", "Upsilon to mu mu Count per Lumi Block", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_KshortCounter_Pi   = TH1F_LW::create("m_Kshort_Counter_pi", "Kshort to pi pi Count per Lumi Block", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();

     if(m_doTrigger) {
       failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_El_triggerAware = TH1F_LW::create("m_Z_Counter_el_triggerAware","Z to e e Count per Lumi Block (trigger aware)",numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
       failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_Mu_triggerAware = TH1F_LW::create("m_Z_Counter_mu_triggerAware","Z to mu mu Count per Lumi Block (trigger aware)", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
       failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_JPsiCounter_Mu_triggerAware   = TH1F_LW::create("m_JPsi_Counter_mu_triggerAware", "JPsi to mu mu Count per Lumi Block (trigger aware)", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
       failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_UpsilonCounter_Mu_triggerAware   = TH1F_LW::create("m_Upsilon_Counter_mu_triggerAware", "Upsilon to mu mu Count per Lumi Block (trigger aware)", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
       failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_KshortCounter_Pi_triggerAware   = TH1F_LW::create("m_Kshort_Counter_pi_triggerAware", "Kshort to pi pi Count per Lumi Block (trigger aware)", numBins, minLumiBlock+0.5, maxLumiBlock+0.5)).isFailure();
     }

     m_graph_Wenu = new TGraph();
     m_graph_Wenu->SetNameTitle("graph_Wenu","W#rightarrowe#nu candidates");
     m_graph_Wenu->GetYaxis()->SetTitle("Event Number");
     m_graph_Wenu->GetXaxis()->SetTitle("LumiBlock");
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_graph_Wenu).isFailure();

     m_graph_Wmunu = new TGraph();
     m_graph_Wmunu->SetNameTitle("graph_Wmunu","W#rightarrow#mu#nu candidates");
     m_graph_Wmunu->GetYaxis()->SetTitle("Event Number");
     m_graph_Wmunu->GetXaxis()->SetTitle("LumiBlock");
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_graph_Wmunu).isFailure();

     m_graph_Zee_ops = new TGraph();
     m_graph_Zee_ops->SetNameTitle("graph_Zee_ops","Z#rightarrowee (op. sign) candidates");
     m_graph_Zee_ops->GetYaxis()->SetTitle("Event Number");
     m_graph_Zee_ops->GetXaxis()->SetTitle("LumiBlock");
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_graph_Zee_ops).isFailure();

     m_graph_Zmumu_ops = new TGraph();
     m_graph_Zmumu_ops->SetNameTitle("graph_Zmumu_ops","Z#rightarrow#mu#mu (op. sign) candidates");
     m_graph_Zmumu_ops->GetYaxis()->SetTitle("Event Number");
     m_graph_Zmumu_ops->GetXaxis()->SetTitle("LumiBlock");
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_graph_Zmumu_ops).isFailure();

     m_graph_Zee_ss = new TGraph();
     m_graph_Zee_ss->SetNameTitle("graph_Zee_ss","Z#rightarrowee (same sign) candidates");
     m_graph_Zee_ss->GetYaxis()->SetTitle("Event Number");
     m_graph_Zee_ss->GetXaxis()->SetTitle("LumiBlock");
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_graph_Zee_ss).isFailure();

     m_graph_Zmumu_ss = new TGraph();
     m_graph_Zmumu_ss->SetNameTitle("graph_Zmumu_ss","Z#rightarrow#mu#mu (same sign) candidates");
     m_graph_Zmumu_ss->GetYaxis()->SetTitle("Event Number");
     m_graph_Zmumu_ss->GetXaxis()->SetTitle("LumiBlock");
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_graph_Zmumu_ss).isFailure();
  }
  
  if (failure) {
     log << MSG::ERROR << "Error Booking histograms " << endreq;
  }

  if(m_doTrigger ) {
    StatusCode sctdt = StatusCode::SUCCESS;
    sctdt = m_trigDec.retrieve();
    if ( sctdt.isFailure() ){
      log << MSG::ERROR << "Can't get handle on TrigDecisionTool" << endreq;
    } else {
      log << MSG::DEBUG << "Got handle on TrigDecisionTool" << endreq;
    }
  }


  return failure;

}


//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::fillHistograms()
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());

  if (m_doRunBeam) {  
     StatusCode sc = StatusCode::SUCCESS;

     //Get LumiBlock and EventNumber
     
     //Int_t lumiBlock = 0;
     //Int_t eventNumber = 0;
	     
     const xAOD::EventInfo* thisEventInfo;
     sc = evtStore()->retrieve(thisEventInfo);
     if(sc.isFailure()) 
       {
	 log << MSG::ERROR << "Could not find EventInfo in evtStore()" << endreq;
	 return sc;
       }
     else
       {
	 lumiBlock = thisEventInfo->lumiBlock() ;
	 eventNumber  = thisEventInfo->eventNumber();
       }
  
     //Get MET
     
     Double_t phiMet = 0, metMet = 0;
     //     const MissingET *missET;
     const xAOD::MissingETContainer *missETcont(0);
     const xAOD::MissingET* missET;
     if ( evtStore()->contains<xAOD::MissingETContainer>(m_METName) ) {
       evtStore()->retrieve(missETcont,m_METName);
       missET = (*missETcont)["LocHadTopo"];
       if (!missET){
	 log << MSG::WARNING << "Cannot retrieve xAOD::MissingET LocHadTopo"  << endreq;
	 if (!printedErrorMet)
	   log << MSG::WARNING << "Cannot retrieve " << m_METName << endreq;
	 printedErrorMet = true;
       }
       else {
	 phiMet = missET->phi();
	 metMet = missET->sumet();
       }
     }
     else {
        if (!printedErrorMet)
           log << MSG::WARNING << "Cannot retrieve xAOD::MissingETContainer " << m_METName << endreq;
        printedErrorMet = true;
     }

     log << MSG::DEBUG << " MET = " << metMet << " and met phi = " << phiMet << endreq;


     //Get Electrons
     
     //     const ElectronContainer* elecTES = 0;
     const xAOD::ElectronContainer* elecTES = 0;
     if ( evtStore()->contains<xAOD::ElectronContainer>(m_electronContainerName) ) {
        sc=evtStore()->retrieve( elecTES, m_electronContainerName);
        if( sc.isFailure()  ||  !elecTES ) {
	  if (!printedErrorEleContainer) log << MSG::WARNING << "No electron container" <<  m_electronContainerName << " found in evtStore" << endreq; 
           printedErrorEleContainer = true;
           return StatusCode::SUCCESS;
        }
     }
     else {
        if (!printedErrorEleContainer) log << MSG::WARNING << "No electron container" <<  m_electronContainerName << " found in evtStore" << endreq; 
        printedErrorEleContainer = true;
        return StatusCode::SUCCESS;
     }
     
     log << MSG::DEBUG << "ElectronContainer successfully retrieved" << endreq;


     //Get Muons
        
     //     const Analysis::MuonContainer* muons  = 0;
     const xAOD::MuonContainer* muons  = 0;
     if(evtStore()->contains<xAOD::MuonContainer>(m_muonContainerName)){
        sc = evtStore()->retrieve(muons,m_muonContainerName);
        if (sc.isFailure()) {
           if (!printedErrorMuColl) log << MSG::WARNING << "No muon collection with name " << m_muonContainerName << " found in evtStore" << endreq;
           printedErrorMuColl = true;
           return StatusCode::SUCCESS;
        }
     } else {
        if (!printedErrorMuColl) log << MSG::WARNING << "evtStore() does not contain muon Collection with name "<< m_muonContainerName <<" " <<endreq;
        printedErrorMuColl = true;
        return StatusCode::SUCCESS;
     }

     log << MSG::DEBUG << "Got muon collection! " << endreq;

     std::vector<CLHEP::HepLorentzVector> goodelectrons;
     std::vector<Int_t> goodelectroncharge;
     std::vector<CLHEP::HepLorentzVector> goodmuonsZ;
     std::vector<CLHEP::HepLorentzVector> goodmuonsJPsi;
     std::vector<Int_t> goodmuonZcharge;
     std::vector<Int_t> goodmuonJPsicharge;

     //get primary vertex info
     //const VxContainer *m_vertices;
     const xAOD::VertexContainer* m_vertices(0);
     bool vertexCut = false;
     
     //EP CHECK( m_xaodConverter.retrieve() );

     if ( evtStore()->contains<xAOD::VertexContainer>(m_VxPrimContainerName)) {
       sc = evtStore()->retrieve(m_vertices,m_VxPrimContainerName);

       if (sc.isFailure()) {
	 log << MSG::WARNING << "No collection with name " << m_VxPrimContainerName << " found in evtStore()" << endreq;
	 //return StatusCode::SUCCESS;
       }
       else {
	 log << MSG::DEBUG << "Collection with name " << m_VxPrimContainerName << " with size " << m_vertices->size() << " found in evtStore()" << endreq;
	 //	 VxContainer::const_iterator vxItr = m_vertices->begin();
	 //      VxContainer::const_iterator vxItrE = m_vertices->end();
	 xAOD::VertexContainer::const_iterator vxItr = m_vertices->begin();
	 xAOD::VertexContainer::const_iterator vxItrE = m_vertices->end();
	 //    for(xAOD::VertexContainer::const_iterator vi=vertices->begin(); vi!=vertices->end()-1; vi++){
	 int ntrkMin=3;
	 for (; vxItr != vxItrE; ++vxItr) {
           if (! ((*vxItr)->vxTrackAtVertexAvailable())) continue;

	   int numTracksPerVertex = (*vxItr)->vxTrackAtVertex().size();
	   if (numTracksPerVertex>(ntrkMin-1)) {
	     vertexCut = true;
	    }
	 }


	 //Get Pi tracks for Kshort monitoring (code from InDetPerformanceMonitoring, IDPerfMonKshort.cxx)
	 const xAOD::TrackParticleContainer* tracks = 0;
	 StatusCode scTracks = evtStore()->retrieve(tracks,m_tracksName);  

	 if (scTracks.isFailure()) {
	   log << MSG::WARNING << "No Collection with name "<<m_tracksName<<" found in evtStore()" << endreq;
	   //return StatusCode::SUCCESS;
	 } 
	 else {
	   log << MSG::DEBUG << "Collection with name "<<m_tracksName<<" found in evtStore()" << endreq;

	   const  xAOD::VertexContainer* vertices = 0;
	   StatusCode scVertices = evtStore()->retrieve(vertices,m_VxPrimContainerName);
	   if (scVertices.isFailure()) {
	     log << MSG::DEBUG << "No Collection with name "<<m_VxPrimContainerName<<" found in evtStore()" << endreq;
	     //return StatusCode::SUCCESS;
	   }
	   else {
	     log << MSG::DEBUG << "Collection with name "<<m_VxPrimContainerName<<" found in evtStore()" << endreq;

	     // Trk::RecVertex *primaryVertex=&((*(vertices->begin()))->recVertex());
	     //EP xAOD::Vertex* primaryVx(0);
	     //EP for (xAOD::VertexContainer::const_iterator fz = vertices->begin(); fz != vertices->end(); ++fz) {
	     //EP   if ((*fz)->vertexType() == xAOD::VxType::PriVtx) {
	     //EP     primaryVx = *fz;
	     //EP     break;
	     //EP   }
	     //EP }
             //EP xAOD::Vertex* primaryVertex = primaryVx;
	     //EP Trk::VxCandidate* primVxCandidate;
	     //EP if( m_xaodConverter->createVxCandidate(*primaryVx,primVxCandidate).isFailure() ) {
	     //EP         ATH_MSG_ERROR("Failed to create VxCandidate for xAODVertex.");
	     //EP }
	     
	     //EP Trk::RecVertex* primaryVertex = &(primVxCandidate->recVertex());
	     
	     
	     //	     const VxContainer* theVxContainer (0);
	     const xAOD::VertexContainer* theVxContainer (0);
	     if(evtStore()->contains<xAOD::VertexContainer>(m_VxContainerName)){
	       scVertices = evtStore()->retrieve(theVxContainer,m_VxContainerName);
	       if (scVertices.isFailure()) {
		 log << MSG::WARNING << "No Collection with name "<<m_VxContainerName<<" found in evtStore()" << endreq;
		 //return StatusCode::SUCCESS;
	       }
	       else {
		 // iterators over the V0 container
		 xAOD::VertexContainer::const_iterator Itr = theVxContainer->begin();
		 xAOD::VertexContainer::const_iterator ItrEnd = theVxContainer->end(); 
		 //log << MSG::DEBUG << "V0 Iterator started" << endreq;
		 
		 //EP Trk::VxCandidate* theVxCandidate;
		 ToolHandle <Trk::V0Tools> myV0Tools("Trk::V0Tools");
		 // = new Trk::V0Tools("V0Tools","myV0Tools",m_parent);
		 //EP double piMass = 139.57018;
		 //EP double ksMassPDG = 497.648; 
		 for (; Itr != ItrEnd; ++Itr) {
		   //EP xAOD::Vertex* vx = (*Itr);
		   //createVxCandidate(vx, theVxCandidate);
		   //EP if( m_xaodConverter->createVxCandidate(*vx,theVxCandidate).isFailure() ) {
		   //EP   ATH_MSG_ERROR("Failed to create VxCandidate for xAODVertex.");
		   //EP   continue;
		   //EP }
		   //const Trk::V0Hypothesis* myV0Hypothesis = myV0Tools->v0Unconstr(theV0Candidate);
		   //const Trk::V0Hypothesis* myKsHypothesis = myV0Tools->v0Kshort(theV0Candidate);
		  
                   // EP: V0Tools is not yet completely migrated to xAOD
                   // see https://indico.cern.ch/event/314902/contribution/7/material/slides/0.pdf slide 5
                   // Useless to rewrite it here. Comment out this part until the tool is migrated.
                   // (the m_xaodConverter gives problems at the ESD2AOD step, not using it for now)
                   /*
		   double ksMass = myV0Tools->invariantMass((const Trk::ExtendedVxCandidate*)theVxCandidate,piMass,piMass);
		   // double ksMassConstrained = myV0Tools->invariantMass((const Trk::ExtendedVxCandidate*)myKsHypothesis,piMass,piMass);
		   //double ksPt = myV0Tools->V04Momentum((const Trk::ExtendedVxCandidate*)theVxCandidate,ksMassPDG).perp();
		   double ksMomentum = myV0Tools->V04Momentum((const Trk::ExtendedVxCandidate*)theVxCandidate,ksMassPDG).vect().mag();
		   CLHEP::Hep3Vector ksMomentumVector = myV0Tools->V04Momentum((const Trk::ExtendedVxCandidate*)theVxCandidate,ksMassPDG).vect();
		   
		   double transverseFlightDistance, totalFlightDistance;
                   Amg::Vector3D flightVector;
		   //EP if(primaryVertex!=NULL) {
		   //EP   transverseFlightDistance = myV0Tools->lxy((const Trk::ExtendedVxCandidate*)theVxCandidate,*primaryVertex);
		   //EP   totalFlightDistance = (myV0Tools->vtx((const Trk::ExtendedVxCandidate*)theVxCandidate)-primaryVertex->position()).mag();
		   //EP   flightVector = myV0Tools->vtx((const Trk::ExtendedVxCandidate*)theVxCandidate)-primaryVertex->position();
		   //EP }
		   //EP else {
		   //EP   transverseFlightDistance = myV0Tools->rxy((const Trk::ExtendedVxCandidate*)theVxCandidate);
		   //EP   totalFlightDistance = (myV0Tools->vtx((const Trk::ExtendedVxCandidate*)theVxCandidate)).mag();      
		   //EP   flightVector = myV0Tools->vtx((const Trk::ExtendedVxCandidate*)theVxCandidate);
		   //EP }
		   
		   double properDecayTime = 1./CLHEP::c_light*ksMassPDG/ksMomentum*totalFlightDistance;
		   
		   double ksPx = ksMomentumVector.x();
		   double ksPy = ksMomentumVector.y();
		   double flightX = flightVector.x();
		   double flightY = flightVector.y();
		   double cosThetaPointing = (ksPx*flightX+ksPy*flightY)/sqrt(ksPx*ksPx+ksPy*ksPy)/sqrt(flightX*flightX+flightY*flightY);
		   
		   int trackPos_nSVTHits = 0;
		   // int trackPos_nPixelHits = 0;
		   int trackNeg_nSVTHits = 0;
		   // int trackNeg_nPixelHits = 0;
		   const Trk::TrackParticleBase* trackPos = myV0Tools->positiveOrigTrackPB((const Trk::ExtendedVxCandidate*)theVxCandidate);
		   if(trackPos!=0) {
		     trackPos_nSVTHits = trackPos->trackSummary()->get(Trk::numberOfSCTHits);
		     // trackPos_nPixelHits = trackPos->trackSummary()->get(Trk::numberOfPixelHits);
		   }
		   const Trk::TrackParticleBase* trackNeg = myV0Tools->negativeOrigTrackPB((const Trk::ExtendedVxCandidate*)theVxCandidate);
		   if(trackNeg!=0) {
		     trackNeg_nSVTHits = trackNeg->trackSummary()->get(Trk::numberOfSCTHits);
		     // trackNeg_nPixelHits = trackNeg->trackSummary()->get(Trk::numberOfPixelHits);
		   }
		   
		   int selectorValue = 0;
		   
		   log << MSG::DEBUG << "ksTau = " << properDecayTime << " Lxy = " <<transverseFlightDistance<< " cosTheta = " << cosThetaPointing <<endreq;
		   log << MSG::DEBUG << "trackPos nSVThits = " << trackPos_nSVTHits << " trackNeg nSVThits = " << trackNeg_nSVTHits <<endreq;
		   // log << MSG::DEBUG << "ksMass = " << ksMass<< " ksMassConstrained = " << ksMassConstrained << endreq;
		   
		   //log << MSG::DEBUG << "ksPt = " << ksPt <<endreq;
		   
		   if( 1 &&
		       properDecayTime > 0.004 && 
		       transverseFlightDistance > 12. &&
		       cosThetaPointing > 0.998 &&
		       ksMass>400. && 
		       ksMass<600. &&
		       trackPos_nSVTHits > 2 &&
		       trackNeg_nSVTHits > 2 ) selectorValue = 1;
		   if(selectorValue==1 ) { 
		     m_Kshort_mass_pipi->Fill(ksMass); 
		     
		     if ((ksMass > 456 && ksMass < 470) || (ksMass > 526 && ksMass < 540)) {
		       ++KshortCounterBG[0];
		       if(m_doTrigger) {
			 if( m_trigDec->isPassed("EF_InDetMon_FS") ) {
			   ++KshortCounterBG[1];
			 }
		       }
		     }
		     if ( ksMass > 484 && ksMass < 512) {
		       ++KshortCounterSBG[0];
		       if(m_doTrigger) {
			 if( m_trigDec->isPassed("EF_InDetMon_FS") ) {
			   ++KshortCounterSBG[1];
			 }
		       }
		     }
		   }
*/
		 }
	       }
	     } 
	     else {
	       log << MSG::WARNING << "evtStore() does not contain Collection with name "<<m_VxContainerName<<" " <<endreq;
	       //return StatusCode::SUCCESS;
	     }
	   }
	 }
       }
     }
     else {
       log << MSG::WARNING << "evtStore() does not contain VxPrimaryCandidate Container" << endreq;
     }



     //MET cleaning
     bool isBad = false;
     const JetContainer* jetTES;
     //std::string m_jetCollectionName = "AntiKt4TopoEMJets";
     bool printedErrorJetCollection = false;
     if (evtStore()->contains<JetContainer>(m_jetCollectionName)) {
       //       log << MSG::DEBUG << "Found JetCollection!" << endreq;
       sc=evtStore()->retrieve(jetTES,m_jetCollectionName);
	      
       //MET cleaning
       //JetContainer::const_iterator fJet = jetTES->begin();
       //JetContainer::const_iterator fJetE = jetTES->end();
       
       //       for (; fJet != fJetE; ++fJet) //loop over jet collection
	 //	 {
       //	   Jet::signalstate_t s = (*fJet)->signalState(); //save the signal state
       //	   JetSignalState::setState(*fJet, P4SignalState::JETEMSCALE); //switch to the em scale
       //	   JetSignalState::setState(*fJet, s); //switch to the initial scale
       //	   isBad = JetCaloQualityUtils::isBad(*fJet); //test jet
       //	   if (isBad) break;
       //	 }
       //
     }
     else {
       log << MSG::WARNING << "Didn't find JetCollection" << endreq;  
       if (!printedErrorJetCollection) log << MSG::WARNING << "No jet collection found in TDS" << endreq;
       printedErrorJetCollection = true;
       isBad = true;
       return StatusCode::SUCCESS;
     }
     log << MSG::DEBUG << "JetCollection successfully retrieved" << endreq;
     

     // Electron Cut Flow
     Int_t El_N = 0;

     //TLorentzVector* tlv_e = new TLorentzvector();
     for (xAOD::ElectronContainer::const_iterator itr=elecTES->begin(); itr != elecTES->end(); ++itr) {
        Float_t eta = (*itr)->eta();
        bool isMedium = 0;
        if (!((*itr)->passSelection(isMedium, "Medium"))) log << MSG::DEBUG << "Electron Medium Not Defined" << endreq;
	if ( ((*itr)->p4().Et() > m_electronEtCut) &&
             ((*itr)->author(xAOD::EgammaParameters::AuthorElectron) == true) &&
             (fabs(eta) < 2.47 && m_doEtaCuts) &&
             //(fabs(eta) < 1.37 && fabs(eta) > 1.52 && m_doEtaCuts) &&
	     (isMedium == false) && // before was ((*itr)->isem((egammaPID::ElectronMedium)) == 0)
	     vertexCut &&
	     !isBad
	     ) {

           El_N++;
	   
           CLHEP::HepLorentzVector thislepton;
           thislepton.setPx((*itr)->p4().Px());
           thislepton.setPy((*itr)->p4().Py());
           thislepton.setPz((*itr)->p4().Pz());
           thislepton.setE((*itr)->p4().E());
           goodelectrons.push_back(thislepton);

           goodelectroncharge.push_back((*itr)->charge());
           //// to be updated to xAOD
           //const EMTrackMatch* pEMTrackMatch = (*itr)->detail<EMTrackMatch>(m_egDetailContainerName);
           //if(pEMTrackMatch) {
           //   Int_t charge = (Int_t)((*itr)->trackParticle()->charge());           
           //   goodelectroncharge.push_back(charge);
           //}
           //else {
           //   Int_t charge = (Int_t)((*itr)->charge());
           //   goodelectroncharge.push_back(charge);
           //}
        }
     }
     //delete tlv_e;        


     // Muon Cut Flow

     Int_t MuZ_N = 0;
     Int_t MuJPsi_N = 0;

     xAOD::MuonContainer::const_iterator muonItr;
     for (muonItr=muons->begin(); muonItr != muons->end(); ++muonItr) {

         // First retreive some standalone muon info
         // This part is imported from PhysicsAnalysis/D3PDMaker/MuonD3PDMaker/src/MuonComponentTrkParameters.cxx

         //EP const Trk::TrackParameters *idtparb=0, *metparb=0;
         //EP const Trk::TrackParameters *idtpar=0, *metpar=0;
         const xAOD::TrackParticle* idtp = *((*muonItr)->inDetTrackParticleLink());
         const xAOD::TrackParticle* metp = *((*muonItr)->muonSpectrometerTrackParticleLink());

         //idtparb = idtp ? & idtp->definingParameters() : 0;
         //metparb = metp ? & metp->definingParameters() : 0;
         //idtpar = idtparb ? dynamic_cast<const Trk::TrackParameters* > (idtparb) : 0;
         //metpar = metparb ? dynamic_cast<const Trk::TrackParameters* > (metparb) : 0;

         //Float_t id_d0 = -99999;
         //Float_t id_z0 = -99999;
         //Float_t id_phi = -99999;
         //Float_t id_theta = -99999;
         Float_t id_qoverp = -99999;
         //Float_t me_d0 = -99999;
         //Float_t me_z0 = -99999;
         //Float_t me_phi = -99999;
         //Float_t me_theta = -99999;
         Float_t me_qoverp = -99999;

         id_qoverp = idtp->qOverP();
         me_qoverp = metp->qOverP();

         //if(idtpar) {
         //  //id_d0 = idtpar->parameters()[Trk::d0];
         //  //id_z0 = idtpar->parameters()[Trk::z0];
         //  //id_phi = idtpar->parameters()[Trk::phi0];
         //  //id_theta = idtpar->parameters()[Trk::theta];
         //  id_qoverp = idtpar->parameters()[Trk::qOverP];
         //}
         //if(metpar) {
         //  //me_d0 = metpar->parameters()[Trk::d0];
         //  //me_z0 = metpar->parameters()[Trk::z0];
         //  //me_phi = metpar->parameters()[Trk::phi0];
         //  //me_theta = metpar->parameters()[Trk::theta];
         //  me_qoverp = metpar->parameters()[Trk::qOverP];
         //}

         // Cut flow from W' early analysis.  Missing cuts: PVzmax,maxd0,maxz0diff 

         //int minTrkinPVCut(3);
         //Float_t maxPVzCut(150);
         Float_t etaCut(2.0);
         Float_t minptCut(20*CLHEP::GeV);
         Float_t minptCutJPsi(1.0*CLHEP::GeV);
         Int_t minPixHitsCut(1);
         Int_t minSCTHitsCut(4);
         Int_t minSiHitsCut(6);
         Float_t maxPMSoverPID(2);
         //Float_t maxd0Cut(1);
         //Float_t maxz0diffCut(5);
         Float_t maxIsoptCut(0.05);
         uint8_t NumOfPixHits = 0;
         uint8_t NumOfSCTHits = 0;
         float   ptcone30=0;
         if (! (*muonItr)->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) )
             log << MSG::DEBUG << "Could not retrieve number of SCT hits"<< endreq;
         if (! (*muonItr)->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) )
             log << MSG::DEBUG << "Could not retrieve number of Pixel hits"<< endreq;
         if (! (*muonItr)->isolation(ptcone30, xAOD::Iso::ptcone30))
             log << MSG::DEBUG << "Could not retrieve ptcone30"<< endreq;

         if ( ((*muonItr)->combinedTrackParticleLink().isValid()) &&
              (fabs((*muonItr)->eta()) < etaCut) &&
              ((*muonItr)->pt() > minptCut) &&
              (NumOfPixHits > minPixHitsCut) &&
              (NumOfSCTHits > minSCTHitsCut) &&
              (NumOfPixHits+NumOfSCTHits > minSiHitsCut) &&
              (1/maxPMSoverPID < id_qoverp/me_qoverp) && (id_qoverp/me_qoverp < maxPMSoverPID) &&
	      //   (fabs((*muonItr)->d0_exPV()) < maxd0Cut) &&
	      //   (fabs((*muonItr)->z0_exPV()) < maxz0diffCut) &&
              (ptcone30/(*muonItr)->pt() < maxIsoptCut) &&
	      vertexCut &&
	      !isBad
	      ) {

             MuZ_N++;

             Float_t px = (*muonItr)->p4().Px();
	     Float_t py = (*muonItr)->p4().Py();
             Float_t pz = (*muonItr)->p4().Pz();
             Float_t e = (*muonItr)->p4().E();
             Int_t charge = 0;//(Int_t)((*muonItr)->charge()); // TODO update when xAODMuon-00-06-00 

             CLHEP::HepLorentzVector thislepton;
             thislepton.setPx(px);
             thislepton.setPy(py);
             thislepton.setPz(pz);
             thislepton.setE(e);
             goodmuonsZ.push_back(thislepton);
             goodmuonZcharge.push_back(charge);
         }
         if ( ((*muonItr)->pt() > minptCutJPsi) ) {
             MuJPsi_N++;

             Float_t px = (*muonItr)->p4().Px();
             Float_t py = (*muonItr)->p4().Py();
             Float_t pz = (*muonItr)->p4().Pz();
             Float_t e = (*muonItr)->p4().E();
             Int_t charge = 0;//(Int_t)((*muonItr)->charge()); // TODO update when xAODMuon-00-06-00

             CLHEP::HepLorentzVector thislepton;
             thislepton.setPx(px);
             thislepton.setPy(py);
             thislepton.setPz(pz);
             thislepton.setE(e);
             goodmuonsJPsi.push_back(thislepton);
             goodmuonJPsicharge.push_back(charge);

         }


     }


     // Check Sum of Candidate Leptons, Return if None


     log << MSG::DEBUG << "Candidate e+mu = " << El_N+MuZ_N+MuJPsi_N << endreq; 
     if ((El_N + MuZ_N + MuJPsi_N) < 1) return sc;

     // Sort Candidates by Pt


     CLHEP::HepLorentzVector leadingEle;
     CLHEP::HepLorentzVector subleadingEle;
     CLHEP::HepLorentzVector leadingMuZ;
     CLHEP::HepLorentzVector subleadingMuZ;
     CLHEP::HepLorentzVector leadingMuJPsi;
     CLHEP::HepLorentzVector subleadingMuJPsi;
     
     leadingEle.setPerp(0);
     subleadingEle.setPerp(0);
     leadingMuZ.setPerp(0);
     subleadingMuZ.setPerp(0);
     leadingMuJPsi.setPerp(0);
     subleadingMuJPsi.setPerp(0);
     
     Int_t leadingElecharge = 0;
     Int_t subleadingElecharge = 0;
     Int_t leadingMuZcharge = 0;
     Int_t subleadingMuZcharge = 0;
     Int_t leadingMuJPsicharge = 0;
     Int_t subleadingMuJPsicharge = 0;


     for (UInt_t iEle = 0; iEle < goodelectrons.size(); iEle++) {
        Float_t pt = goodelectrons[iEle].perp();
        if (pt > leadingEle.perp()) {
           subleadingEle = leadingEle;
	   subleadingElecharge = leadingElecharge;
           leadingEle = goodelectrons[iEle];
	   leadingElecharge = goodelectroncharge[iEle];
        }
        else if (pt > subleadingEle.perp()) {
           subleadingEle = goodelectrons[iEle];
	   subleadingElecharge = goodelectroncharge[iEle];
        }
     }
     for (UInt_t iMu = 0; iMu < goodmuonsZ.size(); iMu++) {
        Float_t pt = goodmuonsZ[iMu].perp();
        if (pt > leadingMuZ.perp()) {
           subleadingMuZ = leadingMuZ;
           subleadingMuZcharge = leadingMuZcharge;
           leadingMuZ = goodmuonsZ[iMu];
           leadingMuZcharge = goodmuonZcharge[iMu];
        }
        else if (pt > subleadingMuZ.perp()) {
           subleadingMuZ = goodmuonsZ[iMu];
           subleadingMuZcharge = goodmuonZcharge[iMu];
        }
     }
     for (UInt_t iMu = 0; iMu < goodmuonsJPsi.size(); iMu++) {
        Float_t pt = goodmuonsJPsi[iMu].perp();
        if (pt > leadingMuJPsi.perp()) {
           subleadingMuJPsi = leadingMuJPsi;
           subleadingMuJPsicharge = leadingMuJPsicharge;
           leadingMuJPsi = goodmuonsJPsi[iMu];
           leadingMuJPsicharge = goodmuonJPsicharge[iMu];
        }
        else if (pt > subleadingMuJPsi.perp()) {
           subleadingMuJPsi = goodmuonsJPsi[iMu];
           subleadingMuJPsicharge = goodmuonJPsicharge[iMu];
        }
     }
     
     
     //MET histograms
     m_metHist->Fill(metMet);
     m_metPhiHist->Fill(phiMet);

     // Z Mass

     bool isZee = (goodelectrons.size() > 1);
     bool isZmumu = (goodmuonsZ.size() > 1);
     bool isJPsimumu = (goodmuonsJPsi.size() > 1);

     if (isZee) {
        CLHEP::HepLorentzVector Zee = leadingEle + subleadingEle;
	Float_t mass = Zee.m();
	Int_t Zeecharge = leadingElecharge + subleadingElecharge;
     
	m_Z_Q_ele->Fill(Zeecharge);
	if (Zeecharge == 0) {
	  m_Z_mass_opsele->Fill(mass);
	  m_graph_Zee_ops->SetPoint(m_graph_Zee_ops->GetN(), lumiBlock, eventNumber);
	  if (mass > 86990 && mass < 95386 ) { //assuming Z mass of 91188 MeV and range of 2 sigma (sigma = 4 198 MeV)
	    ++ZBosonCounterSBG_El[0];
	    if(m_doTrigger) {
	      if( m_trigDec->isPassed("EF_2e12Tvh_loose1") ) {
		++ZBosonCounterSBG_El[1];
	      }
	    }
	  }
	}
	else {
	  m_Z_mass_ssele->Fill(mass);
	  m_graph_Zee_ss->SetPoint(m_graph_Zee_ss->GetN(), lumiBlock, eventNumber);
	}
	log << MSG::DEBUG << "Found a Z to ee candidate!  Mass = " << mass << ", and charge = " << Zeecharge << endreq;
     }

     if (isZmumu) {
        CLHEP::HepLorentzVector Zmumu = leadingMuZ + subleadingMuZ;
        Float_t mass = Zmumu.m();
        Int_t Zmumucharge = leadingMuZcharge + subleadingMuZcharge;

	m_Z_Q_mu->Fill(Zmumucharge);
	if (Zmumucharge == 0) {
	  m_Z_mass_opsmu->Fill(mass);
	  m_graph_Zmumu_ops->SetPoint(m_graph_Zmumu_ops->GetN(), lumiBlock, eventNumber);
          if (mass > 87831 && mass < 94545) {// assuming Z mass of 91188 MeV and range of 2 sigma (sigma = 3357 MeV)
              ++ZBosonCounterSBG_Mu[0];
	      if(m_doTrigger) {
		if( m_trigDec->isPassed("EF_mu24i_tight") ) {
		  ++ZBosonCounterSBG_Mu[1];
		}
	      }
	  }
	}
	else {
	  m_Z_mass_ssmu->Fill(mass);
	  m_graph_Zmumu_ss->SetPoint(m_graph_Zmumu_ss->GetN(), lumiBlock, eventNumber);
	}
	log << MSG::DEBUG << "Found a Z to mumu candidate!  Mass = " << mass << ", and charge = " << Zmumucharge << endreq;
     }     


     //JPsi and Upsilon counter
     if (isJPsimumu) {
         CLHEP::HepLorentzVector JPsimumu = leadingMuJPsi + subleadingMuJPsi;
         Float_t mass = JPsimumu.m();
         Int_t JPsimumucharge = leadingMuJPsicharge + subleadingMuJPsicharge;

        
         if (JPsimumucharge == 0) {
             m_JPsi_mass_opsmu->Fill(mass);
             m_Upsilon_mass_opsmu->Fill(mass);
             //m_graph_Zmumu_ops->SetPoint(m_graph_Zmumu_ops->GetN(), lumiBlock, eventNumber);

		 

             if (mass > 3038 && mass < 3156) {
                 ++JPsiCounterSBG[0];
		 if(m_doTrigger) {
		   if( m_trigDec->isPassed("EF_2mu6_Jpsimumu") ) {
		     ++JPsiCounterSBG[1];
		   }
		 }
             }
             if ((mass > 2802 && mass < 2920) || (mass > 3274 && mass < 3391)) {
                 ++JPsiCounter2BG[0];
		 if(m_doTrigger) {
		   if( m_trigDec->isPassed("EF_2mu6_Jpsimumu") ) {
		     ++JPsiCounter2BG[1];
		   }
		 }
             }
             //assuming JPsi mass of 3 097 MeV and range of 2 sigma (sigma = 59 MeV, based on root best fit of distribution)
	     if (mass > 8242 && mass < 10678) {
                 ++UpsilonCounterSBG[0];
		 if(m_doTrigger) {
		   if( m_trigDec->isPassed("EF_2mu6_Upsimumu") ) {
		     ++UpsilonCounterSBG[1];
		   }
		 }
             }
             if ((mass > 4588 && mass < 5806) || (mass > 13114 && mass < 14332)) {
                 ++UpsilonCounterBG[0];
		 if(m_doTrigger) {
		   if( m_trigDec->isPassed("EF_2mu6_Upsimumu") ) {
		     ++UpsilonCounterBG[1];
		   }
		 }
             }
             //assuming Upsilon mass of 9 460 MeV and range of 2 sigma (sigma = 1218 MeV)


         }
         //else {
         //    m_Z_mass_ssmu->Fill(mass);
         //    m_graph_Zmumu_ss->SetPoint(m_graph_Zmumu_ss->GetN(), lumiBlock, eventNumber);
         //}
         log << MSG::DEBUG << "Found a JPsi/Upsilon to mumu candidate!  Mass = " << mass << ", and charge = " << JPsimumucharge << endreq;
     }  








     // W Transverse Mass

     Float_t metx = metMet*cos(phiMet);
     Float_t mety = metMet*sin(phiMet);
     Float_t m_mtCut = 40000;

     bool isWenu =  ((goodelectrons.size()  > 0) && (metMet > m_metCut) && !isZee);
     bool isWmunu = ((goodmuonsZ.size() > 0) && (metMet > m_metCut) && !isZmumu);


     if (isWenu) {
        Float_t px = leadingEle.px();
        Float_t py = leadingEle.py();
        Float_t pz = leadingEle.pz();
        Float_t p = sqrt(px*px+py*py+pz*pz);
        Float_t et = leadingEle.e()*leadingEle.perp()/p;
        Float_t mt = sqrt(2*(et*metMet-px*metx-py*mety));     

	if (mt > m_mtCut)
	{
	  m_W_pt_v_met_ele->Fill(leadingEle.perp(), metMet);
	  m_W_mt_ele->Fill(mt);
	  m_graph_Wenu->SetPoint(m_graph_Wenu->GetN(), lumiBlock, eventNumber);
	  log << MSG::DEBUG << "Found a W to enu candidate!  M_T = " << mt << ", and MET = " << metMet << ", and mu_pt = " << leadingMuZ.perp() << endreq;
	}
     }

     if (isWmunu) {
        Float_t px = leadingMuZ.px();
        Float_t py = leadingMuZ.py();
        Float_t pz = leadingMuZ.pz();
        Float_t p = sqrt(px*px+py*py+pz*pz);
        Float_t et = leadingMuZ.e()*leadingMuZ.perp()/p;
        Float_t mt = sqrt(2*(et*metMet-px*metx-py*mety));

	if (mt > m_mtCut)
	{
	  m_W_pt_v_met_mu->Fill(leadingMuZ.perp(), metMet);
	  m_W_mt_mu->Fill(mt);
	  m_graph_Wmunu->SetPoint(m_graph_Wmunu->GetN(), lumiBlock, eventNumber);
	  log << MSG::DEBUG << "Found a W to munu candidate!  M_T = " << mt << ", and MET = " << metMet << ", and mu_pt = " << leadingMuZ.perp() << endreq;
	}
     }

     return sc;
 
  } //end ifBeam  
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::procHistograms( )
//StatusCode DQTGlobalWZFinderTool::procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
//----------------------------------------------------------------------------------
{
  //if ( endOfEventsBlock || endOfLumiBlock || endOfRun ) {
  if ( endOfLumiBlock || endOfRun ) {
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "in finalHists()" << endreq;
  }


   //---Filling rate monitoring histograms---//
   if (endOfLumiBlock) {
         JPsiCounterSignal[0] = JPsiCounterSBG[0] - JPsiCounter2BG[0]/2;
         JPsiCounterSignal[1] = JPsiCounterSBG[1] - JPsiCounter2BG[1]/2;

         UpsilonCounterSignal[0] = UpsilonCounterSBG[0] - UpsilonCounterBG[0];
         UpsilonCounterSignal[1] = UpsilonCounterSBG[1] - UpsilonCounterBG[1];

         KshortCounterSignal[0] = KshortCounterSBG[0] - KshortCounterBG[0];
         KshortCounterSignal[1] = KshortCounterSBG[1] - KshortCounterBG[1];

         if (m_doRunBeam) {
             //Map lumiBlock to corresponding bin
             int binNumber = int((lumiBlock-0.50)/((maxLumiBlock - minLumiBlock)/numBins))+1;

	     //JPsi->mu mu
	     if(JPsiCounterSignal[0] < 0) {
	       JPsiCounterSignal[0] = 0;
	     }
	     if(JPsiCounterSignal[1] < 0) {
	       JPsiCounterSignal[1] = 0;
	     }

	     for(int i = 0 ; i < JPsiCounterSignal[0] ; i++) {
	       m_JPsiCounter_Mu->Fill(lumiBlock); 
	     }
	     m_JPsiCounter_Mu->SetBinError(binNumber, TMath::Sqrt(m_JPsiCounter_Mu->GetBinContent( binNumber)));

	     if(m_doTrigger) {
	       for(int i = 0 ; i < JPsiCounterSignal[1] ; i++) {
		 m_JPsiCounter_Mu_triggerAware->Fill(lumiBlock); 
	       }
	       m_JPsiCounter_Mu_triggerAware->SetBinError(binNumber, TMath::Sqrt(m_JPsiCounter_Mu_triggerAware->GetBinContent( binNumber)));
	     }

	     //Upsilon->mu mu
             if(UpsilonCounterSignal[0] < 0) {
	       UpsilonCounterSignal[0]= 0;
             }
             if(UpsilonCounterSignal[1] < 0) {
	       UpsilonCounterSignal[1]= 0;
             }

	     for(int i = 0 ; i < UpsilonCounterSignal[0] ; i++) {
	       m_UpsilonCounter_Mu->Fill(lumiBlock);
	     }
	     m_UpsilonCounter_Mu->SetBinError(binNumber, TMath::Sqrt(m_UpsilonCounter_Mu->GetBinContent( binNumber)));

	     if(m_doTrigger) {
	       for(int i = 0 ; i < UpsilonCounterSignal[1] ; i++) {
		 m_UpsilonCounter_Mu_triggerAware->Fill(lumiBlock);
	       }
	       m_UpsilonCounter_Mu_triggerAware->SetBinError(binNumber, TMath::Sqrt(m_UpsilonCounter_Mu_triggerAware->GetBinContent( binNumber)));
	     }

             //Kshort ->pi pi
	     if(KshortCounterSignal[0] < 0) {
	       KshortCounterSignal[0]=0;
	     }
	     if(KshortCounterSignal[1] < 0) {
	       KshortCounterSignal[1]=0;
	     }

	     for(int i = 0 ; i < KshortCounterSignal[0] ; i++) {
	       m_KshortCounter_Pi->Fill(lumiBlock);
	     }
	     m_KshortCounter_Pi->SetBinError(binNumber, TMath::Sqrt(m_KshortCounter_Pi->GetBinContent( binNumber)));

	     if ( m_doTrigger) {
	       for(int i = 0 ; i < KshortCounterSignal[1] ; i++) {
		 m_KshortCounter_Pi_triggerAware->Fill(lumiBlock);
	       }
	       m_KshortCounter_Pi_triggerAware->SetBinError(binNumber, TMath::Sqrt(m_KshortCounter_Pi_triggerAware->GetBinContent( binNumber)));
	     }

             //Z->e e
	     if(ZBosonCounterSBG_El[0] < 0) {
	       ZBosonCounterSBG_El[0] = 0;
	     }
	     if(ZBosonCounterSBG_El[1] < 0) {
	       ZBosonCounterSBG_El[1] = 0;
	     }

	     for(int i = 0 ; i < ZBosonCounterSBG_El[0] ; i++) {
	       m_ZBosonCounter_El->Fill(lumiBlock);
	     }
	     m_ZBosonCounter_El->SetBinError(binNumber, TMath::Sqrt(m_ZBosonCounter_El->GetBinContent( binNumber)));

	     if(m_doTrigger) {
	       for(int i = 0 ; i < ZBosonCounterSBG_El[1] ; i++) {
		 m_ZBosonCounter_El_triggerAware->Fill(lumiBlock);
	       }
	       m_ZBosonCounter_El_triggerAware->SetBinError(binNumber, TMath::Sqrt(m_ZBosonCounter_El_triggerAware->GetBinContent( binNumber)));
	     }

             //Z->mu mu
	     if(ZBosonCounterSBG_Mu[0] < 0) {
	       ZBosonCounterSBG_Mu[0] = 0;
	     }
	     if(ZBosonCounterSBG_Mu[1] < 0) {
	       ZBosonCounterSBG_Mu[1] = 0;
	     }

	     for(int i = 0 ; i < ZBosonCounterSBG_Mu[0] ; i++) {
	       m_ZBosonCounter_Mu->Fill(lumiBlock);
	     }
	     m_ZBosonCounter_Mu->SetBinError(binNumber, TMath::Sqrt(m_ZBosonCounter_Mu->GetBinContent( binNumber)));

	     if(m_doTrigger) {
	       for(int i = 0 ; i < ZBosonCounterSBG_Mu[1] ; i++) {
		 m_ZBosonCounter_Mu_triggerAware->Fill(lumiBlock);
	       }
	       m_ZBosonCounter_Mu_triggerAware->SetBinError(binNumber, TMath::Sqrt(m_ZBosonCounter_Mu_triggerAware->GetBinContent( binNumber)));
	     }
         }
	 for (int index =0 ; index < 2 ; index++) {
	   JPsiCounterSBG[index]=0;
	   JPsiCounterSBG[index]=0;
	   JPsiCounter2BG[index]=0;
	   JPsiCounterSignal[index]=0;
	   
	   UpsilonCounterSBG[index]=0;
	   UpsilonCounterBG[index]=0;
	   UpsilonCounterSignal[index]=0;
	   
	   KshortCounterSBG[index]=0;
	   KshortCounterBG[index]=0;
	   KshortCounterSignal[index]=0;
	   
	   ZBosonCounterSBG_El[index]=0;
	   ZBosonCounterSBG_Mu[index]=0;
	 }
   }





  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "in checkHists()" << endreq;

  return StatusCode::SUCCESS;
}

