//
//   @file    Analysis_Tier0.cxx        
//                    
//   $Id: Analysis_Tier0.cxx   Thu 18 May 2017 15:35:34 CEST sutt $
//
//   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//   Copyright (C) 2017 M Sutton (sutt@cern.ch)    

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Tier0.h"

#include "TrigInDetAnalysisExample/ChainString.h"


Analysis_Tier0::Analysis_Tier0(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut), m_debug(false), m_eventid(0) 
{  }


void Analysis_Tier0::initialise() {

  ChainString cname = name();

  //  std::cout << "Analysis_Tier0::initialise() name " << name() << std::endl; 

#if 0
  std::cout << "\nAnalysis_Tier0:: chain specification: " << cname << "\t" << cname.raw() << std::endl;
  std::cout << "\tchain: " << cname.head()    << std::endl;
  std::cout << "\tkey:   " << cname.tail()    << std::endl;
  std::cout << "\troi:   " << cname.roi()     << std::endl;
  std::cout << "\tvtx:   " << cname.vtx()     << std::endl;
  std::cout << "\tte:    " << cname.element() << std::endl;
#endif

  m_debug = false;

  m_h_chain = new TH1F( "Chain", cname.c_str(), 5, 0, 5 );
  m_h_chain->GetXaxis()->SetBinLabel(1, "Nrois" );
  m_h_chain->GetXaxis()->SetBinLabel(2, "Nevents" );
  m_h_chain->GetXaxis()->SetBinLabel(3, "N ref tracks" );
  m_h_chain->GetXaxis()->SetBinLabel(4, "N matched tracks" );
  m_h_chain->GetXaxis()->SetBinLabel(5, "" ); //  spare

 
  /// archive the chain name

  addHistogram(m_h_chain);

  /// variable width bins for track occupancy
  
  double vnbins[81] = {
   -0.5,
    0.5,	1.5,	2.5,	3.5,	4.5,	5.5,	6.5,	7.5,	8.5,	9.5,
    10.5,	11.5,	12.5,	13.5,	14.5,	15.5,	16.5,	17.5,	18.5,	19.5,
    20.5,	21.5,	22.5,	23.5,	24.5,	25.5,	26.5,	27.5,	28.5,	29.5,
    31.5,	32.5,	33.5,	34.5,	36.5,	37.5,	39.5,
    40.5,	42.5,	43.5,	45.5,	47.5,	49.5,
    50.5,	52.5,	54.5,	57.5,	59.5,
    61.5,	63.5,	66.5,	69.5,
    71.5,	74.5,	77.5,
    80.5,	83.5,	86.5,
    90.5,	93.5,	97.5,
    100.5,	104.5,	108.5,
    113.5,	117.5,
    122.5,	126.5,
    131.5,	136.5,
    142.5,	147.5,
    153.5,	159.5,
    165.5,
    171.5,	178.5,
    185.5,
    192.5,
    200.5 };



  m_h_ntrk = new TH1F( "reftrk_N", "Reference tracks", 80, vnbins );

  addHistogram(m_h_ntrk);

  /// reference track distributions

  //  double pt_bins[25] = { 0, 1, 2, 3, 4, 8, 10, 12, 14, 16, 18, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100 }; 

  std::vector<double> ptbins;

  for ( int i=0 ; i<=25 ; i++ ) {

    double y = std::log10(1) + i*( std::log10(100)-std::log10(1) )/25;
    
    double x = std::pow(10, y);
    ptbins.push_back(x);

  }

  //  m_h_trkpT  = new TH1F("reftrk_pT" , "Reference track pT",  50,    0.,    100.);
  m_h_trkpT  = new TH1F("reftrk_pT" , "Reference track pT",  25,   &ptbins[0]   );
  m_h_trkphi = new TH1F("reftrk_phi", "Reference track Phi", 25,   -M_PI,   M_PI);
  m_h_trketa = new TH1F("reftrk_eta", "Reference track Eta", 25,   -2.5,     2.5) ;
  m_h_trkd0  = new TH1F("reftrk_d0" , "Reference track d0", 101,   -5.0,     5.0 );
  m_h_trkz0  = new TH1F("reftrk_z0" , "Reference track z0",  50,   -225.,    225.);

  m_h_trkdd0  = new TH1F("reftrk_dd0" , "Reference track sigma(d0)", 101,   -0.5,     0.5);
  m_h_trkdz0  = new TH1F("reftrk_dz0" , "Reference track sigma(z0)", 101,   -2.5,     2.5);

  m_h_trkd0sig = new TH1F("reftrk_d0sig" , "Reference track d0 significance", 101,   -5.,     5.);

  addHistogram(m_h_trkpT);
  addHistogram(m_h_trkphi);
  addHistogram(m_h_trketa);
  addHistogram(m_h_trkd0);
  addHistogram(m_h_trkz0);

  addHistogram(m_h_trkdd0);
  addHistogram(m_h_trkdz0);

  addHistogram(m_h_trkd0sig);

  /// test track distributions

  m_h_ntrk_rec = new TH1F( "testtrk_N", "Test tracks", 80, vnbins );

  addHistogram(m_h_ntrk_rec);

  //  m_h_trkpT_rec  = new TH1F("testtrk_pT" , "Test track pT",  25,    0.,   100.);
  m_h_trkpT_rec  = new TH1F("testtrk_pT" , "Test track pT",  25,   &ptbins[0]   );
  m_h_trkphi_rec = new TH1F("testtrk_phi", "Test track Phi", 25,   -M_PI,  M_PI);
  m_h_trketa_rec = new TH1F("testtrk_eta", "Test track Eta", 25,   -2.5,    2.5) ;
  m_h_trkd0_rec  = new TH1F("testtrk_d0" , "Test track d0", 101,   -5.0,    5.0 );
  m_h_trkz0_rec  = new TH1F("testtrk_z0" , "Test track z0",  50,   -225.,   225.);

  m_h_trkdd0_rec  = new TH1F("testtrk_dd0" , "Test track sigma(d0)", 101,   -0.5,     0.5);
  m_h_trkdz0_rec  = new TH1F("testtrk_dz0" , "Test track sigma(z0)", 101,   -2.5,     2.5);

  m_h_trkd0sig_rec = new TH1F("testtrk_d0sig" , "Test track d0 significance", 101,   -5.0,     5.0);


  addHistogram(m_h_trkpT_rec);
  addHistogram(m_h_trkphi_rec);
  addHistogram(m_h_trketa_rec);
  addHistogram(m_h_trkd0_rec);
  addHistogram(m_h_trkz0_rec);

  addHistogram(m_h_trkdd0_rec);
  addHistogram(m_h_trkdz0_rec);
  
  addHistogram(m_h_trkd0sig_rec);

  /// trigger tracking efficiencies

  double d0bins[40] = { -5.0,  -4.0,  -3.0,  -2.5,   
			-2.0,  -1.8,  -1.6,  -1.4,  -1.2,  
			-1.05, -0.95, -0.85, -0.75, -0.65, -0.55, -0.45, -0.35, -0.25, -0.15, -0.05, 
		 	 0.05,  0.15,  0.25,  0.35,  0.45,  0.55,  0.65,  0.75,  0.85,  0.95,  1.05,  
			 1.2,   1.4,   1.6,   1.8,   2.0,  
			 2.5,   3.0,   4.0,   5.0 };



  m_h_total_efficiency = new TProfile ("Eff_overall",  "total efficiency",  1, 0., 1.);

  // m_h_pTeff = new TProfile( "Eff_pT",     "pT efficiency",     25,    0.,  100.   );
  m_h_pTeff    = new TProfile( "Eff_pT",     "pT efficiency",     25,   &ptbins[0]   );
  m_h_etaeff   = new TProfile( "Eff_Eta",    "eta efficiency",    25,   -2.5,   2.5  );
  m_h_phieff   = new TProfile( "Eff_Phi",    "phi  efficiency",   25,   -M_PI, M_PI  );
  m_h_d0eff    = new TProfile( "Eff_d0",     "d0 efficiency",     39,   d0bins    );
  m_h_z0eff    = new TProfile( "Eff_z0",     "z0 efficiency",     50, -225.,  225.   );
  m_h_nVtxeff  = new TProfile( "Eff_nVtx",   "nVtx efficiency",   71,   -0.5,  70.5  );

  m_h_lbeff = new TProfile( "Eff_lb", "efficinecy vs lumiblock", 301, -0.5, 3009.5 );

  addHistogram( m_h_lbeff );

  m_h_trkvtx_x_lb = new TProfile( "trkvtx_x_vs_lb", "track vertex x vs lumiblock", 301, -0.5, 3009.5 );
  m_h_trkvtx_y_lb = new TProfile( "trkvtx_y_vs_lb", "track vertex y vs lumiblock", 301, -0.5, 3009.5 );
  m_h_trkvtx_z_lb = new TProfile( "trkvtx_z_vs_lb", "track vertex z vs lumiblock", 301, -0.5, 3009.5 );

  addHistogram( m_h_trkvtx_x_lb );
  addHistogram( m_h_trkvtx_y_lb );
  addHistogram( m_h_trkvtx_z_lb );

  /// han config too stufid to deal with spaces in histogram names
  m_h_npixvseta     = new TProfile("npix_vs_eta",      "offline npix vs eta;offline #eta;# mean number of offline pixel hits", 30,   -2.5,  2.5);
  m_h_npixvseta_rec = new TProfile("npix_vs_eta_rec",  "trigger npix vs eta;trigger #eta;# mean number of trigger pixel hits", 30,   -2.5,  2.5);

  m_h_nsctvseta     = new TProfile("nsct_vs_eta",      "offline nsct vs eta;offline #eta;# mean number of offline sct hits", 30,   -2.5,  2.5);
  m_h_nsctvseta_rec = new TProfile("nsct_vs_eta_rec",  "trigger nsct vs eta;trigger #eta;# mean number of trigger sct hits", 30,   -2.5,  2.5);

  m_h_ntrtvseta     = new TProfile("ntrt_vs_eta",      "offline ntrt vs eta;offline #eta;# mean number of offline trt hits", 30,   -2.5, 2.5 );
  m_h_ntrtvseta_rec = new TProfile("ntrt_vs_eta_rec",  "trigger ntrt vs eta;trigger #eta;# mean number of trigger trt hits", 30,   -2.5, 2.5 );

  m_h_npixvsphi     = new TProfile("npix_vs_phi",      "offline npix vs phi;offline #phi;# mean number of offline pixel hits", 30,   -M_PI,  M_PI);
  m_h_npixvsphi_rec = new TProfile("npix_vs_phi_rec",  "trigger npix vs phi;trigger #phi;# mean number of trigger pixel hits", 30,   -M_PI,  M_PI);

  m_h_nsctvsphi     = new TProfile("nsct_vs_phi",      "offline nsct vs phi;offline #phi;# mean number of offline sct hits", 30,   -M_PI,  M_PI);
  m_h_nsctvsphi_rec = new TProfile("nsct_vs_phi_rec",  "trigger nsct vs phi;trigger #phi;# mean number of trigger sct hits", 30,   -M_PI,  M_PI);

  m_h_ntrtvsphi     = new TProfile("ntrt_vs_phi",      "offline ntrt vs phi;offline #phi;# mean number of offline trt hits", 30,   -M_PI, M_PI );
  m_h_ntrtvsphi_rec = new TProfile("ntrt_vs_phi_rec",  "trigger ntrt vs phi;trigger #phi;# mean number of trigger trt hits", 30,   -M_PI, M_PI );

  m_h_npixvsd0     = new TProfile("npix_vs_d0",      "offline npix vs d0;offline #d0;# mean number of offline pixel hits", 39,  d0bins );
  m_h_npixvsd0_rec = new TProfile("npix_vs_d0_rec",  "trigger npix vs d0;trigger #d0;# mean number of trigger pixel hits", 39,  d0bins );

  m_h_nsctvsd0     = new TProfile("nsct_vs_d0",      "offline nsct vs d0;offline #d0;# mean number of offline sct hits", 39,   d0bins );
  m_h_nsctvsd0_rec = new TProfile("nsct_vs_d0_rec",  "trigger nsct vs d0;trigger #d0;# mean number of trigger sct hits", 39,   d0bins );



  m_h_nsihits_lb     = new TProfile( "nsihits_lb",     "offline n sihits vs lumiblock", 301, -0.5, 3009.5 );
  m_h_nsihits_lb_rec = new TProfile( "nsihits_lb_rec", "trigger n sihits vs lumiblock", 301, -0.5, 3009.5 );

  addHistogram( m_h_nsihits_lb );
  addHistogram( m_h_nsihits_lb_rec );





  addHistogram(m_h_total_efficiency);
  addHistogram(m_h_pTeff);
  addHistogram(m_h_etaeff);
  addHistogram(m_h_phieff);
  addHistogram(m_h_z0eff);
  addHistogram(m_h_d0eff);
  addHistogram(m_h_nVtxeff);

  addHistogram( m_h_npixvseta ); 
  addHistogram( m_h_npixvseta_rec ); 

  addHistogram( m_h_nsctvseta ); 
  addHistogram( m_h_nsctvseta_rec ); 

  addHistogram( m_h_ntrtvseta ); 
  addHistogram( m_h_ntrtvseta_rec ); 

  addHistogram( m_h_npixvsphi ); 
  addHistogram( m_h_npixvsphi_rec ); 

  addHistogram( m_h_nsctvsphi ); 
  addHistogram( m_h_nsctvsphi_rec ); 

  addHistogram( m_h_ntrtvsphi ); 
  addHistogram( m_h_ntrtvsphi_rec ); 

  addHistogram( m_h_npixvsd0 ); 
  addHistogram( m_h_npixvsd0_rec ); 

  addHistogram( m_h_nsctvsd0 ); 
  addHistogram( m_h_nsctvsd0_rec ); 

  /// trigger tracking differential resolutions

  /// change all these residiuals to be vs eta, rather than vs themselves

  //  m_h_pTres  = new TProfile("Res_pT", "pT residual;pT [Gev];pT_{trig}-pT_{ref} [GeV]",   25,    0.,   100.);
  // m_h_pTres  = new TProfile("Res_pT", "pT residual;pT [Gev];pT_{trig}-pT_{ref} [GeV]",      25,   &ptbins[0]   );
  // m_h_ipTres = new TProfile("Res_ipT", "ipT residual;pT^{-1} [Gev^{-1}];1/pT_{trig}-1/pT_{ref} [GeV^{-1}]", 55, -5, 5   );
  // h_etares = new TProfile("Res_eta", "Eta residual;#eta;#eta_{trig}-#eta_{ref}", 25,   -2.5,    2.5);
  // m_h_phires = new TProfile("Res_phi", "Phi residual;#phi;#phi_{trig}-#phi_{ref}", 25,   -M_PI, M_PI);
  // m_h_d0res  = new TProfile("Res_d0", "d0 residual;d0 [mm];d0_{trig}-d0_{ref} [mm]",   50,  -10.,    10.);
  // m_h_z0res  = new TProfile("Res_z0", "z0 residual;z0 [mm];z0_{trig}-z0_{ref} [mm]",   50, -225.,   225.);

  m_h_pTres  = new TProfile("Res_pT",  "pT residual;#eta_{ref};#Delta pT [Gev]",  25,   -2.5,  2.5   );
  m_h_ipTres = new TProfile("Res_ipT", "ipT residual;#eta_{ref};#Delta pT^{-1} [Gev^{-1}]", 25, -2.5, 2.5); 
  m_h_etares = new TProfile("Res_eta", "Eta residual;#eta_{ref};#Delta #eta",     25,   -2.5,  2.5);
  m_h_phires = new TProfile("Res_phi", "Phi residual;#eta_{ref};#Delta #phi",     25,   -2.5,  2.5);
  m_h_d0res  = new TProfile("Res_d0",  "d0 residual;#eta_{ref};#Delta d0 [mm]",   25,   -2.5,  2.5);
  m_h_z0res  = new TProfile("Res_z0",  "z0 residual;#eta_{ref};#Delta z0 [mm]",   25,   -2.5,  2.5);

  addHistogram(m_h_pTres);
  addHistogram(m_h_ipTres);
  addHistogram(m_h_etares);
  addHistogram(m_h_phires);
  addHistogram(m_h_d0res);
  addHistogram(m_h_z0res);

  /// residuals

  m_h_trkpT_residual  = new TH1F("residual_pT" , "track pT residual",  201, -100.0,    100.0  );
  m_h_trkipT_residual = new TH1F("residual_ipT", "track ipT residual",  55,   -5.5,      5.5  );
  m_h_trkphi_residual = new TH1F("residual_phi", "track Phi residual",  50,   -0.02,     0.02 );
  m_h_trketa_residual = new TH1F("residual_eta", "track Eta residual",  50,   -0.02,     0.02 );
  m_h_trkd0_residual  = new TH1F("residual_d0" , "track d0 residual ", 251,   -2.5,      2.5  );
  m_h_trkz0_residual  = new TH1F("residual_z0" , "track z0 residual",  401,  -20.0,     20.0  );

  m_h_trkdd0_residual  = new TH1F("residual_dd0" , "track sigma d0 residual ", 251,   -0.5,     0.5  );
  m_h_trkdz0_residual  = new TH1F("residual_dz0" , "track sigma z0 residual",  401,   -1.0,     1.0  );

  addHistogram(m_h_trkpT_residual);
  addHistogram(m_h_trkipT_residual);
  addHistogram(m_h_trkphi_residual);
  addHistogram(m_h_trketa_residual);
  addHistogram(m_h_trkd0_residual);
  addHistogram(m_h_trkz0_residual);

  addHistogram(m_h_trkdd0_residual);
  addHistogram(m_h_trkdz0_residual);

  m_h_npix     = new TH1F("npix",     "npix",     26,  -0.5,     25.5  );
  m_h_npix_rec = new TH1F("npix_rec", "npix_rec", 26,  -0.5,     25.5  );

  m_h_nsct     = new TH1F("nsct",     "nsct",     31,  -0.5,     30.5  );
  m_h_nsct_rec = new TH1F("nsct_rec", "nsct_rec", 31,  -0.5,     30.5  );

  m_h_nsihits     = new TH1F("nsiHits",     "nsiHits",     41,  -0.5,     40.5  );
  m_h_nsihits_rec = new TH1F("nsiHits_rec", "nsiHits_rec", 41,  -0.5,     40.5  );

  m_h_ntrt     = new TH1F("ntrt",     "ntrt",      91,  -0.5,     91.5  );
  m_h_ntrt_rec = new TH1F("ntrt_rec", "ntrt_rec",  91,  -0.5,     91.5  );

  addHistogram( m_h_npix );
  addHistogram( m_h_nsct );
  addHistogram( m_h_nsihits );
  addHistogram( m_h_ntrt );

  addHistogram( m_h_npix_rec );
  addHistogram( m_h_nsct_rec );
  addHistogram( m_h_nsihits_rec );
  addHistogram( m_h_ntrt_rec );

  /// miscelaneous histograms

  m_h_d0vsphi       = new TProfile( "d0_vs_phi_prof", "d0 vs phi_prof",  25, -M_PI, M_PI );
  m_h2d_d0vsphi     = 0; // new     TH2D( "d0_vs_phi",      "d0 vs phi",       25, -M_PI, M_PI, 50, -2.6, 2.6 );
  m_h2d_d0vsphi_rec = 0; // new     TH2D( "d0_vs_phi_rec",  "d0 vs phi rec",   25, -M_PI, M_PI, 50, -2.6, 2.6 );

  addHistogram( m_h_d0vsphi ); 
  //  addHistogram( m_h2d_d0vsphi ); 
  //  addHistogram( m_h2d_d0vsphi_rec ); 

  m_vtxanal = 0;

#if 1

  //  std::cout << "Analysis_Tier0 vertex info: " << name() << std::endl;  

  /// vertex analyses if required ...
  if ( name().find("vtx")!=std::string::npos || name().find("Vtx")!=std::string::npos ||
       name().find("vx")!=std::string::npos || name().find("Vx")!=std::string::npos ) { 

    // std::cout << "Analysis_Tier0 vertex info: " << name() << std::endl;  

    m_vtxanal = new VtxAnalysis("VTX");

    /// is this needed ? 
    store().insert( m_vtxanal, "VTX" );

    /// initialise the vtx analysis
    m_vtxanal->initialise();

    //    std::cout << "vtxanal obj  size() " << m_vtxanal->objects().size() << std::endl;
    //    std::cout << "vtxanal prof size() " << m_vtxanal->profs().size() << std::endl;

    /// store the historams
    for ( unsigned i=0 ; i<m_vtxanal->objects().size() ; i++ ) addHistogram( m_vtxanal->objects()[i] );
    for ( unsigned i=0 ; i<m_vtxanal->profs().size() ; i++ )   addHistogram( m_vtxanal->profs()[i] );
  }
#endif

}


void Analysis_Tier0::execute(const std::vector<TIDA::Track*>& referenceTracks,
			     const std::vector<TIDA::Track*>& testTracks,
			     TrackAssociator* associator) {
  
  /// Loop over reference tracks
  std::vector<TIDA::Track*>::const_iterator  reference    = referenceTracks.begin();
  std::vector<TIDA::Track*>::const_iterator  referenceEnd = referenceTracks.end();

  /// fill number of times this analysis was called - presumably 
  /// the number of passed RoIs for this chain 
  m_h_chain->Fill( 0.5 );

  if ( m_eventid != event()->event_number() ) { 
    /// if the event number has changed, this is a new event
    /// so update the event counts
    m_eventid = event()->event_number(); 
    m_h_chain->Fill( 1.5 );
  }

  m_h_ntrk->Fill( referenceTracks.size() );
  m_h_ntrk_rec->Fill( testTracks.size() );

  for( ; reference!=referenceEnd ; reference++ ) {
    
    // Get reference parameters
    double referenceEta = (*reference)->eta();
    double referencePhi = phi((*reference)->phi());
    double referenceZ0  = (*reference)->z0();
    double referenceD0  = (*reference)->a0();
    double referencePT  = (*reference)->pT();

    double referenceDZ0  = (*reference)->dz0();
    double referenceDD0  = (*reference)->da0();
    
    // Find matched tracks
    const TIDA::Track* test = associator->matched(*reference);

    double    eff_weight = 0;
    if (test) eff_weight = 1;

    m_h_total_efficiency->Fill(0.5, eff_weight );

    m_h_pTeff->Fill( std::fabs(referencePT)*0.001, eff_weight );
    m_h_z0eff->Fill( referenceZ0, eff_weight );
    m_h_d0eff->Fill( referenceD0, eff_weight );
    m_h_etaeff->Fill( referenceEta, eff_weight );
    m_h_phieff->Fill( referencePhi, eff_weight );
    m_h_nVtxeff->Fill( m_nVtx, eff_weight );

    m_h_lbeff->Fill( event()->lumi_block(), eff_weight );

    m_h_trkpT->Fill( std::fabs(referencePT)*0.001 );
    m_h_trketa->Fill( referenceEta );
    m_h_trkphi->Fill( referencePhi );
    m_h_trkd0->Fill( referenceD0 );
    m_h_trkz0->Fill( referenceZ0 );

    m_h_trkdd0->Fill( referenceDD0 );
    m_h_trkdz0->Fill( referenceDZ0 );

    if ( referenceDD0!=0 )  m_h_trkd0sig->Fill( referenceD0/referenceDD0 );

    m_h_npixvseta->Fill( referenceEta,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_h_nsctvseta->Fill( referenceEta,  (*reference)->sctHits() ); 
    m_h_ntrtvseta->Fill( referenceEta,  (*reference)->strawHits() ); 


    m_h_npixvsphi->Fill( referencePhi,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_h_nsctvsphi->Fill( referencePhi,  (*reference)->sctHits() ); 
    m_h_ntrtvsphi->Fill( referencePhi,  (*reference)->strawHits() ); 

    m_h_npixvsd0->Fill( referenceD0,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_h_nsctvsd0->Fill( referenceD0,  (*reference)->sctHits() ); 


    m_h_npix->Fill(  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_h_nsct->Fill(  (*reference)->sctHits() ); 
    m_h_nsihits->Fill(  (*reference)->siHits() ); 
    m_h_ntrt->Fill(  (*reference)->strawHits() ); 
   

    m_h_nsihits_lb->Fill( event()->lumi_block(), (*reference)->siHits() ); 
 
    m_h_d0vsphi->Fill(referencePhi, referenceD0 );
    //   m_h2d_d0vsphi->Fill(referencePhi, referenceD0 );

    m_h_chain->Fill(2.5);


    if(test){

      m_h_chain->Fill(3.5);

      /// NB: do we want to fill the actual *trigger* quantities, or the 
      /// offline quantities for the *matched* tracks?

      /// residual profiles vs the reference variable      
      // m_h_pTres->Fill( referencePT*0.001, (test->pT() - referencePT)*0.001 );
      // m_h_ipTres->Fill( 1000/referencePT, (1000/test->pT() - 1000/referencePT) );
      // m_h_etares->Fill( referenceEta, test->eta() - referenceEta );
      // m_h_phires->Fill( referencePhi, phi(test->phi() - referencePhi) );
      // m_h_d0res->Fill( referenceD0, test->a0() - referenceD0 );
      // m_h_z0res->Fill( referenceZ0, test->z0() - referenceZ0  );

      /// residual profiles vs eta - the more easy to understand
      m_h_pTres->Fill( referenceEta, (test->pT() - referencePT)*0.001 );
      m_h_ipTres->Fill( referenceEta, (1000/test->pT() - 1000/referencePT) );
      m_h_etares->Fill( referenceEta, test->eta() - referenceEta );
      m_h_phires->Fill( referenceEta, phi(test->phi() - referencePhi) );
      m_h_d0res->Fill( referenceEta, test->a0() - referenceD0 );
      m_h_z0res->Fill( referenceEta, test->z0() - referenceZ0  );

      m_h_trkvtx_x_lb->Fill( event()->lumi_block(), beamTestx() );
      m_h_trkvtx_y_lb->Fill( event()->lumi_block(), beamTesty() );
      m_h_trkvtx_z_lb->Fill( event()->lumi_block(), beamTestz() );

      //      std::cout << "SUTT beam x " << beamTestx() << " " << "\tx " << beamTesty() << " " <<  "\ty " << beamTestz() << std::endl;

#if 0
      /// reference tracks values for tracks with a reference track match (not test track values) 
      m_h_trkpT_rec->Fill( referencePT*0.001 );
      m_h_trketa_rec->Fill( referenceEta );
      m_h_trkphi_rec->Fill( referencePhi );
      m_h_trkd0_rec->Fill( referenceD0 );
      m_h_trkz0_rec->Fill( referenceZ0 );
     
#endif

      /// test track distributions for test tracks with a reference track match 
      m_h_trkpT_rec->Fill( std::fabs(test->pT())*0.001 );
      m_h_trketa_rec->Fill( test->eta() );
      m_h_trkphi_rec->Fill( test->phi() );
      m_h_trkd0_rec->Fill( test->a0() );
      m_h_trkz0_rec->Fill( test->z0() );

      m_h_trkdd0_rec->Fill( test->da0() );
      m_h_trkdz0_rec->Fill( test->dz0() );

      if ( test->da0()!=0 )  m_h_trkd0sig_rec->Fill( test->da0()/test->a0() );


      /// 1d residual distributions 
      m_h_trkpT_residual->Fill( (test->pT() - referencePT)*0.001 );
      m_h_trkipT_residual->Fill( (1000/test->pT() - 1000/referencePT) );
      m_h_trketa_residual->Fill( test->eta() - referenceEta );
      m_h_trkphi_residual->Fill( phi(test->phi() - referencePhi) );

      m_h_trkd0_residual->Fill(  test->a0() - referenceD0 );
      m_h_trkz0_residual->Fill( test->z0() - referenceZ0  );

      m_h_trkdd0_residual->Fill( test->da0() - referenceDD0 );
      m_h_trkdz0_residual->Fill( test->dz0() - referenceDZ0  );

      //      m_h2d_d0vsphi_rec->Fill( test->phi(), test->a0() );

      m_h_npixvseta_rec->Fill( referenceEta, int((test->pixelHits()+0.5)*0.5) ); 
      m_h_nsctvseta_rec->Fill( referenceEta, test->sctHits() ); 

      m_h_npixvsphi_rec->Fill( referencePhi, int((test->pixelHits()+0.5)*0.5) ); 
      m_h_nsctvsphi_rec->Fill( referencePhi, test->sctHits() ); 

      m_h_npixvsd0_rec->Fill( referenceD0, int((test->pixelHits()+0.5)*0.5) ); 
      m_h_nsctvsd0_rec->Fill( referenceD0, test->sctHits() ); 

      m_h_npix_rec->Fill(  int((test->pixelHits()+0.5)*0.5) ); 
      m_h_nsct_rec->Fill(  test->sctHits() ); 
      m_h_nsihits_rec->Fill(  test->siHits() ); 

      m_h_nsihits_lb_rec->Fill( event()->lumi_block(), test->siHits() ); 
    
 
      m_h_ntrt_rec->Fill(  test->strawHits() ); 

      m_h_ntrtvseta_rec->Fill( referenceEta, test->strawHits() ); 
      m_h_ntrtvsphi_rec->Fill( referencePhi, test->strawHits() ); 

    }
    //    else { 
    //      if ( referencePT*0.001 > 10 ) { /// in GeV
    // 	      m_debug = true;
    //	      //	std::cout << "SUTT Event " << event_no << "\tlb " << lumi_block<< "\tMissing trigger track " << *(*reference) << std::endl;
    //	     std::cout << "SUTT Event " << "\tMissing trigger track " << *(*reference) << std::endl;
    //      }
    //    }
    
  }
}




void Analysis_Tier0::execute_vtx(const std::vector<TIDA::Vertex*>& vtx0, 
				 const std::vector<TIDA::Vertex*>& vtx1,
				 const TIDA::Event* tevt ) { 
  if ( m_vtxanal ) m_vtxanal->execute( vtx0, vtx1, tevt );
}


void Analysis_Tier0::finalise() {
  if ( m_vtxanal ) m_vtxanal->finalise();
} 



double Analysis_Tier0::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


