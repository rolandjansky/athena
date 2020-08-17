/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

//#include "CLHEP/Units/PhysicalConstants.h"
#include "TLorentzVector.h"
#include <vector>
#include "MissingETPerformance/FakeMissingETTool.h"

//#include "JetUtils/JetSorters.h"
#include "FourMomUtils/P4Sorters.h"
#include "AthenaKernel/Units.h"


using namespace std;
using CLHEP::HepLorentzVector;
using Athena::Units::GeV;
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
    m_jetVariables ("JetVariablesTool") {

  declareInterface<FakeMissingETTool>( this );

  declareProperty("minJetEtCut", m_minJetEtCut   = 10.); // GeV
  declareProperty("fakeFullCut", m_fakeFullCut   = 100.); // GeV
  declareProperty("fakeMuonCut", m_fakeMuonCut   = 100.); // GeV

  declareProperty("nbinsMET", m_nbinsMET   = 200);
  declareProperty("lowerbinMET", m_lowerbinMET   = 0.);
  declareProperty("upperbinMET", m_upperbinMET   = 200.);

  declareProperty("nbinsMETxy", m_nbinsMETxy   = 800);
  declareProperty("lowerbinMETxy", m_lowerbinMETxy   = -400.);
  declareProperty("upperbinMETxy", m_upperbinMETxy   = 400.);

  declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode FakeMissingETTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Can't get THistSvc" << endmsg;
    return sc;
  }

  sc = m_jetVariables.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Can't get handle on JetVariablesTool in BasicPlotsTool" << endmsg;
    return sc;
  }

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endmsg;
     return sc;
  }
  m_h1_met_neut  = new TH1D( "h1_met_neut" ,"True Missing ET", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/h1_met_neut", m_h1_met_neut);
  m_h1_met_fake  = new TH1D( "h1_met_fake" ,"Fake Missing ET (True - Reco, vector difference)", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/h1_met_fake", m_h1_met_fake);

  //Muons
  m_h2_met_fakemuon_vs_fakefull       = new TH2D( "h2_met_fakemuon_vs_fakefull" ,"h2_met_fakemuon_vs_fakefull", m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy,m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h2_met_fakemuon_vs_fakefull", m_h2_met_fakemuon_vs_fakefull);
  m_h1_met_fake_with_mcmu = new TH1D("h1_met_fake_with_mcmu","h1_met_fake_with_mcmu", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_met_fake_with_mcmu", m_h1_met_fake_with_mcmu);
  m_h2_met_fakemuon_vs_fakefull_muon  = new TH2D( "h2_met_fakemuon_vs_fakefull_muon" ,"h2_met_fakemuon_vs_fakefull_muon", m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy,m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h2_met_fakemuon_vs_fakefull_muon", m_h2_met_fakemuon_vs_fakefull_muon);
  m_h1_met_fake_muon    = new TH1D( "h1_met_fake_muon" ,"h1_met_fake_muon", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_met_fake_muon", m_h1_met_fake_muon);
  m_h1_met_fake_muon_xy = new TH1D( "h1_met_fake_muon_xy" ,"h1_met_fake_muon_xy", m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_met_fake_muon_xy", m_h1_met_fake_muon_xy);
  //
  m_h1_missedmuonpt = new TH1D("h1_missedmuonpt","h1_missedmuonpt",500,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_missedmuonpt", m_h1_missedmuonpt);
  m_h1_eta_muons_missed_events = new TH1D("h1_eta_muons_missed_events","h1_eta_muons_missed_events",800,-4.,4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_eta_muons_missed_events", m_h1_eta_muons_missed_events);
  m_h1_phi_muons_missed_events = new TH1D("h1_phi_muons_missed_events","h1_phi_muons_missed_events",63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_phi_muons_missed_events", m_h1_phi_muons_missed_events);
  m_h2_eta_v_phi_muons_missed_events = new TH2D("h2_eta_v_phi_muons_missed_events","h2_eta_v_phi_muons_missed_events", 80, -4., 4., 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h2_eta_v_phi_muons_missed_events", m_h2_eta_v_phi_muons_missed_events);

  m_h1_bad_muon_reco_algo_fake_muon = new TH1D("h1_bad_muon_reco_algo_fake_muon","h1_bad_muon_reco_algo_fake_muon", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_bad_muon_reco_algo_fake_muon", m_h1_bad_muon_reco_algo_fake_muon);
  m_h1_bad_muon_reco_algo_fake_full = new TH1D("h1_bad_muon_reco_algo_fake_full","h1_bad_muon_reco_algo_fake_full", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_bad_muon_reco_algo_fake_full", m_h1_bad_muon_reco_algo_fake_full);
  m_h1_bad_muon_reco_mis_fake_muon = new TH1D("h1_bad_muon_reco_mis_fake_muon","h1_bad_muon_reco_mis_fake_muon", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_bad_muon_reco_mis_fake_muon", m_h1_bad_muon_reco_mis_fake_muon);
  m_h1_bad_muon_reco_mis_fake_full = new TH1D("h1_bad_muon_reco_mis_fake_full","h1_bad_muon_reco_mis_fake_full", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_bad_muon_reco_mis_fake_full", m_h1_bad_muon_reco_mis_fake_full);

  m_h1_mag_muon_diff = new TH1D("h1_mag_muon_diff","h1_mag_muon_diff",500,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h1_mag_muon_diff", m_h1_mag_muon_diff);

  m_h2_mag_muon_diff_V_truth_pt = new TH2D("h2_mag_muon_diff_V_truth_pt","h2_mag_muon_diff_V_truth_pt",300,0.,300.,500,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Muons/h2_mag_muon_diff_V_truth_pt",m_h2_mag_muon_diff_V_truth_pt);

  //
  //
  //Jets
  //
  m_h1_dphi_leadjet_etmiss_mc = new TH1D("h1_dphi_leadjet_etmiss_mc","h1_dphi_leadjet_etmiss_mc", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_leadjet_etmiss_mc", m_h1_dphi_leadjet_etmiss_mc);

  m_h1_dphi_leadjet_etmiss = new TH1D("h1_dphi_leadjet_etmiss","dphi(leadjet,etmiss)", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_leadjet_etmiss", m_h1_dphi_leadjet_etmiss);

  m_h1_dphi_secondjet_etmiss = new TH1D("h1_dphi_secondjet_etmiss","dphi(secondjet,etmiss)", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_secondjet_etmiss", m_h1_dphi_secondjet_etmiss);

  m_h1_dphi_leadjet_etmiss_mc_V_met_neut = new TH2D("h1_dphi_leadjet_etmiss_mc_V_met_neut","h1_dphi_leadjet_etmiss_mc_V_met_neut",315,0,3.15,73,0,1460);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_leadjet_etmiss_mc_V_met_neut", m_h1_dphi_leadjet_etmiss_mc_V_met_neut);

  m_h1_n_reco_jets                    = new TH1D("h1_n_reco_jets","all events, num reco jets",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_n_reco_jets", m_h1_n_reco_jets);

  m_h1_reco_jets_et                   = new TH1D("h1_reco_jets_et","all events, reco jets Et",500,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_reco_jets_et", m_h1_reco_jets_et);

  m_h1_n_reco_jets_fakemet            = new TH1D("h1_n_reco_jets_fakemet","fake events, num reco jets",50,0,50);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_n_reco_jets_fakemet", m_h1_n_reco_jets_fakemet);

  m_h1_reco_jets_et_fakemet           = new TH1D("h1_reco_jets_et_fakemet","fake events, reco jets Et",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_reco_jets_et_fakemet", m_h1_reco_jets_et_fakemet);

  m_h2_met_fakemuon_vs_fakefull_jets  = new TH2D( "h2_met_fakemuon_vs_fakefull_jets" ,"h2_met_fakemuon_vs_fakefull_jets", m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy,m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_met_fakemuon_vs_fakefull_jets", m_h2_met_fakemuon_vs_fakefull_jets);

  m_h1_fake_dphi_leadjet_etmiss = new TH1D("h1_fake_dphi_leadjet_etmiss","h1_fake_dphi_leadjet_etmiss",315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/fake_dphi_leadjet_etmiss", m_h1_fake_dphi_leadjet_etmiss);

  m_h1_fake_dphi_secondjet_etmiss = new TH1D("h1_fake_dphi_secondjet_etmiss","h1_fake_dphi_secondjet_etmiss",315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/fake_dphi_secondjet_etmiss", m_h1_fake_dphi_secondjet_etmiss);

  //dphicut plots
  m_h1_dphicut_leadjet_met_neut_mc = new TH1D("h1_dphicut_leadjet_met_neut_mc","h1_dphicut_leadjet_met_neut_mc",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_dphicut_leadjet_met_neut_mc", m_h1_dphicut_leadjet_met_neut_mc);
  m_h1_lead_dphicut_met_fake = new TH1D("h1_lead_dphicut_met_fake","h1_lead_dphicut_met_fake",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_lead_dphicut_met_fake", m_h1_lead_dphicut_met_fake);
  m_h1_lead_dphicut_met_fake_mc = new TH1D("h1_lead_dphicut_met_fake_mc","h1_lead_dphicut_met_fake_mc",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_lead_dphicut_met_fake_mc", m_h1_lead_dphicut_met_fake_mc);
  //
  m_h1_dphicut_met_fake_with_mcmu = new TH1D("h1_dphicut_met_fake_with_mcmu","h1_dphicut_met_fake_with_mcmu",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_dphicut_met_fake_with_mcmu", m_h1_dphicut_met_fake_with_mcmu);
  m_h1_dphicut_met_fake = new TH1D("h1_dphicut_met_fake","h1_dphicut_met_fake",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_dphicut_met_fake", m_h1_dphicut_met_fake);
  m_h1_dphicut_met_neut = new TH1D("h1_dphicut_met_neut","h1_dphicut_met_neut",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_dphicut_met_neut", m_h1_dphicut_met_neut);
  m_h1_lead_dphicut_met_neut = new TH1D("h1_lead_dphicut_met_neut","h1_lead_dphicut_met_neut",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/dphicut/h1_lead_dphicut_met_neut", m_h1_lead_dphicut_met_neut);

  m_h1_dphi_worstjet_etmiss = new TH1D("h1_dphi_worstjet_etmiss","h1_dphi_worstjet_etmiss", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_worstjet_etmiss", m_h1_dphi_worstjet_etmiss);
  m_h1_dphi_leadjet_etmiss_V_met_fake = new TH2D("h1_dphi_leadjet_etmiss_V_met_fake","h1_dphi_leadjet_etmiss_V_met_fake",315,0,3.15,m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_leadjet_etmiss_V_met_fake", m_h1_dphi_leadjet_etmiss_V_met_fake);

  m_h1_dphi_secondjet_etmiss_V_met_fake = new TH2D("h1_dphi_secondjet_etmiss_V_met_fake","h1_dphi_secondjet_etmiss_V_met_fake",315,0,3.15,m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_secondjet_etmiss_V_met_fake", m_h1_dphi_secondjet_etmiss_V_met_fake);
  //
  m_h1_dphiseccut_met_fake_with_mcmu = new TH1D("h1_dphiseccut_met_fake_with_mcmu","h1_dphiseccut_met_fake_with_mcmu",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphiseccut_met_fake_with_mcmu", m_h1_dphiseccut_met_fake_with_mcmu);
  m_h1_dphiseccut_met_fake = new TH1D("h1_dphiseccut_met_fake","h1_dphiseccut_met_fake",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphiseccut_met_fake", m_h1_dphiseccut_met_fake);
  m_h1_dphiseccut_met_neut = new TH1D("h1_dphiseccut_met_neut","h1_dphiseccut_met_neut",m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphiseccut_met_neut", m_h1_dphiseccut_met_neut);
  m_h1_dphi_sec_worstjet_etmiss = new TH1D("h1_dphi_sec_worstjet_etmiss","h1_dphi_sec_worstjet_etmiss", 315,0,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_dphi_sec_worstjet_etmiss", m_h1_dphi_sec_worstjet_etmiss);
  //
  m_h1_met_fake_jets          = new TH1D( "h1_met_fake_jets" ,"h1_met_fake_jets", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets", m_h1_met_fake_jets);
  m_h1_met_fake_jets_incrack  = new TH1D( "h1_met_fake_jets_incrack" ,"h1_met_fake_jets_incrack", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_incrack", m_h1_met_fake_jets_incrack);
  m_h1_met_fake_jets_outcrack = new TH1D( "h1_met_fake_jets_outcrack" ,"h1_met_fake_jets_outcrack", m_nbinsMET, m_lowerbinMET,m_upperbinMET);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_outcrack", m_h1_met_fake_jets_outcrack);
  //
  m_h1_met_fake_jets_xy          = new TH1D( "h1_met_fake_jets_xy" ,"h1_met_fake_jets_xy", m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_xy", m_h1_met_fake_jets_xy);
  m_h1_met_fake_jets_incrack_xy  = new TH1D( "h1_met_fake_jets_incrack_xy" ,"h1_met_fake_jets_incrack_xy", m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_incrack_xy", m_h1_met_fake_jets_incrack_xy);
  m_h1_met_fake_jets_outcrack_xy = new TH1D( "h1_met_fake_jets_outcrack_xy" ,"h1_met_fake_jets_outcrack_xy", m_nbinsMETxy, m_lowerbinMETxy,m_upperbinMETxy);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_fake_jets_outcrack_xy", m_h1_met_fake_jets_outcrack_xy);
  //
  m_h1_met_worst_jet_maxdEt = new TH1D( "h1_met_worst_jet_maxdEt" ,"h1_met_worst_jet_maxdEt", 1000, -500., 500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_maxdEt", m_h1_met_worst_jet_maxdEt);
  m_h1_met_worst_jet_eta = new TH1D( "h1_met_worst_jet_eta" ,"h1_met_worst_jet_eta", 80, -4., 4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_eta", m_h1_met_worst_jet_eta);
  m_h1_met_worst_jet_abseta = new TH1D( "h1_met_worst_jet_abseta" ,"h1_met_worst_jet_abseta", 40, 0., 4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_abseta", m_h1_met_worst_jet_abseta);
  m_h1_met_worst_jet_phi = new TH1D( "h1_met_worst_jet_phi" ,"h1_met_worst_jet_phi", 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_phi", m_h1_met_worst_jet_phi);
  m_h2_met_worst_jet_eta_vs_phi = new TH2D( "h2_met_worst_jet_eta_vs_phi" ,"h2_met_worst_jet_eta_vs_phi", 80, -4., 4., 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_met_worst_jet_eta_vs_phi", m_h2_met_worst_jet_eta_vs_phi);
  m_h2_met_worst_jet_dMET_vs_eta = new TH2D( "h2_met_worst_jet_dMET_vs_eta" ,"h2_met_worst_jet_dMET_vs_eta", 80,-4,4,100,-500,500);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_met_worst_jet_dMET_vs_eta", m_h2_met_worst_jet_dMET_vs_eta);
  //
  m_h1_met_secondworst_jet_abseta = new TH1D("h1_met_secondworst_jet_abseta","h1_met_secondworst_jet_abseta", 40, 0., 4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_abseta", m_h1_met_secondworst_jet_abseta);
  m_h1_met_secondworst_jet_eta = new TH1D("h1_met_secondworst_jet_eta","h1_met_secondworst_jet_eta", 80, -4., 4.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_eta", m_h1_met_secondworst_jet_eta);
  m_h1_met_secondworst_jet_phi = new TH1D("h1_met_secondworst_jet_phi","h1_met_secondworst_jet_phi", 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_phi", m_h1_met_secondworst_jet_phi);
  m_h2_met_secondworst_jet_eta_vs_phi = new TH2D("h2_met_secondworst_jet_eta_vs_phi","h2_met_secondworst_jet_eta_vs_phi", 80, -4., 4., 63, -3.15, 3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_met_secondworst_jet_eta_vs_phi", m_h2_met_secondworst_jet_eta_vs_phi);
  //
  m_h1_met_worst_jet_div_met = new TH1D("h1_met_worst_jet_div_met","h1_met_worst_jet_div_met",300,0.,3.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_div_met", m_h1_met_worst_jet_div_met);
  m_h1_met_secondworst_jet_div_met = new TH1D("h1_met_secondworst_jet_div_met","h1_met_secondworst_jet_div_met",300,0.,3.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_div_met", m_h1_met_secondworst_jet_div_met);
  m_h1_met_worst_jet_div_met_xy = new TH1D("h1_met_worst_jet_div_met_xy","h1_met_worst_jet_div_met_xy",200,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_worst_jet_div_met_xy", m_h1_met_worst_jet_div_met_xy);
  m_h1_met_secondworst_jet_div_met_xy = new TH1D("h1_met_secondworst_jet_div_met_xy","h1_met_secondworst_jet_div_met_xy",200,-2.,2.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_met_secondworst_jet_div_met_xy", m_h1_met_secondworst_jet_div_met_xy);
  //
  m_h1_leading_reco_jet_Et            = new TH1D( "h1_leading_reco_jet_Et","h1_leading_reco_jet_Et",500,0.,500.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_reco_jet_Et", m_h1_leading_reco_jet_Et);
  m_h1_leading_reco_jet_abseta        = new TH1D( "h1_leading_reco_jet_abseta","h1_leading_reco_jet_abseta",50,0.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_reco_jet_abseta", m_h1_leading_reco_jet_abseta);
  m_h2_leading_reco_jet_abseta_V_fakemet = new TH2D( "h2_leading_reco_jet_abseta_V_fakemet","h2_leading_reco_jet_abseta_V_fakemet",50,0.,5.,300,0.,300.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_leading_reco_jet_abseta_V_fakemet", m_h2_leading_reco_jet_abseta_V_fakemet);
  m_h1_leading_reco_jet_eta           = new TH1D( "h1_leading_reco_jet_eta","h1_leading_reco_jet_eta",1000,-5.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_reco_jet_eta", m_h1_leading_reco_jet_eta);
  m_h1_leading_truth_jet_Et           = new TH1D( "h1_leading_truth_jet_Et","h1_leading_truth_jet_Et",500,0.,500.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_truth_jet_Et", m_h1_leading_truth_jet_Et);
  m_h1_leading_truth_jet_abseta       = new TH1D( "h1_leading_truth_jet_abseta","h1_leading_truth_jet_abseta",50,0.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_truth_jet_abseta", m_h1_leading_truth_jet_abseta);
  m_h1_leading_truth_jet_eta          = new TH1D( "h1_leading_truth_jet_eta","h1_leading_truth_jet_eta",1000,-5.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_leading_truth_jet_eta", m_h1_leading_truth_jet_eta);

  m_h1_second_reco_jet_Et            = new TH1D( "h1_second_reco_jet_Et","h1_second_reco_jet_Et",500,0.,500.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_second_reco_jet_Et", m_h1_second_reco_jet_Et);
  m_h1_second_reco_jet_abseta        = new TH1D( "h1_second_reco_jet_abseta","h1_second_reco_jet_abseta",50,0.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_second_reco_jet_abseta", m_h1_second_reco_jet_abseta);
  m_h2_second_reco_jet_abseta_V_fakemet = new TH2D( "h2_second_reco_jet_abseta_V_fakemet","h2_second_reco_jet_abseta_V_fakemet",50,0.,5.,300,0.,300.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h2_second_reco_jet_abseta_V_fakemet", m_h2_second_reco_jet_abseta_V_fakemet);
  m_h1_second_reco_jet_eta           = new TH1D( "h1_second_reco_jet_eta","h1_second_reco_jet_eta",1000,-5.,5.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/h1_second_reco_jet_eta", m_h1_second_reco_jet_eta);

  m_h2_MET_Topo_vs_evt_emfrac = new TH2F("h2_MET_Topo_vs_evt_emfrac","MissingET from MET_Topo vs event em fraction",
				       400,-2.,2.,300,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_vs_evt_emfrac",m_h2_MET_Topo_vs_evt_emfrac);
  m_h2_MET_RefFinal_vs_evt_emfrac = new TH2F("h2_MET_RefFinal_vs_evt_emfrac","MissingET from MET_RefFinal vs event em fraction",
					   400,-2.,2.,300,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_vs_evt_emfrac",m_h2_MET_RefFinal_vs_evt_emfrac);

  m_h1_emfcut_MET_Topo_met = new TH1F("h1_emfcut_MET_Topo_met",
				    "Missing ET from MET_Topo (with event emf cut)",600,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_Topo_met",m_h1_emfcut_MET_Topo_met);
  m_h1_emfcut_MET_Topo_x = new TH1F("h1_emfcut_MET_Topo_x",
				  "Missing Etx from MET_Topo (with event emf cut)",500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_Topo_x",m_h1_emfcut_MET_Topo_x);
  m_h1_emfcut_MET_Topo_y = new TH1F("h1_emfcut_MET_Topo_y",
				  "Missing Ety from MET_Topo (with event emf cut)",500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_Topo_y",m_h1_emfcut_MET_Topo_y);
  m_h1_emfcut_MET_Topo_sumet = new TH1F("h1_emfcut_MET_Topo_sumet",
				      "Sumet from MET_Topo (with event emf cut)",600,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_Topo_sumet",m_h1_emfcut_MET_Topo_sumet);

  m_h1_emfcut_MET_RefFinal_met = new TH1F("h1_emfcut_MET_RefFinal_met",
					"Missing ET from MET_RefFinal (with event emf cut)",600,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_RefFinal_met",m_h1_emfcut_MET_RefFinal_met);
  m_h1_emfcut_MET_RefFinal_x = new TH1F("h1_emfcut_MET_RefFinal_x",
				      "Missing Etx from MET_RefFinal (with event emf cut)",500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_RefFinal_x",m_h1_emfcut_MET_RefFinal_x);
  m_h1_emfcut_MET_RefFinal_y = new TH1F("h1_emfcut_MET_RefFinal_y",
				      "Missing Ety from MET_RefFinal (with event emf cut)",500,-250.,250.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_RefFinal_y",m_h1_emfcut_MET_RefFinal_y);
  m_h1_emfcut_MET_RefFinal_sumet = new TH1F("h1_emfcut_MET_RefFinal_sumet",
					  "Sumet from MET_RefFinal (with event emf cut)",600,0.,300.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/emfcut_MET_RefFinal_sumet",m_h1_emfcut_MET_RefFinal_sumet);

  m_h2_MET_Topo_met_vs_njets = new TH2F("h2_MET_Topo_met_vs_njets","Missing ET from MET_Topo vs number reco jets",20,0,20,1000,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_met_vs_njets",m_h2_MET_Topo_met_vs_njets);
  m_h2_MET_Topo_x_vs_njets = new TH2F("h2_MET_Topo_x_vs_njets","Missing ETx from MET_Topo vs number reco jets",20,0,20,800,-200.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_x_vs_njets",m_h2_MET_Topo_x_vs_njets);
  m_h2_MET_Topo_y_vs_njets = new TH2F("h2_MET_Topo_y_vs_njets","Missing ETy from MET_Topo vs number reco jets",20,0,20,800,-200.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_y_vs_njets",m_h2_MET_Topo_y_vs_njets);
  m_h2_MET_Topo_sumet_vs_njets = new TH2F("h2_MET_Topo_sumet_vs_njets","Sumet from MET_Topo vs number reco jets",20,0,20,500,0.,1000.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_Topo_sumet_vs_njets",m_h2_MET_Topo_sumet_vs_njets);

  m_h2_MET_RefFinal_met_vs_njets = new TH2F("h2_MET_RefFinal_met_vs_njets","Missing ET from MET_RefFinal vs number reco jets",20,0,20,1000,0.,500.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_met_vs_njets",m_h2_MET_RefFinal_met_vs_njets);
  m_h2_MET_RefFinal_x_vs_njets = new TH2F("h2_MET_RefFinal_x_vs_njets","Missing ETx from MET_RefFinal vs number reco jets",20,0,20,800,-200.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_x_vs_njets",m_h2_MET_RefFinal_x_vs_njets);
  m_h2_MET_RefFinal_y_vs_njets = new TH2F("h2_MET_RefFinal_y_vs_njets","Missing ETy from MET_RefFinal vs number reco jets",20,0,20,800,-200.,200.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_y_vs_njets",m_h2_MET_RefFinal_y_vs_njets);
  m_h2_MET_RefFinal_sumet_vs_njets = new TH2F("h2_MET_RefFinal_sumet_vs_njets","Sumet from MET_RefFinal vs number reco jets",20,0,20,500,0.,1000.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "FakeMissingET/Jets/MET_RefFinal_sumet_vs_njets",m_h2_MET_RefFinal_sumet_vs_njets);

  return StatusCode::SUCCESS;
}

StatusCode FakeMissingETTool::initialize() {
  msg() << MSG::DEBUG << "fakeMissingET initialize() has been called" << endmsg;

  m_npas_jlke_50  = 0;
  m_npas_jlke_100 = 0;
  m_npas_jlke_200 = 0;

  m_fakemu_gt_60 = 0;
  m_fakemu_gt_90 = 0;
  m_fakemu_gt_120 = 0;
  m_fakemu_gt_150 = 0;

  m_fakefull_gt_60 = 0;
  m_fakefull_gt_90 = 0;
  m_fakefull_gt_120 = 0;
  m_fakefull_gt_150 = 0;

  m_countbadmuonevents = 0;
  m_numMissedMuonEvents = 0;
  m_failedcutsevent = 0;
  m_misreconevent = 0;
  m_fakeEvent = 0;
  m_noCat = 0;
  m_skippedevents = 0;
  m_skippedevents2 = 0;

  return StatusCode::SUCCESS;
}

StatusCode FakeMissingETTool::execute(MissingETData *data, MissingETMuonData *muondata) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endmsg;

  sc = m_jetVariables->retrieveContainers();
  if ( sc.isFailure() ) {
    msg() << MSG::WARNING << "JetVariablesTool retrieveContainers() Failed" << endmsg;
    return sc;
  }

  sc = fakeMissingET(data, muondata);

  return sc;
}

StatusCode FakeMissingETTool::fakeMissingET(MissingETData *data, MissingETMuonData *muondata) {

  msg() << MSG::DEBUG << "in fakeMissingET() " << endmsg;

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
  const double fake_full_x = (data->truth_nonInt()->etx()-data->refFinal()->etx())/GeV;
  const double fake_full_y = (data->truth_nonInt()->ety()-data->refFinal()->ety())/GeV;
  double fake_full   = sqrt( pow(fake_full_x,2) + pow(fake_full_y,2) );
  double met_err_signed = (sqrt(pow(data->truth_nonInt()->etx(),2) + pow(data->truth_nonInt()->ety(),2))
			   - sqrt(pow(data->refFinal()->etx(),2) + pow(data->refFinal()->ety(),2)))/GeV;

  double fake_full_with_true_muon_x = (data->truth_nonInt()->etx() - (data->refFinal()->etx() - data->MuonBoy()->etx() + data->truth_muons()->etx())) * 1e-3;
  double fake_full_with_true_muon_y = (data->truth_nonInt()->ety() - (data->refFinal()->ety() - data->MuonBoy()->ety() + data->truth_muons()->ety())) * 1e-3;
  double fake_full_with_true_muon = sqrt(pow(fake_full_with_true_muon_x,2) + pow(fake_full_with_true_muon_y,2));

  double fake_muon_x = (data->truth_muons()->etx()-data->MuonBoy()->etx())/GeV;
  double fake_muon_y = (data->truth_muons()->ety()-data->MuonBoy()->ety())/GeV;
  double fake_muon   = sqrt( pow(fake_muon_x, 2) + pow(fake_muon_y,2) );

  double met_neutrino = sqrt( pow(data->truth_nonInt()->etx(), 2) + pow(data->truth_nonInt()->ety(),2) )/GeV; 

  m_h1_met_fake_with_mcmu->Fill(fake_full_with_true_muon);
  m_h1_met_fake->Fill( fake_full );
  m_h1_met_neut->Fill( met_neutrino );

  const JetCollection* getjets = m_jetVariables->jetCollection();
  const JetCollection* getmcjets = m_jetVariables->jetTruthCollection();

  JetCollection jets;
  JetCollection mcjets;

  for (JetCollection::const_iterator it = getjets->begin(); it != getjets->end(); ++it) {
    if ((*it)->et() > m_minJetEtCut*GeV) {
      jets.push_back(new Jet(*it));
    }//min Et cut
  }//loop over reco jets

  //sort reco jets
  jets.sort(P4Sorters::Descending::Et());

  for (JetCollection::const_iterator it = getmcjets->begin(); it != getmcjets->end(); ++it) {
    if ((*it)->et() > m_minJetEtCut*GeV) {
      mcjets.push_back(new Jet(*it));
    }//min Et cut
  }//loop over mc jets

  //sort mc jets
  mcjets.sort(P4Sorters::Descending::Et());

  m_h1_n_reco_jets->Fill(jets.size());

  for(JetCollection::const_iterator jetItr = jets.begin(); jetItr != jets.end(); ++jetItr) {
    m_h1_reco_jets_et->Fill((*jetItr)->et()/GeV);
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
    leadTruthJetEt  = mcjets.at(0)->et()/GeV;
    leadTruthJetEta = mcjets.at(0)->eta();
    leadTruthJetPhi = mcjets.at(0)->phi();
  }
  if (jets.size() > 0) {
    leadRecoJetEt  = jets.at(0)->et()/GeV;
    leadRecoJetEta = jets.at(0)->eta();
    leadRecoJetPhi = jets.at(0)->phi();
  }
  if (jets.size() > 1) {
    secondRecoJetEt  = jets.at(1)->et()/GeV;
    secondRecoJetEta = jets.at(1)->eta();
    secondRecoJetPhi = jets.at(1)->phi();
  }

  // Total fake MET vs. fake MET from muon
  m_h2_met_fakemuon_vs_fakefull->Fill( fake_muon_x, fake_full_x );
  m_h2_met_fakemuon_vs_fakefull->Fill( fake_muon_y, fake_full_y );

  if (fake_full > 60.) ++m_fakefull_gt_60;
  if (fake_full > 90.) ++m_fakefull_gt_90;
  if (fake_full > 120.) ++m_fakefull_gt_120;
  if (fake_full > 150.) ++m_fakefull_gt_150;

  if ( (fake_muon > fake_full/2.)  && fake_muon > 60. )  ++m_fakemu_gt_60;
  if ( (fake_muon > fake_full/2.)  && fake_muon > 90. )  ++m_fakemu_gt_90;
  if ( (fake_muon > fake_full/2.)  && fake_muon > 120. )  ++m_fakemu_gt_120;
  if ( (fake_muon > fake_full/2.)  && fake_muon > 150. )  ++m_fakemu_gt_150;
  //fake from jets
  if ( (fake_muon <= fake_full/2.)  && fake_full > 50. )  ++m_npas_jlke_50;
  if ( (fake_muon <= fake_full/2.)  && fake_full > 100. ) ++m_npas_jlke_100;
  if ( (fake_muon <= fake_full/2.)  && fake_full > 200. ) ++m_npas_jlke_200;

  double dphi_lead_etmiss_mc = fabs(leadTruthJetPhi - data->truth_nonInt()->phi());
  if (dphi_lead_etmiss_mc > M_PI) dphi_lead_etmiss_mc = fabs(dphi_lead_etmiss_mc - 2*M_PI);

  double dphi_lead_etmiss = fabs(leadRecoJetPhi - data->refFinal()->phi());
  if (dphi_lead_etmiss > M_PI) dphi_lead_etmiss = fabs(dphi_lead_etmiss - 2*M_PI);

  double dphi_second_etmiss = fabs(secondRecoJetPhi - data->refFinal()->phi());
  if (dphi_second_etmiss > M_PI) dphi_second_etmiss = fabs(dphi_second_etmiss - 2*M_PI);

  m_h1_dphi_leadjet_etmiss_mc->Fill(dphi_lead_etmiss_mc);
  m_h1_dphi_leadjet_etmiss_mc_V_met_neut->Fill(dphi_lead_etmiss_mc, met_neutrino);

  if (dphi_lead_etmiss_mc > 0.3 && dphi_lead_etmiss_mc < 2.8) {
    m_h1_dphicut_leadjet_met_neut_mc->Fill( met_neutrino );
  }

  m_h1_dphi_leadjet_etmiss->Fill(dphi_lead_etmiss);
  m_h1_dphi_secondjet_etmiss->Fill(dphi_second_etmiss);

  m_h2_MET_Topo_vs_evt_emfrac->Fill(m_jetVariables->JetPtWeightedEventEMfraction(),data->topo()->et()/GeV);
  m_h2_MET_RefFinal_vs_evt_emfrac->Fill(m_jetVariables->JetPtWeightedEventEMfraction(),data->refFinal()->et()/GeV);

  //event em frac cut
  if (fabs(m_jetVariables->JetPtWeightedEventEMfraction()) > 0.1) {
    m_h1_emfcut_MET_Topo_met->Fill(data->topo()->et()/GeV);
    m_h1_emfcut_MET_Topo_x->Fill(data->topo()->etx()/GeV);
    m_h1_emfcut_MET_Topo_y->Fill(data->topo()->ety()/GeV);
    m_h1_emfcut_MET_Topo_sumet->Fill(data->topo()->sumet()/GeV);

    m_h1_emfcut_MET_RefFinal_met->Fill(data->refFinal()->et()/GeV);
    m_h1_emfcut_MET_RefFinal_x->Fill(data->refFinal()->etx()/GeV);
    m_h1_emfcut_MET_RefFinal_y->Fill(data->refFinal()->ety()/GeV);
    m_h1_emfcut_MET_RefFinal_sumet->Fill(data->refFinal()->sumet()/GeV);
  }

  //MET plots versus n jets
  m_h2_MET_Topo_met_vs_njets->Fill(jets.size(),data->topo()->et()/GeV);
  m_h2_MET_Topo_x_vs_njets->Fill(jets.size(),data->topo()->etx()/GeV);
  m_h2_MET_Topo_y_vs_njets->Fill(jets.size(),data->topo()->ety()/GeV);
  m_h2_MET_Topo_sumet_vs_njets->Fill(jets.size(),data->topo()->sumet()/GeV);

  m_h2_MET_RefFinal_met_vs_njets->Fill(jets.size(),data->refFinal()->et()/GeV);
  m_h2_MET_RefFinal_x_vs_njets->Fill(jets.size(),data->refFinal()->etx()/GeV);
  m_h2_MET_RefFinal_y_vs_njets->Fill(jets.size(),data->refFinal()->ety()/GeV);
  m_h2_MET_RefFinal_sumet_vs_njets->Fill(jets.size(),data->refFinal()->sumet()/GeV);

  // Dominant fake MET from jets
  if ( (fake_muon < fake_full/2.) && (fake_full > m_fakeFullCut) ) { // Dominant fake MET from jets
    m_h2_met_fakemuon_vs_fakefull_jets->Fill( fake_muon_x, fake_full_x );
    m_h2_met_fakemuon_vs_fakefull_jets->Fill( fake_muon_y, fake_full_y );

    m_h1_met_fake_jets->Fill( fake_full );
    m_h1_met_fake_jets_xy->Fill( fake_full_x );
    m_h1_met_fake_jets_xy->Fill( fake_full_y );
    // When jets point to crack
    bool incrack = false;
    for ( unsigned int j = 0; j < mcjets.size(); ++j ) {
      double eta = mcjets.at(j)->eta();
      double et  = mcjets.at(j)->et()/GeV;
      if ( et > 50. && ( abs(eta) > 1.3 && abs(eta) < 1.6 ) ) incrack = true;
      if ( et > 50. && ( abs(eta) > 3.1 && abs(eta) < 3.3 ) ) incrack = true;
    }
    if ( incrack == true ) {
      m_h1_met_fake_jets_incrack->Fill( fake_full );
      m_h1_met_fake_jets_incrack_xy->Fill( fake_full_x );
      m_h1_met_fake_jets_incrack_xy->Fill( fake_full_y );
    } 
    if ( incrack == false ) {
      m_h1_met_fake_jets_outcrack->Fill( fake_full );
      m_h1_met_fake_jets_outcrack_xy->Fill( fake_full_x );
      m_h1_met_fake_jets_outcrack_xy->Fill( fake_full_y );
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
          double dEt = ((*it)->et() - (*it_rec)->et())/GeV;
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

    m_h1_n_reco_jets_fakemet->Fill(jets.size());

    for(JetCollection::const_iterator jetItr = jets.begin(); jetItr != jets.end(); ++jetItr) {
      m_h1_reco_jets_et_fakemet->Fill((*jetItr)->et()/GeV);
    }

    m_h2_leading_reco_jet_abseta_V_fakemet->Fill(fabs(leadRecoJetEta), fake_full);

    m_h1_leading_reco_jet_Et->Fill(leadRecoJetEt);
    m_h1_leading_reco_jet_abseta->Fill(fabs(leadRecoJetEta));
    m_h1_leading_reco_jet_eta->Fill(leadRecoJetEta);
    m_h1_leading_truth_jet_Et->Fill(leadTruthJetEt);
    m_h1_leading_truth_jet_abseta->Fill(fabs(leadTruthJetEta));
    m_h1_leading_truth_jet_eta->Fill(leadTruthJetEta);

    m_h1_second_reco_jet_Et->Fill(secondRecoJetEt);
    m_h1_second_reco_jet_abseta->Fill(fabs(secondRecoJetEta));
    m_h1_second_reco_jet_eta->Fill(secondRecoJetEta);

    m_h1_met_worst_jet_maxdEt->Fill(maxdEt);
    m_h1_met_worst_jet_eta->Fill(maxEta);
    m_h1_met_worst_jet_abseta->Fill(fabs(maxEta));
    m_h1_met_worst_jet_phi->Fill(maxPhi);
    m_h2_met_worst_jet_eta_vs_phi->Fill(maxEta,maxPhi);
    m_h2_met_worst_jet_dMET_vs_eta->Fill(maxEta,met_err_signed);

    m_h1_met_secondworst_jet_eta->Fill(secondEta);
    m_h1_met_secondworst_jet_abseta->Fill(fabs(secondEta));
    m_h1_met_secondworst_jet_phi->Fill(secondPhi);
    m_h2_met_secondworst_jet_eta_vs_phi->Fill(secondEta,secondPhi);

    const double inv_fake_full_x = 1. / fake_full_x;
    const double inv_fake_full_y = 1. / fake_full_y;

    //what fraction  of MET comes from worst jet?  second worst jet?
    //-1 times jet for the direction of MET
    m_h1_met_worst_jet_div_met_xy->Fill(-1*maxdEx * inv_fake_full_x);
    m_h1_met_worst_jet_div_met_xy->Fill(-1*maxdEy * inv_fake_full_y);
    m_h1_met_secondworst_jet_div_met_xy->Fill(-1*seconddEx * inv_fake_full_x);
    m_h1_met_secondworst_jet_div_met_xy->Fill(-1*seconddEy * inv_fake_full_y);

    m_h1_met_worst_jet_div_met->Fill(sqrt(pow((maxdEx * inv_fake_full_x),2) + pow((maxdEy*inv_fake_full_y),2)));
    m_h1_met_secondworst_jet_div_met->Fill(sqrt(pow((seconddEx * inv_fake_full_x),2) + pow((seconddEy * inv_fake_full_y),2)));

    double dphi_worst_etmiss = fabs(maxPhi - data->refFinal()->phi());
    if (dphi_worst_etmiss > M_PI) dphi_worst_etmiss = fabs(dphi_worst_etmiss - 2*M_PI);

    m_h1_dphi_worstjet_etmiss->Fill(dphi_worst_etmiss);
    m_h1_fake_dphi_leadjet_etmiss->Fill(dphi_lead_etmiss);
    m_h1_dphi_leadjet_etmiss_V_met_fake->Fill(dphi_lead_etmiss,fake_full);

    m_h1_fake_dphi_secondjet_etmiss->Fill(dphi_second_etmiss);
    m_h1_dphi_secondjet_etmiss_V_met_fake->Fill(dphi_second_etmiss,fake_full);

    if (dphi_lead_etmiss_mc > 0.3 && dphi_lead_etmiss_mc < 2.8) {
      m_h1_lead_dphicut_met_fake_mc->Fill( fake_full );
      m_h1_lead_dphicut_met_neut->Fill( met_neutrino );
    }

    if (dphi_lead_etmiss > 0.3 && dphi_lead_etmiss < 2.8) {
      m_h1_lead_dphicut_met_fake->Fill( fake_full );
    }

    if (dphi_worst_etmiss > 0.3 && dphi_worst_etmiss < 2.8) {
      m_h1_dphicut_met_fake_with_mcmu->Fill(fake_full_with_true_muon);
      m_h1_dphicut_met_fake->Fill( fake_full );
      m_h1_dphicut_met_neut->Fill( met_neutrino );

      double dphi_sec_worst_etmiss = fabs(secondPhi - data->refFinal()->phi());

      if (dphi_sec_worst_etmiss > M_PI) dphi_sec_worst_etmiss = fabs(dphi_sec_worst_etmiss - 2*M_PI);
      m_h1_dphi_sec_worstjet_etmiss->Fill(dphi_sec_worst_etmiss);

      if (dphi_sec_worst_etmiss > 0.3 && dphi_sec_worst_etmiss < 2.8) {
        m_h1_dphiseccut_met_fake_with_mcmu->Fill(fake_full_with_true_muon);
        m_h1_dphiseccut_met_fake->Fill( fake_full );
        m_h1_dphiseccut_met_neut->Fill( met_neutrino );
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
  if (fake_muon > fake_full/2. && fake_muon > m_fakeMuonCut) {
    m_h2_met_fakemuon_vs_fakefull_muon->Fill( fake_muon_x, fake_full_x );
    m_h2_met_fakemuon_vs_fakefull_muon->Fill( fake_muon_y, fake_full_y );
    m_h1_met_fake_muon->Fill( fake_full );
    m_h1_met_fake_muon_xy->Fill( fake_full_x );
    m_h1_met_fake_muon_xy->Fill( fake_full_y );

    m_countbadmuonevents++;
    if (skipevent)   {m_skippedevents++;}
    if (skipevent2)  {m_skippedevents2++;}
    if (!skipevent && !skipevent2) {
      //flag all kinds of bad events
      bool missedevent = false;
      bool failedcuts = false;
      bool misrecon = false;
      bool fakeevent = false;

      //Loop over truth muons
      for (std::vector<const HepMC::GenParticle*>::const_iterator it = truth_muons.begin(); it != truth_muons.end(); ++it ) {

	if ((*it)->momentum().perp() > 10.0*GeV) {
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
	      double pxdiff = fabs((*irec)->px() - (*it)->momentum().px())/GeV;
	      double pydiff = fabs((*irec)->py() - (*it)->momentum().py())/GeV;
	      double mag_muon_diff = sqrt(pxdiff*pxdiff + pydiff*pydiff);

	      m_h1_mag_muon_diff->Fill(mag_muon_diff);
	      m_h2_mag_muon_diff_V_truth_pt->Fill((*it)->momentum().perp()/GeV,mag_muon_diff);
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
	    m_h1_missedmuonpt->Fill((*it)->momentum().perp()/GeV);
	    if ((*it)->momentum().perp() > 40.0*GeV) {
	      m_h1_eta_muons_missed_events->Fill((*it)->momentum().eta());
	      m_h1_phi_muons_missed_events->Fill((*it)->momentum().phi());
	      m_h2_eta_v_phi_muons_missed_events->Fill((*it)->momentum().eta(), (*it)->momentum().phi());
	      //flag event with a truth muon which is not reconstructed
	      missedevent = true;
	    }//mc muon pt > 40
	  }//no matching reco muon
	}//if muon mc pt > 10
      }//loop over mc muons

      for (std::vector<HepLorentzVector*>::const_iterator irec = used_muons.begin(); irec != used_muons.end(); ++irec) {
	bool foundmatch=false;
	if ((*irec)->perp() < 40.0*GeV) {
	  continue;
	}
	for (std::vector<const HepMC::GenParticle*>::const_iterator it = truth_muons.begin(); it != truth_muons.end(); ++it ) {
	  if ((*it)->momentum().perp() < 10.0*GeV) {
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

      if (missedevent) {m_numMissedMuonEvents++;}
      if (failedcuts) {m_failedcutsevent++;}
      if (misrecon) {m_misreconevent++;}
      if (fakeevent) {m_fakeEvent++;}
      if (!missedevent && !failedcuts && !misrecon && !fakeevent) {m_noCat++;}

      //look at met error distributions (fake_muon and fake_full) for 
      //the two more general sources of met error (no reco muon and bad muon reco)
      if (missedevent || failedcuts) {
	m_h1_bad_muon_reco_algo_fake_muon->Fill(fake_muon);
	m_h1_bad_muon_reco_algo_fake_full->Fill(fake_full);
      }
      if (misrecon || fakeevent) {
	m_h1_bad_muon_reco_mis_fake_muon->Fill(fake_muon);
	m_h1_bad_muon_reco_mis_fake_full->Fill(fake_full);
      }

    }//fakemuon > m_fakeMuonCu
  }//fake_muon > fake_full/2.

  return sc;
}

//------------------------------------------------------------------------------
StatusCode FakeMissingETTool::finalize() {
  msg() << MSG::DEBUG << "finalize() has been called" << endmsg;

  msg() << MSG::INFO << " jets_like_50 = "  << m_npas_jlke_50
	<< " jets_like_100 = " << m_npas_jlke_100
	<< " jets_like_200 = " << m_npas_jlke_200
	<< endmsg;

  msg() << MSG::INFO << "fake muon > 60: " << m_fakemu_gt_60 << endmsg;
  msg() << MSG::INFO << "fake muon > 90: " << m_fakemu_gt_90 << endmsg;
  msg() << MSG::INFO << "fake muon > 120: " << m_fakemu_gt_120 << endmsg;
  msg() << MSG::INFO << "fake muon > 150: " << m_fakemu_gt_150 << endmsg;

  msg() << MSG::INFO << "fake full > 60: " << m_fakefull_gt_60 << endmsg;
  msg() << MSG::INFO << "fake full > 90: " << m_fakefull_gt_90 << endmsg;
  msg() << MSG::INFO << "fake full > 120: " << m_fakefull_gt_120 << endmsg;
  msg() << MSG::INFO << "fake full > 150: " << m_fakefull_gt_150 << endmsg;

  msg() << MSG::INFO << "Total number of events with fake muon MET > 60 GeV:                    " << m_countbadmuonevents << endmsg;
  msg() << MSG::INFO << "Of these, number of events with a missed truth muon:                   " << m_numMissedMuonEvents << endmsg;
  msg() << MSG::INFO << "          number with a reconstructed muon that failed MET algo cuts:  " << m_failedcutsevent << endmsg;
  msg() << MSG::INFO << "          number with a very badly mismeasured muon:                   " << m_misreconevent << endmsg;
  msg() << MSG::INFO << "          number with a fake muon:                                     " << m_fakeEvent << endmsg;
  msg() << MSG::INFO << "          skipped events (inconsistent reco information in AOD)        " << m_skippedevents << endmsg;
  msg() << MSG::INFO << "          skipped events (inconsistent truth information in AOD)       " << m_skippedevents2 << endmsg;
  msg() << MSG::INFO << "          no category                                                  " << m_noCat << endmsg;

  return StatusCode::SUCCESS;
}
