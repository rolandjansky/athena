//
//   @file    ConfAnalysis.cxx         
//   
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: ConfAnalysis.cxx 800361 2017-03-12 14:33:19Z sutt $


#include "ConfAnalysis.h"


#include "ConfVtxAnalysis.h"

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/TIDAEvent.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

#include <fstream>

#include "BinConfig.h"

#include "TF1.h"
#include "TMath.h"

#include "globals.h"

bool PRINT_BRESIDUALS = false;

std::ofstream dumpfile("dumpfile.log");

void Normalise(TH1* h) { 

  for ( int i=1 ; i<=h->GetNbinsX() ; i++ ) {

    double del = h->GetBinLowEdge(i+1)-h->GetBinLowEdge(i);

    h->SetBinContent( i, h->GetBinContent(i)/del );
    h->SetBinError( i, h->GetBinError(i)/del );
  }

}




BinConfig binConfig("standard");

BinConfig electronBinConfig("electron");
BinConfig muonBinConfig("muon");
BinConfig tauBinConfig("tau");
BinConfig bjetBinConfig("bjet");
BinConfig cosmicBinConfig("cosmic");


/// book all the histograms

void ConfAnalysis::initialise() { 
  if ( !m_initialiseFirstEvent ) { 
    std::cout << "ConfAnalysis::initialize() " << std::endl;
    initialiseInternal();
  }
}

void ConfAnalysis::initialiseInternal() { 

  m_initialised = true;

  //  std::cout << "ConfAnalysis::initialise() " << name() << std::endl;


  BinConfig& _binConfig = binConfig;
 
  if       ( name().find("_e")!=std::string::npos   )   _binConfig = electronBinConfig;
  else if  ( name().find("_mu")!=std::string::npos  )   _binConfig =     muonBinConfig;
  else if  ( name().find("_tau")!=std::string::npos )   _binConfig =      tauBinConfig;
  else if  ( name().find("_b")!=std::string::npos   )   _binConfig =     bjetBinConfig;
  else if  ( name().find("cosmic")!=std::string::npos ) _binConfig =   cosmicBinConfig;

  
  //+++ pT ranges
  //  double tmp_maxPt    = 50000.;
  //  double tmp_absResPt = 0.0005;
  //double tmp_maxPt    = 50.;
  double tmp_absResPt = 0.5;

  const int pTResBins = int(100*_binConfig.ptres_NScale);

  //+++ Eta ranges
  double tmp_maxEta    = 3.;
  double tmp_absResEta = 0.04; // 0.0005;

  //+++ Phi ranges
  double tmp_maxPhi    = 3.142;
  double tmp_absResPhi = 0.02; // 0.0001;


  //  std::cout << "ConfAnalysis::initialise() " << name() << " config: " << _binConfig << std::endl;


  int etaBins          = int(30*_binConfig.eta_NScale);
  const int etaResBins = int(600*_binConfig.eta_NScale);

  const int phiBins    = int(30*_binConfig.phi_NScale);
  const int phiResBins = int(100*_binConfig.phires_NScale);

  const int    zBins = int(100*_binConfig.z0_NScale);
  const double zMax  = 200;

  

  const int    zresBins = 100;      
  const double zresMax  = 10;

  const int    d0Bins = int(100*_binConfig.d0_NScale);
  const double d0Max  = 30;

  const int    d0resBins = 100;      
  const double d0resMax  = 5;

  // beamspot corrected position
  const int    a0Bins = int(300*_binConfig.a0_NScale);
  const double a0Max  = 15;

  const int    a0resBins = 100;      
  const double a0resMax  = 5;

  //+++ Book histograms

  // calculate a logarithmic binning in pt

  int     Npt  = 0;
  double  pt_a = 1;
  double  pt_b = 1;
  
  //  Npt = int(40*_binConfig.pt_NScale);
  //  pt_a = 3.5;
  Npt = int(45*_binConfig.pt_NScale);
  pt_a = 4;
  pt_b = 2;
  // etaBins = 12;
  //  }
  // else { 
  //  Npt = 40;
  //  pt_a = 4;
  // pt_b = 1;
  //  }
  

  const int  ptnbins = Npt;  
  std::vector<double> ptbinlimsv(ptnbins+1);
  double*   ptbinlims = &ptbinlimsv[0];
  //  for ( int i=0 ; i<=ptnbins ; i++ ) {     ptbinlims[i] = std::pow(10, 2.0*i/ptnbins+2)/1000; }
  //  for ( int i=0 ; i<=ptnbins ; i++ ) {     ptbinlims[i] = std::pow(10, 2.3*i/ptnbins+2); }
  for ( int i=0 ; i<=ptnbins ; i++ ) ptbinlims[i] = std::pow(10, pt_a*i/ptnbins+pt_b)/1000;  
 

  // ADDED BY JK - FOR SIGNED PT PLOTS
  //-----
  const int ptnbins2 = (2*ptnbins);
  // std::cout << "ptnbins2 = " << ptnbins2 << std::endl;
  std::vector<double> ptbinlims2v(ptnbins2 + 1);
  double* ptbinlims2 = &ptbinlims2v[0];
  //  std::cout << "ptbinlims2v.size() = " << ptbinlims2v.size() << std::endl;
  int ptnbin_counter = 0;
  for ( int i=ptnbins; i>0 ; i-- ) {
    ptbinlims2[ptnbin_counter] = std::pow(10, pt_a*i/ptnbins+pt_b)/(-2000);
    // std::cout << "ptbinlims[" << i << "] = " << ptbinlims[i] << "  ,  so ptbinlims2[" << ptnbin_counter << "] = " << ptbinlims2[ptnbin_counter] << std::endl;
    ptnbin_counter++;
  }

  for ( int i=0 ; i<ptnbins+1 ; i++ ) {
    ptbinlims2[ptnbin_counter] = std::pow(10, pt_a*i/ptnbins+pt_b)/2000;
    // std::cout << "ptbinlims[" << i << "] = " << ptbinlims[i] << "  ,  so ptbinlims2[" << ptnbin_counter << "] = " << ptbinlims2[ptnbin_counter] << std::endl;
    ptnbin_counter++;
  }
  //-----

  const int  iptnbins = 20;  
  const double minmaxipt=0.5;
  std::vector<double> iptbinlimsv(iptnbins+1);
  double*   iptbinlims = &iptbinlimsv[0];
  for ( int i=0 ; i<=iptnbins ; i++ ) {
    iptbinlims[i] = -minmaxipt+i*minmaxipt*2./iptnbins;  
  }



  TDirectory* dir = gDirectory;

  //  std::cout << "ConfAnalysis::initialize() Directory " << gDirectory->GetName() << " " << name() << std::endl;

  mdir = new TIDDirectory(name());
  mdir->push();

  //  std::cout << "ConfAnalysis::initialize() Directory " << gDirectory->GetName() << " " << name() << std::endl;

  if ( !m_initialiseFirstEvent ) std::cout << "ConfAnalysis::initialize() Directory " << name() << std::endl;
  
  if ( name() != gDirectory->GetName() )  std::cerr << "ConfAnalysis::initialize() Directory: problem with directory " << gDirectory->GetName() << " " << name() << std::endl;
  
  //  TIDDirectory d("histos");
  //  d.push();

  //  std::cout << "ConfAnalysis::initialize() Directory " << gDirectory->GetName() << " package directory, " << name() << std::endl;


  mres.push_back( rnpix_eta  = new Resplot( "npix_eta",  /* 2* */ etaBins, -tmp_maxEta, tmp_maxEta,  22, -0.5, 21.5 ) );
  mres.push_back( rnsct_eta  = new Resplot( "nsct_eta",  /* 2* */ etaBins, -tmp_maxEta, tmp_maxEta,  22, -0.5, 21.5 ) );
  mres.push_back( rntrt_eta  = new Resplot( "ntrt_eta",  /* 2* */ etaBins, -tmp_maxEta, tmp_maxEta, 100, -0.5, 99.5 ) );
  mres.push_back( rnsihit_eta= new Resplot( "nsihit_eta",etaBins, -tmp_maxEta, tmp_maxEta,  22, -0.5, 21.5 ) );

  mres.push_back( rnpix_lb  = new Resplot( "npix_lb", 250, 0, 2500,  22, -0.5, 21.5 ) );

  mres.push_back(  rnpix_phi  = new Resplot( "npix_phi",  etaBins, -M_PI, M_PI,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsct_phi  = new Resplot( "nsct_phi",  etaBins, -M_PI, M_PI,  22, -0.5, 21.5 ) );
  mres.push_back(  rntrt_phi  = new Resplot( "ntrt_phi",  etaBins, -M_PI, M_PI, 100, -0.5, 99.5 ) );

  mres.push_back(  rnpix_pt = new Resplot( "npix_pt", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsct_pt = new Resplot( "nsct_pt", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rntrt_pt = new Resplot( "ntrt_pt", ptnbins, ptbinlims, 100, -0.5, 99.5 ) );

  mres.push_back(  rnpixh_pt = new Resplot( "npixh_pt", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rnscth_pt = new Resplot( "nscth_pt", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );


  mres.push_back(  rnpix_pt_bad = new Resplot( "npix_pt_bad", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsct_pt_bad = new Resplot( "nsct_pt_bad", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rntrt_pt_bad = new Resplot( "ntrt_pt_bad", ptnbins, ptbinlims, 100, -0.5, 99.5 ) );
  
  
  mres.push_back(  rnpix_eta_rec  = new Resplot( "npix_eta_rec",  /* 2* */ etaBins, -tmp_maxEta, tmp_maxEta,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsct_eta_rec  = new Resplot( "nsct_eta_rec",  /* 2* */ etaBins, -tmp_maxEta, tmp_maxEta,  22, -0.5, 21.5 ) );
  mres.push_back(  rntrt_eta_rec  = new Resplot( "ntrt_eta_rec",  /* 2* */ etaBins, -tmp_maxEta, tmp_maxEta, 100, -0.5, 99.5 ) );
  mres.push_back(  rnsihit_eta_rec= new Resplot( "nsihit_eta_rec", etaBins, -tmp_maxEta, tmp_maxEta,  22, -0.5, 21.5 ) );

  mres.push_back(  rnpix_phi_rec  = new Resplot( "npix_phi_rec",  etaBins, -M_PI, M_PI,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsct_phi_rec  = new Resplot( "nsct_phi_rec",  etaBins, -M_PI, M_PI,  22, -0.5, 21.5 ) );
  mres.push_back(  rntrt_phi_rec  = new Resplot( "ntrt_phi_rec",  etaBins, -M_PI, M_PI, 100, -0.5, 99.5 ) );

  mres.push_back(  rnpix_pt_rec = new Resplot( "npix_pt_rec", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsct_pt_rec = new Resplot( "nsct_pt_rec", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rntrt_pt_rec = new Resplot( "ntrt_pt_rec", ptnbins, ptbinlims, 100, -0.5, 99.5 ) );

  mres.push_back(  rnpixh_pt_rec = new Resplot( "npixh_pt_rec", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rnscth_pt_rec = new Resplot( "nscth_pt_rec", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );


  mres.push_back(  rChi2prob = new Resplot( "Chi2prob", ptnbins, ptbinlims,  20, 0,   1 ) );
  mres.push_back(  rChi2     = new Resplot( "Chi2",     ptnbins, ptbinlims, 200, 0, 100 ) );
  mres.push_back(  rChi2dof  = new Resplot( "Chi2dof",  ptnbins, ptbinlims, 100, 0,  10 ) );

  mres.push_back(  rChi2prob_bad = new Resplot( "Chi2prob_bad", ptnbins, ptbinlims,  20, 0,   1 ) );
  mres.push_back(  rChi2_bad     = new Resplot( "Chi2_bad",     ptnbins, ptbinlims, 200, 0, 100 ) );
  mres.push_back(  rChi2dof_bad  = new Resplot( "Chi2dof_bad",  ptnbins, ptbinlims, 100, 0,  10 ) );

  /// additional resplots for additional si hit and hold monitoring

  double d0bins[40] = { -5.0,  -4.0,  -3.0,  -2.5,
                        -2.0,  -1.8,  -1.6,  -1.4,  -1.2,
                        -1.05, -0.95, -0.85, -0.75, -0.65, -0.55, -0.45, -0.35, -0.25, -0.15, -0.05,
                         0.05,  0.15,  0.25,  0.35,  0.45,  0.55,  0.65,  0.75,  0.85,  0.95,  1.05,
                         1.2,   1.4,   1.6,   1.8,   2.0,
                         2.5,   3.0,   4.0,   5.0 };



  mres.push_back(  rnpix_d0 = new Resplot( "npix_d0", 39, d0bins,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsct_d0 = new Resplot( "nsct_d0", 39, d0bins,  22, -0.5, 21.5 ) );
  mres.push_back(  rntrt_d0 = new Resplot( "ntrt_d0", 39, d0bins,  22, -0.5, 21.5 ) );

  mres.push_back(  rnpixh_d0 = new Resplot( "npixh_d0", 39, d0bins,  22, -0.5, 21.5 ) );
  mres.push_back(  rnscth_d0 = new Resplot( "nscth_d0", 39, d0bins,  22, -0.5, 21.5 ) );

  mres.push_back(   rnsi_pt = new Resplot(  "nsi_pt", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsih_pt = new Resplot( "nsih_pt", ptnbins, ptbinlims,  22, -0.5, 21.5 ) );

  mres.push_back(   rnsi_d0 = new Resplot(  "nsi_d0", 39, d0bins,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsih_d0 = new Resplot( "nsih_d0", 39, d0bins,  22, -0.5, 21.5 ) );

  mres.push_back(   rnsi_eta = new Resplot(  "nsi_eta", etaBins, -tmp_maxEta, tmp_maxEta,  22, -0.5, 21.5 ) );
  mres.push_back(  rnsih_eta = new Resplot( "nsih_eta", etaBins, -tmp_maxEta, tmp_maxEta,  22, -0.5, 21.5 ) );

  mres.push_back(   rnbl_d0 = new Resplot(  "nbl_d0", 39, d0bins,  5, -0.5, 4.5 ) );
  mres.push_back(  rnblh_d0 = new Resplot( "nblh_d0", 39, d0bins,  5, -0.5, 4.5 ) );



  //  int Nptbins = 7;
  //  double _ptlims[8] = { 0, 500, 1000, 1500, 2000, 5000, 8000, 12000 };


  //  addHistogram( hchi2=new TH1F("chi2", "chi2", 100, 0, 20) );

  // "reference" quantities
  addHistogram(  new TH1F(  "pT",   "pT",     ptnbins,   ptbinlims ) );
  addHistogram(  new TH1F( "eta",  "eta",     etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram(  new TH1F( "phi",  "phi",     phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  addHistogram(  new TH1F(  "z0",   "z0",     2*zBins,      -8*zMax,     8*zMax ) );
  addHistogram(  new TH1F(  "d0",   "d0",      d0Bins,       -d0Max,      d0Max ) );
  addHistogram(  new TH1F(  "a0",   "a0",      a0Bins,       -a0Max,      a0Max ) );

  // error study histograms (reference)                                                                                                                           
  addHistogram(  new TH1F( "dpT",  "dpT",  80, 0, 20 ) );
  addHistogram(  new TH1F( "deta", "deta", 50, 0, 1  ) );
  addHistogram(  new TH1F( "dphi", "dphi", 50, 0, 1  ) );
  addHistogram(  new TH1F( "dz0",  "dz0", 100, 0, 2  ) );
  addHistogram(  new TH1F( "dd0",  "dd0",  50, 0, 0.5  ) );
  addHistogram(  new TH1F( "da0",  "da0",  50, 0, 0.5  ) );

  addHistogram(  new TH1F( "roi_deta", "roi_deta",  50, -1, 1  ) );
  addHistogram(  new TH1F( "roi_dphi", "roi_dphi",  50, -1, 1  ) );
  addHistogram(  new TH1F( "roi_dR",   "roi_dR",    50,  0, 1  ) );

  // efficiencies and purities
  eff_pt  = new Efficiency( find("pT"),  "pT_eff"  );
  eff_pt->Hist()->GetXaxis()->SetTitle("P_{T} [GeV]");
  eff_pt->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_eta = new Efficiency( find("eta"), "eta_eff" );
  eff_eta->Hist()->GetXaxis()->SetTitle("#eta");
  eff_eta->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_phi = new Efficiency( find("phi"), "phi_eff" );
  eff_phi->Hist()->GetXaxis()->SetTitle("#phi");
  eff_phi->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_z0  = new Efficiency( find("z0"),  "z0_eff"  );
  eff_z0->Hist()->GetXaxis()->SetTitle("z0");
  eff_z0->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_d0  = new Efficiency( find("d0"),  "d0_eff"  );
  eff_d0->Hist()->GetXaxis()->SetTitle("d0");
  eff_d0->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_a0  = new Efficiency( find("a0"),  "a0_eff"  );
  eff_a0->Hist()->GetXaxis()->SetTitle("a0");
  eff_a0->Hist()->GetYaxis()->SetTitle("Efficiency [%]");
          
  eff_ptm = new Efficiency( find("pT"), "pTm_eff" );
  eff_ptm->Hist()->GetXaxis()->SetTitle("Negative P_{T} [GeV]");
  eff_ptm->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_ptp = new Efficiency( find("pT"), "pTp_eff" );
  eff_ptp->Hist()->GetXaxis()->SetTitle("Positive P_{T} [GeV]");
  eff_ptp->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_roi_deta = new Efficiency( find("roi_deta"), "roi_deta_eff" );
  eff_roi_deta->Hist()->GetXaxis()->SetTitle("RoI #Delta#eta");
  eff_roi_deta->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_roi_dphi = new Efficiency( find("roi_dphi"), "roi_dphi_eff" );
  eff_roi_dphi->Hist()->GetXaxis()->SetTitle("RoI #Delta#phi");
  eff_roi_dphi->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  eff_roi_dR = new Efficiency( find("roi_dR"), "roi_dR_eff" );
  eff_roi_dR->Hist()->GetXaxis()->SetTitle("RoI #Delta R");
  eff_roi_dR->Hist()->GetYaxis()->SetTitle("Efficiency [%]");

  // addHistogram ( hDeltaR = new TH1F("DeltaR", "DeltaR", 100, 0, 0.1 ) );
  addHistogram ( hDeltaR = new TH1F("DeltaR", "DeltaR", 100, 0, 0.2 ) );

  purity_pt  = new Efficiency( find("pT"),  "pT_pur"  );
  purity_eta = new Efficiency( find("eta"), "eta_pur" );
  purity_phi = new Efficiency( find("phi"), "phi_pur" );
  purity_z0  = new Efficiency( find("z0"),  "z0_pur"  );
  purity_d0  = new Efficiency( find("d0"),  "d0_pur"  );
  purity_a0  = new Efficiency( find("a0"),  "a0_pur"  );

  // "test" quantities
  addHistogram(    new TH1F(  "pT_rec",   "pT_rec",   ptnbins,    ptbinlims ) );
  addHistogram(    new TH1F( "eta_rec",  "eta_rec",   etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram(    new TH1F( "phi_rec",  "phi_rec",   phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  addHistogram(    new TH1F(  "z0_rec",   "z0_rec",     zBins,        -zMax,       zMax ) );
  addHistogram(    new TH1F(  "d0_rec",   "d0_rec",     d0Bins,       -d0Max,      d0Max ) );
  addHistogram(    new TH1F(  "a0_rec",   "a0_rec",     a0Bins,       -a0Max,      a0Max ) );

  addHistogram2D(  new TH2F( "eta_phi_rec",  "eta_phi_rec", (tmp_maxEta+1)*30  ,  -tmp_maxEta-1, tmp_maxEta+1,   (tmp_maxPhi+1)*30,  -tmp_maxPhi-1, tmp_maxPhi+1 ) );
  addHistogram2D(  new TH2F( "phi_d0_rec",  "phi_d0_rec", (2*tmp_maxPhi+2)*15,  -tmp_maxPhi-1, tmp_maxPhi+1 ,d0Bins+20,       -d0Max+7,      d0Max-7 ));

  // error study histograms (test)                                                                                                                                                                  
  addHistogram(  new TH1F( "dpT_rec",  "dpT_rec",  80, 0, 20.00 ) );
  addHistogram(  new TH1F( "deta_rec", "deta_rec", 50, 0,  0.02 ) );
  addHistogram(  new TH1F( "dphi_rec", "dphi_rec", 50, 0,  0.02 ) );
  addHistogram(  new TH1F( "dz0_rec",  "dz0_rec", 100, 0,  1.5  ) );
  addHistogram(  new TH1F( "dd0_rec",  "dd0_rec",  50, 0,  0.5 ) );
  addHistogram(  new TH1F( "da0_rec",  "da0_rec",  50, 0,  0.5 ) );



  // error study histograms (pull test-ref)                                                                                                                       
  addHistogram( new TH1F("pT_pull",  "pT_pull",  100, -10, 10) );
  addHistogram( new TH1F("eta_pull", "eta_pull", 100, -10, 10) );
  addHistogram( new TH1F("phi_pull", "phi_pull", 100, -10, 10) );
  addHistogram( new TH1F("z0_pull",  "z0_pull",  100, -10, 10) );
  addHistogram( new TH1F("d0_pull",  "d0_pull",  100, -10, 10) );
  addHistogram( new TH1F("a0_pull",  "a0_pull",  100, -10, 10) );

  // error study histograms (pull test-ref) - SIMPLE VERSION                                                                                                      
  addHistogram( new TH1F("pT_pull_simple",  "pT_pull_simple",  100, -10, 10) );
  addHistogram( new TH1F("eta_pull_simple", "eta_pull_simple", 100, -10, 10) );
  addHistogram( new TH1F("phi_pull_simple", "phi_pull_simple", 100, -10, 10) );
  addHistogram( new TH1F("z0_pull_simple",  "z0_pull_simple",  100, -10, 10) );
  addHistogram( new TH1F("d0_pull_simple",  "d0_pull_simple",  100, -10, 10) );
  addHistogram( new TH1F("a0_pull_simple",  "a0_pull_simple",  100, -10, 10) );

  
  // resolutions
  TH1F* pT_res = new TH1F(  "pT_res",   "pT_res",  4*pTResBins,   -0.1,  0.1  ); 
  pT_res->GetXaxis()->SetTitle("#Delta P_{T} [GeV]");
  pT_res->GetYaxis()->SetTitle("Entries");
  addHistogram( pT_res );


  TH1F* spT_res = new TH1F( "spT_res",  "spT_res",  4*pTResBins,   -0.1,  0.1 );
  spT_res->GetXaxis()->SetTitle("#Delta sP_{T} [GeV]");
  spT_res->GetYaxis()->SetTitle("Entries");
  addHistogram( spT_res );


  TH1F* ipT_res = new TH1F( "ipT_res",  "ipT_res",  4*pTResBins,   -0.4,  0.4 );
  ipT_res->GetXaxis()->SetTitle("#Delta 1/P_{T} [GeV^{-1}]");
  ipT_res->GetYaxis()->SetTitle("Entries");
  addHistogram( ipT_res );

  TH1F* eta_res = new TH1F("eta_res",  "eta_res",   etaResBins,  -2*tmp_absResEta, 2*tmp_absResEta );
  eta_res->GetXaxis()->SetTitle("#Delta #eta");
  eta_res->GetYaxis()->SetTitle("Entries");
  addHistogram( eta_res );
  addHistogram( new TH1F("etai_res", "etai_res", 1000,  -0.04, 0.04 ) );
  
  //  TH1F* phi_res =  new TH1F( "phi_res",  "phi_res;#Delta #phi;Entries", 2*phiResBins,  -2*tmp_absResPhi, 2*tmp_absResPhi );
  //  phi_res->GetXaxis()->SetTitle("#Delta #phi");
  //  phi_res->GetYaxis()->SetTitle("Entries");
  //  addHistogram( phi_res );

  addHistogram(    new TH1F( "phi_res",  "phi_res;#Delta #phi;Entries", 2*phiResBins,  -2*tmp_absResPhi, 2*tmp_absResPhi ) );
  addHistogram(    new TH1F(  "z0_res",   "z0_res;#Deltaz_{0};Entries",  16*zresBins,        -8*zresMax,       8*zresMax ) );
  addHistogram(    new TH1F(  "d0_res",   "d0_res;#Deltad_{0};Entries",  4*d0resBins,     -0.2*d0resMax,    0.2*d0resMax ) );
  addHistogram(    new TH1F(  "a0_res",   "a0_res;#Deltaa_{0};Entries",  4*a0resBins,     -0.2*a0resMax,    0.2*a0resMax ) );


  addHistogram(    new TH1F( "dphi_res",  "dphi_res;#Delta #phi;Entries", 2*phiResBins,  -0.2*tmp_absResPhi, 0.2*tmp_absResPhi ) );
  addHistogram(    new TH1F(  "dz0_res",   "dz0_res;#Deltaz_{0};Entries",   8*zresBins,        -0.8*zresMax,       0.8*zresMax ) );
  addHistogram(    new TH1F(  "dd0_res",   "dd0_res;#Deltad_{0};Entries",  4*d0resBins,      -0.05*d0resMax,     0.05*d0resMax ) );
  addHistogram(    new TH1F(  "da0_res",   "da0_res;#Deltaa_{0};Entries",  4*a0resBins,      -0.05*a0resMax,     0.05*a0resMax ) );


  //  std::cout << "booking resplots" << std::endl;

  int factor = 10;

  int wfactor = 2;
  int zfactor = 16;


  rDd0res.push_back(  new Resplot("rDd0_vs_pt",  ptnbins, ptbinlims, 1200, -0.1, 0.1 ) );
  rDd0res.push_back(  new Resplot("rDd0_vs_eta",  5*etaBins, -tmp_maxEta, tmp_maxEta, 1200, -0.1, 0.1 ) );
  rDd0res.push_back(  new Resplot("rDd0_vs_zed",  zBins, -zMax, zMax, 1200, -0.1, 0.1 ) );
  rDd0res.push_back(  new Resplot("rDd0_vs_d0",   20, -1.5, 1.5, 1200, -0.1, 0.1 ) );
  //  rDd0res.push_back(  new Resplot("rDd0_vs_phi",  int(2*M_PI/0.1), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02), 1200, -0.01, 0.05 ) );
  rDd0res.push_back(  new Resplot("rDd0_vs_phi",  128, -M_PI, M_PI, 1200, -0.1, 0.1 ) );


  rDa0res.push_back(  new Resplot("rDa0_vs_pt",  ptnbins, ptbinlims, 100, -0.1, 0.1 ) );
  rDa0res.push_back(  new Resplot("rDa0_vs_eta",  etaBins, -tmp_maxEta, tmp_maxEta, 100, -0.1, 0.1 ) );
  rDa0res.push_back(  new Resplot("rDa0_vs_zed",  0.2*zBins, -zMax, zMax, 100, -0.1, 0.1 ) );
  rDa0res.push_back(  new Resplot("rDa0_vs_da0", 20, -1.5, 1.5, 100, -0.1, 0.1 ) );
  rDa0res.push_back(  new Resplot("rDa0_vs_phi",     int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02), 100, -0.1, 0.1 ) );
  rDa0res.push_back(  new Resplot("rDa0_rec_vs_phi", int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02), 100, -0.1, 0.1 ) );

  rDa0res.push_back(  new Resplot("rda0_vs_phi",     int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02), 100, -0.1, 0.1 ) );
  rDa0res.push_back(  new Resplot("rda0_rec_vs_phi", int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02), 100, -0.1, 0.1 ) );

  //  rDd0res.push_back(  new Resplot("rDd0_vs_ipt",  iptnbins, iptbinlims, 100, -0.1, 0.1 ) );
  //  rDz0res.push_back(  new Resplot("rDz0_vs_ipt",  iptnbins, iptbinlims, 100, -1, 1 ) );


  rDz0res.push_back(  new Resplot("rDz0_vs_pt",   ptnbins, ptbinlims, 501, -1, 1 ) );
  rDz0res.push_back(  new Resplot("rDz0_vs_eta",  5*etaBins, -tmp_maxEta, tmp_maxEta, 500, -1, 1 ) );
  rDz0res.push_back(  new Resplot("rDz0_vs_zed",  zBins, -zMax, zMax, 500, -1, 1 ) );
  rDz0res.push_back(  new Resplot("rDz0_vs_phi",  128, -M_PI, M_PI, 500, -1, 1 ) );



  /// what is goping on here? the bins for the residuals should depend on the residual itself, not the x variable, 
  /// how come these are all different?  
  retares.push_back( new Resplot("reta_vs_ipt", iptnbins, iptbinlims, 2*etaResBins,  -wfactor*tmp_absResEta, wfactor*tmp_absResEta ) );
  rphires.push_back( new Resplot("rphi_vs_ipt", iptnbins, iptbinlims, 8*phiResBins,  -wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );
  rzedres.push_back( new Resplot("rzed_vs_ipt", iptnbins, iptbinlims, 8*zfactor*zresBins,   -2*zfactor*zresMax,      2*zfactor*zresMax       ) );
  riptres.push_back( new Resplot("ript_vs_ipt", iptnbins, iptbinlims, 16*pTResBins,  -wfactor*tmp_absResPt,  wfactor*tmp_absResPt  ) ); 
  rptres.push_back(  new Resplot("rpt_vs_ipt",  iptnbins, iptbinlims, 8*pTResBins,   -wfactor*tmp_absResPt,  wfactor*tmp_absResPt  ) ); 
  rd0res.push_back(  new Resplot("rd0_vs_ipt",  iptnbins, iptbinlims, factor*8*a0resBins,   -wfactor*a0resMax,  wfactor*a0resMax  ) );


  retares.push_back( new Resplot("reta_vs_pt", ptnbins, ptbinlims, 8*etaResBins,  -wfactor*tmp_absResEta, wfactor*tmp_absResEta ) );
  rphires.push_back( new Resplot("rphi_vs_pt", ptnbins, ptbinlims, 8*phiResBins,  -wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );
  rzedres.push_back( new Resplot("rzed_vs_pt", ptnbins, ptbinlims, 24*zfactor*zresBins,   -2*zfactor*zresMax,      2*zfactor*zresMax       ) );
  //rzedres.push_back( new Resplot("rzed_vs_pt", ptnbins, ptbinlims, 4*zfactor*zresBins,   -2*zwidthfactor*zresMax,      2*zwidthfactor*zresMax       ) );
  riptres.push_back( new Resplot("ript_vs_pt", ptnbins, ptbinlims, 16*pTResBins,  -wfactor*tmp_absResPt,  wfactor*tmp_absResPt  ) ); 
  rptres.push_back(  new Resplot("rpt_vs_pt",  ptnbins, ptbinlims, 8*pTResBins,   -wfactor*tmp_absResPt,  wfactor*tmp_absResPt  ) ); 
  rd0res.push_back(  new Resplot("rd0_vs_pt",  ptnbins, ptbinlims, factor*24*a0resBins,   -wfactor*a0resMax,  wfactor*a0resMax  ) );


  //  retares.push_back( new Resplot("reta_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  4*etaResBins,  -tmp_absResEta, tmp_absResEta ) );
  retares.push_back( new Resplot("reta_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  4*etaResBins,  -wfactor*tmp_absResEta, wfactor*tmp_absResEta ) );
  rphires.push_back( new Resplot("rphi_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  8*phiResBins,  -wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );
  rzedres.push_back( new Resplot("rzed_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  12*zfactor*zresBins,   -2*zfactor*zresMax,  2*zfactor*zresMax       ) );
  //rzedres.push_back( new Resplot("rzed_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  4*zfactor*zresBins,   -2*zwidthfactor*zresMax,  2*zwidthfactor*zresMax       ) );
  riptres.push_back( new Resplot("ript_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  16*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) ); 
  rptres.push_back(  new Resplot("rpt_vs_eta",  etaBins, -tmp_maxEta, tmp_maxEta,  8*pTResBins,   -tmp_absResPt, tmp_absResPt  ) ); 
  rd0res.push_back(  new Resplot("rd0_vs_eta",  etaBins, -tmp_maxEta, tmp_maxEta,  factor*24*a0resBins,   -wfactor*a0resMax,  wfactor*a0resMax  ) );


  //  rphivsDd0res = new Resplot( "rphi_vs_Dd0", 10, 0, 0.1, int(2*M_PI/0.02), -0.2*int(M_PI/0.02), 0.2*int(M_PI/0.02) );
  hphivsDd0res[0] = new TH1F( "hphi_vs_Dd0_0", "phi for Dd0<0.1", int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02) );
  hphivsDd0res[1] = new TH1F( "hphi_vs_Dd0_1", "phi for Dd0>0.1", int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02) );
  hphivsDd0res[2] = new TH1F( "hphi_vs_Dd0_2", "all phi",         int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02) );

  hphivsDa0res[0] = new TH1F( "hphi_vs_Da0_0", "phi for Da0<0.1", int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02) );
  hphivsDa0res[1] = new TH1F( "hphi_vs_Da0_1", "phi for Da0>0.1", int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02) );
  hphivsDa0res[2] = new TH1F( "hphi_vs_Da0_2", "all phi",         int(2*M_PI/0.02), -0.02*int(M_PI/0.02), 0.02*int(M_PI/0.02) );
  

  for ( unsigned ih=0 ; ih<3 ; ih++ ) { 
    hphivsDd0res[ih]->SetDirectory(0);
    hphivsDa0res[ih]->SetDirectory(0);
  }  

  retares.push_back( new Resplot("reta_vs_zed", 0.2*zBins, -zMax, zMax,  2*etaResBins,  -tmp_absResEta, tmp_absResEta ) );
  rphires.push_back( new Resplot("rphi_vs_zed", 0.2*zBins, -zMax, zMax,  8*phiResBins,  -wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );
  rzedres.push_back( new Resplot("rzed_vs_zed", 0.2*zBins, -zMax, zMax,  8*zfactor*zresBins,   -2*zfactor*zresMax,   2*zfactor*zresMax       ) );
  //rzedres.push_back( new Resplot("rzed_vs_zed", 0.2*zBins, -zMax, zMax,  4*zfactor*zresBins,   -2*zwidthfactor*zresMax,   2*zwidthfactor*zresMax       ) );
  //rzedres.push_back( new Resplot("rzed_vs_zed", zBins, -zMax, zMax,  4*zfactor*zresBins,   -2*zwidthfactor*zresMax,   2*zwidthfactor*zresMax       ) );
  riptres.push_back( new Resplot("ript_vs_zed", 0.2*zBins, -zMax, zMax,  2*pTResBins,     -2*tmp_absResPt,  2*tmp_absResPt  ) ); 
  rptres.push_back(  new Resplot("rpt_vs_zed",  0.2*zBins, -zMax, zMax,  8*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) ); 
  rd0res.push_back(  new Resplot("rd0_vs_zed",  0.2*zBins, -zMax, zMax,  factor*8*a0resBins,  -wfactor*a0resMax,      wfactor*a0resMax  ) );


  retares.push_back( new Resplot("reta_vs_nvtx", 12, 0, 36,  4*etaResBins,  -tmp_absResEta, tmp_absResEta ) );
  rphires.push_back( new Resplot("rphi_vs_nvtx", 12, 0, 36,  8*phiResBins,  -wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );
  rzedres.push_back( new Resplot("rzed_vs_nvtx", 12, 0, 36,  4*zfactor*zresBins,   -zfactor*zresMax,      zfactor*zresMax       ) );
  //rzedres.push_back( new Resplot("rzed_vs_nvtx", 12, 0, 36,  4*zfactor*zresBins,   -zfactor*0.5*zresMax,      zfactor*0.5*zresMax       ) );
  riptres.push_back( new Resplot("ript_vs_nvtx", 12, 0, 36,  4*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) ); 
  rptres.push_back(  new Resplot("rpt_vs_nvtx",  12, 0, 36,  8*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) ); 
  rd0res.push_back(  new Resplot("rd0_vs_nvtx",  12, 0, 36,  factor*8*a0resBins,   -wfactor*a0resMax,  wfactor*a0resMax  ) );


  retares.push_back( new Resplot("reta_vs_ntracks", 60, 0, 600,  4*etaResBins,  -tmp_absResEta, tmp_absResEta ) );
  rphires.push_back( new Resplot("rphi_vs_ntracks", 60, 0, 600,  8*phiResBins,  -wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );
  rzedres.push_back( new Resplot("rzed_vs_ntracks", 60, 0, 600,  4*zfactor*zresBins,   -zfactor*zresMax,     zfactor*zresMax       ) );
  //rzedres.push_back( new Resplot("rzed_vs_ntracks", 60, 0, 600,  4*zfactor*zresBins,   -zfactor*0.5*zresMax,     zfactor*0.5*zresMax       ) );
  riptres.push_back( new Resplot("ript_vs_ntracks", 60, 0, 600,  4*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) ); 
  rptres.push_back(  new Resplot("rpt_vs_ntracks",  60, 0, 600,  8*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) ); 
  rd0res.push_back(  new Resplot("rd0_vs_ntracks",  60, 0, 600,  factor*8*a0resBins,   -wfactor*a0resMax,  wfactor*a0resMax  ) );


  retares.push_back( new Resplot("reta_vs_phi", 128, -M_PI, M_PI, 2*etaResBins,       -wfactor*tmp_absResEta, wfactor*tmp_absResEta ) );
  rphires.push_back( new Resplot("rphi_vs_phi", 128, -M_PI, M_PI, 8*phiResBins,       -wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );
  rzedres.push_back( new Resplot("rzed_vs_phi", 128, -M_PI, M_PI, 8*zfactor*zresBins, -2*zfactor*zresMax,     2*zfactor*zresMax       ) );
  riptres.push_back( new Resplot("ript_vs_phi", 128, -M_PI, M_PI, 16*pTResBins,       -wfactor*tmp_absResPt,  wfactor*tmp_absResPt  ) ); 
  rptres.push_back(  new Resplot("rpt_vs_phi",  128, -M_PI, M_PI, 8*pTResBins,        -wfactor*tmp_absResPt,  wfactor*tmp_absResPt  ) ); 
  rd0res.push_back(  new Resplot("rd0_vs_phi",  128, -M_PI, M_PI, factor*8*a0resBins, -wfactor*a0resMax,      wfactor*a0resMax  ) );


  //ADDED BY JK
  //-----
  rzedres.push_back( new Resplot("rzed_vs_signed_pt", ptnbins2, ptbinlims2, 4*zfactor*zresBins,   -zfactor*zresMax,      zfactor*zresMax       ) );
  rzedres.push_back( new Resplot("rzed_vs_ABS_pt",    ptnbins,  ptbinlims,  4*zfactor*zresBins,   -2*zfactor*zresMax,  2*zfactor*zresMax       ) );
  //rzedres.push_back( new Resplot("rzed_vs_signed_pt", ptnbins2, ptbinlims2, 4*zfactor*zresBins,   -2*zwidthfactor*zresMax,      2*zwidthfactor*zresMax       ) );
  //rzedres.push_back( new Resplot("rzed_vs_ABS_pt", ptnbins, ptbinlims, 4*zfactor*zresBins,   -2*zwidthfactor*zresMax,      2*zwidthfactor*zresMax       ) );
  rd0res.push_back( new Resplot("rd0_vs_signed_pt", ptnbins2, ptbinlims2, factor*8*a0resBins,   -wfactor*a0resMax,  wfactor*a0resMax  ) );
  rd0res.push_back( new Resplot("rd0_vs_ABS_pt",    ptnbins,  ptbinlims,  factor*8*a0resBins,   -wfactor*a0resMax,  wfactor*a0resMax  ) );
  //-----


  //  std::cout << "booked" << std::endl;

  retaresPull.push_back( new Resplot("retaPull_vs_ipt", iptnbins, iptbinlims, 2*etaResBins, -5,5));//-wfactor*tmp_absResEta, wfactor*tmp_absResEta ) );           
  rphiresPull.push_back( new Resplot("rphiPull_vs_ipt", iptnbins, iptbinlims, 8*phiResBins, -5,5));//-wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );           
  rzedresPull.push_back( new Resplot("rzedPull_vs_ipt", iptnbins, iptbinlims, 4*zfactor*zresBins, -5,5));//-2*zfactor*zresMax, 2*zfactor*zresMax ) );             
  //riptresPull.push_back( new Resplot("ript_vs_ipt", iptnbins, iptbinlims, 16*pTResBins, -wfactor*tmp_absResPt, wfactor*tmp_absResPt ) );                        
  rptresPull.push_back( new Resplot("rptPull_vs_ipt", iptnbins, iptbinlims, 8*pTResBins, -5,5));//-wfactor*tmp_absResPt, wfactor*tmp_absResPt ) );                
  rd0resPull.push_back( new Resplot("rd0Pull_vs_ipt", iptnbins, iptbinlims, factor*8*a0resBins, -5,5));//-wfactor*a0resMax, wfactor*a0resMax ) ) ;                

  retaresPull.push_back( new Resplot("retaPull_vs_pt", ptnbins, ptbinlims, 2*etaResBins,  -5,5));//-wfactor*tmp_absResEta, wfactor*tmp_absResEta ) );             
  rphiresPull.push_back( new Resplot("rphiPull_vs_pt", ptnbins, ptbinlims, 8*phiResBins,  -5,5));//-wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );             
  rzedresPull.push_back( new Resplot("rzedPull_vs_pt", ptnbins, ptbinlims, 4*zfactor*zresBins,   -5,5));//-2*zfactor*zresMax,      2*zfactor*zresMax       ) );   
  //rzedres.push_back( new Resplot("rzed_vs_pt", ptnbins, ptbinlims, 4*zfactor*zresBins,   -2*zwidthfactor*zresMax,      2*zwidthfactor*zresMax       ) );
  //riptresPull.push_back( new Resplot("ript_vs_pt", ptnbins, ptbinlims, 16*pTResBins,  -wfactor*tmp_absResPt,  wfactor*tmp_absResPt  ) );                        
  rptresPull.push_back(  new Resplot("rptPull_vs_pt",  ptnbins, ptbinlims, 8*pTResBins,   -5,5));//-wfactor*tmp_absResPt,  wfactor*tmp_absResPt  ) );             
  rd0resPull.push_back(  new Resplot("rd0Pull_vs_pt",  ptnbins, ptbinlims, factor*8*a0resBins,   -5,5));//-wfactor*a0resMax,  wfactor*a0resMax  ) );              

  retaresPull.push_back( new Resplot("retaPull_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  4*etaResBins,  -5,5));//-tmp_absResEta, tmp_absResEta ) );             
  rphiresPull.push_back( new Resplot("rphiPull_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  8*phiResBins,  -5,5));//-wfactor*tmp_absResPhi, wfactor*tmp_absResPhi) );
  rzedresPull.push_back( new Resplot("rzedPull_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  4*zfactor*zresBins,   -5,5));//-2*zfactor*zresMax,  2*zfactor*zresMax) );
  //rzedres.push_back( new Resplot("rzed_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  4*zfactor*zresBins,   -2*zwidthfactor*zresMax,  2*zwidthfactor*zresMax) );
  //riptresPull.push_back( new Resplot("ript_vs_eta", etaBins, -tmp_maxEta, tmp_maxEta,  16*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) );                       
  rptresPull.push_back(  new Resplot("rptPull_vs_eta",  etaBins, -tmp_maxEta, tmp_maxEta,  8*pTResBins,   -5,5));//-tmp_absResPt, tmp_absResPt  ) );              
  rd0resPull.push_back(  new Resplot("rd0Pull_vs_eta",  etaBins, -tmp_maxEta, tmp_maxEta,  factor*8*a0resBins,   -5,5));//-wfactor*a0resMax,  wfactor*a0resMax  ));  
  retaresPull.push_back( new Resplot("retaPull_vs_zed", 0.2*zBins, -zMax, zMax,  2*etaResBins,  -5,5));//-tmp_absResEta, tmp_absResEta ) ); 
  rphiresPull.push_back( new Resplot("rphiPull_vs_zed", 0.2*zBins, -zMax, zMax,  8*phiResBins,  -5,5));//-wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );       
  rzedresPull.push_back( new Resplot("rzedPull_vs_zed", 0.2*zBins, -zMax, zMax,  4*zfactor*zresBins,   -5,5));//-2*zfactor*zresMax,   2*zfactor*zresMax       ) ) ; 
  //rzedres.push_back( new Resplot("rzed_vs_zed", 0.2*zBins, -zMax, zMax,  4*zfactor*zresBins,   -2*zwidthfactor*zresMax,   2*zwidthfactor*zresMax       ) );  
  //rzedres.push_back( new Resplot("rzed_vs_zed", zBins, -zMax, zMax,  4*zfactor*zresBins,   -2*zwidthfactor*zresMax,   2*zwidthfactor*zresMax       ) ); 
  //riptresPull.push_back( new Resplot("ript_vs_zed", 0.2*zBins, -zMax, zMax,  2*pTResBins,     -2*tmp_absResPt,  2*tmp_absResPt  ) );                            
  rptresPull.push_back(  new Resplot("rptPull_vs_zed",  0.2*zBins, -zMax, zMax,  8*pTResBins,   -5,5));//-tmp_absResPt,  tmp_absResPt  ) );                       
  rd0resPull.push_back(  new Resplot("rd0Pull_vs_zed",  0.2*zBins, -zMax, zMax,  factor*8*a0resBins,  -5,5));//-wfactor*a0resMax,      wfactor*a0resMax  ) );     
  
  retaresPull.push_back( new Resplot("retaPull_vs_nvtx", 12, 0, 36,  4*etaResBins,  -5,5));//-tmp_absResEta, tmp_absResEta ) );                                   
  rphiresPull.push_back( new Resplot("rphiPull_vs_nvtx", 12, 0, 36,  8*phiResBins,  -5,5));//-wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );                   
  rzedresPull.push_back( new Resplot("rzedPull_vs_nvtx", 12, 0, 36,  4*zfactor*zresBins,   -5,5));//-zfactor*zresMax,      zfactor*zresMax       ) );             
  //rzedres.push_back( new Resplot("rzed_vs_nvtx", 12, 0, 36,  4*zfactor*zresBins,   -zfactor*0.5*zresMax,      zfactor*0.5*zresMax       ) );         
  
  //riptresPull.push_back( new Resplot("ript_vs_nvtx", 12, 0, 36,  4*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) );                                              
  rptresPull.push_back(  new Resplot("rptPull_vs_nvtx",  12, 0, 36,  8*pTResBins,   -5,5));//-tmp_absResPt,  tmp_absResPt  ) );                                   
  rd0resPull.push_back(  new Resplot("rd0Pull_vs_nvtx",  12, 0, 36,  factor*8*a0resBins,   -5,5));//-wfactor*a0resMax,  wfactor*a0resMax  ) );                    

  retaresPull.push_back( new Resplot("retaPull_vs_ntracks", 60, 0, 600,  4*etaResBins,  -5,5));//-tmp_absResEta, tmp_absResEta ) );                               
  rphiresPull.push_back( new Resplot("rphiPull_vs_ntracks", 60, 0, 600,  8*phiResBins,  -5,5));//-wfactor*tmp_absResPhi, wfactor*tmp_absResPhi ) );               
  rzedresPull.push_back( new Resplot("rzedPull_vs_ntracks", 60, 0, 600,  4*zfactor*zresBins,   -5,5));//-zfactor*zresMax,     zfactor*zresMax       ) );          
  //rzedres.push_back( new Resplot("rzed_vs_ntracks", 60, 0, 600,  4*zfactor*zresBins,   -zfactor*0.5*zresMax,     zfactor*0.5*zresMax       ) );    
  //riptresPull.push_back( new Resplot("ript_vs_ntracks", 60, 0, 600,  4*pTResBins,   -tmp_absResPt,  tmp_absResPt  ) );                                          
  rptresPull.push_back(  new Resplot("rptPull_vs_ntracks",  60, 0, 600,  8*pTResBins,   -5,5));//-tmp_absResPt,  tmp_absResPt  ) );                               
  rd0resPull.push_back(  new Resplot("rd0Pull_vs_ntracks",  60, 0, 600,  factor*8*a0resBins,   -5,5));//-wfactor*a0resMax,  wfactor*a0resMax  ) );                
  
  rzedresPull.push_back( new Resplot("rzedPull_vs_signed_pt", ptnbins2, ptbinlims2, 4*zfactor*zresBins,   -5,5));//-2*zfactor*zresMax,      2*zfactor*zresMax  ) );
  rzedresPull.push_back( new Resplot("rzedPull_vs_ABS_pt", ptnbins, ptbinlims, 4*zfactor*zresBins,   -5,5));//-2*zfactor*zresMax,      2*zfactor*zresMax       ));
  //rzedres.push_back( new Resplot("rzed_vs_signed_pt", ptnbins2, ptbinlims2, 4*zfactor*zresBins,   -2*zwidthfactor*zresMax,      2*zwidthfactor*zresMax       ));
  //rzedres.push_back( new Resplot("rzed_vs_ABS_pt", ptnbins, ptbinlims, 4*zfactor*zresBins,   -2*zwidthfactor*zresMax,      2*zwidthfactor*zresMax       ) );              
  rd0resPull.push_back( new Resplot("rd0Pull_vs_signed_pt", ptnbins2, ptbinlims2, factor*8*a0resBins,   -5,5));//-wfactor*a0resMax,  wfactor*a0resMax  ) );       
  rd0resPull.push_back( new Resplot("rd0Pull_vs_ABS_pt", ptnbins, ptbinlims, factor*8*a0resBins,   -5,5));//-wfactor*a0resMax,  wfactor*a0resMax  ) ); 

  rzedreslb = new Resplot("rzed_vs_lb", 301, -0.5, 3009.5, 8*zfactor*zresBins, -2*zfactor*zresMax,     2*zfactor*zresMax  );
  

  rd0_vs_phi     = new Resplot( "d0_vs_phi",     20, -M_PI, M_PI, 200, -10, 10 ); 
  rd0_vs_phi_rec = new Resplot( "d0_vs_phi_rec", 20, -M_PI, M_PI, 200, -10, 10 ); 


  /// Roi - track residuals

  rRoi_deta_vs_eta = new Resplot( "rRoi_deta_vs_eta", 20, -2.5, 2.5, 101,  -0.5,  0.5 );
  rRoi_dphi_vs_eta = new Resplot( "rRoi_dphi_vs_eta", 20, -2.5, 2.5, 101,  -0.5,  0.5 );
  rRoi_dzed_vs_eta = new Resplot( "rRoi_dzed_vs_eta", 20, -2.5, 2.5, 401, -30.0, 30.0 );

  //  std::cout << "ROI RESPLOTS " <<  rRoi_deta_vs_eta->Name() << " " << rRoi_dphi_vs_eta->Name() << std::endl;


  // hit occupancies

  int   NHits = 40;
  int Ntracks = 1000;

  addHistogram( new TH1F( "nsct",     "nsct",     NHits, -0.5, float(NHits-0.5) ) );
  addHistogram( new TH1F( "nsct_rec", "nsct_rec", NHits, -0.5, float(NHits-0.5) ) );

  addHistogram( new TH1F( "npix",     "npix",     NHits, -0.5, float(NHits-0.5) ) );
  addHistogram( new TH1F( "npix_rec", "npix_rec", NHits, -0.5, float(NHits-0.5) ) );

  addHistogram( new TH1F( "nsi",     "nsi",     NHits, -0.5, float(NHits-0.5) ) );
  addHistogram( new TH1F( "nsi_rec", "nsi_rec", NHits, -0.5, float(NHits-0.5) ) );
  addHistogram( new TH1F( "nsi_matched", "nsi_matched", NHits, -0.5, float(NHits-0.5) ) );


  addHistogram( new TH1F( "ntrt",     "ntrt",     NHits, -0.5, float(NHits-0.5) ) );
  addHistogram( new TH1F( "ntrt_rec", "ntrt_rec", NHits, -0.5, float(NHits-0.5) ) );

  addHistogram( new TH1F( "nstraw",     "nstraw",     NHits*4, -0.5, float(4*NHits-0.5) ) );
  addHistogram( new TH1F( "nstraw_rec", "nstraw_rec", NHits*4, -0.5, float(4*NHits-0.5) ) );

  addHistogram( new TH1F( "ntracks",     "ntracks",     Ntracks+1, -0.5, float(Ntracks+0.5) ) );
  addHistogram( new TH1F( "ntracks_rec", "ntracks_rec", Ntracks+1, -0.5, float(Ntracks+0.5) ) );


  // beam offset fitting histos
  h2  = new Resplot( "d0vphi",       phiBins, -3.142, 3.142, d0Bins, -d0Max, d0Max );  
  h2r = new Resplot( "d0vphi_rec",   phiBins, -3.142, 3.142, d0Bins, -d0Max, d0Max );  
  h2m = new Resplot( "d0vphi_match", phiBins, -3.142, 3.142, d0Bins, -d0Max, d0Max );  

  h2a0  = new Resplot( "a0vphi",       phiBins, -3.142, 3.142, d0Bins, -d0Max, d0Max );  
  h2a0r = new Resplot( "a0vphi_rec",   phiBins, -3.142, 3.142, d0Bins, -d0Max, d0Max );  

  /// efficiency vs lumi block


  TH1F heffvlb("eff vs lb", "eff vs lb", 301, -0.5, 3009.5 );

    //	       100, 
    //	       1270515000, 1270560000 
    //	       1272040000, 1272200000
    //    );
    // 1270518944,
    // 1270558762 );
    
    //	       1260470000, 
    //	       1260680000 ); 
    //	       1260470000, 
    //	       1260690000 ); 
    // TH1F heffvlb("eff vs lb", "eff vs lb", 600, 0, 3000);
    
    //  TH1F heffvlb("eff vs lb", "eff vs lb", 600, 1260400000, 1260700000); 


  eff_vs_lb = new Efficiency( &heffvlb );

  //  z_vs_lb = new Resplot("z vs lb", 100, 1270515000,  1270560000, 100, -250, 250);
  z_vs_lb = new Resplot("z vs lb", 301, -0.5,  3009.5, 100, -250, 250);

  //rmap[142165] = 0;
  //rmap[142166] = 500;
  //rmap[142174] = 1000;
  //rmap[142191] = 1500;
  //rmap[142193] = 2000;
  //rmap[142195] = 2500;


  mdeltaR_v_eta = new Resplot("R v eta", 10, -2.5, 2.5,     100, 0, 0.1 );
  mdeltaR_v_pt  = new Resplot("R v pt", ptnbins, ptbinlims, 100, 0, 0.1 );


  TH1F* _eff_vs_mult = new TH1F( "eff_vs_mult", "eff_vs_mult", 25, 0, 25 );

  eff_vs_mult = new Efficiency( _eff_vs_mult, "eff_mult" );


  n_vtx_tracks   = new TH1F("nvtxtracks", "nvtxtracks", 150, 0, 600);
  eff_vs_ntracks = new Efficiency( n_vtx_tracks, "eff_vs_ntracks");



  //double oldnbins[21] = { 1,   5,   10,  15,  20, 
  //	25,  30,  35,  40,  45, 
  //	50,  60,  80,  100, 150, 
  //	200, 250, 300, 400, 500, 600 };


  //double _nbins[23] = {  0,         29.4,    39.9179, 48.4358, 56.1813,  62.9524,  70.1377, 76.8907,
  //	83.4667,   90.559,  97.4902, 105.737, 113.547,	121.281,  129.015, 139.824,
  //	150.589,   164.093, 179.096, 206.867, 400,      500,      600 };

  double nbins[23] = {  0,      29.5,  39.5,  48.5,  56.5,  63.5,  70.5,  77.5,
			83.5,   91.5,  97.5,  106.5, 114.5, 121.5, 129.5, 140.5,
			151.5,  164.5, 200.5, 250.5, 300.5, 400.5, 600 };

  TH1F* n_vtx_tracks2   = new TH1F("nvtxtracks2", "nvtxtracks2", 22, nbins);
  eff_vs_ntracks2 = new Efficiency( n_vtx_tracks2, "eff_vs_ntracks2");
  delete n_vtx_tracks2;

  n_vtx       = new TH1F("nvtx", "nvtx", 31, -0.5, 30.5);
  eff_vs_nvtx = new Efficiency( n_vtx, "eff_vs_nvtx");
  //mu          = new TH1F("mu", "mu", 3000, -0.5, 29.5);
  mu          = new TH1F("mu", "mu", 70, 0, 70);
  eff_vs_mu   = new Efficiency( mu, "eff_vs_mu");


  //  std::cout << "initialize() Directory " << gDirectory->GetName() << " on leaving" << std::endl;

  ConfVtxAnalysis* vtxanal = 0;
  store().find( vtxanal, "rvtx" );
  if ( vtxanal ) vtxanal->initialise();

  //  std::cout << "initialize() Directory " << gDirectory->GetName() << " on leaving" << std::endl;


  mdir->pop();

  dir->cd();



}





// fit Gaussian to histogram

TF1* FitFWGaussian(TH1D* s, double a, double b) {

  //  std::cout << "FitFWGaussian " << s->GetName() << std::endl;

  //  TF1* f1 = new TF1("gausr", "gaus"); 
  TF1* f1 = new TF1("gausr", "[0]*exp(-(x-[1])*(x-[1])/([2]*[2]))"); 

  f1->SetNpx(5000);
  f1->SetLineWidth(1);

  f1->SetParameter(0,s->GetBinContent(s->GetMaximumBin()) );
  f1->SetParameter(1,s->GetBinCenter(s->GetMaximumBin()));
  //  f1->SetParameter(2,s->GetRMS());
  f1->SetParameter(2,1.5);
  f1->FixParameter(2,1.5);

  f1->SetParName(0, "Norm");
  f1->SetParName(1, "Mean");
  f1->SetParName(2, "Sigma");

  int nbins=0;
  for (int j=1 ; j<=s->GetNbinsX() ; j++) if (s->GetBinContent(j)) nbins++;

  if (nbins>2) {
    if ( a==-999 || b==-999 )  s->Fit(f1,"Q");
    else                       s->Fit(f1,"Q", "", a, b);
  }
  else for ( int j=0 ; j<3 ; j++ ) f1->SetParameter(j, 0);

  //  std::cout << "return" << std::endl;

  return f1;
}





void fitSin( TH1D* h, const std::string& parent="" ) { 

  static TF1* fsin = new TF1( "sinp", "sqrt([0]*[0])*sin([1]-x)" ); // , -M_PI, M_PI );

  fsin->SetParameter(0,1);
  fsin->SetParameter(1,0);

  fsin->SetLineWidth(1);

  //  h->SetTitle(";track #phi;d_{0} [mm]");
 
  std::cout << __FUNCTION__ << " " << h->GetName() << std::endl; 
  h->Fit( fsin, "", "", -M_PI, M_PI );

  double radius = fsin->GetParameter(0);
  double phi    = fsin->GetParameter(1);
  
  double y = radius*sin(phi);
  double x = radius*cos(phi);
  
  std::cout << parent << "\t" << h->GetTitle() << "\tx = " << x << "\ty = " << y << std::endl;

}




/// calculate the efficiencies and write them out with all the histograms 

void ConfAnalysis::finalise() {

  //  std::cout << "\n\nConfAnalysis::finalise() " << name() << std::endl;

  //  gDirectory->pwd();

  if ( !m_initialised ) return;


  std::cout << "ConfAnalysis::finalise() " << name();
  if ( name().size()<19 ) std::cout << "\t";
  if ( name().size()<30 ) std::cout << "\t";
  if ( name().size()<41 ) std::cout << "\t";
  if ( name().size()<52 ) std::cout << "\t";

  std::cout << "\tNreco "    << Nreco  
	    << "\tNref "     << Nref
	    << "\tNmatched " << Nmatched;

  if (Nref) {
    std::cout << " tracks approx " << (100*Nmatched)/Nref << "%" ;
  }
  std::cout  << std::endl;

  //  if ( Nreco==0 ) return;

  //  TIDDirectory d( name() );
  //  d.push();

  mdir->push();

  std::map<std::string, TH1F*>::iterator hitr=m_histos.begin();
  std::map<std::string, TH1F*>::iterator hend=m_histos.end();
  for ( ; hitr!=hend ; hitr++ ) hitr->second->Write();     

  //  std::cout << "DBG >" << eff_pt->Hist()->GetName() << "< DBG" << std::endl;

  //  std::vector<Efficiency*> heff = { eff_pt,

  const unsigned Neff = 11;
  Efficiency*  heff[Neff] = { eff_pt,
			      eff_eta,
			      eff_phi,
			      eff_z0,
			      eff_d0,
			      eff_a0,
			      eff_ptm,
			      eff_ptp,
			      eff_roi_deta,
			      eff_roi_dphi,
			      eff_roi_dR };
  
  for ( unsigned i=0 ; i<Neff ; i++ ) {
    heff[i]->finalise();  
    heff[i]->Bayes()->Write( ( heff[i]->name()+"_tg" ).c_str() );
  } // heff[i]->Hist()->Write(); } 

  //  std::cout << "DBG >" << purity_pt->Hist()->GetName() << "< DBG" << std::endl;


  eff_vs_mult->finalise();

  //  Normalise(n_vtx_tracks);

  eff_vs_ntracks->finalise();
  eff_vs_ntracks2->finalise();

  eff_vs_nvtx->finalise();
  eff_vs_mu->finalise();

  const unsigned Npurity = 6;
  Efficiency* hpurity[Npurity] = {
    purity_pt, 
    purity_eta, 
    purity_phi,
    purity_z0, 
    purity_d0,
    purity_a0 };

  for ( unsigned i = 0 ; i<Npurity ; i++ ) hpurity[i]->finalise();

  rd0_vs_phi->Finalise(Resplot::FitNull95);
  fitSin( rd0_vs_phi->Mean(), name()+"/rd0_vs_phi" );
  rd0_vs_phi->Write();

  rd0_vs_phi_rec->Finalise(Resplot::FitNull95);
  fitSin( rd0_vs_phi_rec->Mean(), name()+"/rd0_vs_phi_rec" );
  rd0_vs_phi_rec->Write();
  

  std::cout << "ConfAnalysis::finalise() " << name() << std::endl;

  std::string spstr[5] = { "npix", "nsct", "nsi", "ntrt", "nbl" };
  for ( int i=mres.size() ; i-- ; ) { 
    TF1* (*resfit)(TH1D* s, double a, double b) = Resplot::FitNull95;    
    for ( int ir=0 ; ir<5 ; ir++ ) if ( mres[i]->Name().find(spstr[ir])!=std::string::npos ) { resfit = Resplot::FitNull; break; }
    mres[i]->Finalise( resfit ); 
    mres[i]->Write();
  }

  mdeltaR_v_eta->Finalise();   mdeltaR_v_eta->Write(); 
  mdeltaR_v_pt->Finalise();    mdeltaR_v_pt->Write(); 



  for ( unsigned i=rDd0res.size() ; i-- ; ) { 
    rDd0res[i]->Finalise(Resplot::FitNull95);
    rDd0res[i]->Write();
    delete rDd0res[i];
  }

  for ( unsigned i=rDa0res.size() ; i-- ; ) { 
    rDa0res[i]->Finalise(Resplot::FitNull95);
    rDa0res[i]->Write();
    delete rDa0res[i];
  }

  for ( unsigned i=rDz0res.size() ; i-- ; ) { 
    rDz0res[i]->Finalise(Resplot::FitNull95);
    rDz0res[i]->Write();
    delete rDz0res[i];
  }


  for ( unsigned ih=0 ; ih<2 ; ih++ ) { 
    hphivsDd0res[ih]->Divide( hphivsDd0res[2] );
    hphivsDa0res[ih]->Divide( hphivsDa0res[2] );
  }  

  for ( unsigned ih=0 ; ih<3 ; ih++ ) { 
    hphivsDd0res[ih]->Write();
    hphivsDa0res[ih]->Write();

    delete hphivsDd0res[ih];
    delete hphivsDa0res[ih];
  }  

  /// roi residuals 

  rRoi_deta_vs_eta->Finalise(Resplot::FitNull95);
  rRoi_dphi_vs_eta->Finalise(Resplot::FitNull95);
  rRoi_dzed_vs_eta->Finalise(Resplot::FitNull95);
  
  rRoi_deta_vs_eta->Write();
  rRoi_dphi_vs_eta->Write();
  rRoi_dzed_vs_eta->Write();

  delete  rRoi_deta_vs_eta;
  delete  rRoi_dphi_vs_eta;
  delete  rRoi_dzed_vs_eta;

  /// standard residuals



  for ( unsigned i=retares.size() ; i-- ; ) { 

#if 1

    retares[i]->Finalise(Resplot::FitNull95);
    rphires[i]->Finalise(Resplot::FitNull95);
    riptres[i]->Finalise(Resplot::FitNull95);
    rzedres[i]->Finalise(Resplot::FitNull95);
    //    rptres[i]->Finalise(Resplot::FitBreit);
    //rptres[i]->Finalise(Resplot::FitNull95);
    rd0res[i]->Finalise(Resplot::FitNull95);
    //  rd0res[i]->Finalise(Resplot::FitCentralGaussian);
    //  rd0res_rms[i]->Finalise(Resplot::FitNull);

#else

    retares[i]->Finalise();
    rphires[i]->Finalise();
    rzedres[i]->Finalise();
    riptres[i]->Finalise();
    rptres[i]->Finalise();
    rd0res[i]->Finalise();

#endif    

    retares[i]->Write();
    rphires[i]->Write();
    rzedres[i]->Write();
    riptres[i]->Write();
    rptres[i]->Write();
    rd0res[i]->Write();

    delete retares[i];
    delete rphires[i];
    delete rzedres[i];
    delete riptres[i];
    delete rptres[i];
    delete rd0res[i];


  }

  rzedreslb->Finalise(Resplot::FitNull95);


  for ( unsigned i=retares.size()-1 ; i-- ; ) { 

    retaresPull[i]->Finalise(Resplot::FitNull);
    rphiresPull[i]->Finalise(Resplot::FitNull);
    rptresPull[i]->Finalise(Resplot::FitNull);
    rzedresPull[i]->Finalise(Resplot::FitNull);
    rd0resPull[i]->Finalise(Resplot::FitNull);

    retaresPull[i]->Write();
    rphiresPull[i]->Write();
    rptresPull[i]->Write();
    rzedresPull[i]->Write();
    rd0resPull[i]->Write();

    delete retaresPull[i];
    delete rphiresPull[i];
    delete rptresPull[i];
    delete rzedresPull[i];
    delete rd0resPull[i];

  }


  rzedreslb->Write();
  delete rzedreslb;

  //  td->cd();


  //ADDED BY JK
  //-----Only one more element in d0 and z0 vectors than eta now
  //rzedres[5]->Finalise(Resplot::FitNull95);
  //rzedres[5]->Write();
  rzedres[rphires.size()]->Finalise(Resplot::FitNull95);
  rzedres[rphires.size()]->Write();
  //rd0res[5]->Finalise(Resplot::FitNull95);
  //rd0res[5]->Write();
  rd0res[rphires.size()]->Finalise(Resplot::FitNull95);
  rd0res[rphires.size()]->Write();
  //-----

  eff_vs_lb->finalise();

  z_vs_lb->Finalise(); z_vs_lb->Write();
  delete z_vs_lb;

  //  TH1F* hefflb = eff_vs_lb->Hist();
  //  hefflb->Fit("pol0");

  //  ConfVtxAnalysis* vtxanal = 0;
  //  store().find( vtxanal, "rvtx" );
  //  if ( vtxanal ) vtxanal->finalise();

  //  d.pop();

  ConfVtxAnalysis* vtxanal = 0;
  store().find( vtxanal, "rvtx" );
  if ( vtxanal ) vtxanal->finalise();


  mdir->pop();

}

extern int Nvtxtracks;
extern int NvtxCount;


/// fill all the histograms - matched histograms, efficiencies etc


double wrapphi( double phi ) { 
  if ( phi<-M_PI ) phi += 2*M_PI;
  if ( phi> M_PI ) phi -= 2*M_PI;
  return phi;
} 


void ConfAnalysis::execute(const std::vector<TIDA::Track*>& reftracks,
			   const std::vector<TIDA::Track*>& testtracks,
			   TrackAssociator* matcher ) { 

  if ( !m_initialised ) initialiseInternal();
    
  if ( m_print ) { 
    std::cout << "ConfAnalysis::execute() \t " << name() 
	      << "\tref "  <<  reftracks.size() 
	      << "\ttest " << testtracks.size();
    if ( groi ) std::cout << "\tgroi "  << groi << " " << *groi;
    std::cout << std::endl;
  }    

  //  std::cout << "ConfAnalysis (resolutions really) filling " << std::endl;

  // should have these as a class variable   
  static std::string  varName[16] = { "pT",   "eta",  "phi",  "z0",    "d0",     "a0",  
				      "nsct", "npix", "nsi",  "ntrt",  "nstraw", 
				      "dd0",  "da0",  "dz0",  "deta",  "dphi" };  

  std::map<std::string, TH1F*>::iterator hmitr = m_histos.find("ntracks");
  if ( hmitr!=m_histos.end() )   hmitr->second->Fill( reftracks.size() );

  hmitr = m_histos.find("ntracks_rec");
  if ( hmitr!=m_histos.end() )   hmitr->second->Fill( testtracks.size() );

  bool dump = false;

  Nreco += testtracks.size();
  Nref  += reftracks.size();


  //  std::cout << "ConfAnalysis ref tracks " << std::endl; 

  // why don't we use this vertex position any more ???
  // Nref = 0;
  // for ( int i=reftracks.size() ; i-- ; ) {
  //  double phit = reftracks[i]->phi();  
  //  //  double a0t  =  reftracks[i]->a0() + sin(phit)*m_xBeamReference - cos(phit)*m_yBeamReference; 
  //  //   if ( std::fabs(a0t)<a0 ) Nref++; ???
  // }

  //  if ( testtracks.size() ) std::cout << "NTRACKS " << testtracks.size() << std::endl;

  for ( int i=reftracks.size() ; i-- ; ) { 

    /// fill roi residuals

    if ( groi!=0 ) { 
      //  std::cout << "ConfAnalysis::Fill() groi " << *groi << std::endl;

      double deta = reftracks[i]->eta() - groi->eta();
      double dphi = wrapphi( reftracks[i]->phi() - groi->phi() );
      double dzed = reftracks[i]->z0() - groi->zed();
      
      rRoi_deta_vs_eta->Fill( groi->eta(), deta );
      rRoi_dphi_vs_eta->Fill( groi->eta(), dphi );
      rRoi_dzed_vs_eta->Fill( groi->eta(), dzed );
    }

    /// kinematics
    double ipTt = 1./(reftracks[i]->pT()/1000.);     
    double pTt  = reftracks[i]->pT()/1000; 

    double etat = reftracks[i]->eta(); 
    double phit = reftracks[i]->phi(); 

    double thetat = 2*std::atan( exp( (-1)*etat ) );

    /// correct the tracks during creation rather than during the analysis
    ///    double z0t = reftracks[i]->z0()+((std::cos(phit)*m_xBeamReference + std::sin(phit)*m_yBeamReference)/std::tan(thetat));    
    ///    double d0t  = reftracks[i]->a0(); 
    ///    double a0t  = reftracks[i]->a0() + std::sin(phit)*m_xBeamReference - std::cos(phit)*m_yBeamReference; 

    /// BUT we should still allow them to be corrected, but afetr setting with a flag instead

    double z0t  = reftracks[i]->z0();
    double d0t  = reftracks[i]->a0() - std::sin(phit)*m_xBeamReference + std::cos(phit)*m_yBeamReference;  
    double a0t  = reftracks[i]->a0();

    if ( m_xBeamReference!=0 && m_yBeamReference!=0 ) { 
      /// this is such a mess - why, why, why, why, why oh why, can't we just have one convention 
      /// and stick to it.
      z0t = reftracks[i]->z0()+((std::cos(phit)*m_xBeamReference + std::sin(phit)*m_yBeamReference)/std::tan(thetat));    
      d0t  = reftracks[i]->a0(); 
      a0t  = reftracks[i]->a0() + std::sin(phit)*m_xBeamReference - std::cos(phit)*m_yBeamReference; 
    }

    //   if ( m_lfirst ) { 
    //       std::cout << "\na0t " << a0t << "(shifted " << d0t << ")" << std::endl;
    //       std::cout << "\nz0t " << z0t                              << std::endl;
    
    //       std::cout << "\txBeamReference " << m_xBeamReference << "\tyBeamReference " << m_yBeamReference << std::endl;  
    //   }
    


    //    std::cout << "a0t " << a0t << std::endl;;

    rChi2prob->Fill( pTt, TMath::Prob(reftracks[i]->chi2(),reftracks[i]->dof()) ); 
    rChi2->Fill( pTt, reftracks[i]->chi2() ); 
    rChi2dof->Fill( pTt, reftracks[i]->chi2()/reftracks[i]->dof() );

    //    static double xbeamref = 0;
    //    if ( m_lfirst || m_xBeamReference!=xbeamref) { 
    //      std::cout << __FUNCTION__ << "\tbeamline " << m_xBeamReference << " " << m_yBeamReference << " (ref)" << std::endl;
    //    }
    //    xbeamref = m_xBeamReference;

 
    /// error estimates
    double dpTt  = reftracks[i]->dpT()/1000;
    double detat = reftracks[i]->deta();
    double dphit = reftracks[i]->dphi();

    //RoI variables
    float droi_detat = groi->eta() - reftracks[i]->eta();
    float droi_dphit = groi->phi() - reftracks[i]->phi();
    if ( droi_dphit<-M_PI ) droi_dphit +=2*M_PI;
    if ( droi_dphit>M_PI )  droi_dphit -=2*M_PI;
    float droi_dRt = std::sqrt(droi_dphit*droi_dphit + droi_detat*droi_detat);

    
    //    double dz0t = reftracks[i]->dz0()+((std::cos(phit)*m_xBeamReference + std::sin(phit)*m_yBeamReference)/std::tan(thetat));
    //    double dd0t = reftracks[i]->da0();
    double dz0t = reftracks[i]->dz0();
    double dd0t = reftracks[i]->da0() - std::sin(phit)*m_xBeamReference + std::cos(phit)*m_yBeamReference;
    
    // this will be changed when we know the beam spot position
    //   double a0t  =  reftracks[i]->a0() + sin(phit)*m_xBeam - cos(phit)*m_yBeam; 
    double da0t = reftracks[i]->da0();

#if 0
    std::cout << "etat = " << etat << " +/- " << detat << std::endl;
    std::cout << "phit = " << phit << " +/- " << dphit << std::endl;
    std::cout << "z0t = " << z0t << " +/- " << dz0t << std::endl;
    std::cout << "d0t = " << d0t << " +/- " << dd0t << std::endl;
    std::cout << "a0t = " << a0t << " +/- " << da0t << std::endl;
    std::cout << "pTt = " << pTt << " +/- " << dpTt << std::endl;
#endif

    if ( std::fabs(a0t)>a0 ) continue;

    //    double chi2t = reftracks[i]->chi2(); 
    //    hchi2->Fill( chi2t );

    double nsctt = reftracks[i]->sctHits(); 
    double npixt = reftracks[i]->pixelHits(); 
    double nsit  = reftracks[i]->pixelHits() * 0.5 + reftracks[i]->sctHits(); 

    double nsctht = reftracks[i]->sctHoles(); 
    double npixht = reftracks[i]->pixelHoles(); 
    double nsiht  = reftracks[i]->pixelHoles() + reftracks[i]->sctHoles(); 

    double nbl    = reftracks[i]->bLayerHits();
    double nblh   = ( ( reftracks[i]->expectBL() && reftracks[i]->bLayerHits()<1 ) ? 1 : 0 ); 

    //    double ntrtt   = reftracks[i]->trHits(); 
    double nstrawt = reftracks[i]->strawHits(); 

    //    double ts_scale = (ts-1260400000)*3000.0/(1260700000-1260400000); 

    //    std::cout << "Fill h2 " << " " << h2m << " " << *reftracks[i] << std::endl; 

    h2->Fill( phit, d0t );
    h2a0->Fill( phit, a0t );

    rd0_vs_phi->Fill( phit, a0t );

    const TIDA::Track* matchedreco = matcher->matched(reftracks[i]); 

    //    std::cout << "\t\tConfAnalysis " << name() << "\t" << i << " " << *reftracks[i] << " -> ";        

    // raw reference track distributions 
    double vpart[16] = { std::fabs(pTt), etat, phit, z0t, d0t, a0t, nsctt, npixt, nsctt, nsit, nstrawt, dd0t, da0t, dz0t, detat, dphit };  
  
    /// NB: the dd0, da0 etc plots will be filled only for ref tracks 
    ///     with *matched* test tracks 
    for ( int it=0 ; it<11 ; it++ ) { 
      // std::string hname = varName[it];
      // std::map<std::string, TH1F*>::iterator hmitr = m_histos.find(hname);
      //  if ( hmitr!=m_histos.end() )   hmitr->second->Fill( vpart[it] );

      if ( TH1F* hptr = find( varName[it] ) ) hptr->Fill( vpart[it] ); 
      else std::cerr << "hmmm histo " << varName[it] << " not found" << std::endl;
    }  


    rnpix_eta->Fill( etat, npixt*0.5 );
    rnsct_eta->Fill( etat, nsctt*1.0 );
    rntrt_eta->Fill( etat, nstrawt*1.0 );
    rnsihit_eta->Fill( etat, npixt*0.5 + nsctt*1.);
    
    rnpix_phi->Fill(  phit, npixt*0.5 );
    rnsct_phi->Fill(  phit, nsctt*1.0 );
    rntrt_phi->Fill(  phit, nstrawt*1.0 );
    
    rnpix_pt->Fill( std::fabs(pTt), npixt*0.5 );
    rnsct_pt->Fill( std::fabs(pTt), nsctt*1.0 );
    rntrt_pt->Fill( std::fabs(pTt), nstrawt*1.0 );


    rnpix_d0->Fill( a0t, npixt*0.5 );
    rnsct_d0->Fill( a0t, nsctt*1.0 );
    rntrt_d0->Fill( a0t, nstrawt*1.0 );

    rnpixh_d0->Fill( a0t, npixht );
    rnscth_d0->Fill( a0t, nsctht );

    rnsi_pt->Fill( std::fabs(pTt), nsit );
    rnsih_pt->Fill( std::fabs(pTt), nsiht );

    rnsi_d0->Fill( a0t, nsit );
    rnsih_d0->Fill( a0t, nsiht );

    rnsi_eta->Fill( etat, nsit );
    rnsih_eta->Fill(etat, nsiht );

    rnbl_d0->Fill(  a0t, nbl  );
    rnblh_d0->Fill( a0t, nblh );

    
    rnpixh_pt->Fill( std::fabs(pTt), npixht );
    rnscth_pt->Fill( std::fabs(pTt), nsctht );

    rnpix_lb->Fill( gevent->lumi_block(), npixt*0.5 );
    
    if ( matchedreco )  {

      // efficiency histos
      eff_pt->Fill(std::fabs(pTt));
      eff_z0->Fill(z0t);
      eff_eta->Fill(etat);
      eff_phi->Fill(phit);
      eff_d0->Fill(d0t);
      eff_a0->Fill(a0t);
      eff_roi_deta->Fill(droi_detat);
      eff_roi_dphi->Fill(droi_dphit);
      eff_roi_dR->Fill(droi_dRt);

      // signed pT
      if ( pTt<0 ) eff_ptm->Fill(std::fabs(pTt));
      else         eff_ptp->Fill(std::fabs(pTt));

      eff_vs_mult->Fill( Nref );

      //    eff_vs_lb->Fill( rmap[r]+lb );
      // eff_vs_lb->Fill( ts_scale );
      //   eff_vs_lb->Fill( ts );

      eff_vs_lb->Fill( gevent->lumi_block() );

      Nmatched++;

      /// fill residual histos

      /// kinematics
      double pTr  = matchedreco->pT()/1000;
      double etar = matchedreco->eta();
      double phir = matchedreco->phi();
      //double z0r  = matchedreco->z0() + std::cos(phir)*m_xBeamTest + std::sin(phir)*m_yBeamTest; ; 
      double thetar = 2*std::atan( exp( (-1)*etar) );

      //      double z0r    = matchedreco->z0()+((std::cos(phir)*m_xBeamTest + std::sin(phir)*m_yBeamTest)/std::tan(thetar));    
      //      double d0r  = matchedreco->a0(); 
      //      double a0r  = matchedreco->a0() + sin(phir)*m_xBeamTest - cos(phir)*m_yBeamTest; // this will be changed when we know the beam spot position

      
      


      //      static bool tfirst = true;
      //      static double xbeamtest = 0;
      //      if ( m_lfirst || xbeamtest!=m_xBeamTest) { 
      //	std::cout << __FUNCTION__ << "\tbeamline " << m_xBeamTest << " " << m_yBeamTest << " (test)" << std::endl;
      //      }
      //      xbeamtest = m_xBeamTest;

      double d0r  = 0;
      double a0r  = 0;
      double z0r  = 0;
      
      d0r  = matchedreco->a0()  - sin(phir)*m_xBeamTest + cos(phir)*m_yBeamTest; // this will be changed when we know the beam spot position 
      a0r  = matchedreco->a0();
      z0r  = matchedreco->z0();

      if ( m_xBeamTest!=0 && m_yBeamTest!=0 ) {
	d0r  = matchedreco->a0(); 
	a0r  = matchedreco->a0() + sin(phir)*m_xBeamTest - cos(phir)*m_yBeamTest; // this will be changed when we know the beam spot position
	z0r  = matchedreco->z0()+((std::cos(phir)*m_xBeamTest + std::sin(phir)*m_yBeamTest)/std::tan(thetar));    
      }


      //      if ( m_lfirst ) { 
      //	std::cout << "\na0r " << a0r << "(shifted " << d0r << ")" << std::endl;
      //	std::cout << "\nz0r " << z0r                              << std::endl;
      //	std::cout << "\txBeamReference " << m_xBeamReference << "\tyBeamReference " << m_yBeamReference << std::endl;  
      //      }


      //      static int it=0;

      //      std::cout << "it " << it++ << std::endl; 
      
      //      m_lfirst = false;

      double nsctr = matchedreco->sctHits(); 
      double npixr = matchedreco->pixelHits(); 
      double nsir  = matchedreco->pixelHits() * 0.5 + matchedreco->sctHits(); 
      
      double nscthr = matchedreco->sctHoles(); 
      double npixhr = matchedreco->pixelHoles(); 


      //double ntrtr   = matchedreco->trHits(); 
      double nstrawr = matchedreco->strawHits(); 

      /// kinematic error estimates

      double dpTr  = matchedreco->dpT()/1000;
      double detar = matchedreco->deta();
      double dphir = matchedreco->dphi();

      //      double dz0r  = matchedreco->dz0()+((std::cos(phir)*m_xBeamTest + std::sin(phir)*m_yBeamTest)/std::tan(thetar));
      //      double dd0r  = matchedreco->da0();
      //      double da0r  = matchedreco->da0() + sin(phir)*m_xBeamTest - cos(phir)*m_yBeamTest;

      double dz0r  = matchedreco->dz0();
      double dd0r  = matchedreco->da0() - sin(phir)*m_xBeamTest + cos(phir)*m_yBeamTest;
      double da0r  = matchedreco->da0();

#if 0
      std::cout << "etar = " << etar << " +/- " << detar << std::endl;
      std::cout << "phir = " << phir << " +/- " << dphir << std::endl;
      std::cout << "pTr = " << pTr << " +/- " << dpTr << std::endl;
      std::cout << "a0r = " << a0r << " +/- " << da0r << std::endl;
      std::cout << "d0r = " << d0r << " +/- " << dd0r << std::endl;
      std::cout << "z0r = " << z0r << " +/- " << dz0r << std::endl;
#endif

      if ( h2m ) h2m->Fill( phit, d0t );

      rd0_vs_phi_rec->Fill( phir, a0r );


      /// fill them all the resplots from a loop ...
      double resfiller[7] = { ipTt, pTt, etat, z0t, double(NvtxCount), double(Nvtxtracks), phit };
      
      for ( int irfill=0 ; irfill<7 ; irfill++ ) { 
        rphires[irfill]->Fill( resfiller[irfill],  phir-phit );
        riptres[irfill]->Fill( resfiller[irfill],  1/pTr-1/pTt );
        retares[irfill]->Fill( resfiller[irfill],  etar-etat );
        rptres[irfill]->Fill(  resfiller[irfill],  pTr-pTt );
        rzedres[irfill]->Fill( resfiller[irfill],  z0r-z0t );
        rd0res[irfill]->Fill(  resfiller[irfill],  a0r-a0t );
      }
      

      double lb = gevent->lumi_block();

      rzedreslb->Fill( lb,  z0r-z0t );

      for ( int irfill=0 ; irfill<6 ; irfill++ ) { 
        rphiresPull[irfill]->Fill( resfiller[irfill], (phir - phit) / sqrt( (dphit*dphit) + (dphir*dphir) ) );
        retaresPull[irfill]->Fill( resfiller[irfill], (etar - etat) / sqrt( (detat*detat) + (detar*detar) ) );
        rptresPull[irfill]->Fill(  resfiller[irfill], (pTr - pTt) / sqrt( (dpTt*dpTt) + (dpTr*dpTr) ) );
        rzedresPull[irfill]->Fill( resfiller[irfill], (z0r - z0t) / sqrt( (dz0t*dz0t) + (dz0r*dz0r) ) );
        rd0resPull[irfill]->Fill(  resfiller[irfill], (a0r - a0t) / sqrt( (da0t*da0t) + (da0r*da0r) ) );	
      }
      
        
      
      rDz0res[0]->Fill( std::fabs(pTt), dz0r-dz0t );  
      rDz0res[1]->Fill( etat, dz0r-dz0t );  
      rDz0res[2]->Fill( z0t, dz0r-dz0t );  
      rDz0res[3]->Fill( phit, dz0r-dz0t );  
      

      if ( dumpflag ) {
	std::ostream& dumpstream = dumpfile; 
	if ( dz0t>0 && std::fabs( dz0r-dz0t )>0.04 ) { 
	  dump = true;
	  dumpstream << "POOR sigma(z0) agreement \n\trefrack:  " << *reftracks[i] << "\n\ttestrack: " << *matchedreco << std::endl; 
	  //	    std::cout << "dz0r dz0t" << dz0r << "\t" << dz0t << std::endl;
	}
      }
      
	
      /// rDx0res[3] = { vs pt, vs eta, vs zed } 
      rDd0res[0]->Fill( std::fabs(pTt), dd0r-dd0t );  
      rDd0res[1]->Fill( etat, dd0r-dd0t );  
      rDd0res[2]->Fill( z0t, dd0r-dd0t );  
      rDd0res[3]->Fill( d0t, dd0r-dd0t );  
      rDd0res[4]->Fill( phit, dd0r-dd0t );  
      


      
      rDa0res[0]->Fill( std::fabs(pTt), da0r-da0t );  
      rDa0res[1]->Fill( etat, da0r-da0t );  
      rDa0res[2]->Fill( z0t, da0r-da0t );  
      rDa0res[3]->Fill( da0t, da0r-da0t );  
      
      
      double _Deltaphi = 2*M_PI/NMod;
	
      
      double phistart = 11.0819;
      if ( NMod==22 ) phistart = 7.05803;

      double _phit = phit - phistart*M_PI/180;
      
      if ( _phit<-M_PI ) _phit += 2*M_PI;
      
      double iphi = _phit - (_Deltaphi*int((_phit+M_PI)/_Deltaphi) - M_PI);
      
      //	double iphi = phit - M_PI*int(7*(phit+M_PI)/M_PI)/7 - 11.0819*M_PI/180 + M_PI;
      //	double iphi = phit - M_PI*int(7*phit/M_PI)/7.0 - 11.0819*M_PI/180;
      
      rDa0res[4]->Fill( phit, da0r-da0t );  
      rDa0res[5]->Fill( iphi, da0r-da0t );  
      
      rDa0res[6]->Fill( iphi, da0t );  
      rDa0res[7]->Fill( iphi, da0r );  
      
      //ADDED BY JK
      //-----
      //	std::cout << "rphires.size() = " << rphires.size() << std::endl;
      rzedres[rphires.size()]->Fill( resfiller[1], z0r-z0t );
      rzedres[rphires.size()+1]->Fill( resfiller[1], z0r-z0t );
      rd0res[rphires.size()]->Fill( resfiller[1], a0r-a0t );
      rd0res[rphires.size()+1]->Fill( fabs(resfiller[1]), a0r-a0t ); //
      
      rzedresPull[rphires.size()-1]->Fill(   resfiller[1],       (z0r - z0t) / std::sqrt( (dz0t*dz0t) + (dz0r*dz0r) ) );
      rzedresPull[rphires.size()]->Fill( fabs(resfiller[1]), (z0r - z0t) / std::sqrt( (dz0t*dz0t) + (dz0r*dz0r) ) );
      rd0resPull[rphires.size()-1]->Fill(    resfiller[1],       (a0r - a0t) / std::sqrt( (da0t*da0t) + (da0r*da0r) ) );
      rd0resPull[rphires.size()]->Fill(  fabs(resfiller[1]), (a0r - a0t) / std::sqrt( (da0t*da0t) + (da0r*da0r) ) );
      
      //-----
	
      rnpix_eta_rec->Fill(  etat, npixr*0.5 );
      rnsct_eta_rec->Fill(  etat, nsctr*1.0 );
      rntrt_eta_rec->Fill(  etat, nstrawr*1.0 );
      rnsihit_eta_rec->Fill(  etat, npixr*0.5 + nsctr*1.0);

      rnpix_phi_rec->Fill(  phit, npixr*0.5 );
      rnsct_phi_rec->Fill(  phit, nsctr*1.0 );
      rntrt_phi_rec->Fill(  phit, nstrawr*1.0 );

      rnpix_pt_rec->Fill( std::fabs(pTt), npixr*0.5 );
      rnsct_pt_rec->Fill( std::fabs(pTt), nsctr*1.0 );
      rntrt_pt_rec->Fill( std::fabs(pTt), nstrawr*1.0 );

      rnpixh_pt_rec->Fill( std::fabs(pTt), npixhr*0.5 );
      rnscth_pt_rec->Fill( std::fabs(pTt), nscthr*1.0 );


      eff_vs_ntracks->Fill( Nvtxtracks );
      eff_vs_ntracks2->Fill( Nvtxtracks );
      n_vtx_tracks->Fill( Nvtxtracks );

      eff_vs_nvtx->Fill( NvtxCount );
      n_vtx->Fill( NvtxCount );
      double mu_val = gevent->mu();
      //std::cout << "<mu>\t" <<  mu_val << std::endl;
      eff_vs_mu->Fill( mu_val );
      mu->Fill( mu_val );

      //    hnpix_v_sct->Fill( nsctt*0.5, npixt*0.5 );


      double vres[6] = { 1.0/std::fabs(pTr)-1.0/std::fabs(pTt), etar-etat, phir-phit, z0r-z0t, d0r-d0t, a0r-a0t };
      for ( int it=0 ; it<6 ; it++ ) { 
	if ( it==0 ) { 
	  find("ipT_res")->Fill( vres[0] ); 
	  find("spT_res")->Fill( 1.0/pTr-1.0/pTt ); 
	}
	if ( TH1F* hptr = find(varName[it]+"_res") ) hptr->Fill( vres[it] ); 
	else std::cerr << "hmmm histo " << varName[it]+"_res" << " not found" << std::endl;
      }  
      //2D plot                                                                                                                                                   
      if ( TH2F* hptr = find2D("eta_phi_rec") ) {
        hptr->Fill( etar,phir );
        hptr->GetXaxis()->SetTitle("#eta");
        hptr->GetYaxis()->SetTitle("#phi");
        //hptr->SetFillStyle("COLZ");                                                                                                                             
      }
      if ( TH2F* hptr = find2D("phi_d0_Rec") ) {
        hptr->Fill( phir,d0r );
        hptr->GetXaxis()->SetTitle("#phi");
        hptr->GetYaxis()->SetTitle("d_{0} [mm]");
        //hptr->SetFillStyle("COLZ");                                                                                                                             
      }
    
      // raw matched test track errors                                                                                                                            
      if ( TH1F* hptr = find("dpT_rec") ) hptr->Fill(dpTr);
      if ( TH1F* hptr =find("deta_rec"))  hptr->Fill(detar);
      if ( TH1F* hptr =find("dphi_rec"))  hptr->Fill(dphir);
      if ( TH1F* hptr =find("dz0_rec"))   hptr->Fill(dz0r);
      if ( TH1F* hptr =find("dd0_rec"))   hptr->Fill(dd0r);
      if ( TH1F* hptr =find("da0_rec"))   hptr->Fill(da0r);

      // raw matched reference track errors                                                                                                                       
      if ( TH1F* hptr = find("dpT") ) hptr->Fill(dpTt);
      if ( TH1F* hptr = find("deta")) hptr->Fill(detat);
      if ( TH1F* hptr = find("dphi")) hptr->Fill(dphit);
      if ( TH1F* hptr = find("dz0"))  hptr->Fill(dz0t);
      if ( TH1F* hptr = find("dd0"))  hptr->Fill(dd0t);
      if ( TH1F* hptr = find("da0"))  hptr->Fill(da0t);


      if ( TH1F* hptr = find("dd0_res"))  hptr->Fill(dd0r-dd0t);
      if ( TH1F* hptr = find("da0_res"))  hptr->Fill(da0r-da0t);
      if ( TH1F* hptr = find("dz0_res"))  hptr->Fill(dz0r-dz0t);
      
      double Dd0 = dd0r-dd0t;
      double Da0 = da0r-da0t;
	
      double Ddphi = dphir - dphit;

      hphivsDd0res[2]->Fill( phit ); 

      //      if ( matchedreco->bLayerHits()<=3 ) std::cout << "\nov2\t" << Dd0 << " " << *reftracks[i] << std::endl;
      //      else                                std::cout << "\nov4\t" << Dd0 << " " << *reftracks[i] << std::endl;



      if (  PRINT_BRESIDUALS ) { 
	if ( matchedreco->bLayerHits()<=3 ) std::cout << "\nov2\t" << Dd0 << " " << *matchedreco << std::endl;
	else                                std::cout << "\nov4\t" << Dd0 << " " << *matchedreco << std::endl;
      }

      if ( std::fabs(Dd0)<0.01 ) { 
	hphivsDd0res[0]->Fill( phit );

	if ( PRINT_BRESIDUALS ) { 
	  std::cout << "close residual " << Dd0 << " " << Ddphi
		    << " "<< reftracks[i]->bLayerHits()-matchedreco->bLayerHits()
		    << " "<< reftracks[i]->pixelHits()-matchedreco->pixelHits();
	  std::cout << "\nccr\t" << Dd0 << " " << Ddphi << " " << *reftracks[i];
	  std::cout << "\ncct\t" << Dd0 << " " << Ddphi << " " << *matchedreco << std::endl;
	}
      } 
      else { 
	hphivsDd0res[1]->Fill( phit );  
	if ( PRINT_BRESIDUALS ) { 
	  std::cout << "far   residual " << Dd0 << " " << Ddphi
		    << " "<< reftracks[i]->bLayerHits()-matchedreco->bLayerHits()
		    << " "<< reftracks[i]->pixelHits()-matchedreco->pixelHits();
	  std::cout << "\nffr\t" << Dd0 << " " << Ddphi << " " << *reftracks[i];
	  std::cout << "\nfft\t" << Dd0 << " " << Ddphi << " " << *matchedreco << std::endl;
	}
      }


      hphivsDa0res[2]->Fill( iphi ); 
      if ( std::fabs(Da0)<0.01 ) hphivsDa0res[0]->Fill( iphi ); 
      else                       hphivsDa0res[1]->Fill( iphi );  
 

      // pull stats                                                                                                                                               
      double pull_pt  = (pTr - pTt) / std::sqrt( (dpTt*dpTt) + (dpTr*dpTr) );
      double pull_eta = (etar - etat) / std::sqrt( (detat*detat) + (detar*detar) );
      double pull_phi = (phir - phit) / std::sqrt( (dphit*dphit) + (dphir*dphir) );
      double pull_z0  = (z0r - z0t) / std::sqrt( (dz0t*dz0t) + (dz0r*dz0r) );
      double pull_d0  = (d0r - d0t) / std::sqrt( (dd0t*dd0t) + (dd0r*dd0r) );
      double pull_a0  = (a0r - a0t) / std::sqrt( (da0t*da0t) + (da0r*da0r) );

      if ( TH1F* hptr = find("pT_pull") ) hptr->Fill(pull_pt);
      if ( TH1F* hptr = find("eta_pull")) hptr->Fill(pull_eta);
      if ( TH1F* hptr = find("phi_pull")) hptr->Fill(pull_phi);
      if ( TH1F* hptr = find("z0_pull")) hptr->Fill(pull_z0);
      if ( TH1F* hptr = find("d0_pull")) hptr->Fill(pull_d0);
      if ( TH1F* hptr = find("a0_pull")) hptr->Fill(pull_a0);

      // pull stats - SIMPLE VERSION                                                                                                                             
      double pull_pt_simp  = (pTr - pTt) / sqrt( dpTr*dpTr );
      double pull_eta_simp = (etar - etat) / sqrt( detar*detar );
      double pull_phi_simp = (phir - phit) / sqrt( dphir*dphir );
      double pull_z0_simp  = (z0r - z0t) / sqrt( dz0r*dz0r );
      double pull_d0_simp  = (d0r - d0t) / sqrt( dd0r*dd0r );
      double pull_a0_simp  = (a0r - a0t) / sqrt( da0r*da0r );

      if ( TH1F* hptr = find("pT_pull_simple") ) hptr->Fill(pull_pt_simp);
      if ( TH1F* hptr = find("eta_pull_simple")) hptr->Fill(pull_eta_simp);
      if ( TH1F* hptr = find("phi_pull_simple")) hptr->Fill(pull_phi_simp);
      if ( TH1F* hptr = find("z0_pull_simple"))  hptr->Fill(pull_z0_simp);
      if ( TH1F* hptr = find("d0_pull_simple"))  hptr->Fill(pull_d0_simp);
      if ( TH1F* hptr = find("a0_pull_simple"))  hptr->Fill(pull_a0_simp);


      if ( TH1F* hptr = find("etai_res") ) hptr->Fill( etat-etar ); 


      double Delphi = phit-phir;
      double Deleta = etat-etar;

      if ( Delphi<-M_PI ) Delphi+=2*M_PI;
      if ( Delphi>M_PI ) Delphi -=2*M_PI;

      double DeltaR = std::sqrt(Delphi*Delphi+Deleta*Deleta);

      hDeltaR->Fill(DeltaR);

      mdeltaR_v_eta->Fill(etat, DeltaR);
      mdeltaR_v_pt->Fill(pTt, DeltaR);

      // in this loop over the reference tracks, could fill efficiency 
      // histograms

      //       eff_vs_lb->Fill( rmap[r]+lb );

      if ( TH1F* hptr = find("nsi_matched"))  hptr->Fill(nsir);

      /// matched track distributions

    }
    else {
      
      /// fill the track occupancies etc for the missed tracks

      rnpix_pt_bad->Fill( std::fabs(pTt), npixt*0.5 );
      rnsct_pt_bad->Fill( std::fabs(pTt), nsctt*1.0 );
      rntrt_pt_bad->Fill( std::fabs(pTt), nstrawt*1.0 );
      
      rChi2prob_bad->Fill( pTt, TMath::Prob(reftracks[i]->chi2(),reftracks[i]->dof()) ); 
      rChi2_bad->Fill( pTt, reftracks[i]->chi2() ); 
      rChi2dof_bad->Fill( pTt, reftracks[i]->chi2()/reftracks[i]->dof() );
      

      // fill efficiencies with unmatched histos
      //       std::cout << "NULL" << std::endl;
      eff_pt->FillDenom(std::fabs(pTt));
      eff_z0->FillDenom(z0t);
      eff_eta->FillDenom(etat);
      eff_phi->FillDenom(phit);
      eff_d0->FillDenom(d0t);
      eff_a0->FillDenom(a0t);

      // signed pT
      if ( pTt<0 ) eff_ptm->FillDenom(std::fabs(pTt));
      else         eff_ptp->FillDenom(std::fabs(pTt));

      eff_roi_deta->FillDenom(droi_detat);
      eff_roi_dphi->FillDenom(droi_dphit);
      eff_roi_dR->FillDenom(droi_dRt);

      eff_vs_mult->FillDenom( Nref );

      dump = false; 

      eff_vs_ntracks->FillDenom( Nvtxtracks );
      eff_vs_ntracks2->FillDenom( Nvtxtracks );
      n_vtx_tracks->Fill( Nvtxtracks );


      eff_vs_nvtx->FillDenom( NvtxCount );
      n_vtx->Fill( NvtxCount );

      double mu_val = gevent->mu();

      eff_vs_mu->FillDenom(mu_val);

      mu->Fill( mu_val );



      if ( dumpflag ) {
	std::ostream& dumpstream = dumpfile; 
  
	if ( std::fabs(pTt)>1 ) { 
	  dump = true; 

	  hipt = true;
	  dumpstream << mname << "\tMISSING TRACK run " << r << "\tevent " << ev 
		    << "\tlb " << lb << "\tN vertices " << NvtxCount << std::endl;
	  dumpstream << mname << "\tMISSING TRACK RoI   " << *groi << std::endl;
	  dumpstream << mname << "\tMISSING TRACK Track " << *reftracks[i];
	  if ( std::fabs(pTt)>=30 ) dumpstream << "\tvery high pt";
	  if ( std::fabs(pTt)>4 &&
	       std::fabs(pTt)<30  ) dumpstream << "\t     high pt";
	  dumpstream << std::endl;

	  if ( std::fabs(pTt)>=20 ){
	    dumpstream << "Test tracks " << std::endl;
	    for (unsigned int ii=0; ii<testtracks.size(); ii++){
	      dumpstream << *testtracks[ii] << std::endl;
	    }
	  }
	}
      }


      //      eff_vs_lb->FillDenom( ts );
      eff_vs_lb->FillDenom( gevent->lumi_block() );
    }

  }


  // for fake/purity histograms, loop over the test tracks
  // and get the corresponding matched reference tracks from the 
  // reverse map in the TrackAssociator class  - revmatched() 

  static int icount = 0;

  //  if ( icount%1000 ) std::cout << "chain " << name() << "\t " << Nreco << " tracks" << std::endl;  
  // if ( icount%1000 ) 
  if ( m_print ) std::cout << "ConfAnalysis::execute() \t " << name() << "\t " << icount << " events\t " << testtracks.size() << " tracks (" << Nreco << ")" << "\n---------------" << std::endl;  

  icount++;

  for ( int i=testtracks.size() ; i-- ; ) { 

    //    std::cout << "\t\tConfAnalysis purity " << name() << "\t" << i << " " << *testtracks[i] << " -> ";

    //    double pTr  = std::fabs(testtracks[i]->pT()); 
    double pTr     = testtracks[i]->pT()/1000; 
    double etar    = testtracks[i]->eta(); 
    double phir    = testtracks[i]->phi(); 
    double thetar  = 2*std::atan( exp( (-1)*etar) );
    
    double z0r    = testtracks[i]->z0(); //  + ((std::cos(phir)*m_xBeamTest + std::sin(phir)*m_yBeamTest)/std::tan(thetar));
    double d0r    = testtracks[i]->a0() - sin(phir)*m_xBeamTest + cos(phir)*m_yBeamTest; // this will be changed when we know the beam spot position
    double a0r    = testtracks[i]->a0(); 
    //    double a0rp = testtracks[i]->a0() - sin(phir)*m_xBeam - cos(phir)*m_yBeam; // this will be changed when we know the beam spot position

    if ( m_xBeamTest!=0 && m_yBeamTest!=0 ) { 
      d0r  = testtracks[i]->a0(); 
      a0r  = testtracks[i]->a0() + sin(phir)*m_xBeamTest - cos(phir)*m_yBeamTest; // this will be changed when we know the beam spot position
      z0r  = testtracks[i]->z0()+((std::cos(phir)*m_xBeamTest + std::sin(phir)*m_yBeamTest)/std::tan(thetar));    
    }
    

    //    std::cout << "d0 " << d0r << "\tphi " << phir << "\tx " << m_xBeamTest << "\ty " << m_yBeamTest << std::endl;

    double nsctr = testtracks[i]->sctHits(); 
    double npixr = testtracks[i]->pixelHits(); 
    double nsir = testtracks[i]->pixelHits() * 0.5 + testtracks[i]->sctHits(); 

    double ntrtr   = testtracks[i]->trHits(); 
    double nstrawr = testtracks[i]->strawHits(); 

#if 0
    double dpTr_b  = testtracks[i]->dpT()/1000;
    double detar_b = testtracks[i]->deta();
    double dphir_b = testtracks[i]->dphi();
    double dz0r_b  = testtracks[i]->dz0(); // + ((std::cos(phir)*m_xBeamTest + std::sin(phir)*m_yBeamTest)/std::tan(thetar));
    double dd0r_b  = testtracks[i]->da0() - sin(phir)*m_xBeamTest + cos(phir)*m_yBeamTest;
    double da0r_b  = testtracks[i]->da0(); 


    std::cout << "pTr_b  = " << pTr  << " +/- " << dpTr_b  << std::endl;
    std::cout << "phir_b = " << phir << " +/- " << dphir_b << std::endl;
    std::cout << "z0r_b  = " << z0r  << " +/- " << dz0r_b  << std::endl;
    std::cout << "d0r_b  = " << d0r  << " +/- " << dd0r_b  << std::endl;
    std::cout << "a0r_b  = " << a0r  << " +/- " << da0r_b  << std::endl;
    std::cout << "etar_b = " << etar << " +/- " << detar_b << std::endl;
#endif

    //    double ts_scale = (ts-1260400000)*3000.0/(1260700000-1260400000); 

    //    z_vs_lb->Fill( rmap[r]+lb, z0r );
    z_vs_lb->Fill( ts, z0r );

    //    hnpix_v_sct_rec->Fill( nsctr*0.5, npixr*0.5 );

    if ( h2r )   h2r->Fill( phir, d0r );
    if ( h2a0r ) h2a0r->Fill( phir, a0r );

    const TIDA::Track* matchedref = matcher->revmatched(testtracks[i]); 

    //    if ( matchedref )  std::cout << *matchedref << std::endl;
    //    else               std::cout << "NULL" << std::endl;     

#if 1       
    // raw test track distributions 
    double vpart[11] = { std::fabs(pTr), etar, phir, z0r, d0r, a0r, nsctr, npixr, nsir, ntrtr, nstrawr };
    for ( int it=0 ; it<11 ; it++ ) { 
      // std::string hname = name()+"_"+varName[it]+"_rec";
      //      std::string hname = varName[it]+"_rec";
      //      std::map<std::string, TH1F*>::iterator hmitr = m_histos.find(hname);
      //      if ( hmitr!=m_histos.end() )   hmitr->second->Fill( vpar[it] );
      //      else std::cerr << "hmmm histo " << hname << " not found" << std::endl;
      if ( TH1F* hptr = find(varName[it]+"_rec") ) hptr->Fill( vpart[it] ); 
      else std::cerr << "hmmm histo " << varName[it]+"_rec" << " not found" << std::endl;
    }  
    //2D plot
    if ( TH2F* hptr = find2D("eta_phi_rec") ) {
        hptr->Fill( etar,phir );
	hptr->GetXaxis()->SetTitle("#eta");
	hptr->GetYaxis()->SetTitle("#phi");
	//hptr->SetFillStyle("COLZ");
    }
    if ( TH2F* hptr = find2D("phi_d0_rec") ) {
        hptr->Fill( phir,d0r );
	hptr->GetXaxis()->SetTitle("#phi");
	hptr->GetYaxis()->SetTitle("d_{0} [mm]");
	//hptr->SetFillStyle("COLZ");
    }
#endif    


    // purities
    if ( matchedref )  {

      //       std::cout << *matchedref << std::endl;

      purity_pt->Fill(std::fabs(pTr));
      purity_z0->Fill(z0r);
      purity_eta->Fill(etar);
      purity_phi->Fill(phir);
      purity_d0->Fill(d0r);
      purity_a0->Fill(a0r);

      //  hnpix_v_sct_match->Fill( nsctr*0.5, npixr*0.5 );

    }
    else { 
      //       std::cout << "NULL" << std::endl;
      purity_pt->FillDenom(std::fabs(pTr));
      purity_z0->FillDenom(z0r);
      purity_eta->FillDenom(etar);
      purity_phi->FillDenom(phir);
      purity_d0->FillDenom(d0r);
      purity_a0->FillDenom(a0r);
    }

  }

  if ( dump && m_print ) { 

    if ( m_print ) std::cout << "ConfAnalysis::execute() missed a high pT track - dumping tracks" << std::endl;

    for ( int i=reftracks.size() ; i-- ; ) {

      if ( std::fabs( reftracks[i]->pT() ) > 1000 ) { 
	std::cout << "\t dump " << *reftracks[i];
	const TIDA::Track* matchedreco = matcher->matched(reftracks[i]); 
	if ( matchedreco ) std::cout << " <--> " << *matchedreco << std::endl;
	else               std::cout << std::endl;
      }

    }

    for ( int i=testtracks.size() ; i-- ; ) {     
      const TIDA::Track* matchedref = matcher->revmatched(testtracks[i]); 
      if ( matchedref==0 ) std::cout << "\t\t\t\t\t " << *testtracks[i] << std::endl;      
    }

  }

  if ( m_print ) std::cout << "ConfAnalysis::execute() exiting" << std::endl;

}





