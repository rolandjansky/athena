/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

//#include "CLHEP/Units/PhysicalConstants.h"
#include "TLorentzVector.h"
#include <vector>
#include "MissingETPerformance/FakeMissingETTool.h"
using namespace std;

//#include "JetUtils/JetSorters.h"
#include "FourMomUtils/P4Sorters.h"


using CLHEP::HepLorentzVector;
//using namespace JetSorters;


class sortEt
{
public:
  bool operator()(const TLorentzVector& t1, const TLorentzVector& t2)
  { return ( t1.Et() > t2.Et() ); }
};

//------------------------------------------------------------------------------
FakeMissingETTool::FakeMissingETTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ), 
    _jetVariables ("JetVariablesTool") {

  declareInterface<FakeMissingETTool>( this );

  declareProperty("minJetEtCut", _minJetEtCut   = 10.); // GeV
  declareProperty("fakeFullCut", _fakeFullCut   = 100.); // GeV
  declareProperty("fakeMuonCut", _fakeMuonCut   = 100.); // GeV

  declareProperty("nbinsMET", _nbinsMET   = 200);
  declareProperty("lowerbinMET", _lowerbinMET   = 0.);
  declareProperty("upperbinMET", _upperbinMET   = 200.);

  declareProperty("nbinsMETxy", _nbinsMETxy   = 800);
  declareProperty("lowerbinMETxy", _lowerbinMETxy   = -400.);
  declareProperty("upperbinMETxy", _upperbinMETxy   = 400.);

  declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode FakeMissingETTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Can't get THistSvc" << endreq;
    return sc;
  }

  sc = _jetVariables.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Can't get handle on JetVariablesTool in BasicPlotsTool" << endreq;
    return sc;
  }

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endreq;
     return sc;
  }
  h1_met_neut  = new TH1D( "h1_met_neut" ,"True Missing ET", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/h1_met_neut", h1_met_neut);
  h1_met_fake  = new TH1D( "h1_met_fake" ,"Fake Missing ET (True - Reco, vector difference)", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/h1_met_fake", h1_met_fake);

  //Muons
  h2_met_fakemuon_vs_fakefull       = new TH2D( "h2_met_fakemuon_vs_fakefull" ,"h2_met_fakemuon_vs_fakefull", _nbinsMETxy, _lowerbinMETxy,_upperbinMETxy,_nbinsMETxy, _lowerbinMETxy,_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h2_met_fakemuon_vs_fakefull", h2_met_fakemuon_vs_fakefull);
  h1_met_fake_with_mcmu = new TH1D("h1_met_fake_with_mcmu","h1_met_fake_with_mcmu", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_met_fake_with_mcmu", h1_met_fake_with_mcmu);
  h2_met_fakemuon_vs_fakefull_muon  = new TH2D( "h2_met_fakemuon_vs_fakefull_muon" ,"h2_met_fakemuon_vs_fakefull_muon", _nbinsMETxy, _lowerbinMETxy,_upperbinMETxy,_nbinsMETxy, _lowerbinMETxy,_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h2_met_fakemuon_vs_fakefull_muon", h2_met_fakemuon_vs_fakefull_muon);
  h1_met_fake_muon    = new TH1D( "h1_met_fake_muon" ,"h1_met_fake_muon", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_met_fake_muon", h1_met_fake_muon);
  h1_met_fake_muon_xy = new TH1D( "h1_met_fake_muon_xy" ,"h1_met_fake_muon_xy", _nbinsMETxy, _lowerbinMETxy,_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_met_fake_muon_xy", h1_met_fake_muon_xy);
  //
  h1_missedmuonpt = new TH1D("h1_missedmuonpt","h1_missedmuonpt",500,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_missedmuonpt", h1_missedmuonpt);
  h1_eta_muons_missed_events = new TH1D("h1_eta_muons_missed_events","h1_eta_muons_missed_events",800,-4.,4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_eta_muons_missed_events", h1_eta_muons_missed_events);
  h1_phi_muons_missed_events = new TH1D("h1_phi_muons_missed_events","h1_phi_muons_missed_events",63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_phi_muons_missed_events", h1_phi_muons_missed_events);
  h2_eta_v_phi_muons_missed_events = new TH2D("h2_eta_v_phi_muons_missed_events","h2_eta_v_phi_muons_missed_events", 80, -4., 4., 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h2_eta_v_phi_muons_missed_events", h2_eta_v_phi_muons_missed_events);

  h1_bad_muon_reco_algo_fake_muon = new TH1D("h1_bad_muon_reco_algo_fake_muon","h1_bad_muon_reco_algo_fake_muon", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_bad_muon_reco_algo_fake_muon", h1_bad_muon_reco_algo_fake_muon);
  h1_bad_muon_reco_algo_fake_full = new TH1D("h1_bad_muon_reco_algo_fake_full","h1_bad_muon_reco_algo_fake_full", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_bad_muon_reco_algo_fake_full", h1_bad_muon_reco_algo_fake_full);
  h1_bad_muon_reco_mis_fake_muon = new TH1D("h1_bad_muon_reco_mis_fake_muon","h1_bad_muon_reco_mis_fake_muon", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_bad_muon_reco_mis_fake_muon", h1_bad_muon_reco_mis_fake_muon);
  h1_bad_muon_reco_mis_fake_full = new TH1D("h1_bad_muon_reco_mis_fake_full","h1_bad_muon_reco_mis_fake_full", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_bad_muon_reco_mis_fake_full", h1_bad_muon_reco_mis_fake_full);

  h1_mag_muon_diff = new TH1D("h1_mag_muon_diff","h1_mag_muon_diff",500,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_mag_muon_diff", h1_mag_muon_diff);

  h2_mag_muon_diff_V_truth_pt = new TH2D("h2_mag_muon_diff_V_truth_pt","h2_mag_muon_diff_V_truth_pt",300,0.,300.,500,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h2_mag_muon_diff_V_truth_pt",h2_mag_muon_diff_V_truth_pt);

  //
  //
  //Jets
  //
  h1_dphi_leadjet_etmiss_mc = new TH1D("h1_dphi_leadjet_etmiss_mc","h1_dphi_leadjet_etmiss_mc", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_leadjet_etmiss_mc", h1_dphi_leadjet_etmiss_mc);

  h1_dphi_leadjet_etmiss = new TH1D("h1_dphi_leadjet_etmiss","dphi(leadjet,etmiss)", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_leadjet_etmiss", h1_dphi_leadjet_etmiss);

  h1_dphi_secondjet_etmiss = new TH1D("h1_dphi_secondjet_etmiss","dphi(secondjet,etmiss)", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_secondjet_etmiss", h1_dphi_secondjet_etmiss);

  h1_dphi_leadjet_etmiss_mc_V_met_neut = new TH2D("h1_dphi_leadjet_etmiss_mc_V_met_neut","h1_dphi_leadjet_etmiss_mc_V_met_neut",315,0,3.15,73,0,1460);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_leadjet_etmiss_mc_V_met_neut", h1_dphi_leadjet_etmiss_mc_V_met_neut);

  h1_n_reco_jets                    = new TH1D("h1_n_reco_jets","all events, num reco jets",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_n_reco_jets", h1_n_reco_jets);

  h1_reco_jets_et                   = new TH1D("h1_reco_jets_et","all events, reco jets Et",500,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_reco_jets_et", h1_reco_jets_et);

  h1_n_reco_jets_fakemet            = new TH1D("h1_n_reco_jets_fakemet","fake events, num reco jets",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_n_reco_jets_fakemet", h1_n_reco_jets_fakemet);

  h1_reco_jets_et_fakemet           = new TH1D("h1_reco_jets_et_fakemet","fake events, reco jets Et",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_reco_jets_et_fakemet", h1_reco_jets_et_fakemet);

  h2_met_fakemuon_vs_fakefull_jets  = new TH2D( "h2_met_fakemuon_vs_fakefull_jets" ,"h2_met_fakemuon_vs_fakefull_jets", _nbinsMETxy, _lowerbinMETxy,_upperbinMETxy,_nbinsMETxy, _lowerbinMETxy,_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_met_fakemuon_vs_fakefull_jets", h2_met_fakemuon_vs_fakefull_jets);

  h1_fake_dphi_leadjet_etmiss = new TH1D("h1_fake_dphi_leadjet_etmiss","h1_fake_dphi_leadjet_etmiss",315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/fake_dphi_leadjet_etmiss", h1_fake_dphi_leadjet_etmiss);

  h1_fake_dphi_secondjet_etmiss = new TH1D("h1_fake_dphi_secondjet_etmiss","h1_fake_dphi_secondjet_etmiss",315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/fake_dphi_secondjet_etmiss", h1_fake_dphi_secondjet_etmiss);

  //dphicut plots
  h1_dphicut_leadjet_met_neut_mc = new TH1D("h1_dphicut_leadjet_met_neut_mc","h1_dphicut_leadjet_met_neut_mc",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_dphicut_leadjet_met_neut_mc", h1_dphicut_leadjet_met_neut_mc);
  h1_lead_dphicut_met_fake = new TH1D("h1_lead_dphicut_met_fake","h1_lead_dphicut_met_fake",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_lead_dphicut_met_fake", h1_lead_dphicut_met_fake);
  h1_lead_dphicut_met_fake_mc = new TH1D("h1_lead_dphicut_met_fake_mc","h1_lead_dphicut_met_fake_mc",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_lead_dphicut_met_fake_mc", h1_lead_dphicut_met_fake_mc);
  //
  h1_dphicut_met_fake_with_mcmu = new TH1D("h1_dphicut_met_fake_with_mcmu","h1_dphicut_met_fake_with_mcmu",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_dphicut_met_fake_with_mcmu", h1_dphicut_met_fake_with_mcmu);
  h1_dphicut_met_fake = new TH1D("h1_dphicut_met_fake","h1_dphicut_met_fake",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_dphicut_met_fake", h1_dphicut_met_fake);
  h1_dphicut_met_neut = new TH1D("h1_dphicut_met_neut","h1_dphicut_met_neut",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_dphicut_met_neut", h1_dphicut_met_neut);
  h1_lead_dphicut_met_neut = new TH1D("h1_lead_dphicut_met_neut","h1_lead_dphicut_met_neut",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_lead_dphicut_met_neut", h1_lead_dphicut_met_neut);

  h1_dphi_worstjet_etmiss = new TH1D("h1_dphi_worstjet_etmiss","h1_dphi_worstjet_etmiss", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_worstjet_etmiss", h1_dphi_worstjet_etmiss);
  h1_dphi_leadjet_etmiss_V_met_fake = new TH2D("h1_dphi_leadjet_etmiss_V_met_fake","h1_dphi_leadjet_etmiss_V_met_fake",315,0,3.15,_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_leadjet_etmiss_V_met_fake", h1_dphi_leadjet_etmiss_V_met_fake);

  h1_dphi_secondjet_etmiss_V_met_fake = new TH2D("h1_dphi_secondjet_etmiss_V_met_fake","h1_dphi_secondjet_etmiss_V_met_fake",315,0,3.15,_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_secondjet_etmiss_V_met_fake", h1_dphi_secondjet_etmiss_V_met_fake);
  //
  h1_dphiseccut_met_fake_with_mcmu = new TH1D("h1_dphiseccut_met_fake_with_mcmu","h1_dphiseccut_met_fake_with_mcmu",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphiseccut_met_fake_with_mcmu", h1_dphiseccut_met_fake_with_mcmu);
  h1_dphiseccut_met_fake = new TH1D("h1_dphiseccut_met_fake","h1_dphiseccut_met_fake",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphiseccut_met_fake", h1_dphiseccut_met_fake);
  h1_dphiseccut_met_neut = new TH1D("h1_dphiseccut_met_neut","h1_dphiseccut_met_neut",_nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphiseccut_met_neut", h1_dphiseccut_met_neut);
  h1_dphi_sec_worstjet_etmiss = new TH1D("h1_dphi_sec_worstjet_etmiss","h1_dphi_sec_worstjet_etmiss", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_sec_worstjet_etmiss", h1_dphi_sec_worstjet_etmiss);
  //
  h1_met_fake_jets          = new TH1D( "h1_met_fake_jets" ,"h1_met_fake_jets", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets", h1_met_fake_jets);
  h1_met_fake_jets_incrack  = new TH1D( "h1_met_fake_jets_incrack" ,"h1_met_fake_jets_incrack", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_incrack", h1_met_fake_jets_incrack);
  h1_met_fake_jets_outcrack = new TH1D( "h1_met_fake_jets_outcrack" ,"h1_met_fake_jets_outcrack", _nbinsMET, _lowerbinMET,_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_outcrack", h1_met_fake_jets_outcrack);
  //
  h1_met_fake_jets_xy          = new TH1D( "h1_met_fake_jets_xy" ,"h1_met_fake_jets_xy", _nbinsMETxy, _lowerbinMETxy,_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_xy", h1_met_fake_jets_xy);
  h1_met_fake_jets_incrack_xy  = new TH1D( "h1_met_fake_jets_incrack_xy" ,"h1_met_fake_jets_incrack_xy", _nbinsMETxy, _lowerbinMETxy,_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_incrack_xy", h1_met_fake_jets_incrack_xy);
  h1_met_fake_jets_outcrack_xy = new TH1D( "h1_met_fake_jets_outcrack_xy" ,"h1_met_fake_jets_outcrack_xy", _nbinsMETxy, _lowerbinMETxy,_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_outcrack_xy", h1_met_fake_jets_outcrack_xy);
  //
  h1_met_worst_jet_maxdEt = new TH1D( "h1_met_worst_jet_maxdEt" ,"h1_met_worst_jet_maxdEt", 1000, -500., 500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_maxdEt", h1_met_worst_jet_maxdEt);
  h1_met_worst_jet_eta = new TH1D( "h1_met_worst_jet_eta" ,"h1_met_worst_jet_eta", 80, -4., 4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_eta", h1_met_worst_jet_eta);
  h1_met_worst_jet_abseta = new TH1D( "h1_met_worst_jet_abseta" ,"h1_met_worst_jet_abseta", 40, 0., 4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_abseta", h1_met_worst_jet_abseta);
  h1_met_worst_jet_phi = new TH1D( "h1_met_worst_jet_phi" ,"h1_met_worst_jet_phi", 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_phi", h1_met_worst_jet_phi);
  h2_met_worst_jet_eta_vs_phi = new TH2D( "h2_met_worst_jet_eta_vs_phi" ,"h2_met_worst_jet_eta_vs_phi", 80, -4., 4., 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_met_worst_jet_eta_vs_phi", h2_met_worst_jet_eta_vs_phi);
  h2_met_worst_jet_dMET_vs_eta = new TH2D( "h2_met_worst_jet_dMET_vs_eta" ,"h2_met_worst_jet_dMET_vs_eta", 80,-4,4,100,-500,500);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_met_worst_jet_dMET_vs_eta", h2_met_worst_jet_dMET_vs_eta);
  //
  h1_met_secondworst_jet_abseta = new TH1D("h1_met_secondworst_jet_abseta","h1_met_secondworst_jet_abseta", 40, 0., 4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_abseta", h1_met_secondworst_jet_abseta);
  h1_met_secondworst_jet_eta = new TH1D("h1_met_secondworst_jet_eta","h1_met_secondworst_jet_eta", 80, -4., 4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_eta", h1_met_secondworst_jet_eta);
  h1_met_secondworst_jet_phi = new TH1D("h1_met_secondworst_jet_phi","h1_met_secondworst_jet_phi", 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_phi", h1_met_secondworst_jet_phi);
  h2_met_secondworst_jet_eta_vs_phi = new TH2D("h2_met_secondworst_jet_eta_vs_phi","h2_met_secondworst_jet_eta_vs_phi", 80, -4., 4., 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_met_secondworst_jet_eta_vs_phi", h2_met_secondworst_jet_eta_vs_phi);
  //
  h1_met_worst_jet_div_met = new TH1D("h1_met_worst_jet_div_met","h1_met_worst_jet_div_met",300,0.,3.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_div_met", h1_met_worst_jet_div_met);
  h1_met_secondworst_jet_div_met = new TH1D("h1_met_secondworst_jet_div_met","h1_met_secondworst_jet_div_met",300,0.,3.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_div_met", h1_met_secondworst_jet_div_met);
  h1_met_worst_jet_div_met_xy = new TH1D("h1_met_worst_jet_div_met_xy","h1_met_worst_jet_div_met_xy",200,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_div_met_xy", h1_met_worst_jet_div_met_xy);
  h1_met_secondworst_jet_div_met_xy = new TH1D("h1_met_secondworst_jet_div_met_xy","h1_met_secondworst_jet_div_met_xy",200,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_div_met_xy", h1_met_secondworst_jet_div_met_xy);
  //
  h1_leading_reco_jet_Et            = new TH1D( "h1_leading_reco_jet_Et","h1_leading_reco_jet_Et",500,0.,500.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_reco_jet_Et", h1_leading_reco_jet_Et);
  h1_leading_reco_jet_abseta        = new TH1D( "h1_leading_reco_jet_abseta","h1_leading_reco_jet_abseta",50,0.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_reco_jet_abseta", h1_leading_reco_jet_abseta);
  h2_leading_reco_jet_abseta_V_fakemet = new TH2D( "h2_leading_reco_jet_abseta_V_fakemet","h2_leading_reco_jet_abseta_V_fakemet",50,0.,5.,300,0.,300.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_leading_reco_jet_abseta_V_fakemet", h2_leading_reco_jet_abseta_V_fakemet);
  h1_leading_reco_jet_eta           = new TH1D( "h1_leading_reco_jet_eta","h1_leading_reco_jet_eta",1000,-5.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_reco_jet_eta", h1_leading_reco_jet_eta);
  h1_leading_truth_jet_Et           = new TH1D( "h1_leading_truth_jet_Et","h1_leading_truth_jet_Et",500,0.,500.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_truth_jet_Et", h1_leading_truth_jet_Et);
  h1_leading_truth_jet_abseta       = new TH1D( "h1_leading_truth_jet_abseta","h1_leading_truth_jet_abseta",50,0.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_truth_jet_abseta", h1_leading_truth_jet_abseta);
  h1_leading_truth_jet_eta          = new TH1D( "h1_leading_truth_jet_eta","h1_leading_truth_jet_eta",1000,-5.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_truth_jet_eta", h1_leading_truth_jet_eta);

  h1_second_reco_jet_Et            = new TH1D( "h1_second_reco_jet_Et","h1_second_reco_jet_Et",500,0.,500.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_second_reco_jet_Et", h1_second_reco_jet_Et);
  h1_second_reco_jet_abseta        = new TH1D( "h1_second_reco_jet_abseta","h1_second_reco_jet_abseta",50,0.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_second_reco_jet_abseta", h1_second_reco_jet_abseta);
  h2_second_reco_jet_abseta_V_fakemet = new TH2D( "h2_second_reco_jet_abseta_V_fakemet","h2_second_reco_jet_abseta_V_fakemet",50,0.,5.,300,0.,300.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_second_reco_jet_abseta_V_fakemet", h2_second_reco_jet_abseta_V_fakemet);
  h1_second_reco_jet_eta           = new TH1D( "h1_second_reco_jet_eta","h1_second_reco_jet_eta",1000,-5.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_second_reco_jet_eta", h1_second_reco_jet_eta);

  h2_MET_Topo_vs_evt_emfrac = new TH2F("h2_MET_Topo_vs_evt_emfrac","MissingET from MET_Topo vs event em fraction",
				       400,-2.,2.,300,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_vs_evt_emfrac",h2_MET_Topo_vs_evt_emfrac);
  h2_MET_RefFinal_vs_evt_emfrac = new TH2F("h2_MET_RefFinal_vs_evt_emfrac","MissingET from MET_RefFinal vs event em fraction",
					   400,-2.,2.,300,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_vs_evt_emfrac",h2_MET_RefFinal_vs_evt_emfrac);

  h1_emfcut_MET_Topo_met = new TH1F("h1_emfcut_MET_Topo_met",
				    "Missing ET from MET_Topo (with event emf cut)",600,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_Topo_met",h1_emfcut_MET_Topo_met);
  h1_emfcut_MET_Topo_x = new TH1F("h1_emfcut_MET_Topo_x",
				  "Missing Etx from MET_Topo (with event emf cut)",500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_Topo_x",h1_emfcut_MET_Topo_x);
  h1_emfcut_MET_Topo_y = new TH1F("h1_emfcut_MET_Topo_y",
				  "Missing Ety from MET_Topo (with event emf cut)",500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_Topo_y",h1_emfcut_MET_Topo_y);
  h1_emfcut_MET_Topo_sumet = new TH1F("h1_emfcut_MET_Topo_sumet",
				      "Sumet from MET_Topo (with event emf cut)",600,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_Topo_sumet",h1_emfcut_MET_Topo_sumet);

  h1_emfcut_MET_RefFinal_met = new TH1F("h1_emfcut_MET_RefFinal_met",
					"Missing ET from MET_RefFinal (with event emf cut)",600,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_RefFinal_met",h1_emfcut_MET_RefFinal_met);
  h1_emfcut_MET_RefFinal_x = new TH1F("h1_emfcut_MET_RefFinal_x",
				      "Missing Etx from MET_RefFinal (with event emf cut)",500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_RefFinal_x",h1_emfcut_MET_RefFinal_x);
  h1_emfcut_MET_RefFinal_y = new TH1F("h1_emfcut_MET_RefFinal_y",
				      "Missing Ety from MET_RefFinal (with event emf cut)",500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_RefFinal_y",h1_emfcut_MET_RefFinal_y);
  h1_emfcut_MET_RefFinal_sumet = new TH1F("h1_emfcut_MET_RefFinal_sumet",
					  "Sumet from MET_RefFinal (with event emf cut)",600,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_RefFinal_sumet",h1_emfcut_MET_RefFinal_sumet);

  h2_MET_Topo_met_vs_njets = new TH2F("h2_MET_Topo_met_vs_njets","Missing ET from MET_Topo vs number reco jets",20,0,20,1000,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_met_vs_njets",h2_MET_Topo_met_vs_njets);
  h2_MET_Topo_x_vs_njets = new TH2F("h2_MET_Topo_x_vs_njets","Missing ETx from MET_Topo vs number reco jets",20,0,20,800,-200.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_x_vs_njets",h2_MET_Topo_x_vs_njets);
  h2_MET_Topo_y_vs_njets = new TH2F("h2_MET_Topo_y_vs_njets","Missing ETy from MET_Topo vs number reco jets",20,0,20,800,-200.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_y_vs_njets",h2_MET_Topo_y_vs_njets);
  h2_MET_Topo_sumet_vs_njets = new TH2F("h2_MET_Topo_sumet_vs_njets","Sumet from MET_Topo vs number reco jets",20,0,20,500,0.,1000.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_sumet_vs_njets",h2_MET_Topo_sumet_vs_njets);

  h2_MET_RefFinal_met_vs_njets = new TH2F("h2_MET_RefFinal_met_vs_njets","Missing ET from MET_RefFinal vs number reco jets",20,0,20,1000,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_met_vs_njets",h2_MET_RefFinal_met_vs_njets);
  h2_MET_RefFinal_x_vs_njets = new TH2F("h2_MET_RefFinal_x_vs_njets","Missing ETx from MET_RefFinal vs number reco jets",20,0,20,800,-200.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_x_vs_njets",h2_MET_RefFinal_x_vs_njets);
  h2_MET_RefFinal_y_vs_njets = new TH2F("h2_MET_RefFinal_y_vs_njets","Missing ETy from MET_RefFinal vs number reco jets",20,0,20,800,-200.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_y_vs_njets",h2_MET_RefFinal_y_vs_njets);
  h2_MET_RefFinal_sumet_vs_njets = new TH2F("h2_MET_RefFinal_sumet_vs_njets","Sumet from MET_RefFinal vs number reco jets",20,0,20,500,0.,1000.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_sumet_vs_njets",h2_MET_RefFinal_sumet_vs_njets);

  return StatusCode::SUCCESS;
}

StatusCode FakeMissingETTool::initialize() {
  msg() << MSG::DEBUG << "fakeMissingET initialize() has been called" << endreq;

  npas_jlke_50  = 0;
  npas_jlke_100 = 0;
  npas_jlke_200 = 0;

  fakemu_gt_60 = 0;
  fakemu_gt_90 = 0;
  fakemu_gt_120 = 0;
  fakemu_gt_150 = 0;

  fakefull_gt_60 = 0;
  fakefull_gt_90 = 0;
  fakefull_gt_120 = 0;
  fakefull_gt_150 = 0;

  countbadmuonevents = 0;
  numMissedMuonEvents = 0;
  failedcutsevent = 0;
  misreconevent = 0;
  fakeEvent = 0;
  noCat = 0;
  skippedevents = 0;
  skippedevents2 = 0;

  return StatusCode::SUCCESS;
}

StatusCode FakeMissingETTool::execute(MissingETData *data, MissingETMuonData *muondata) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  sc = _jetVariables->retrieveContainers();
  if ( sc.isFailure() ) {
    msg() << MSG::WARNING << "JetVariablesTool retrieveContainers() Failed" << endreq;
    return sc;
  }

  sc = fakeMissingET(data, muondata);

  return sc;
}

StatusCode FakeMissingETTool::fakeMissingET(MissingETData *data, MissingETMuonData *muondata) {

  msg() << MSG::DEBUG << "in fakeMissingET() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  const std::vector<const HepMC::GenParticle*> &truth_muons        = muondata->used_truth_muons();
  const std::vector<const Analysis::Muon*>   &spectroMuons               = muondata->used_MuonBoy_Spectro_Muons();
  const std::vector<const Analysis::Muon*>   &trackMuons                 = muondata->used_MuonBoy_Track_Muons();
  const std::vector<HepLorentzVector*> &used_muons                 = muondata->all_used_muons();

  float calculated_truth_metx = 0.;
  float calculated_truth_mety = 0.;
  for (std::vector<const HepMC::GenParticle*>::const_iterator it = truth_muons.begin(); it != truth_muons.end(); ++it ) {
    calculated_truth_metx -= (*it)->momentum().px();
    calculated_truth_mety -= (*it)->momentum().py();
  }

  float calculated_track_metx = 0.;
  float calculated_track_mety = 0.;
  for (std::vector<const Analysis::Muon*>::const_iterator it = trackMuons.begin(); it != trackMuons.end(); ++it ) {
    calculated_track_metx -= (*it)->px();
    calculated_track_mety -= (*it)->py();
  }

  float calculated_spectro_metx = 0.;
  float calculated_spectro_mety = 0.;
  for (std::vector<const Analysis::Muon*>::const_iterator it = spectroMuons.begin(); it != spectroMuons.end(); ++it ) {
    calculated_spectro_metx -= (*it)->muonSpectrometerTrackParticle()->px();
    calculated_spectro_mety -= (*it)->muonSpectrometerTrackParticle()->py();
  }

  /////////////////////////////////////////////////////////////////////
  double fake_full_x = (data->truth_nonInt()->etx()-data->refFinal()->etx())/1000.;
  double fake_full_y = (data->truth_nonInt()->ety()-data->refFinal()->ety())/1000.;
  double fake_full   = sqrt( pow(fake_full_x,2) + pow(fake_full_y,2) );
  double met_err_signed = (sqrt(pow(data->truth_nonInt()->etx(),2) + pow(data->truth_nonInt()->ety(),2))
			   - sqrt(pow(data->refFinal()->etx(),2) + pow(data->refFinal()->ety(),2)))/1000.;

  double fake_full_with_true_muon_x = (data->truth_nonInt()->etx() - (data->refFinal()->etx() - data->MuonBoy()->etx() + data->truth_muons()->etx())) / 1000.0;
  double fake_full_with_true_muon_y = (data->truth_nonInt()->ety() - (data->refFinal()->ety() - data->MuonBoy()->ety() + data->truth_muons()->ety())) / 1000.0;
  double fake_full_with_true_muon = sqrt(pow(fake_full_with_true_muon_x,2) + pow(fake_full_with_true_muon_y,2));

  double fake_muon_x = (data->truth_muons()->etx()-data->MuonBoy()->etx())/1000.;
  double fake_muon_y = (data->truth_muons()->ety()-data->MuonBoy()->ety())/1000.;
  double fake_muon   = sqrt( pow(fake_muon_x, 2) + pow(fake_muon_y,2) );

  double met_neutrino = sqrt( pow(data->truth_nonInt()->etx(), 2) + pow(data->truth_nonInt()->ety(),2) )/1000.; 

  h1_met_fake_with_mcmu->Fill(fake_full_with_true_muon);
  h1_met_fake->Fill( fake_full );
  h1_met_neut->Fill( met_neutrino );

  const JetCollection* getjets = _jetVariables->jetCollection();
  const JetCollection* getmcjets = _jetVariables->jetTruthCollection();

  JetCollection jets;
  JetCollection mcjets;

  for (JetCollection::const_iterator it = getjets->begin(); it != getjets->end(); ++it) {
    if ((*it)->et()/1000. > _minJetEtCut) {
      jets.push_back(new Jet(*it));
    }//min Et cut
  }//loop over reco jets

  //sort reco jets
  jets.sort(P4Sorters::Descending::Et());

  for (JetCollection::const_iterator it = getmcjets->begin(); it != getmcjets->end(); ++it) {
    if ((*it)->et()/1000. > _minJetEtCut) {
      mcjets.push_back(new Jet(*it));
    }//min Et cut
  }//loop over mc jets

  //sort mc jets
  mcjets.sort(P4Sorters::Descending::Et());

  h1_n_reco_jets->Fill(jets.size());

  for(JetCollection::const_iterator jetItr = jets.begin(); jetItr != jets.end(); ++jetItr) {
    h1_reco_jets_et->Fill((*jetItr)->et()/1000.);
  }

  //leading truth jet info
  double leadTruthJetEt = -999.;
  double leadTruthJetEta = -999.;
  double leadTruthJetPhi = -999.;

  //leading reco jet info
  double leadRecoJetEt = -999.;
  double leadRecoJetEta = -999.;
  double leadRecoJetPhi = -999.;
  //second reco jet info
  double secondRecoJetEt = -999.;
  double secondRecoJetEta = -999.;
  double secondRecoJetPhi = -999.;

  //loop over reco jets
  if (mcjets.size() > 0) {
    leadTruthJetEt  = mcjets.at(0)->et()/1000.;
    leadTruthJetEta = mcjets.at(0)->eta();
    leadTruthJetPhi = mcjets.at(0)->phi();
  }
  if (jets.size() > 0) {
    leadRecoJetEt  = jets.at(0)->et()/1000.;
    leadRecoJetEta = jets.at(0)->eta();
    leadRecoJetPhi = jets.at(0)->phi();
  }
  if (jets.size() > 1) {
    secondRecoJetEt  = jets.at(1)->et()/1000.;
    secondRecoJetEta = jets.at(1)->eta();
    secondRecoJetPhi = jets.at(1)->phi();
  }

  // Total fake MET vs. fake MET from muon
  h2_met_fakemuon_vs_fakefull->Fill( fake_muon_x, fake_full_x );
  h2_met_fakemuon_vs_fakefull->Fill( fake_muon_y, fake_full_y );

  if (fake_full > 60.) ++fakefull_gt_60;
  if (fake_full > 90.) ++fakefull_gt_90;
  if (fake_full > 120.) ++fakefull_gt_120;
  if (fake_full > 150.) ++fakefull_gt_150;

  if ( (fake_muon > fake_full/2.)  && fake_muon > 60. )  ++fakemu_gt_60;
  if ( (fake_muon > fake_full/2.)  && fake_muon > 90. )  ++fakemu_gt_90;
  if ( (fake_muon > fake_full/2.)  && fake_muon > 120. )  ++fakemu_gt_120;
  if ( (fake_muon > fake_full/2.)  && fake_muon > 150. )  ++fakemu_gt_150;
  //fake from jets
  if ( (fake_muon <= fake_full/2.)  && fake_full > 50. )  ++npas_jlke_50;
  if ( (fake_muon <= fake_full/2.)  && fake_full > 100. ) ++npas_jlke_100;
  if ( (fake_muon <= fake_full/2.)  && fake_full > 200. ) ++npas_jlke_200;

  double dphi_lead_etmiss_mc = fabs(leadTruthJetPhi - data->truth_nonInt()->phi());
  if (dphi_lead_etmiss_mc > M_PI) dphi_lead_etmiss_mc = fabs(dphi_lead_etmiss_mc - 2*M_PI);

  double dphi_lead_etmiss = fabs(leadRecoJetPhi - data->refFinal()->phi());
  if (dphi_lead_etmiss > M_PI) dphi_lead_etmiss = fabs(dphi_lead_etmiss - 2*M_PI);

  double dphi_second_etmiss = fabs(secondRecoJetPhi - data->refFinal()->phi());
  if (dphi_second_etmiss > M_PI) dphi_second_etmiss = fabs(dphi_second_etmiss - 2*M_PI);

  h1_dphi_leadjet_etmiss_mc->Fill(dphi_lead_etmiss_mc);
  h1_dphi_leadjet_etmiss_mc_V_met_neut->Fill(dphi_lead_etmiss_mc, met_neutrino);

  if (dphi_lead_etmiss_mc > 0.3 && dphi_lead_etmiss_mc < 2.8) {
    h1_dphicut_leadjet_met_neut_mc->Fill( met_neutrino );
  }

  h1_dphi_leadjet_etmiss->Fill(dphi_lead_etmiss);
  h1_dphi_secondjet_etmiss->Fill(dphi_second_etmiss);

  h2_MET_Topo_vs_evt_emfrac->Fill(_jetVariables->JetPtWeightedEventEMfraction(),data->topo()->et()/1000.0);
  h2_MET_RefFinal_vs_evt_emfrac->Fill(_jetVariables->JetPtWeightedEventEMfraction(),data->refFinal()->et()/1000.0);

  //event em frac cut
  if (fabs(_jetVariables->JetPtWeightedEventEMfraction()) > 0.1) {
    h1_emfcut_MET_Topo_met->Fill(data->topo()->et()/1000.0);
    h1_emfcut_MET_Topo_x->Fill(data->topo()->etx()/1000.0);
    h1_emfcut_MET_Topo_y->Fill(data->topo()->ety()/1000.0);
    h1_emfcut_MET_Topo_sumet->Fill(data->topo()->sumet()/1000.0);

    h1_emfcut_MET_RefFinal_met->Fill(data->refFinal()->et()/1000.0);
    h1_emfcut_MET_RefFinal_x->Fill(data->refFinal()->etx()/1000.0);
    h1_emfcut_MET_RefFinal_y->Fill(data->refFinal()->ety()/1000.0);
    h1_emfcut_MET_RefFinal_sumet->Fill(data->refFinal()->sumet()/1000.0);
  }

  //MET plots versus n jets
  h2_MET_Topo_met_vs_njets->Fill(jets.size(),data->topo()->et()/1000.0);
  h2_MET_Topo_x_vs_njets->Fill(jets.size(),data->topo()->etx()/1000.0);
  h2_MET_Topo_y_vs_njets->Fill(jets.size(),data->topo()->ety()/1000.0);
  h2_MET_Topo_sumet_vs_njets->Fill(jets.size(),data->topo()->sumet()/1000.0);

  h2_MET_RefFinal_met_vs_njets->Fill(jets.size(),data->refFinal()->et()/1000.0);
  h2_MET_RefFinal_x_vs_njets->Fill(jets.size(),data->refFinal()->etx()/1000.0);
  h2_MET_RefFinal_y_vs_njets->Fill(jets.size(),data->refFinal()->ety()/1000.0);
  h2_MET_RefFinal_sumet_vs_njets->Fill(jets.size(),data->refFinal()->sumet()/1000.0);

  // Dominant fake MET from jets
  if ( (fake_muon < fake_full/2.) && (fake_full > _fakeFullCut) ) { // Dominant fake MET from jets
    h2_met_fakemuon_vs_fakefull_jets->Fill( fake_muon_x, fake_full_x );
    h2_met_fakemuon_vs_fakefull_jets->Fill( fake_muon_y, fake_full_y );

    h1_met_fake_jets->Fill( fake_full );
    h1_met_fake_jets_xy->Fill( fake_full_x );
    h1_met_fake_jets_xy->Fill( fake_full_y );
    // When jets point to crack
    bool incrack = false;
    for ( unsigned int j = 0; j < mcjets.size(); ++j ) {
      double eta = mcjets.at(j)->eta();
      double et  = mcjets.at(j)->et()/1000.;
      if ( et > 50. && ( abs(eta) > 1.3 && abs(eta) < 1.6 ) ) incrack = true;
      if ( et > 50. && ( abs(eta) > 3.1 && abs(eta) < 3.3 ) ) incrack = true;
    }
    if ( incrack == true ) {
      h1_met_fake_jets_incrack->Fill( fake_full );
      h1_met_fake_jets_incrack_xy->Fill( fake_full_x );
      h1_met_fake_jets_incrack_xy->Fill( fake_full_y );
    } 
    if ( incrack == false ) {
      h1_met_fake_jets_outcrack->Fill( fake_full );
      h1_met_fake_jets_outcrack_xy->Fill( fake_full_x );
      h1_met_fake_jets_outcrack_xy->Fill( fake_full_y );
    }

    //properties of worst measured jet
    double maxdEt = 0.;
    double maxdEx = 0.;
    double maxdEy = 0.;
    double maxEta = -10.;
    double maxPhi = 0.;
    //properties of second-worst measured jet
    double seconddEt = 0.;
    double seconddEx = 0.;
    double seconddEy = 0.;
    double secondEta = 0.;
    double secondPhi = 0.;

    //loop over truth jets
    for (JetCollection::iterator it = mcjets.begin(); it != mcjets.end(); ++it) {

      //loop over reco jets and look for a match
      for (JetCollection::iterator it_rec = jets.begin(); it_rec != jets.end(); ++it_rec) {
        double deta = (*it)->eta() - (*it_rec)->eta();
        double dphi = fabs((*it)->phi() - (*it_rec)->phi());

        if (dphi > M_PI) dphi = fabs(dphi - 2*M_PI);
        double dR = sqrt(deta*deta + dphi*dphi);
        if (dR < 0.1) {
          //now want to check the Et diff between matched truth and reco jets
          double dEt = ((*it)->et()/1000. - (*it_rec)->et()/1000.);
          double dEx = ((*it)->px() - (*it_rec)->px());
          double dEy = ((*it)->py() - (*it_rec)->py());

          if (fabs(dEt) > fabs(maxdEt)) {
            //old worst error becomes second-largest
            seconddEt = maxdEt;
            seconddEx = maxdEx;
            seconddEy = maxdEy;
            secondEta = maxEta;
            secondPhi = maxPhi;
            //set worst error jet
            maxdEt = dEt;
            maxdEx = dEx;
            maxdEy = dEy;
            maxEta = (*it)->eta();
            maxPhi = (*it)->phi();
          }
          //if this jet has error between old worst and second-worst, reset the second worst
          else if (fabs(dEt) > fabs(seconddEt)) {
            seconddEt = dEt;
            seconddEx = dEx;
            seconddEy = dEy;
            secondEta = (*it)->eta();
            secondPhi = (*it)->phi();
          }//else set second worst jet variables
        }//match with dR
      }//loop over reco jets
    }//loop over truth jets

    h1_n_reco_jets_fakemet->Fill(jets.size());

    for(JetCollection::const_iterator jetItr = jets.begin(); jetItr != jets.end(); ++jetItr) {
      h1_reco_jets_et_fakemet->Fill((*jetItr)->et()/1000.);
    }

    h2_leading_reco_jet_abseta_V_fakemet->Fill(fabs(leadRecoJetEta), fake_full);

    h1_leading_reco_jet_Et->Fill(leadRecoJetEt);
    h1_leading_reco_jet_abseta->Fill(fabs(leadRecoJetEta));
    h1_leading_reco_jet_eta->Fill(leadRecoJetEta);
    h1_leading_truth_jet_Et->Fill(leadTruthJetEt);
    h1_leading_truth_jet_abseta->Fill(fabs(leadTruthJetEta));
    h1_leading_truth_jet_eta->Fill(leadTruthJetEta);

    h1_second_reco_jet_Et->Fill(secondRecoJetEt);
    h1_second_reco_jet_abseta->Fill(fabs(secondRecoJetEta));
    h1_second_reco_jet_eta->Fill(secondRecoJetEta);

    h1_met_worst_jet_maxdEt->Fill(maxdEt);
    h1_met_worst_jet_eta->Fill(maxEta);
    h1_met_worst_jet_abseta->Fill(fabs(maxEta));
    h1_met_worst_jet_phi->Fill(maxPhi);
    h2_met_worst_jet_eta_vs_phi->Fill(maxEta,maxPhi);
    h2_met_worst_jet_dMET_vs_eta->Fill(maxEta,met_err_signed);

    h1_met_secondworst_jet_eta->Fill(secondEta);
    h1_met_secondworst_jet_abseta->Fill(fabs(secondEta));
    h1_met_secondworst_jet_phi->Fill(secondPhi);
    h2_met_secondworst_jet_eta_vs_phi->Fill(secondEta,secondPhi);

    //what fraction  of MET comes from worst jet?  second worst jet?
    //-1 times jet for the direction of MET
    h1_met_worst_jet_div_met_xy->Fill(-1*maxdEx/fake_full_x);
    h1_met_worst_jet_div_met_xy->Fill(-1*maxdEy/fake_full_y);
    h1_met_secondworst_jet_div_met_xy->Fill(-1*seconddEx/fake_full_x);
    h1_met_secondworst_jet_div_met_xy->Fill(-1*seconddEy/fake_full_y);

    h1_met_worst_jet_div_met->Fill(sqrt(pow((maxdEx/fake_full_x),2) + pow((maxdEy/fake_full_y),2)));
    h1_met_secondworst_jet_div_met->Fill(sqrt(pow((seconddEx/fake_full_x),2) + pow((seconddEy/fake_full_y),2)));

    double dphi_worst_etmiss = fabs(maxPhi - data->refFinal()->phi());
    if (dphi_worst_etmiss > M_PI) dphi_worst_etmiss = fabs(dphi_worst_etmiss - 2*M_PI);

    h1_dphi_worstjet_etmiss->Fill(dphi_worst_etmiss);
    h1_fake_dphi_leadjet_etmiss->Fill(dphi_lead_etmiss);
    h1_dphi_leadjet_etmiss_V_met_fake->Fill(dphi_lead_etmiss,fake_full);

    h1_fake_dphi_secondjet_etmiss->Fill(dphi_second_etmiss);
    h1_dphi_secondjet_etmiss_V_met_fake->Fill(dphi_second_etmiss,fake_full);

    if (dphi_lead_etmiss_mc > 0.3 && dphi_lead_etmiss_mc < 2.8) {
      h1_lead_dphicut_met_fake_mc->Fill( fake_full );
      h1_lead_dphicut_met_neut->Fill( met_neutrino );
    }

    if (dphi_lead_etmiss > 0.3 && dphi_lead_etmiss < 2.8) {
      h1_lead_dphicut_met_fake->Fill( fake_full );
    }

    if (dphi_worst_etmiss > 0.3 && dphi_worst_etmiss < 2.8) {
      h1_dphicut_met_fake_with_mcmu->Fill(fake_full_with_true_muon);
      h1_dphicut_met_fake->Fill( fake_full );
      h1_dphicut_met_neut->Fill( met_neutrino );

      double dphi_sec_worst_etmiss = fabs(secondPhi - data->refFinal()->phi());

      if (dphi_sec_worst_etmiss > M_PI) dphi_sec_worst_etmiss = fabs(dphi_sec_worst_etmiss - 2*M_PI);
      h1_dphi_sec_worstjet_etmiss->Fill(dphi_sec_worst_etmiss);

      if (dphi_sec_worst_etmiss > 0.3 && dphi_sec_worst_etmiss < 2.8) {
        h1_dphiseccut_met_fake_with_mcmu->Fill(fake_full_with_true_muon);
        h1_dphiseccut_met_fake->Fill( fake_full );
        h1_dphiseccut_met_neut->Fill( met_neutrino );
      }
    }//large MET error
  }//dominant fake from MET from jets

  ///////////////////////////////////////////////////////
  //In this section we separate the events with 
  //a bad MET_Muon measurement into different categories.
  //The categories of events are:
  // 1) Event contains a real muon (seen as muon MC truth 
  //    particle) which is not reconstructed.
  // 2) Event contains a reconstructed muon which fails
  //    the quality cuts imposed by the MET reconstruction
  //    algorithm and so is not included in the MET sum.
  // 3) Event contains a real muon which has it's 
  //    momentum measured very poorly.
  // 4) Event contains a reconstructed muon which does
  //    not correspond to any real (MC truth) muon - a 
  //    "fake" muon.
  ///////////////////////////////////////////////////////
  //First calculate muon met term from muons
  //Only use events that have the same result reported by the met (sanity check)
  //Skip the event if the sum of muons doesn't equal the met muon term
  bool skipevent = false;
  if ((fabs((calculated_track_metx + calculated_spectro_metx) - data->MuonBoy()->etx()) > 1000.0) || 
      (fabs((calculated_track_mety + calculated_spectro_mety) - data->MuonBoy()->ety()) > 1000.0)) {
    skipevent=true;
  }
  //skip event if sum of mc muons doesn't equal met muon truth term 
  bool skipevent2 = false;
  if ((fabs(calculated_truth_metx - data->truth_muons()->etx()) > 1000.0) || 
      (fabs(calculated_truth_mety - data->truth_muons()->ety()) > 1000.0)) {
    skipevent2=true;
  }
  //
  if (fake_muon > fake_full/2. && fake_muon > _fakeMuonCut) {
    h2_met_fakemuon_vs_fakefull_muon->Fill( fake_muon_x, fake_full_x );
    h2_met_fakemuon_vs_fakefull_muon->Fill( fake_muon_y, fake_full_y );
    h1_met_fake_muon->Fill( fake_full );
    h1_met_fake_muon_xy->Fill( fake_full_x );
    h1_met_fake_muon_xy->Fill( fake_full_y );

    countbadmuonevents++;
    if (skipevent)   {skippedevents++;}
    if (skipevent2)  {skippedevents2++;}
    if (!skipevent && !skipevent2) {
      //flag all kinds of bad events
      bool missedevent = false;
      bool failedcuts = false;
      bool misrecon = false;
      bool fakeevent = false;

      //Loop over truth muons
      for (std::vector<const HepMC::GenParticle*>::const_iterator it = truth_muons.begin(); it != truth_muons.end(); ++it ) {

	if ((*it)->momentum().perp()/1000. > 10.0) {
	  //try to find a matching reco muon
	  bool foundone = false;

	  for (std::vector<HepLorentzVector*>::const_iterator irec = used_muons.begin(); irec != used_muons.end(); ++irec) {
	    double deta = (*it)->momentum().eta() - (*irec)->eta();
	    double dphi = fabs((*it)->momentum().phi() - (*irec)->phi());
	    if (dphi > M_PI) dphi = fabs(dphi - 2*M_PI);
	    double dR = sqrt(deta*deta + dphi*dphi);

	    //find a matching muon with dR < 0.1
	    if (dR < 0.1) {
	      foundone = true;
	      double pxdiff = fabs((*irec)->px()/1000.0 - (*it)->momentum().px()/1000.0);
	      double pydiff = fabs((*irec)->py()/1000.0 - (*it)->momentum().py()/1000.0);
	      double mag_muon_diff = sqrt(pxdiff*pxdiff + pydiff*pydiff);

	      h1_mag_muon_diff->Fill(mag_muon_diff);
	      h2_mag_muon_diff_V_truth_pt->Fill((*it)->momentum().perp()/1000.0,mag_muon_diff);
	      //we call it badly mismeasued if the error vector
	      //has magnitude > 40.0 GeV
	      if (mag_muon_diff > 40.0 ) {
		//this event has a misreconstructed muon
		misrecon = true;
	      }
	    }//dR<0.01
	  }//loop over muonboy reco muons

	  //if we have a mc muon with no matching reco muoon
	  if (!foundone) {
	    h1_missedmuonpt->Fill((*it)->momentum().perp()/1000.0);
	    if ((*it)->momentum().perp()/1000.0 > 40.0) {
	      h1_eta_muons_missed_events->Fill((*it)->momentum().eta());
	      h1_phi_muons_missed_events->Fill((*it)->momentum().phi());
	      h2_eta_v_phi_muons_missed_events->Fill((*it)->momentum().eta(), (*it)->momentum().phi());
	      //flag event with a truth muon which is not reconstructed
	      missedevent = true;
	    }//mc muon pt > 40
	  }//no matching reco muon
	}//if muon mc pt > 10
      }//loop over mc muons

      for (std::vector<HepLorentzVector*>::const_iterator irec = used_muons.begin(); irec != used_muons.end(); ++irec) {
	bool foundmatch=false;
	if ((*irec)->perp()/1000.0 < 40.0) {
	  continue;
	}
	for (std::vector<const HepMC::GenParticle*>::const_iterator it = truth_muons.begin(); it != truth_muons.end(); ++it ) {
	  if ((*it)->momentum().perp()/1000.0 < 10.0) {
	    continue;
	  }
	  double deta = (*it)->momentum().eta() - (*irec)->eta();
	  double dphi = fabs((*it)->momentum().phi() - (*irec)->phi());
	  if (dphi > M_PI) dphi = fabs(dphi - 2*M_PI);
	  double dR = sqrt(deta*deta + dphi*dphi);
	  if (dR < 0.1) {
	    foundmatch=true;
	  }
	}//loop over mc muons
	if (!foundmatch) {
	  fakeevent=true;
	}//fake event
      }//end loop over muonboy reco muons

      if (missedevent) {numMissedMuonEvents++;}
      if (failedcuts) {failedcutsevent++;}
      if (misrecon) {misreconevent++;}
      if (fakeevent) {fakeEvent++;}
      if (!missedevent && !failedcuts && !misrecon && !fakeevent) {noCat++;}

      //look at met error distributions (fake_muon and fake_full) for 
      //the two more general sources of met error (no reco muon and bad muon reco)
      if (missedevent || failedcuts) {
	h1_bad_muon_reco_algo_fake_muon->Fill(fake_muon);
	h1_bad_muon_reco_algo_fake_full->Fill(fake_full);
      }
      if (misrecon || fakeevent) {
	h1_bad_muon_reco_mis_fake_muon->Fill(fake_muon);
	h1_bad_muon_reco_mis_fake_full->Fill(fake_full);
      }

    }//fakemuon > _fakeMuonCut
  }//fake_muon > fake_full/2.

  return sc;
}

//------------------------------------------------------------------------------
StatusCode FakeMissingETTool::finalize() {
  msg() << MSG::DEBUG << "finalize() has been called" << endreq;

  msg() << MSG::INFO << " jets_like_50 = "  << npas_jlke_50
	<< " jets_like_100 = " << npas_jlke_100
	<< " jets_like_200 = " << npas_jlke_200
	<< endreq;

  msg() << MSG::INFO << "fake muon > 60: " << fakemu_gt_60 << endreq;
  msg() << MSG::INFO << "fake muon > 90: " << fakemu_gt_90 << endreq;
  msg() << MSG::INFO << "fake muon > 120: " << fakemu_gt_120 << endreq;
  msg() << MSG::INFO << "fake muon > 150: " << fakemu_gt_150 << endreq;

  msg() << MSG::INFO << "fake full > 60: " << fakefull_gt_60 << endreq;
  msg() << MSG::INFO << "fake full > 90: " << fakefull_gt_90 << endreq;
  msg() << MSG::INFO << "fake full > 120: " << fakefull_gt_120 << endreq;
  msg() << MSG::INFO << "fake full > 150: " << fakefull_gt_150 << endreq;

  msg() << MSG::INFO << "Total number of events with fake muon MET > 60 GeV:                    " << countbadmuonevents << endreq;
  msg() << MSG::INFO << "Of these, number of events with a missed truth muon:                   " << numMissedMuonEvents << endreq;
  msg() << MSG::INFO << "          number with a reconstructed muon that failed MET algo cuts:  " << failedcutsevent << endreq;
  msg() << MSG::INFO << "          number with a very badly mismeasured muon:                   " << misreconevent << endreq;
  msg() << MSG::INFO << "          number with a fake muon:                                     " << fakeEvent << endreq;
  msg() << MSG::INFO << "          skipped events (inconsistent reco information in AOD)        " << skippedevents << endreq;
  msg() << MSG::INFO << "          skipped events (inconsistent truth information in AOD)       " << skippedevents2 << endreq;
  msg() << MSG::INFO << "          no category                                                  " << noCat << endreq;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
FakeMissingETTool::~FakeMissingETTool() {}
