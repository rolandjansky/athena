/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// make this using
//   make SUSYPhysValMonitoring

/***********************************************************************

SUSY reconstruction: make Meff for 0,1 leptons or taus and dilepton
distribution.

***********************************************************************/

#include <TMatrixDSymEigen.h>

// headers for local tools
#include <SUSYPhysValMonitoring/rel17_SUSYSusyRec.h>

// using namespace TruthHelper;
using namespace Analysis;
using namespace Rec;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

SUSYSusyRec::SUSYSusyRec(const std::string& type, const std::string& name, const IInterface* parent )
  :  SUSYPlotTool(type, name, parent), m_analysisTools( "AnalysisTools" ) { //, m_trackToVertexTool("Reco::TrackToVertex") {

  // declare the interface for this tool
  declareInterface< ISUSYPlotTool > (this);

  /// switches to control the analysis through job options
  declareProperty("AnalysisTools", m_analysisTools );
//   declareProperty("ReleaseNo", m_isRelease15 = false);

  declareProperty("ElectronName", m_electronName = "ElectronAODCollection");
  declareProperty("MuonName", m_muonName = "StacoMuonCollection");
  declareProperty("TauName", m_tauName = "TauRecContainer");
  declareProperty("JetName", m_jetName = "AntiKt4LCTopoJets");
  declareProperty("MetName", m_metName = "MET_Final");
  declareProperty("TruthName", m_truthName = "SpclMC");

//   declareProperty("SUSYJetContainer", m_SUSYjetName = "AntiKt4LCTopoJets");
//   declareProperty("SUSYElectronContainer", m_SUSYelectronName = "SUSYElectronAODCollection");
  declareProperty("SUSYMissingET", m_SUSYmetName = "MET_RefFinal_Simplified20");
  declareProperty("SUSYMissingETMuons", m_SUSYmetmuonName = "MET_Muon_Total_Staco_Simplified20");
//   declareProperty("SUSYMissingETJets",  m_SUSYmetjetName = "MET_RefJet_Simplified20");

  declareProperty("PtLeptonCut", m_ptlCut = 20*GeV);
  declareProperty("PtLeptonPreCut", m_ptl2Cut = 20*GeV);
  declareProperty("EtIsolCut", m_etisolCut = 10*GeV);
  declareProperty("EtMissCut", m_metCut = 125*GeV);
  declareProperty("etaJetCut", m_etajCut = 3.0);
  declareProperty("PtJetCut", m_ptjCut = 20*GeV);
  declareProperty("PtJet0Cut", m_ptj0Cut = 60*GeV);
  declareProperty("PtJet1Cut", m_ptj1Cut = 30*GeV);
  declareProperty("MeffCut", m_MeffCut = 500*GeV);
  declareProperty("MTCut", m_mTCut = 100*GeV);
  declareProperty("STCut", m_STCut = 0.2);

  declareProperty( "HistBaseDirectory", m_c_histBaseDirectory = "/PhysVal2/SUSY/ETmiss/");
  declareProperty( "METContainerName",  m_c_metContainerName  = "MET_RefFinal" );
  declareProperty( "EventInfoName",     m_c_eventInfoName     = "EventInfo" );
  declareProperty( "PVContainerName",   m_c_pvContainerName   = "PrimaryVertices" );


}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation

SUSYSusyRec::~SUSYSusyRec() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// book histograms

StatusCode SUSYSusyRec::initialize() {
  /// Call base initialize (essential)
  StatusCode sc = SUSYPlotTool::initialize();
  if (sc.isFailure() )
    msg(MSG::ERROR) << "Base class initialization failed. " <<endreq;

  /// get a handle on the analysis tools
  sc = m_analysisTools.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Can't get handle on analysis tools" << endreq;
    return sc;
  }

  // has to be called for muon track variables
  sc = service("StoreGateSvc", m_storeGate);
  if(sc.isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve StoreGate";
    return StatusCode::FAILURE;
  }

//   sc = m_trackToVertexTool.retrieve();
//   if(sc.isFailure()) {
//     REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve TrackToVertexTool";
//     return StatusCode::FAILURE;
//   }

  //------------------------

  msg(MSG::INFO) <<"PtLeptonCut    " <<m_ptlCut      <<endreq;
  msg(MSG::INFO) <<"PtLeptonpreCut " <<m_ptl2Cut      <<endreq;
  msg(MSG::INFO) <<"EtIsolCut      " <<m_etisolCut   <<endreq;
  msg(MSG::INFO) <<"EtMissCut      " <<m_metCut      <<endreq;
  msg(MSG::INFO) <<"etaJetCut      " <<m_etajCut      <<endreq;
  msg(MSG::INFO) <<"PtJetCut       " <<m_ptjCut      <<endreq;
  msg(MSG::INFO) <<"PtJet0Cut      " <<m_ptj0Cut     <<endreq;
  msg(MSG::INFO) <<"PtJet1Cut      " <<m_ptj1Cut     <<endreq;
  msg(MSG::INFO) <<"MeffCut        " <<m_MeffCut     <<endreq;
  msg(MSG::INFO) <<"MTCut          " <<m_mTCut       <<endreq;
  msg(MSG::INFO) <<"STCut          " <<m_STCut       <<endreq;

  //Containers
//   PreElectrons = new xAOD::ElectronContainer(SG::OWN_ELEMENTS); // TODO: VIEW_ELEMENTS?
//   PreSignalElectrons = new xAOD::ElectronContainer(SG::OWN_ELEMENTS);
//   goodElectrons = new xAOD::ElectronContainer(SG::OWN_ELEMENTS);
//   goodSignalElectrons = new xAOD::ElectronContainer(SG::OWN_ELEMENTS);
//   PreMuons_forMET = new xAOD::MuonContainer(SG::OWN_ELEMENTS);
//   PreMuons = new xAOD::MuonContainer(SG::OWN_ELEMENTS);
//   PreSignalMuons = new xAOD::MuonContainer(SG::OWN_ELEMENTS);
//   goodMuons = new xAOD::MuonContainer(SG::OWN_ELEMENTS);
//   goodSignalMuons = new xAOD::MuonContainer(SG::OWN_ELEMENTS);
//   goodTaus = new xAOD::TauJetContainer(SG::OWN_ELEMENTS);
//   goodJets = new xAOD::JetContainer(SG::OWN_ELEMENTS);
//   goodJets.setAuthor("JetCollectionFilterView");
//   const JetMomentMap * map= goodJets.getMomentMap();
//   JetMomentMapManager::s_instance.remove(map);
//   goodJetsC = new xAOD::JetContainer;


  //use default settings for electron energy smearing
  m_eRescale.useDefaultCalibConstants();

  //initialize smearing
  m_mcp_smear.UseScale(1);

  //initialise OTX check class
  //m_checkOQ.initialize();


  /// histograms
//   std::string dir = m_foldername;
  std::string dir = m_c_histBaseDirectory + "rel17/";

  msg(MSG::INFO) << "Initializing " << name() << " and booking histograms in " << dir << endreq;

  m_h_vx_n = new TH1F("primary_vx_n","Number of primary vertices per event(nTracks>4); Nvertices",51,-0.5,50.5 );
  sc = sc & m_thistSvc->regHist(dir + "primary_vx_n", m_h_vx_n);

  m_h_averageIntPerXing_n = new TH1F("averageIntPerXing_n","Number of averageIntPerXing per event(nTracks>4); N_averageIntPerXing",51,-0.5,50.5 );
  sc = sc & m_thistSvc->regHist(dir + "averageIntPerXing_n", m_h_averageIntPerXing_n);

  m_h_actualIntPerXing_n = new TH1F("actualIntPerXing_n","Number of actualIntPerXing per event(nTracks>4); N_actualIntPerXing",51,-0.5,50.5 );
  sc = sc & m_thistSvc->regHist(dir + "actualIntPerXing_n", m_h_actualIntPerXing_n);

  m_h_averageIntPerXing_vs_vx = new TH2F("averageIntPerXing_vs_vx","averageIntPerXing vs primary verties(nTracks>4); Nvertices; N_averageIntPerXing",51,-0.5,50.5, 51,-0.5,50.5);
  sc = sc & m_thistSvc->regHist(dir + "averageIntPerXing_vs_vx", m_h_averageIntPerXing_vs_vx);

  m_h_pre_Jet_n = new TH1F("pre_Jet_n","N preselected jets by JetEvent/JetCollection.h; Njets",16,-0.5,15.5);
  sc = sc & m_thistSvc->regHist(dir + "pre_Jet_n", m_h_pre_Jet_n);

  m_h_pre_Jet50_n = new TH1F("pre_Jet50_n","N preselected jets w/ p_{T} > 50 GeV; Njets",16,-0.5,15.5);
  sc = sc & m_thistSvc->regHist(dir + "pre_Jet50_n", m_h_pre_Jet50_n);

  m_h_pre_pt = new TH1F("pre_pt","preselected jet p_{T} / 10 GeV ;p_{T} [GeV]",100,0,1000.);
  sc = sc & m_thistSvc->regHist(dir + "pre_pt", m_h_pre_pt);

  m_h_pre_ptlo = new TH1F("pre_ptlo","preselected jet p_{T} / 1 GeV;p_{T} [GeV]",100,0,100.);
  sc = sc & m_thistSvc->regHist(dir + "pre_ptlo", m_h_pre_ptlo);

  m_h_pre_pt0 = new TH1F("pre_pt0","Pres. leading jet p_{T} / 10 GeV;p_{T} [GeV]",100,0,1000.);
  sc = sc & m_thistSvc->regHist(dir + "pre_pt0", m_h_pre_pt0);

  m_h_pre_pt1 = new TH1F("pre_pt1","Pres. 2^{nd} jet p_{T} / 10 GeV;p_{T} [GeV]",50,0,500.);
  sc = sc & m_thistSvc->regHist(dir + "pre_pt1", m_h_pre_pt1);

  m_h_pre_pt2 = new TH1F("pre_pt2","Pres. 3^{rd} jet p_{T} / 10 GeV;p_{T} [GeV]",50,0,500.);
  sc = sc & m_thistSvc->regHist(dir + "pre_pt2", m_h_pre_pt2);

  m_h_pre_pt3 = new TH1F("pre_pt3","Pres. 4^{th} jet p_{T} / 10 GeV;p_{T} [GeV]",50,0,500.);
  sc = sc & m_thistSvc->regHist(dir + "pre_pt3", m_h_pre_pt3);

  m_h_pre_ptHardLep = new TH1F("pre_ptHardLep","Hardest Lepton p_{T} / 5 GeV;p_{T} [GeV]",100,0.00001,500.);
  sc = sc & m_thistSvc->regHist(dir + "pre_ptHardLep", m_h_pre_ptHardLep);

  m_h_pre_etmiss = new TH1F("pre_etmiss","MET / 1 GeV; MET [GeV]",75,0,750.);
  sc = sc & m_thistSvc->regHist(dir + "pre_etmiss", m_h_pre_etmiss);

  m_h_pre_METangle0 = new TH1F("pre_METangle0","angle(Leading jet, MET) / 0.1;MET angle",35,0,3.5);
  sc = m_thistSvc->regHist(dir + "pre_METangle0", m_h_pre_METangle0);

  m_h_pre_METangle1 = new TH1F("pre_METangle1","angle(2^{nd} jet, MET) / 0.1;MET angle",35,0,3.5);
  sc = m_thistSvc->regHist(dir + "pre_METangle1", m_h_pre_METangle1);

  m_h_pre_METangle2 = new TH1F("pre_METangle2","angle(3^{nd} jet, MET) / 0.1;MET angle",35,0,3.5);
  sc = m_thistSvc->regHist(dir + "pre_METangle2", m_h_pre_METangle2);

  m_h_pre_METangle3 = new TH1F("pre_METangle3","angle(4^{th} jet, MET) / 0.1;MET angle",35,0,3.5);
  sc = m_thistSvc->regHist(dir + "pre_METangle3", m_h_pre_METangle3);

  m_h_pre_METangleLept = new TH1F("pre_METangleLept","angle (hardest lepton, MET) / 0.1;MET angle",35,0,3.5);
  sc = m_thistSvc->regHist(dir + "pre_METangleLept", m_h_pre_METangleLept);

  m_h_pre_mT = new TH1F("pre_mT","Lepton Transverse Mass / 5 GeV;m_{T} [GeV]",100,0,500.);
  sc = sc & m_thistSvc->regHist(dir + "pre_mT", m_h_pre_mT);

  m_h_pre_ST = new TH1F("pre_ST","Transverse Spericity / 0.02; S_{T}",50,0.0001,1.);
  sc = sc & m_thistSvc->regHist(dir + "pre_ST", m_h_pre_ST);

  m_h_pre_Meff = new TH1F("pre_Meff","Event Effective Mass / 50 GeV;M_{eff} [GeV]",400,0,2000.);
  sc = sc & m_thistSvc->regHist(dir + "pre_Meff", m_h_pre_Meff);


  m_h_pre_Ne = new TH1F("pre_Ne","N e SUSYElectronAODCollection; Nparticles",16,-0.5,15.5);
  sc = sc & m_thistSvc->regHist(dir + "pre_Ne", m_h_pre_Ne);

  m_h_pre_Nm = new TH1F("pre_Nm","N #mu muonEvent/MuonContainer.h; Nparticles",16,-0.5,15.5);
  sc = sc & m_thistSvc->regHist(dir + "pre_Nm", m_h_pre_Nm);

  m_h_pre_Nt = new TH1F("pre_Nt","N #tau tauEvent/TauJetContainer.h; Nparticles",16,-0.5,15.5);
  sc = sc & m_thistSvc->regHist(dir + "pre_Nt", m_h_pre_Nt);

  m_h_pre_pte = new TH1F("pre_pte","p_{T} e after OR / 2 GeV;p_{T} [GeV]",100,0,200.);
  sc = sc & m_thistSvc->regHist(dir + "pre_pte", m_h_pre_pte);

  m_h_pre_ptm = new TH1F("pre_ptm","p_{T} #mu after OR / 2 GeV;p_{T} [GeV]",100,0,200.);
  sc = sc & m_thistSvc->regHist(dir + "pre_ptm", m_h_pre_ptm);

  m_h_pre_ptt = new TH1F("pre_ptt","p_{T} #tau > 20 GeV, after lepton veto / 2 GeV;p_{T} [GeV]",100,0,200.);
  sc = sc & m_thistSvc->regHist(dir + "pre_ptt", m_h_pre_ptt);

  m_h_pre_etae = new TH1F("pre_etae","#eta e after OR / 0.1;#eta",30,0,3.);
  sc = sc & m_thistSvc->regHist(dir + "pre_etae", m_h_pre_etae);

  m_h_pre_etam = new TH1F("pre_etam","#eta #mu after OR / 0.1;#eta",30,0,3.);
  sc = sc & m_thistSvc->regHist(dir + "pre_etam", m_h_pre_etam);

  m_h_pre_etat = new TH1F("pre_etat","#eta #tau > 20 GeV, after lepton veto / 0.1;#eta",30,0,3.);
  sc = sc & m_thistSvc->regHist(dir + "pre_etat", m_h_pre_etat);

  // new histograms
  h_select_el = new TH1F("h_select_el","Electron Object Selection;selection cut;# electrons passing cut",15,-0.5,14.5);
  sc = sc & m_thistSvc->regHist(dir + "h_select_el", h_select_el);
  h_select_mu = new TH1F("h_select_mu","Muon Object Selection;selection cut;# muons passing cut",15,-0.5,14.5);
  sc = sc & m_thistSvc->regHist(dir + "h_select_mu", h_select_mu);
  // ---

  m_h_Trigger_Flow = new TH1F("Trigger_Flow","Trigger Cut Flow; All,L1,L2,EF,Offline",5,0,5.);
  sc = sc & m_thistSvc->regHist(dir + "Trigger_Flow", m_h_Trigger_Flow);

  m_h_susy_0lep_CutFlow = new TH1F("susy_0lep_CutFlow","0Lepton Cut Flow",12,0,12.);
  sc = sc & m_thistSvc->regHist(dir + "susy_0lep_CutFlow", m_h_susy_0lep_CutFlow);

  m_h_susy_1muon_CutFlow = new TH1F("susy_1muon_CutFlow","1Muon Cut Flow",12,0,12.);
  sc = sc & m_thistSvc->regHist(dir + "susy_1muon_CutFlow", m_h_susy_1muon_CutFlow);

  m_h_susy_1el_CutFlow = new TH1F("susy_1el_CutFlow","1Electron Cut Flow",12,0,12.);
  sc = sc & m_thistSvc->regHist(dir + "susy_1el_CutFlow", m_h_susy_1el_CutFlow);

  m_h_susy_2lep_CutFlow = new TH1F("susy_2lep_CutFlow","2Lepton Cut Flow",12,0,12.);
  sc = sc & m_thistSvc->regHist(dir + "susy_2lep_CutFlow", m_h_susy_2lep_CutFlow);

  m_h_jet_CutFlow = new TH1F("jet_CutFlow","Jet Cut Flow",10,0,10.);
  sc = sc & m_thistSvc->regHist(dir + "jet_CutFlow", m_h_jet_CutFlow);


  m_h_susy_ptHardLep = new TH1F("susy_ptHardLep","Hardest Lepton p_{T}: selectd during 1 lept CutFlow / 5 GeV;p_{T} [GeV]",100,0.00001,500.);
  sc = sc & m_thistSvc->regHist(dir + "susy_ptHardLep", m_h_susy_ptHardLep);

  m_h_susy_pt0 = new TH1F("susy_pt0","Leading jet p_{T}: selectd during 1 lept CutFlow / 10 GeV;p_{T} [GeV]",100,0,1000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_pt0", m_h_susy_pt0);

  m_h_susy_pt1 = new TH1F("susy_pt1","2^{nd} jet p_{T}: selectd during 1 lept CutFlow / 10 GeV;p_{T} [GeV]",100,0,1000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_pt1", m_h_susy_pt1);

  m_h_susy_etmiss = new TH1F("susy_etmiss","MET: selectd during 1 lept CutFlow / 10 GeV; MET [GeV]",75,0,750.);
  sc = sc & m_thistSvc->regHist(dir + "susy_etmiss", m_h_susy_etmiss);

  m_h_susy_mT = new TH1F("susy_mT"," Transverse Mass: 1 lepton channel / 5 GeV; m_{T};",100,0,500.);
  sc = sc & m_thistSvc->regHist(dir + "susy_mT", m_h_susy_mT);

  m_h_susy_ST = new TH1F("susy_ST","Transverse Spericity: selectd during 1 lept CutFlow / 0.02; S_{T}",50,0.0001,1.);
  sc = sc & m_thistSvc->regHist(dir + "susy_ST", m_h_susy_ST);

  m_h_susy_Meff = new TH1F("susy_Meff","M_{eff}: selectd during 1 lept CutFlow / 50 GeV;M_{eff}",400,0,2000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_Meff", m_h_susy_Meff);

  m_h_susy_Jet_n = new TH1F("susy_Jet_n","Number of jets: after 1 lept cuts; Njets",16,-0.5,15.5);
  sc = sc & m_thistSvc->regHist(dir + "susy_Jet_n", m_h_susy_Jet_n);

  m_h_susy_etmiss1lep = new TH1F("susy_etmiss1lep","MET: 1 lepton channel (after all cuts) / 1 GeV; MET [GeV]",75,0,750.);
  sc = sc & m_thistSvc->regHist(dir + "susy_etmiss1lep", m_h_susy_etmiss1lep);

  m_h_susy_etmiss1ell = new TH1F("susy_etmiss1ell","MET: 1 el channel (after all cuts) / 1 GeV; MET [GeV]",75,0,750.);
  sc = sc & m_thistSvc->regHist(dir + "susy_etmiss1ell", m_h_susy_etmiss1ell);

  m_h_susy_etmiss1mu = new TH1F("susy_etmiss1mu","MET: 1 #mu channel (after all cuts) / 1 GeV; MET [GeV]",75,0,750.);
  sc = sc & m_thistSvc->regHist(dir + "susy_etmiss1mu", m_h_susy_etmiss1mu);

  m_h_susy_Meff1lep = new TH1F("susy_Meff1lep","M_{eff}: 1 lepton channel (after all cuts) / 5 GeV;M_{eff}",400,0,2000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_Meff1lep", m_h_susy_Meff1lep);

  m_h_susy_Meff1ell = new TH1F("susy_Meff1ell","M_{eff}: 1 el channel (after all cuts) / 5 GeV;M_{eff};M_{eff}",400,0,2000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_Meff1ell", m_h_susy_Meff1ell);

  m_h_susy_Meff1mu = new TH1F("susy_Meff1mu","M_{eff}: 1 #mu channel (after all cuts) / 5 GeV;M_{eff};M_{eff}",400,0,2000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_Meff1mu", m_h_susy_Meff1mu);


  m_h_susy_mll = new TH1F("susy_mll","invariant mass lead + subl lep / 5 GeV; Mass [GeV]",40,0,200.);
  sc = sc & m_thistSvc->regHist(dir + "susy_mll", m_h_susy_mll);

//   m_h_susy_mee = new TH1F("susy_mee","M el-el; Mass [GeV]",100,0,200.);
//   sc = sc & m_thistSvc->regHist(dir + "susy_mee", m_h_susy_mee);
//
//   m_h_susy_mmm = new TH1F("susy_mmm","M #mu - #mu; Mass [GeV]",100,0,200.);
//   sc = sc & m_thistSvc->regHist(dir + "susy_mmm", m_h_susy_mmm);

//   m_h_susy_mem = new TH1F("susy_mem","M el - #mu; Mass [GeV]",100,0,200.);
//   sc = sc & m_thistSvc->regHist(dir + "susy_mem", m_h_susy_mem);


  m_h_susy_mllq = new TH1F("susy_mllq","Lower invariant lept-lept-Jet mass / 5 GeV; Mass [GeV]",200,0,1000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_mllq", m_h_susy_mllq);

  m_h_susy_tllq = new TH1F("susy_tllq","Max invariant lept-lept-Jet mass / 5 GeV; Mass [GeV]",200,0,1000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_tllq", m_h_susy_tllq);

  m_h_susy_mlq1 = new TH1F("susy_mlq1","Max invariant lept-hardestJet Mass / 5 GeV; Mass [GeV]",200,0,1000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_mlq1", m_h_susy_mlq1);

  m_h_susy_mlq2 = new TH1F("susy_mlq2","Second invariant lept-hardestJet Mass / 5 GeV; Mass [GeV]",200,0,1000.);
  sc = sc & m_thistSvc->regHist(dir + "susy_mlq2", m_h_susy_mlq2);


//   m_h_susy_mtautauos = new TH1F("susy_mtautauos","M #tau - #tau opposite sign / 5 GeV; Mass [GeV]",200,0,200.);
//   sc = sc & m_thistSvc->regHist(dir + "susy_mtautauos", m_h_susy_mtautauos);

//   m_h_susy_mtautauss = new TH1F("susy_mtautauss","M #tau - #tau same sign / 5 GeV; Mass [GeV]",200,0,200.);
//   sc = sc & m_thistSvc->regHist(dir + "susy_mtautauss", m_h_susy_mtautauss);


  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Failed to book all histograms" <<endreq;
    return sc;
  }

  msg(MSG::INFO) <<"Initialized all" <<endreq;
  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode SUSYSusyRec::takeAction() {

  msg(MSG::DEBUG) <<"Begin SUSYSusyRec::takeAction" <<endreq;

  //goodJets.setAuthor("JetCollectionFilterView");
  //const JetMomentMap * map= goodJets.getMomentMap();

  //Protect against memory leak
//   PreElectrons.clear();
//   PreSignalElectrons.clear();
//   goodElectrons.clear();
//   goodSignalElectrons.clear();
//   PreMuons_forMET.clear();
//   PreMuons.clear();
//   PreSignalMuons.clear();
//   goodMuons.clear();
//   goodSignalMuons.clear();
//   goodTaus.clear();
//   goodJets.clear();
//   msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction memory leak protection" <<endreq;

  // Use results from base class
  double wt = m_eventWeight;

  // clear vectors
  calibElectrons.clear();
  calibMuons.clear();
  PreElectrons.clear();
  PreSignalElectrons.clear();
  goodElectrons.clear();
  goodSignalElectrons.clear();
  PreMuons_forMET.clear();
  PreMuons.clear();
  PreSignalMuons.clear();
  goodMuons.clear();
  goodSignalMuons.clear();
  goodTaus.clear();
  goodJets.clear();


  ////////////////////////////
  // Get all the stuff we need
  ////////////////////////////

  // Retrieve EventInfo:
//   //from: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/EventInfoExample
//   const EventInfo* eventInfo = 0;
//   sc = m_storeGate->retrieve(eventInfo);
//   if (sc.isFailure() || eventInfo == 0)
//   {
//     msg(MSG::ERROR) << "Unable to retrieve EventInfo. Exiting !" << endreq;
//     return StatusCode::FAILURE;
//   }
  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo >( m_c_eventInfoName ); // can only be read const!
  if (!eventInfo) {
    ATH_MSG_ERROR ("Couldn't retrieve event info with key: " << m_c_eventInfoName);
//       return StatusCode::FAILURE;
  }

//     const xAOD::EventInfoContainer* eventInfoCont = evtStore()->retrieve< const xAOD::EventInfoContainer >( m_c_eventInfoName );
//     if (!eventInfoCont) {
//       ATH_MSG_ERROR ("Couldn't retrieve event info container with key: " << m_c_eventInfoName);
// //       return StatusCode::FAILURE;
//     }

  // Retrieve elec container:
//   const ElectronContainer* evElectrons = 0;
//   if(m_isRelease15)
//   {
//     sc = evtStore()->retrieve(evElectrons, m_SUSYelectronName);
//     if(  sc.isFailure() )
//     {
//       msg(MSG::ERROR) << "Could not retrieve ElectronContainer with name " << m_SUSYelectronName << endreq;
//       return StatusCode::FAILURE;
//     }
//   }
//   else
//   {
//     sc = evtStore()->retrieve(evElectrons, m_electronName);
//     if(  sc.isFailure() )
//     {
//       msg(MSG::ERROR) << "Could not retrieve ElectronContainer with name " << m_electronName << endreq;
//       return StatusCode::FAILURE;
//     }
//   }

  const xAOD::ElectronContainer* evElectrons = evtStore()->retrieve< const xAOD::ElectronContainer >( m_electronName );
  if (!evElectrons) {
    ATH_MSG_ERROR ("Couldn't retrieve electron container with key: " << m_electronName);
    return StatusCode::FAILURE;
  }

  // Retrieve muon container:
//   const Analysis::MuonContainer* evMuons = 0;
//   sc = evtStore()->retrieve(evMuons, m_muonName);
//   if(  sc.isFailure() )
//   {
//     msg(MSG::ERROR) << "Could not retrieve MuonContainer with name " << m_muonName << endreq;
//     return StatusCode::FAILURE;
//   }
  const xAOD::MuonContainer* evMuons = evtStore()->retrieve< const xAOD::MuonContainer >( m_muonName );
  if (!evMuons) {
    ATH_MSG_ERROR ("Couldn't retrieve muon container with key: " << m_muonName);
    return StatusCode::FAILURE;
  }

  // Retrieve tau container:
//   const Analysis::TauJetContainer* eventTauJets = 0;
//   sc = evtStore()->retrieve(eventTauJets,m_tauName);
//   if(  sc.isFailure() || eventTauJets == 0 )
//   {
//     msg(MSG::ERROR)  << "Could not retrieve Analysis::TauJetContainer with name " << m_tauName << endreq;
//     return StatusCode::FAILURE;
//   }
  const xAOD::TauJetContainer* eventTauJets = evtStore()->retrieve< const xAOD::TauJetContainer >( m_tauName );
  if (!eventTauJets) {
    ATH_MSG_ERROR ("Couldn't retrieve tau container with key: " << m_tauName);
    return StatusCode::FAILURE;
  }

  // Retrieve jet container:
//   const JetCollection* evJets = 0;
//   sc = evtStore()->retrieve(evJets, m_SUSYjetName);
//   if(  sc.isFailure() )
//   {
//     msg(MSG::ERROR) << "Could not retrieve JetCollection with name " << m_SUSYjetName << endreq;
//     return StatusCode::FAILURE;
//   }
  const xAOD::JetContainer* evJets = evtStore()->retrieve< const xAOD::JetContainer >( m_jetName );
  if (!evJets) {
    ATH_MSG_ERROR ("Couldn't retrieve jet container with key: " << m_jetName);
    return StatusCode::FAILURE;
  }

  // Retrieve MET:
//   const MissingET* missET;
//   sc = evtStore()->retrieve(missET, m_SUSYmetName);
//   if ( sc.isFailure() )
//   {
//     msg(MSG::ERROR) << "Could not retrieve MissingET object with key " << m_SUSYmetName << endreq;
//     return StatusCode::FAILURE;
//   }
//
//   const MissingET* missET_Muon_Total_Staco;
//   sc = evtStore()->retrieve(missET_Muon_Total_Staco, m_SUSYmetmuonName); // "MET_Muon_Total_Staco_Simplified20"
//   if ( sc.isFailure() ) {
//     msg(MSG::ERROR) << "Could not retrieve MissingET object with key " << m_SUSYmetmuonName << endreq;
//     return StatusCode::FAILURE;
//   }
  const xAOD::MissingETContainer* met_container = evtStore()->retrieve< const xAOD::MissingETContainer >( m_c_metContainerName );
  if (!met_container) {
    ATH_MSG_ERROR ("Couldn't retrieve MET container " << m_c_metContainerName);
    return StatusCode::FAILURE;
  }
  const xAOD::MissingET* missET = (*met_container)[m_SUSYmetName];
  if (!missET) {
    ATH_MSG_ERROR ("Couldn't retrieve MET with key " << m_SUSYmetName << " from container");
    return StatusCode::FAILURE;
  }
  const xAOD::MissingET* missET_Muon_Total_Staco = 0;
  if (!m_SUSYmetmuonName.empty()) {
    missET_Muon_Total_Staco = (*met_container)[m_SUSYmetmuonName];
    if (!missET_Muon_Total_Staco) {
      ATH_MSG_ERROR ("Couldn't retrieve MET with key " << m_SUSYmetmuonName << " from container");
      return StatusCode::FAILURE;
    }
  }

  // Retrieve primary vertices
//   const VxContainer* vxContainer(0);
//   sc = m_storeGate->retrieve(vxContainer, "VxPrimaryCandidate");
//   if (sc.isFailure() || !vxContainer)
//   {
//     REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve primary vertex info: " << "VxPrimaryCandidate" << " will not fill from MuonTrackAtPVFiller";
//     return StatusCode::FAILURE;
//   }
//   if(vxContainer->size()<1)
//   {
//      REPORT_MESSAGE (MSG::WARNING) << "No primary vertices reconstructed";
//      return StatusCode::FAILURE;
//   }
  const xAOD::VertexContainer* vxContainer = evtStore()->retrieve< const xAOD::VertexContainer >( m_c_pvContainerName );
  if (!vxContainer) {
    ATH_MSG_ERROR ("Couldn't retrieve vertex container " << m_c_pvContainerName );
    return StatusCode::FAILURE;
  }



  /* NOT USED? -> removed 07.02.2013
  const MissingET* missET_RefJet;
  sc = evtStore()->retrieve(missET_RefJet, m_SUSYmetjetName); // "MET_RefJet_Simplified20"
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve MissingET object with key " << m_SUSYmetjetName << endreq;
    return StatusCode::FAILURE;
  }
  */

//   // Do not require truthparticles
//   const TruthParticleContainer* truthParticles = 0;
//
//   if( m_doTruth ) {
//     sc = evtStore()->retrieve(truthParticles, m_truthName);
//     if(  sc.isFailure() || truthParticles==0 ) {
//       msg(MSG::DEBUG)  << "Could not retrieve TruthParticleContainer with name " << m_truthName << endreq;
//     return StatusCode::FAILURE;
//     }
//   }





  //////////////////////
  // Basic distributions and variable preparation
  //////////////////////

  //for track variables
  double m_averageIntPerXing =  eventInfo->averageInteractionsPerCrossing();
  double m_actualIntPerXing  =  eventInfo->actualInteractionsPerCrossing();

  //from: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/EventInfoExample
//   unsigned int RunNumber = eventInfo->event_ID()->run_number();
  unsigned int RunNumber = eventInfo->runNumber();
  //unsigned int EventNumber = eventInfo->event_ID()->event_number();

  // Vertex --------------------------------------------------------------------------------------------------
//   std::vector<int>  vx_nTracks;
//   vx_nTracks.clear();

//   //from: https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/D3PDMaker/TrackD3PDMaker/trunk/src/VertexKineFillerTool.cxx
//   for (VxContainer::const_iterator vxItr = vxContainer->begin(); vxItr != vxContainer->end(); vxItr++ )
//   {
//       vx_nTracks.push_back( (*vxItr)->vxTrackAtVertex()->size() );
//   }
//
  bool isGoodPrimaryVertex = false;
//   for (unsigned int ivx = 0; ivx < vx_nTracks.size(); ivx++)
//   {
//     if (vx_nTracks.at(ivx) > 4)
//     {
//       isGoodPrimaryVertex = true; // Jet-MET wg requirement
//       break;
//     }
//   }
  for (const xAOD::Vertex* vtx: *vxContainer) {
    if (vtx->nTrackParticles() > 4) {
      isGoodPrimaryVertex = true;
      break;
    }
  }



  if(isGoodPrimaryVertex) {
//       m_h_vx_n->Fill(vx_nTracks.size(), wt);
    m_h_vx_n->Fill(vxContainer->size(), wt);
    m_h_averageIntPerXing_n->Fill(m_averageIntPerXing, wt);
    m_h_actualIntPerXing_n->Fill(m_actualIntPerXing, wt);
//       m_h_averageIntPerXing_vs_vx->Fill(m_averageIntPerXing, vx_nTracks.size(), wt);
    m_h_averageIntPerXing_vs_vx->Fill(m_averageIntPerXing, vxContainer->size(), wt);
  }

  // Leptons -------------------------------------------------------------------------------------------------

  double ptlep = 0;
  double ptlep2 = 0;
  TLorentzVector HardLep;
  TLorentzVector SecondLep;
  double sumpx2 = 0;
  double sumpy2 = 0;
  double sumpxpy = 0;


  //Electron preselection------------------------------------------------------------------------------------------



  ATH_MSG_DEBUG("SUSYRec: before e pres");

//   for ( const xAOD::Electron* eItr: *evElectrons )
  for (xAOD::ElectronContainer::const_iterator eItr  = evElectrons->begin(); eItr != evElectrons->end(); eItr++ ) {
    
    int el_cut = -1;
    
    calibElectrons.push_back(std::unique_ptr<xAOD::Electron>(new xAOD::Electron)); // need to create copy
    xAOD::Electron *electron = calibElectrons.back().get();
//     electron->makePrivateStore( *eItr );
    electron->makePrivateStore( **eItr );
//     const xAOD::Electron *electron = *eItr; // cannot use const because not changing p4

    float el_pt = electron->pt();
    float el_eta = electron->eta();
    float el_phi = electron->phi();
    //float el_m = electron->m();
    uint8_t el_nPixHits = 255;
    uint8_t el_nSCTHits = 255;
//     unsigned int el_isEM = electron->author(); // TODO: electron->isem();
//     unsigned int el_OQ = electron->isGoodOQ(xAOD::EgammaParameters::ALLOQ);
    int el_author = electron->author();

//     const EMShower* shower = electron->detail<EMShower>("egDetailAOD");
//     float el_ptcone20 = shower ? shower->parameter(egammaParameters::ptcone20) : 0;
    float el_ptcone20;
    electron->isolationValue(el_ptcone20, xAOD::EgammaParameters::ptcone20);

    const xAOD::TrackParticle* etp = electron->trackParticle();
    // get electron track summary //
//     const Trk::TrackSummary* trksumy= etp ? etp->trackSummary() : 0;
//     el_nPixHits = trksumy ? trksumy->get(Trk::numberOfPixelHits) : -999; //variable defined!
//     el_nSCTHits = trksumy ? trksumy->get(Trk::numberOfSCTHits) : -999; //variable defined!
    if (etp) {
      etp->summaryValue(el_nPixHits, xAOD::numberOfPixelHits);
      etp->summaryValue(el_nSCTHits, xAOD::numberOfSCTHits);
    }


    // -------------------------------------------------------------------------------

    if (el_nPixHits+el_nSCTHits < 4) {
      el_eta = electron->caloCluster()->eta();
      el_phi = electron->caloCluster()->phi();
    }
//     else
    else if (etp) {
//       el_eta = electron->trackParticle()->eta();
//       el_phi = electron->trackParticle()->phi();
      el_eta = etp->eta();
      el_phi = etp->phi();
    }

    float el_E = electron->caloCluster()->e();
    el_pt = el_E/cosh(el_eta);

    float el_px = el_pt*cos(el_phi);
    float el_py = el_pt*sin(el_phi);
    float el_pz = el_pt/ tan( 2* atan( exp( -el_eta )));

    if (RunNumber<150000) { // Do this on MC only
      m_eRescale.SetRandomSeed((int)1e+5*fabs(electron->caloCluster()->phi()));

      double smearcorr = m_eRescale.getSmearingCorrection(electron->caloCluster()->eta(), electron->caloCluster()->e()/1000., 0);

      el_E *= smearcorr;
    }

//     HepLorentzVector el_HLV;
//     el_HLV.setPx( el_px );
//     el_HLV.setPy( el_py );
//     el_HLV.setPz( el_pz );
//     el_HLV.setE( el_E );

    //E = Sqrt(x*x+y*y+z*z+m*m)
    //Px = pt*cos(phi)
    //Py = pt*sin(phi)
    //p = pt/sin(theta)
    //Pz = p cos(theta) => pz=pt/tan(theta)
    //eta = -ln(tan(theta/2)) => theta = 2*arctan(exp(-eta))
    //Pz = pt / tan (2*arctan(exp(-eta)))
//     const HepLorentzVector c_el_HLV = el_HLV;

    TLorentzVector el_HLV;
    el_HLV.SetPxPyPzE(el_px, el_py, el_pz, el_E);
    electron->setP4(el_HLV.Pt(), el_HLV.Eta(), el_HLV.Phi(), el_HLV.M()); //  set4Mom(c_el_HLV); unfortunately we cannot just overwrite the FourMom_t because there is no setter


    // cuts
    el_pt = electron->pt(); //get new pt after correction
    el_eta = electron->eta(); //get new eta after correction

    h_select_el->Fill(++el_cut);
    if ( el_pt <= 20.*GeV )
      continue;

    h_select_el->Fill(++el_cut);
    if ( el_eta >= 2.47 )
      continue;

    h_select_el->Fill(++el_cut);
    if ( !(el_author == 1 || el_author == 3) )
      continue;

    h_select_el->Fill(++el_cut);
//     if (el_isEM & egammaPID::ElectronMedium) // ElectronMedium_WithTrackMatch == ElectronMedium in r16.6.X
    bool pass_selection;
    if (!electron->passSelection(pass_selection, "Medium") || !pass_selection)
      continue;
      
    h_select_el->Fill(++el_cut);
//     if ((el_OQ & egammaPID::BADCLUSELECTRON) != 0) // Check object quality flag
    if (electron->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON))
      continue;

// 	if(fabs((*seItr)->cluster()->eta()) <= 1.37 || fabs((*seItr)->cluster()->eta()) >= 1.52)
// 	continue; //Crack veto (obsolete)

    PreElectrons.push_back(electron);

    h_select_el->Fill(++el_cut);
//     if (el_isEM & egammaPID::ElectronTight_WithTrackMatch)
    if (!electron->passSelection(pass_selection, "Tight") || !pass_selection)
      continue;

    h_select_el->Fill(++el_cut);
    if ( el_pt <= 25.*GeV)  // eT cut (might be necessary for leading electron to pass trigger)
      continue;

    h_select_el->Fill(++el_cut);
    if (el_pt > 0. && el_ptcone20/el_pt >= 0.1)
      continue;

    PreSignalElectrons.push_back(electron);


    /*  old preselection

        double pt = (*eItr)->pt();
        //Eta and Isol done by ElectronMediumIso
        if( (*eItr)->isem(egammaPID::ElectronMediumIso) != 0 ) continue;
        if( pt < m_ptl2Cut ) continue;
    */
    //Overlap removal done for GoodElectrons



  }
  msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction memory PreElectrons vector filled" <<endreq;




  //Muon preselection -------------------------------------------------------------------------------------------------

  for ( xAOD::MuonContainer::const_iterator muItr  = evMuons->begin(); muItr != evMuons->end(); muItr++ ) {
    
    int mu_cut = -1;

    // need to create copy
    calibMuons.push_back(std::unique_ptr<xAOD::Muon>(new xAOD::Muon));
    xAOD::Muon *muon = calibMuons.back().get();
    muon->makePrivateStore( **muItr );

    float mu_staco_pt = muon->pt();
    float mu_staco_eta = muon->eta();
    float mu_staco_phi = muon->phi();
    float mu_staco_m = 105.66;	 //muon->m();
//     int mu_staco_isCombinedMuon = muon->isAuthor(xAOD::Muon::Author::STACO); // TODO = muon->isCombinedMuon();
    int mu_staco_isCombinedMuon = (muon->muonType() == xAOD::Muon::MuonType::Combined);
//     int mu_staco_isLowPtReconstructedMuon = muon->isLowPtReconstructedMuon();
//     int mu_staco_tight = muon->isTight();
    uint8_t mu_staco_nBLHits;
    uint8_t mu_staco_nPixHits;
    uint8_t mu_staco_nSCTHits;
    uint8_t mu_staco_nPixHoles;
    uint8_t mu_staco_nSCTHoles;
    uint8_t mu_staco_nTRTOutliers;
    uint8_t mu_staco_nTRTHits;
    muon->summaryValue(mu_staco_nBLHits, xAOD::numberOfBLayerHits);
    muon->summaryValue(mu_staco_nPixHits, xAOD::numberOfPixelHits);
    muon->summaryValue(mu_staco_nSCTHits, xAOD::numberOfSCTHits);
    muon->summaryValue(mu_staco_nPixHoles, xAOD::numberOfPixelHoles);
    muon->summaryValue(mu_staco_nSCTHoles, xAOD::numberOfSCTHoles);
    muon->summaryValue(mu_staco_nTRTOutliers, xAOD::numberOfTRTOutliers);
    muon->summaryValue(mu_staco_nTRTHits, xAOD::numberOfTRTHits);
    uint8_t mu_staco_expectBLayerHit = 255;
    uint8_t mu_staco_nPixelDeadSensors = 255;
    uint8_t mu_staco_nSCTDeadSensors = 255;

    //float mu_staco_matchchi2 = muon->matchChi2();
    float mu_staco_ptcone20 = 0; // = muon->parameter(MuonParameters::ptcone20); //used after MET
    muon->isolation(mu_staco_ptcone20, xAOD::Iso::ptcone20);
    float mu_staco_id_theta = 0;
    float mu_staco_z0_exPV = 0;
//     float mu_staco_d0_exPV = 0;
    float mu_staco_id_theta_exPV = 0;
    float mu_staco_id_qoverp_exPV = 0;
    float mu_staco_me_theta_exPV = 0;
    float mu_staco_me_qoverp_exPV = 0;

    std::string THESTRING="";


    // from: https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/D3PDMaker/MuonD3PDMaker/trunk/src/MuonComponentTrkParameters.cxx
    // for the track parameters without exPV
//     const Trk::ParametersBase *idtparb=0;
//     const Trk::TrackParameters *idtpar=0;
//     const Rec::TrackParticle* idtp = muon->inDetTrackParticle();
    const xAOD::TrackParticle* idtp = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
//     idtparb = idtp ? & idtp->definingParameters() : 0;
//     idtpar = idtparb ? dynamic_cast<const Trk::TrackParameters* > (idtparb) : 0;
//         mu_staco_id_theta = idtpar ? idtpar->parameters()[Trk::theta] : -999999; //variable defined!
    if (idtp)
      mu_staco_id_theta = idtp->theta();

    // get ID track summary
//     const Trk::TrackSummary* trksumy= idtp ? idtp->trackSummary(): 0;
    // from: https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/D3PDMaker/MuonD3PDMaker/trunk/src/MuonTrkHitFillerTool.cxx
//     mu_staco_expectBLayerHit = trksumy ? trksumy->get(Trk::expectBLayerHit) : -999; //variable defined!
//     mu_staco_nPixelDeadSensors = trksumy ? trksumy->get(Trk::numberOfPixelDeadSensors): -999; //variable defined!
//     mu_staco_nSCTDeadSensors = trksumy ? trksumy->get(Trk::numberOfSCTDeadSensors): -999; //variable defined!
    if (idtp) {
      idtp->summaryValue(mu_staco_expectBLayerHit, xAOD::expectBLayerHit);
      idtp->summaryValue(mu_staco_nPixelDeadSensors, xAOD::numberOfPixelDeadSensors);
      idtp->summaryValue(mu_staco_nSCTDeadSensors, xAOD::numberOfSCTDeadSensors);
    }

    // -------------------------------------------------------------------------------

    /*
    // from: https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/D3PDMaker/MuonD3PDMaker/trunk/src/MuonTrackAtPVFiller.cxx
    // for the track parameters with exPV

    xAOD::VertexContainer::const_iterator vxI = vxContainer->begin();

    // primary track
    {
    //       const Rec::TrackParticle* track = muon->track();
      const xAOD::TrackParticle* track = muon->primaryTrackParticle();
    //       bool trackok = (track && track->measuredPerigee()->localErrorMatrix().covariance().num_row() != 0);
      bool trackok = (track && track->perigeeParameters().covariance()->rows() != 0);
      if(!trackok && track) {
    REPORT_MESSAGE(MSG::WARNING)
    << "MuonTrackAtPVFiller: muon (primary) track has null covariance matrix. "
    << "Can't get IP.";
      }
    //       const Trk::MeasuredPerigee* perigee = 0;
      const Trk::Perigee* perigee = 0;
      if(track && trackok && (*vxI) ) {
        perigee = m_trackToVertexTool->perigeeAtVertex((*track), (*vxI)->recVertex().position());
      }
      if(track && trackok && perigee) {
    const HepVector& params = perigee->parameters();

      */
//     TODO:
// 	mu_staco_z0_exPV = params[Trk::z0]; //variable defined!
//  	mu_staco_d0_exPV = params[Trk::d0]; //variable defined!
    if (muon->primaryTrackParticle()) {
      mu_staco_z0_exPV = muon->primaryTrackParticle()->z0();
//         mu_staco_d0_exPV = muon->primaryTrackParticle()->d0();
    }

//       }
//       if(perigee) delete perigee;
//     }

    // ID
//     {
//       const Rec::TrackParticle* trackid = muon->inDetTrackParticle();
//       bool trackidok = (trackid && trackid->measuredPerigee()->localErrorMatrix().covariance().num_row() != 0);
//       if(!trackidok && trackid) {
// 	REPORT_MESSAGE(MSG::WARNING)
// 	  << "MuonTrackAtPVFiller: muon (ID) track has null covariance matrix. "
// 	  << "Can't get IP.";
//       }
//       const Trk::MeasuredPerigee* perigeeid = 0;
//       if(trackid && trackidok && (*vxI) ) {
// 	perigeeid = m_trackToVertexTool->perigeeAtVertex((*trackid), (*vxI)->recVertex().position());
//       }
//       if(trackid && trackidok && perigeeid) {
// 	const HepVector& params = perigeeid->parameters();

//         mu_staco_id_theta_exPV = params[Trk::theta]; //variable defined!
//         mu_staco_id_qoverp_exPV = params[Trk::qOverP]; //variable defined!
    if (idtp) {
      mu_staco_id_theta_exPV  = idtp->theta();
      mu_staco_id_qoverp_exPV = idtp->qOverP();
    }

//       }
//       if(perigeeid) delete perigeeid;
//     }


    // ME
//     {
//       const Rec::TrackParticle* trackme = muon->muonExtrapolatedTrackParticle();
    const xAOD::TrackParticle* trackme = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
//       bool trackmeok = (trackme && trackme->measuredPerigee()->localErrorMatrix().covariance().num_row() != 0);
//       if(!trackmeok && trackme) {
// 	REPORT_MESSAGE(MSG::WARNING)
// 	  << "MuonTrackAtPVFiller: muon (MS extrapolated) track has null covariance matrix. "
// 	  << "Can't get IP.";
//       }
//       const Trk::MeasuredPerigee* perigeeme = 0;
//       if(trackme && trackmeok && (*vxI) ) {
// 	perigeeme = m_trackToVertexTool->perigeeAtVertex((*trackme), (*vxI)->recVertex().position());
//       }
//       if(trackme && trackmeok && perigeeme) {
// 	const HepVector& params = perigeeme->parameters();
//
//      mu_staco_me_theta_exPV = params[Trk::theta]; //variable defined!
//      mu_staco_me_qoverp_exPV = params[Trk::qOverP]; //variable defined!
    if (trackme) {
      mu_staco_me_theta_exPV  = trackme->theta();
      mu_staco_me_qoverp_exPV = trackme->qOverP();
    }
//
//       }
//       if(perigeeme) delete perigeeme;
//     }
    // -------------------------------------------------------------------------------




    float mypt = mu_staco_pt;
    if (RunNumber<150000) { // Do this on MC only // TODO: check no longer correct
      // Use the MC event number to set seed so that the random numbers are reproducible by different analyzers
      float ptcb = mu_staco_pt;
      float ptid = (mu_staco_id_qoverp_exPV != 0.) ? fabs(sin(mu_staco_id_theta_exPV)/mu_staco_id_qoverp_exPV) : 0.;
      float ptms = (mu_staco_me_qoverp_exPV != 0.) ? fabs(sin(mu_staco_me_theta_exPV)/mu_staco_me_qoverp_exPV) : 0.;
      m_mcp_smear.SetSeed((int)1e+5*fabs(mu_staco_phi));
      m_mcp_smear.Event(ptms,ptid,ptcb,mu_staco_eta);

      if (THESTRING == "") {
        if (mu_staco_isCombinedMuon)
          mypt = m_mcp_smear.pTCB();
        else
          mypt = m_mcp_smear.pTID();
      } else {
        double pTMS_smeared = 0.;
        double pTID_smeared = 0.;
        double pTCB_smeared = 0.;

        // Valid values for "THESTRING": {"MSLOW", "MSUP", "IDLOW", "IDUP"}
        m_mcp_smear.PTVar(pTMS_smeared, pTID_smeared, pTCB_smeared, THESTRING);

        if (mu_staco_isCombinedMuon)
          mypt = pTCB_smeared;
        else
          mypt = pTID_smeared;
      }

      mu_staco_pt = mypt; // Overwrite muon pt for later use in analysis
    }

    float mu_px = mu_staco_pt*cos(mu_staco_phi);
    float mu_py = mu_staco_pt*sin(mu_staco_phi);
    float mu_pz = mu_staco_pt/ tan( 2* atan( exp( -mu_staco_eta )));

//     HepLorentzVector mu_HLV;
//     mu_HLV.setPx( mu_px );
//     mu_HLV.setPy( mu_py );
//     mu_HLV.setPz( mu_pz );
//     mu_HLV.setE( sqrt (pow(mu_px,2) + pow(mu_py,2) + pow(mu_pz,2) + pow(mu_staco_m,2) ) );

    //E = Sqrt(x*x+y*y+z*z+m*m)
    //Px = pt*cos(phi)
    //Py = pt*sin(phi)
    //p = pt/sin(theta)
    //Pz = p cos(theta) => pz=pt/tan(theta)
    //eta = -ln(tan(theta/2)) => theta = 2*arctan(exp(-eta))
    //Pz = pt / tan (2*arctan(exp(-eta)))

//     const HepLorentzVector c_mu_HLV = mu_HLV;
//     muon->set4Mom( c_mu_HLV );

    TLorentzVector mu_HLV;
    mu_HLV.SetPxPyPzE(mu_px, mu_py, mu_pz, sqrt (pow(mu_px,2) + pow(mu_py,2) + pow(mu_pz,2) + pow(mu_staco_m,2) ));
//     muon->setP4(mu_HLV.Pt(), mu_HLV.Eta(), mu_HLV.Phi(), mu_HLV.M(), mu_HLV.E()); //  set4Mom(c_mu_HLV); unfortunately we cannot just overwrite the FourMom_t because there is no setter
    muon->setP4(mu_HLV.Pt(), mu_HLV.Eta(), mu_HLV.Phi()); //  set4Mom(c_mu_HLV); unfortunately we cannot just overwrite the FourMom_t because there is no setter


    // cuts
    h_select_mu->Fill(++mu_cut);
    if ( mu_staco_pt <= 10.*GeV)
      continue;

    h_select_mu->Fill(++mu_cut);
    if ( fabs( mu_staco_eta ) >=2.4 )		// part. id. crit.: leptons: |eta|<2.4
      continue;

    // Muon quality cuts (combined muons only at the moment)
    h_select_mu->Fill(++mu_cut);
    if (!(mu_staco_isCombinedMuon)) //  if (!(mu_staco_isCombinedMuon || mu_staco_isLowPtReconstructedMuon))
      continue;		// Muonsel.: combined inner and muon detector Tracks OR segment-tagged muon

    h_select_mu->Fill(++mu_cut);
    //if ( mu_staco_tight != 1)
    //continue;

    // Track quality requirements
    h_select_mu->Fill(++mu_cut);
    if ( mu_staco_expectBLayerHit && mu_staco_nBLHits == 0)
      continue;

    h_select_mu->Fill(++mu_cut);
    if ( mu_staco_nPixHits + mu_staco_nPixelDeadSensors <= 1)
      continue;

    h_select_mu->Fill(++mu_cut);
    if ( mu_staco_nSCTHits + mu_staco_nSCTDeadSensors < 6)
      continue;

    h_select_mu->Fill(++mu_cut);
    if ( mu_staco_nPixHoles + mu_staco_nSCTHoles >= 2)
      continue;

    int nTRTOutliers = mu_staco_nTRTOutliers;
    int nTRTTotal = nTRTOutliers + mu_staco_nTRTHits;
    float trackEta = -log(tan(mu_staco_id_theta/2));

    h_select_mu->Fill(++mu_cut);
    if ( (fabs(trackEta) < 1.9) && (nTRTTotal <= 5))
      continue;

    h_select_mu->Fill(++mu_cut);
    if ( (nTRTTotal > 5) && (nTRTOutliers >= 0.9*nTRTTotal))
      continue;

    /* SUSYValidation v19
    	// Muon cosmic rejection cuts
    	if ( fabs( mu_staco_z0_exPV ) >=5. )	// muon impact parameter in z with respect to primary vertex < 10mm
    	continue;

    	if ( fabs( mu_staco_d0_exPV ) >=2. )
    	continue;
    */

    PreMuons_forMET.push_back( muon );

    //Isolation done after MET definition see below in MET section
    h_select_mu->Fill(++mu_cut);
    if ( mu_staco_ptcone20 >= 1.8*GeV )		// part. id. crit.: isolation in cone dR<0.2:
      continue;					// transverse isol. en. < 10 GeV !old one for etcone20

    PreMuons.push_back( muon );

    /* SUSYValidation v19
     	if ( mu_staco_pt <= 20.*GeV)
     	continue;
    */
    h_select_mu->Fill(++mu_cut);
    if ( fabs( mu_staco_z0_exPV ) >=10 )	// muon impact parameter in z with respect to primary vertex < 10mm
      continue;

    PreSignalMuons.push_back( muon );

    //part. id. crit.: overlap removal see below

    //Overlap removal done in for GoodMuons

  }

  ATH_MSG_DEBUG("SUSYSusyRec::takeAction PreMuons done");
  //End Muons


  // Missing ET---------------------------------------------------------------------------------------
  double Meff = 0; // MET + 4 hardest jets + the good electron or muon

  double etmiss = missET->met();
  // TODO: might not have this MET any more
//   double exmiss = missET->etx() - missET_Muon_Total_Staco->etx();
//   double eymiss = missET->ety() - missET_Muon_Total_Staco->ety();
  double exmiss = missET->mpx();
  double eymiss = missET->mpy();
  if (!m_SUSYmetmuonName.empty()) {
    exmiss -= missET_Muon_Total_Staco->mpx();
    eymiss -= missET_Muon_Total_Staco->mpy();
  }

  for( const xAOD::Muon* muItr: PreMuons_forMET ) {// xAOD::MuonContainer::iterator muItr  = PreMuons_forMET.begin(); muItr != PreMuons_forMET.end(); muItr++) {
//       xAOD::Muon *muon = *muItr;
    TLorentzVector v;
    v.SetPtEtaPhiE(muItr->pt(), muItr->eta(), muItr->phi(), muItr->e());
    exmiss = exmiss - v.Px();//(*muItr)->px();
    eymiss = eymiss - v.Py();//(*muItr)->py();
    etmiss = sqrt ( pow(exmiss,2) + pow(eymiss,2) );

  }

//   Hep3Vector emiss=Hep3Vector(exmiss,eymiss,0.);
  TVector3 emiss=TVector3(exmiss,eymiss,0.);
  m_h_pre_etmiss->Fill(etmiss/GeV, wt);
  Meff += etmiss;

  ATH_MSG_DEBUG("SUSYSusyRec::takeAction h_pre_Etmiss filled");



  // Tau selection -----------------------------------------------------------------------------------

  for (const xAOD::TauJet* tauItr: *eventTauJets ) {
    double pt = tauItr->pt();
//     const Analysis::TauPID*  p_tauid  = (*tauItr)->tauID();
//     if(!p_tauid->isTau(TauJetParameters::TauCutTight)) continue;
//     if(p_tauid->isTau(TauJetParameters::ElectronVeto)) continue;
//     if(p_tauid->isTau(TauJetParameters::MuonVeto)) continue;
    if(!tauItr->isTau(xAOD::TauJetParameters::TauCutTight)) continue;
    if(tauItr->isTau(xAOD::TauJetParameters::ElectronVeto)) continue;
    if(tauItr->isTau(xAOD::TauJetParameters::MuonVeto)) continue;
    if( pt < m_ptl2Cut ) continue;
    m_h_pre_ptt->Fill(pt/GeV, wt);
    m_h_pre_etat->Fill(fabs(tauItr->eta()),wt);
//     goodTaus.push_back(const_cast<Analysis::TauJet*> (*tauItr));
    goodTaus.push_back(tauItr);
    msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction PreTaus done" <<endreq;
    // Taus are not currently trusted
    //     Meff += pt;
    //     sumpx2 +=(*tauItr)->px()*(*tauItr)->px();
    //     sumpy2 +=(*tauItr)->py()*(*tauItr)->py();
    //     sumpxpy +=(*tauItr)->px()*(*tauItr)->py();
  }

  ATH_MSG_DEBUG("SUSYSusyRec::takeAction taus selected");



  // Jets -------------------------------------------------------------------------------------------------

    ATH_MSG_DEBUG("SUSYSusyRec::takeAction njets = " << evJets->size());
    int njet50 = 0;
//   for (JetCollection::const_iterator jetItr = evJets->begin(); jetItr != evJets->end(); jetItr++ )
    for (const xAOD::Jet* jetItr: *evJets) {
    double eta = fabs((jetItr)->eta());
    double pt = jetItr->pt();
    double px = jetItr->px();
    double py = jetItr->py();
    m_h_jet_CutFlow->Fill(0., wt);
    if( pt < m_ptjCut )
      continue;
    m_h_jet_CutFlow->Fill(1., wt);
    if ( eta > m_etajCut )
      continue;
    m_h_jet_CutFlow->Fill(2., wt);

    ATH_MSG_DEBUG("SUSYSusyRec::takeAction now doing OR");


    //do overlap removal for jets and e -----------------------------------------------------------------------

    bool egoverlap=false;
    for (const xAOD::Electron* eIter: PreElectrons ) {
      if (jetItr->p4().DeltaR(eIter->p4()) < 0.2) {
        egoverlap=true;
        break;
      }
    }
    if(egoverlap==true) continue;
    m_h_jet_CutFlow->Fill(3., wt);
    //Taus are not currently trusted
    //     do overlap removal for jets and tau
    //     bool toverlap=false;
    //     for (TauJetContainer::const_iterator tauIter = goodTaus.begin(); tauIter != goodTaus.end(); tauIter++ ){
    //       if ((((*jetItr)->p4()).deltaR((*tIter)->p4()))<0.2){
    // 	toverlap=true;
    // 	break;
    //       }
    //     }
    //if(toverlap==true) continue;
    m_h_jet_CutFlow->Fill(4., wt);

    m_h_pre_pt->Fill(pt/GeV,wt);
    m_h_pre_ptlo->Fill(pt/GeV,wt);
    msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction jet histos filled" <<endreq;
    sumpx2 += px*px;
    sumpy2 += py*py;
    sumpxpy += px*py;

    if( pt > 50*GeV ) ++njet50;
//     goodJets.push_back( const_cast<Jet*> (*jetItr) );
    goodJets.push_back( jetItr );

    msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction good jets filled" <<endreq;
  }
  m_analysisTools->sortPT(&goodJets);
  msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction good jets sorted" <<endreq;




  //Final electron  selection--------------------------------------------------------------------------
  //baseline electrons
  for (const xAOD::Electron* eIter: PreElectrons ) {
    bool overlap=false;
//     for (JetCollection::const_iterator jetIter = goodJets.begin(); jetIter != goodJets.end(); jetIter++ )
    for (const xAOD::Jet* jetIter: goodJets ) {
      // for (unsigned int jetIter = 0; jetIter != goodJets.size(); jetIter++ ){
      if((((eIter)->p4()).DeltaR((jetIter)->p4()))<0.4)  overlap=true;
    }
    if(overlap)continue;
    //if(fabs((*eIter)->eta() )> 1.37 && fabs((*eIter)->eta() ) < 1.52)continue;

    goodElectrons.push_back(eIter);
  }
//   m_analysisTools->sortPT<std::vector<const xAOD::Electron*> >(&goodElectrons);
  m_analysisTools->sortPT(&goodElectrons);
  msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction good electrons sorted" <<endreq;


  //signal electrons
  for (std::vector<const xAOD::Electron*>::const_iterator eIter  = PreSignalElectrons.begin(); eIter != PreSignalElectrons.end(); eIter++ ) {
    double pt  = (*eIter)->pt();
    bool overlap=false;
//     for (JetCollection::const_iterator jetIter = goodJets.begin(); jetIter != goodJets.end(); jetIter++ )
    for (std::vector<const xAOD::Jet*>::const_iterator jetIter = goodJets.begin(); jetIter != goodJets.end(); jetIter++ ) {
      // for (unsigned int jetIter = 0; jetIter != goodJets.size(); jetIter++ ){
      if((((*eIter)->p4()).DeltaR((*jetIter)->p4()))<0.4)  overlap=true;
    }
    if(overlap)continue;
    //if(fabs((*eIter)->eta() )> 1.37 && fabs((*eIter)->eta() ) < 1.52)continue;
    m_h_pre_pte->Fill(pt/GeV, wt);
    m_h_pre_etae->Fill(fabs((*eIter)->eta()),wt);

    goodSignalElectrons.push_back(*eIter);
    if(pt > ptlep ) {
      ptlep2 = ptlep;
      ptlep = pt;
      SecondLep = HardLep;
      HardLep  = (*eIter)->p4();
    }
    if(pt < ptlep && pt > ptlep2 ) {
      ptlep2 = pt;
      SecondLep = (*eIter)->p4();
    }
  }
  m_analysisTools->sortPT(&goodSignalElectrons);
  msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction good signal electrons sorted" <<endreq;


  //Final  muon selection----------------------------------------------------------------------------------
  //baseline muons
  for(std::vector<const xAOD::Muon*>::const_iterator muIter  = PreMuons.begin(); muIter != PreMuons.end(); muIter++) {
    bool overlap=false;
//     for (JetCollection::const_iterator jetIter = goodJets.begin(); jetIter != goodJets.end(); jetIter++ )
    for (std::vector<const xAOD::Jet*>::const_iterator jetIter = goodJets.begin(); jetIter != goodJets.end(); jetIter++ ) {
      //for (unsigned int jetIter = 0; jetIter != goodJets.size(); jetIter++ ){
      if((((*muIter)->p4()).DeltaR((*jetIter)->p4()))<0.4)  overlap=true;
    }
    if(overlap)continue;

//     goodMuons.push_back(const_cast<Analysis::Muon*> (*muIter) );
    goodMuons.push_back( (*muIter) );
  }
  m_analysisTools->sortPT(&goodMuons);
  msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction good muons sortedx" <<endreq;

  //signal muons
  for(std::vector<const xAOD::Muon*>::const_iterator muIter  = PreSignalMuons.begin(); muIter != PreSignalMuons.end(); muIter++) {
    double pt  = (*muIter)->pt();
    bool overlap=false;
//     for (JetCollection::const_iterator jetIter = goodJets.begin(); jetIter != goodJets.end(); jetIter++ )
    for (std::vector<const xAOD::Jet*>::const_iterator  jetIter = goodJets.begin(); jetIter != goodJets.end(); jetIter++ ) {
      //for (unsigned int jetIter = 0; jetIter != goodJets.size(); jetIter++ ){
      if((((*muIter)->p4()).DeltaR((*jetIter)->p4()))<0.4)  overlap=true;
    }
    if(overlap)continue;
    m_h_pre_ptm->Fill(pt/GeV, wt);
    m_h_pre_etam->Fill(fabs((*muIter)->eta()),wt);

    goodSignalMuons.push_back(*muIter);
    if( pt > ptlep ) {
      ptlep2 = ptlep;
      ptlep = pt;
      SecondLep = HardLep;
      HardLep  = (*muIter)->p4();
    }
    if(pt < ptlep && pt > ptlep2 ) {
      ptlep2 = pt;
      SecondLep = (*muIter)->p4();
    }
  }
  m_analysisTools->sortPT(&goodSignalMuons);
  msg(MSG::DEBUG) <<"SUSYSusyRec::takeAction good signal muons sortedx" <<endreq;



  //---------------------------------------------------------------------------------------------

  int ne = goodElectrons.size();
  int nmu = goodMuons.size();
  int ne_signal = goodSignalElectrons.size();
  int nmu_signal = goodSignalMuons.size();
  int ntau = goodTaus.size();
  int njet = goodJets.size();

  m_h_pre_Ne->Fill(ne, wt);
  m_h_pre_Nm->Fill(nmu, wt);
  m_h_pre_Nt->Fill(ntau, wt);
  m_h_pre_Jet_n->Fill(njet, wt);
  m_h_pre_Jet50_n->Fill(njet50, wt);

  TLorentzVector pjet0;
  TLorentzVector pjet1;
  TLorentzVector pjet2;
  TLorentzVector pjet3;

  if(njet > 0) pjet0=(goodJets[0])->p4();
  if(njet > 1) pjet1=(goodJets[1])->p4();
  if(njet > 2) pjet2=(goodJets[2])->p4();
  if(njet > 3) pjet3=(goodJets[3])->p4();

  if(pjet0.Perp() > 0) {
    m_h_pre_pt0->Fill(pjet0.Perp()/GeV, wt);
    m_h_pre_METangle0->Fill(fabs(pjet0.Vect().DeltaPhi(emiss)), wt);
    Meff += pjet0.Perp();
  }
  if(pjet1.Perp() > 0) {
    m_h_pre_pt1->Fill(pjet1.Perp()/GeV, wt);
    m_h_pre_METangle1->Fill(fabs(pjet1.Vect().DeltaPhi(emiss)), wt);
    Meff += pjet1.Perp();
  }
  if(pjet2.Perp() > 0) {
    m_h_pre_pt2->Fill(pjet2.Perp()/GeV, wt);
    m_h_pre_METangle2->Fill(fabs(pjet2.Vect().DeltaPhi(emiss)), wt);
    Meff += pjet2.Perp();
  }
  if(pjet3.Perp() > 0) {
    m_h_pre_pt3->Fill(pjet3.Perp()/GeV, wt);
    m_h_pre_METangle3->Fill(fabs(pjet3.Vect().DeltaPhi(emiss)), wt);
    Meff += pjet3.Perp();
  }

  double mT = 0;// Transverse mass
  double pxlep = HardLep.Px();
  double pylep = HardLep.Py();
  if(ptlep > 0) {
    m_h_pre_ptHardLep->Fill(ptlep/GeV, wt);
    m_h_pre_METangleLept->Fill(fabs(HardLep.Vect().DeltaPhi(emiss)), wt);
    Meff += HardLep.Perp();
    sumpx2 += HardLep.Px()*HardLep.Px();
    sumpy2 += HardLep.Py()*HardLep.Py();
    sumpxpy += HardLep.Px()*HardLep.Py();
    mT = sqrt(pow(etmiss+ptlep,2) - pow(missET->mpx()+pxlep,2) - pow(missET->mpy()+pylep,2));
    m_h_pre_mT->Fill(mT/GeV, wt);
  }

  double ST = 0; //Transverse Sphericity
  ST = CalcTranSphericity(sumpx2,sumpy2,sumpxpy);
  m_h_pre_ST->Fill(ST, wt);
  m_h_pre_Meff->Fill(Meff/GeV, wt);

  ////////////////////////////////
  //////////Trigger - Set for single lepton trigger
  ///////////////////////////////
  if( m_doTrigger ) {
    m_h_Trigger_Flow->Fill(0.,wt);
    if(m_trigDec->isPassed("L1_EM7",TrigDefs::eventAccepted) || m_trigDec->isPassed("L1_MU6",TrigDefs::eventAccepted))  
      m_h_Trigger_Flow->Fill(1.,wt);
    if(m_trigDec->isPassed("L2_e10_medium",TrigDefs::eventAccepted) || m_trigDec->isPassed("L2_mu10",TrigDefs::eventAccepted)) 
      m_h_Trigger_Flow->Fill(2.,wt);
    if(m_trigDec->isPassed("EF_e10_medium",TrigDefs::eventAccepted) || m_trigDec->isPassed("EF_mu10",TrigDefs::eventAccepted)) 
      m_h_Trigger_Flow->Fill(3.,wt);
    if( (m_trigDec->isPassed("EF_e10_medium",TrigDefs::eventAccepted) && PreElectrons.size() >0 )
        || (m_trigDec->isPassed("L2_mu10",TrigDefs::eventAccepted) && PreMuons.size() >0)) 
      m_h_Trigger_Flow->Fill(4.,wt);
  }

  /////////////////////////////////
  //  SUSY Analysis
  /////////////////////////////////

  ATH_MSG_DEBUG("filling cutflows");

  // One Lepton Mode  ---------------------------------------------------------------
  m_h_susy_1muon_CutFlow->Fill(0., wt);
  m_h_susy_1el_CutFlow->Fill(0., wt);



  // Muon Selection -----------------------------------------------------------------
  // Trigger v16: EF_mu18   v15: EF_mu10_MSonly
  if( !m_doTrigger || m_trigDec->isPassed("EF_mu18",TrigDefs::eventAccepted) ) {

    if (m_trigDec->isPassed("EF_mu18",TrigDefs::eventAccepted))
      m_h_susy_1muon_CutFlow->Fill(1., wt);

    // jet cleaning
    //m_h_susy_1muon_CutFlow->Fill(2., wt);

    // primary vertex
    if ( isGoodPrimaryVertex ) {

      m_h_susy_1muon_CutFlow->Fill(3., wt);


      if ( ne_signal + nmu_signal == 1) {
        m_h_susy_ptHardLep->Fill(ptlep/GeV, wt);
      }


      // 1 Lepton (Electron or Muon) pT>20 GeV
      if ( ne ==0 && nmu_signal == 1 && nmu == 1 && ptlep > m_ptlCut ) {

        m_h_susy_1muon_CutFlow->Fill(4., wt);

        if ( goodJets.size() >=2 ) {
          m_h_susy_pt0->Fill(pjet0.Perp()/GeV, wt);
          m_h_susy_pt1->Fill(pjet1.Perp()/GeV, wt);
        }


        // >= 1 jet with pT>60 GeV
        if ( goodJets.size() >=1 && pjet0.Perp() > m_ptj0Cut ) {

          m_h_susy_1muon_CutFlow->Fill(5., wt);


          // >= 3 jets with pT>30 GeV
          if ( goodJets.size() >=3 && pjet2.Perp() > m_ptj1Cut ) {

            m_h_susy_1muon_CutFlow->Fill(6., wt);

            m_h_susy_etmiss->Fill(etmiss/GeV, wt);


            // DeltaPhi(jets, MET) > 0.2
            if ( fabs(pjet0.Vect().DeltaPhi(emiss)) > 0.2 && fabs(pjet1.Vect().DeltaPhi(emiss)) > 0.2
                 && fabs(pjet2.Vect().DeltaPhi(emiss)) > 0.2 ) {

              m_h_susy_1muon_CutFlow->Fill(7., wt);


              // Etmiss > 125 GeV
              if ( etmiss > m_metCut ) {

                m_h_susy_1muon_CutFlow->Fill(8., wt);

                m_h_susy_mT->Fill(mT/GeV, wt);


                // TransverseMass > 100 GeV
                if ( mT > m_mTCut ) {

                  m_h_susy_1muon_CutFlow->Fill(9., wt);


                  // Etmiss > 0.25 Meff
                  if ( etmiss > 0.25*Meff ) {

                    m_h_susy_1muon_CutFlow->Fill(10., wt);

                    m_h_susy_ST->Fill(ST, wt);
                    m_h_susy_Meff->Fill(Meff/GeV, wt);


                    // Meff > 500 GeV
                    if(Meff > m_MeffCut) {

                      m_h_susy_1muon_CutFlow->Fill(11., wt);

                      m_h_susy_Jet_n->Fill(njet, wt);

                      m_h_susy_Meff1lep->Fill(Meff/GeV, wt);
                      m_h_susy_etmiss1lep->Fill(etmiss/GeV, wt);

                      if(ne == 1) {
                        m_h_susy_Meff1ell->Fill(Meff/GeV, wt);
                        m_h_susy_etmiss1ell->Fill(etmiss/GeV, wt);
                      }
                      if(nmu == 1 ) {
                        m_h_susy_Meff1mu->Fill(Meff/GeV, wt);
                        m_h_susy_etmiss1mu->Fill(etmiss/GeV, wt);
                      }

                    }//end Meff
                  }//end Etmiss>0.25*Meff
                }//end mT
              }//end Etmiss
            }//end deltaPhi
          }//end 3jets
        }//end 1jet
      }//end 1 muon
    }//END PRIMARY vertex
  }//end trigger

  ATH_MSG_DEBUG("filling muon cutflow done");


  // Electron Selection -----------------------------------------------------------------
  // Trigger v16: EF_e20_medium   v15: EF_e10_medium
  if( !m_doTrigger || m_trigDec->isPassed("EF_e20_medium",TrigDefs::eventAccepted) ) {

    if (m_trigDec->isPassed("EF_e20_medium",TrigDefs::eventAccepted))
      m_h_susy_1el_CutFlow->Fill(1., wt);

    // jet cleaning
    //m_h_susy_1el_CutFlow->Fill(2., wt);

    // primary vertex
    if ( isGoodPrimaryVertex ) {

      m_h_susy_1el_CutFlow->Fill(3., wt);


      if ( ne_signal + nmu_signal == 1) {
        m_h_susy_ptHardLep->Fill(ptlep/GeV, wt);
      }


      // 1 Lepton (Electron or Muon) pT>20 GeV
      if ( nmu ==0 && ne_signal == 1 && ne == 1 && ptlep > m_ptlCut ) {

        m_h_susy_1el_CutFlow->Fill(4., wt);

        if ( goodJets.size() >=2 ) {
          m_h_susy_pt0->Fill(pjet0.Perp()/GeV, wt);
          m_h_susy_pt1->Fill(pjet1.Perp()/GeV, wt);
        }


        // >= 1 jet with pT>60 GeV
        if ( goodJets.size() >=1 && pjet0.Perp() > m_ptj0Cut ) {

          m_h_susy_1el_CutFlow->Fill(5., wt);


          // >= 3 jets with pT>30 GeV
          if ( goodJets.size() >=3 && pjet2.Perp() > m_ptj1Cut ) {

            m_h_susy_1el_CutFlow->Fill(6., wt);

            m_h_susy_etmiss->Fill(etmiss/GeV, wt);


            // DeltaPhi(jets, MET) > 0.2
            if ( fabs(pjet0.Vect().DeltaPhi(emiss)) > 0.2 && fabs(pjet1.Vect().DeltaPhi(emiss)) > 0.2
                 && fabs(pjet2.Vect().DeltaPhi(emiss)) > 0.2 ) {

              m_h_susy_1el_CutFlow->Fill(7., wt);


              // Etmiss > 125 GeV
              if ( etmiss > m_metCut ) {

                m_h_susy_1el_CutFlow->Fill(8., wt);

                m_h_susy_mT->Fill(mT/GeV, wt);


                // TransverseMass > 100 GeV
                if ( mT > m_mTCut ) {

                  m_h_susy_1el_CutFlow->Fill(9., wt);


                  // Etmiss > 0.25 Meff
                  if ( etmiss > 0.25*Meff ) {

                    m_h_susy_1el_CutFlow->Fill(10., wt);

                    m_h_susy_ST->Fill(ST, wt);
                    m_h_susy_Meff->Fill(Meff/GeV, wt);


                    // Meff > 500 GeV
                    if(Meff > m_MeffCut) {

                      m_h_susy_1el_CutFlow->Fill(11., wt);

                      m_h_susy_Jet_n->Fill(njet, wt);

                      m_h_susy_Meff1lep->Fill(Meff/GeV, wt);
                      m_h_susy_etmiss1lep->Fill(etmiss/GeV, wt);

                      if(ne == 1) {
                        m_h_susy_Meff1ell->Fill(Meff/GeV, wt);
                        m_h_susy_etmiss1ell->Fill(etmiss/GeV, wt);
                      }
                      if(nmu == 1 ) {
                        m_h_susy_Meff1mu->Fill(Meff/GeV, wt);
                        m_h_susy_etmiss1mu->Fill(etmiss/GeV, wt);
                      }

                    }//end Meff
                  }//end Etmiss>0.25*Meff
                }//end mT
              }//end Etmiss
            }//end deltaPhi
          }//end 3jets
        }//end 1jet
      }//end 1 muon
    }//end primary vertex
  }//end trigger

  ATH_MSG_DEBUG("filling electron cutflow done");


  // Zero Lepton Mode  ---------------------------------------------------------------
  m_h_susy_0lep_CutFlow->Fill(0., wt);

  // jet cleaning
  // use loose cleaning for 3 jet channel, use tight cleaning for 2/1 jet channel
  //m_h_susy_0lep_CutFlow->Fill(1., wt);

  // primary vertex
  //m_h_susy_0lep_CutFlow->Fill(2., wt);


  // Lepton Veto
  if ( ne + nmu + ntau == 0) {

    m_h_susy_0lep_CutFlow->Fill(3., wt);


    // >= 1 jet with pT>120 GeV
    if ( goodJets.size() >=1 && pjet0.Perp() > 120*GeV ) {

      m_h_susy_0lep_CutFlow->Fill(4., wt);


      // >= 3 jets with pT>40 GeV, chf<0.02 && emf>0.98 && fabs(η)<2 for the 3 leading jets
      // implement chf and emf
      if ( goodJets.size() >=3 && pjet2.Perp() > m_ptj1Cut && pjet0.Eta() <2 && pjet1.Eta() <2 && pjet2.Eta() <2) {

        m_h_susy_0lep_CutFlow->Fill(5., wt);


        // Etmiss > 100 GeV
        if ( etmiss > 100*GeV ) {

          m_h_susy_0lep_CutFlow->Fill(6., wt);


          // DeltaPhi(jets, MET) > 0.4
          if ( fabs(pjet0.Vect().DeltaPhi(emiss)) > 0.4 && fabs(pjet1.Vect().DeltaPhi(emiss)) > 0.4
               && fabs(pjet2.Vect().DeltaPhi(emiss)) > 0.4 ) {

            m_h_susy_0lep_CutFlow->Fill(7., wt);


            // Etmiss > 0.25 Meff
            if ( etmiss > 0.25*Meff ) {

              m_h_susy_0lep_CutFlow->Fill(8., wt);


              // Meff > 500 GeV
              if(Meff > m_MeffCut) {

                m_h_susy_0lep_CutFlow->Fill(9., wt);

              }
            }
          }
        }
      }
    }
  }

  ATH_MSG_DEBUG("filling zero lepton cutflow done");

  // Two Lepton Mode  ---------------------------------------------------------------
  m_h_susy_2lep_CutFlow->Fill(0., wt);

  // jet cleaning
  //m_h_susy_2lep_CutFlow->Fill(1., wt);

  // primary vertex
  //m_h_susy_2lep_CutFlow->Fill(2., wt);

  // maybe crack veto


  // 2 Leptons (Electron or Muon)
  if ( ne + nmu >= 2 ) {

    /*
    int mode_2lep =0; 		// 1= ee, 2= mumu, 3=emu, 4=mue

    if (nmu ==0) mode_2lep= 1;
    if (ne ==0)  mode_2lep= 2;

    if ( ne >=1 && nmu >=1 )
    {
      if ( (*goodElectrons)[0]->pt() > (*goodMuons)[0]->pt() )
      {
    if ( ne >1 && (*goodElectrons)[1]->pt() > (*goodMuons)[0]->pt() )
    mode_2lep=1;
    else
    mode_2lep=3;
      }
      else
      {
    if (nmu >1 && (*goodMuons)[1]->pt() > (*goodElectrons)[0]->pt() )
    mode_2lep=2;
    else
    mode_2lep=4;
      }

    if ( mode_2lep == 1 || mode_2lep == 4 )
      ptlep2 = (*goodElectrons)[1]->pt();
    if ( mode_2lep == 2 || mode_2lep == 3 )
      ptlep2 = (*goodMuons)[1]->pt();
    }
    */

    // pt(lep)>10,10
    if ( ptlep > 10*GeV && ptlep2 > 10*GeV ) {

      m_h_susy_2lep_CutFlow->Fill(3., wt);


      // >= 2 jets with pT>30 GeV
      if ( goodJets.size() >=2 && pjet0.Perp() > 30*GeV && pjet1.Perp() > 20*GeV) {

        m_h_susy_2lep_CutFlow->Fill(4., wt);



        // Etmiss > 30 GeV
        if ( etmiss > 30*GeV ) {

          m_h_susy_2lep_CutFlow->Fill(5., wt);


          // Etmiss > 0.3 Meff
          if ( etmiss > 0.3*Meff ) {

            m_h_susy_2lep_CutFlow->Fill(6., wt);


            // TransverseSpericity > 0.2
            if ( ST > 0.2 ) {

              m_h_susy_2lep_CutFlow->Fill(7., wt);


              // DeltaPhi(jets, MET) > 0.2
              if ( fabs(pjet0.Vect().DeltaPhi(emiss)) > 0.2 && fabs(pjet1.Vect().DeltaPhi(emiss)) > 0.2 ) {

                m_h_susy_2lep_CutFlow->Fill(8., wt);


                // M(ll) > 5 GeV
                double mll = (HardLep + SecondLep).M();
// 		m_h_susy_mee->Fill(mll/GeV, wt);
                m_h_susy_mll->Fill(mll/GeV, wt);

                double mllq0 = (HardLep+SecondLep+pjet0).M();
                double mllq1 = (HardLep+SecondLep+pjet1).M();
                double mllqmx = (mllq0 > mllq1) ? mllq0 : mllq1;
                double mllqmn = (mllq0 < mllq1) ? mllq0 : mllq1;
                m_h_susy_mllq->Fill(mllqmn/GeV, wt);

                if ( mll > 5.*GeV) {
                  m_h_susy_2lep_CutFlow->Fill(9., wt);

                }
                if( mll > 77.0*GeV ) m_h_susy_tllq->Fill(mllqmx/GeV, wt);

                TLorentzVector pj = pjet0;
                if( mllq1 < mllq0 ) pj = pjet1;
                double mlq0 = (HardLep+pj).M();
                double mlq1 = (SecondLep+pj).M();
                double mlqmx = (mlq0 > mlq1) ? mlq0 : mlq1;
                double mlqmn = (mlq0 < mlq1) ? mlq0 : mlq1;
                m_h_susy_mlq1->Fill(mlqmx/GeV, wt);
                m_h_susy_mlq2->Fill(mlqmn/GeV, wt);

              }
            }
          }
        }
      }
    }
  }

  ATH_MSG_DEBUG("filling two lepton cutflow done");


  /*
   // old cutflow ----------------------------------------------------------------------
    m_h_susy_CutFlow->Fill(0., wt);
    if (goodJets.size()<4) return StatusCode::SUCCESS;
    JetMomentMapManager::s_instance.remove(map);

    m_h_susy_1muon_CutFlow->Fill(1., wt);
    m_h_susy_pt0->Fill(pjet0.Perp()/GeV, wt);
    m_h_susy_pt1->Fill(pjet1.Perp()/GeV, wt);
    if( pjet0.Perp() < m_ptj0Cut || pjet1.Perp() < m_ptj1Cut
        || pjet2.Perp() < m_ptj1Cut|| pjet3.Perp() < m_ptj1Cut) return StatusCode::SUCCESS;

    m_h_susy_CutFlow->Fill(2., wt);
    m_h_susy_etmiss->Fill(etmiss/GeV, wt);
    if( etmiss < m_metCut || etmiss < 0.2*Meff ) return StatusCode::SUCCESS;

    m_h_susy_CutFlow->Fill(3., wt);
    m_h_susy_ST->Fill(ST, wt);
    if(ST < m_STCut) return StatusCode::SUCCESS;

    m_h_susy_CutFlow->Fill(4., wt);
    m_h_susy_Meff->Fill(Meff/GeV, wt);
    if(Meff < m_MeffCut) return StatusCode::SUCCESS;

    m_h_susy_CutFlow->Fill(5., wt);
    m_h_susy_Jet_n->Fill(njet, wt);

    ////////////////////////////////
    //Single lepton mode
    ////////////////////////////////

    if( ne + nmu == 1 ) {
      m_h_susy_ptHardLep->Fill(ptlep/GeV, wt);
      if(ptlep>m_ptlCut){

        m_h_susy_CutFlow->Fill(6., wt);
        m_h_susy_mT->Fill(mT/GeV, wt);
        if(mT > m_mTCut){
          m_h_susy_CutFlow->Fill(7., wt);
          m_h_susy_Meff1lep->Fill(Meff/GeV, wt);
          m_h_susy_etmiss1lep->Fill(etmiss/GeV, wt);

          if(ne == 1) {
            m_h_susy_Meff1ell->Fill(Meff/GeV, wt);
            m_h_susy_etmiss1ell->Fill(etmiss/GeV, wt);
          }
          if(nmu == 1 ) {
            m_h_susy_Meff1mu->Fill(Meff/GeV, wt);
           m_h_susy_etmiss1mu->Fill(etmiss/GeV, wt);
          }
        }
      }
    }
    */

  ////////////////////////////////
  // Dilepton Mode
  ////////////////////////////////






  /*
  // tau tau
  for (TauJetContainer::const_iterator gTauItr = goodTaus.begin(); gTauItr != goodTaus.end(); gTauItr++ ){
    for (TauJetContainer::const_iterator gTauItr2 = goodTaus.begin(); gTauItr2 != goodTaus.end(); gTauItr2++ ){
      if( (*gTauItr) == (*gTauItr2) ) continue;
      double mtautau = ((*gTauItr)->hlv()+(*gTauItr2)->hlv()).m();
      if( (*gTauItr)->charge() * (*gTauItr2)->charge() == 0 )  msg(MSG::WARNING) << "charge() returning 0 for Tau"  << endreq;
      if( (*gTauItr)->charge() * (*gTauItr2)->charge() < 0 ) {
  m_h_susy_mtautauos->Fill(mtautau/GeV,wt);
      } else {
        m_h_susy_mtautauss->Fill(mtautau/GeV,wt);
      }
    }
  }
  */

  ATH_MSG_DEBUG("All done for this event.");


  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////
////calculate Transverse spericity
double SUSYSusyRec::CalcTranSphericity(double sumpx2,double sumpy2,double sumpxpy) {
  double marray[4];
  for(int i=0; i<4; i++) {
    marray[i]=0;
  }
  marray[0]=sumpx2;
  marray[1]=sumpxpy;
  marray[2]=sumpxpy;
  marray[3]=sumpy2;

  // use root matrix to find eigenvalues...
  TMatrixDSym matrix(2);
  matrix.SetMatrixArray(marray);

  TMatrixDSymEigen eigen(matrix);
  TVectorD E = eigen.GetEigenValues();

  // from the babar sphericity code...
  double lambda1 = 0;
  double lambda2 = 0;

  if(E[0] < E[1]) {
    lambda1 = E[0];
    lambda2 = E[1];
  } else {
    lambda1 = E[1];
    lambda2 = E[0];
  }

  double ST = 0;
  ST = 2*lambda1/( lambda1 + lambda2);
  return ST;
}
