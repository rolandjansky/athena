/**
 **     @file    Analysis_Tier0.cxx
 **
 **     @author  mark sutton
 **     @date    $Id: Analysis_Tier0.cxx   Thu 18 May 2017 15:35:34 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


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

  h_chain = new TH1F( "Chain", cname.c_str(), 5, 0, 5 );
  h_chain->GetXaxis()->SetBinLabel(1, "Nrois" );
  h_chain->GetXaxis()->SetBinLabel(2, "Nevents" );
  h_chain->GetXaxis()->SetBinLabel(3, "N ref tracks" );
  h_chain->GetXaxis()->SetBinLabel(4, "N matched tracks" );
  h_chain->GetXaxis()->SetBinLabel(5, "" ); //  spare

 
  /// archive the chain name

  addHistogram(h_chain);

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



  h_ntrk = new TH1F( "reftrk_N", "Reference tracks", 80, vnbins );

  addHistogram(h_ntrk);

  /// reference track distributions

  //  double pt_bins[25] = { 0, 1, 2, 3, 4, 8, 10, 12, 14, 16, 18, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100 }; 

  std::vector<double> ptbins;

  for ( int i=0 ; i<=25 ; i++ ) {

    double y = std::log10(1) + i*( std::log10(100)-std::log10(1) )/25;
    
    double x = std::pow(10, y);
    ptbins.push_back(x);

  }

  //  h_trkpT  = new TH1F("reftrk_pT" , "Reference track pT",  50,    0.,    100.);
  h_trkpT  = new TH1F("reftrk_pT" , "Reference track pT",  25,   &ptbins[0]   );
  h_trkphi = new TH1F("reftrk_phi", "Reference track Phi", 25,   -M_PI,   M_PI);
  h_trketa = new TH1F("reftrk_eta", "Reference track Eta", 25,   -2.5,     2.5) ;
  h_trkd0  = new TH1F("reftrk_d0" , "Reference track d0", 101,   -5.0,     5.0 );
  h_trkz0  = new TH1F("reftrk_z0" , "Reference track z0",  50,   -225.,    225.);

  h_trketaroi  = new TH1F("reftrk_etaroi",  "Reference track Eta - Roi Eta",      100,   -2.5,     2.5) ;
  h_trketazroi = new TH1F("reftrk_etazroi", "Reference track (Eta - Roi Eta)*Dz", 100,   -2.5,     2.5) ;


  h_trkdd0  = new TH1F("reftrk_dd0" , "Reference track sigma(d0)", 101,   -0.5,     0.5);
  h_trkdz0  = new TH1F("reftrk_dz0" , "Reference track sigma(z0)", 101,   -2.5,     2.5);

  h_trkd0sig = new TH1F("reftrk_d0sig" , "Reference track d0 significance", 101,   -5.,     5.);

  addHistogram(h_trkpT);
  addHistogram(h_trkphi);
  addHistogram(h_trketa);
  addHistogram(h_trkd0);
  addHistogram(h_trkz0);

  addHistogram(h_trketaroi);
  addHistogram(h_trketazroi);


  addHistogram(h_trkdd0);
  addHistogram(h_trkdz0);

  addHistogram(h_trkd0sig);

  /// test track distributions

  h_ntrk_rec = new TH1F( "testtrk_N", "Test tracks", 80, vnbins );

  addHistogram(h_ntrk_rec);

  //  h_trkpT_rec  = new TH1F("testtrk_pT" , "Test track pT",  25,    0.,   100.);
  h_trkpT_rec  = new TH1F("testtrk_pT" , "Test track pT",  25,   &ptbins[0]   );
  h_trkphi_rec = new TH1F("testtrk_phi", "Test track Phi", 25,   -M_PI,  M_PI);
  h_trketa_rec = new TH1F("testtrk_eta", "Test track Eta", 25,   -2.5,    2.5) ;
  h_trkd0_rec  = new TH1F("testtrk_d0" , "Test track d0", 101,   -5.0,    5.0 );
  h_trkz0_rec  = new TH1F("testtrk_z0" , "Test track z0",  50,   -225.,   225.);

  h_trkdd0_rec  = new TH1F("testtrk_dd0" , "Test track sigma(d0)", 101,   -0.5,     0.5);
  h_trkdz0_rec  = new TH1F("testtrk_dz0" , "Test track sigma(z0)", 101,   -2.5,     2.5);

  h_trkd0sig_rec = new TH1F("testtrk_d0sig" , "Test track d0 significance", 101,   -5.0,     5.0);


  addHistogram(h_trkpT_rec);
  addHistogram(h_trkphi_rec);
  addHistogram(h_trketa_rec);
  addHistogram(h_trkd0_rec);
  addHistogram(h_trkz0_rec);

  addHistogram(h_trkdd0_rec);
  addHistogram(h_trkdz0_rec);
  
  addHistogram(h_trkd0sig_rec);

  /// trigger tracking efficiencies

  double d0bins[40] = { -5.0,  -4.0,  -3.0,  -2.5,   
			-2.0,  -1.8,  -1.6,  -1.4,  -1.2,  
			-1.05, -0.95, -0.85, -0.75, -0.65, -0.55, -0.45, -0.35, -0.25, -0.15, -0.05, 
		 	 0.05,  0.15,  0.25,  0.35,  0.45,  0.55,  0.65,  0.75,  0.85,  0.95,  1.05,  
			 1.2,   1.4,   1.6,   1.8,   2.0,  
			 2.5,   3.0,   4.0,   5.0 };



  h_total_efficiency = new TProfile ("Eff_overall",  "total efficiency",  1, 0., 1.);

  // h_pTeff = new TProfile( "Eff_pT",     "pT efficiency",     25,    0.,  100.   );
  h_pTeff    = new TProfile( "Eff_pT",     "pT efficiency",     25,   &ptbins[0]   );
  h_etaeff   = new TProfile( "Eff_Eta",    "eta efficiency",    25,   -2.5,   2.5  );
  h_phieff   = new TProfile( "Eff_Phi",    "phi  efficiency",   25,   -M_PI, M_PI  );
  h_d0eff    = new TProfile( "Eff_d0",     "d0 efficiency",     39,   d0bins    );
  h_z0eff    = new TProfile( "Eff_z0",     "z0 efficiency",     50, -225.,  225.   );
  h_nVtxeff  = new TProfile( "Eff_nVtx",   "nVtx efficiency",   71,   -0.5,  70.5  );

  h_lbeff = new TProfile( "Eff_lb", "efficinecy vs lumiblock", 301, -0.5, 3009.5 );

  addHistogram( h_lbeff );

  h_trkvtx_x_lb = new TProfile( "trkvtx_x_vs_lb", "track vertex x vs lumiblock", 301, -0.5, 3009.5 );
  h_trkvtx_y_lb = new TProfile( "trkvtx_y_vs_lb", "track vertex y vs lumiblock", 301, -0.5, 3009.5 );
  h_trkvtx_z_lb = new TProfile( "trkvtx_z_vs_lb", "track vertex z vs lumiblock", 301, -0.5, 3009.5 );

  addHistogram( h_trkvtx_x_lb );
  addHistogram( h_trkvtx_y_lb );
  addHistogram( h_trkvtx_z_lb );

  /// han config too stufid to deal with spaces in histogram names
  h_npixvseta     = new TProfile("npix_vs_eta",      "offline npix vs eta;offline #eta;# mean number of offline pixel hits", 30,   -2.5,  2.5);
  h_npixvseta_rec = new TProfile("npix_vs_eta_rec",  "trigger npix vs eta;offline #eta;# mean number of trigger pixel hits", 30,   -2.5,  2.5);

  h_nsctvseta     = new TProfile("nsct_vs_eta",      "offline nsct vs eta;offline #eta;# mean number of offline sct hits", 30,   -2.5,  2.5);
  h_nsctvseta_rec = new TProfile("nsct_vs_eta_rec",  "trigger nsct vs eta;offline #eta;# mean number of trigger sct hits", 30,   -2.5,  2.5);

  h_ntrtvseta     = new TProfile("ntrt_vs_eta",      "offline ntrt vs eta;offline #eta;# mean number of offline trt hits", 30,   -2.5, 2.5 );
  h_ntrtvseta_rec = new TProfile("ntrt_vs_eta_rec",  "trigger ntrt vs eta;offline #eta;# mean number of trigger trt hits", 30,   -2.5, 2.5 );

  h_npixvsphi     = new TProfile("npix_vs_phi",      "offline npix vs phi;offline #phi;# mean number of offline pixel hits", 30,   -M_PI,  M_PI);
  h_npixvsphi_rec = new TProfile("npix_vs_phi_rec",  "trigger npix vs phi;offline #phi;# mean number of trigger pixel hits", 30,   -M_PI,  M_PI);

  h_nsctvsphi     = new TProfile("nsct_vs_phi",      "offline nsct vs phi;offline #phi;# mean number of offline sct hits", 30,   -M_PI,  M_PI);
  h_nsctvsphi_rec = new TProfile("nsct_vs_phi_rec",  "trigger nsct vs phi;offline #phi;# mean number of trigger sct hits", 30,   -M_PI,  M_PI);

  h_ntrtvsphi     = new TProfile("ntrt_vs_phi",      "offline ntrt vs phi;offline #phi;# mean number of offline trt hits", 30,   -M_PI, M_PI );
  h_ntrtvsphi_rec = new TProfile("ntrt_vs_phi_rec",  "trigger ntrt vs phi;offline #phi;# mean number of trigger trt hits", 30,   -M_PI, M_PI );

  h_npixvsd0     = new TProfile("npix_vs_d0",      "offline npix vs d0;offline #d0;# mean number of offline pixel hits", 39,  d0bins );
  h_npixvsd0_rec = new TProfile("npix_vs_d0_rec",  "trigger npix vs d0;offline #d0;# mean number of trigger pixel hits", 39,  d0bins );

  h_nsctvsd0     = new TProfile("nsct_vs_d0",      "offline nsct vs d0;offline #d0;# mean number of offline sct hits", 39,   d0bins );
  h_nsctvsd0_rec = new TProfile("nsct_vs_d0_rec",  "trigger nsct vs d0;offline #d0;# mean number of trigger sct hits", 39,   d0bins );

  h_npixvspT     = new TProfile("npix_vs_pT",      "offline npix vs pT;offline #p_{T};# mean number of offline pixel hits", 25, &ptbins[0] );
  h_npixvspT_rec = new TProfile("npix_vs_pT_rec",  "trigger npix vs pT;offline #p_{T};# mean number of trigger pixel hits", 25, &ptbins[0] );

  h_nsctvspT     = new TProfile("nsct_vs_pT",      "offline nsct vs pT;offline #p_{T} [GeV];# mean number of offline pixel hits", 25, &ptbins[0] );
  h_nsctvspT_rec = new TProfile("nsct_vs_pT_rec",  "trigger nsct vs pT;offline #p_{T} [GeV];# mean number of trigger pixel hits", 25, &ptbins[0] );


  h_nsihits_lb     = new TProfile( "nsihits_lb",     "offline n sihits vs lumiblock", 301, -0.5, 3009.5 );
  h_nsihits_lb_rec = new TProfile( "nsihits_lb_rec", "trigger n sihits vs lumiblock", 301, -0.5, 3009.5 );

  addHistogram( h_nsihits_lb );
  addHistogram( h_nsihits_lb_rec );

  m_h_layer_rec  = new TH1F("layer_rec" , "hit layers", 32,   -0.5,  31.5  );
  m_h_layer      = new TH1F("layer",      "hit layers", 32,   -0.5,  31.5  );

  addHistogram(m_h_layer);
  addHistogram(m_h_layer_rec);



  addHistogram(h_total_efficiency);
  addHistogram(h_pTeff);
  addHistogram(h_etaeff);
  addHistogram(h_phieff);
  addHistogram(h_z0eff);
  addHistogram(h_d0eff);
  addHistogram(h_nVtxeff);

  addHistogram( h_npixvseta ); 
  addHistogram( h_npixvseta_rec ); 

  addHistogram( h_nsctvseta ); 
  addHistogram( h_nsctvseta_rec ); 

  addHistogram( h_ntrtvseta ); 
  addHistogram( h_ntrtvseta_rec ); 

  addHistogram( h_npixvsphi ); 
  addHistogram( h_npixvsphi_rec ); 

  addHistogram( h_nsctvsphi ); 
  addHistogram( h_nsctvsphi_rec ); 

  addHistogram( h_ntrtvsphi ); 
  addHistogram( h_ntrtvsphi_rec ); 

  addHistogram( h_npixvsd0 ); 
  addHistogram( h_npixvsd0_rec ); 

  addHistogram( h_nsctvsd0 ); 
  addHistogram( h_nsctvsd0_rec ); 

  addHistogram( h_npixvspT ); 
  addHistogram( h_npixvspT_rec ); 

  addHistogram( h_nsctvspT ); 
  addHistogram( h_nsctvspT_rec ); 

  /// trigger tracking differential resolutions

  /// change all these residiuals to be vs eta, rather than vs themselves

  //  h_pTres  = new TProfile("Res_pT", "pT residual;pT [Gev];pT_{trig}-pT_{ref} [GeV]",   25,    0.,   100.);
  // h_pTres  = new TProfile("Res_pT", "pT residual;pT [Gev];pT_{trig}-pT_{ref} [GeV]",      25,   &ptbins[0]   );
  // h_ipTres = new TProfile("Res_ipT", "ipT residual;pT^{-1} [Gev^{-1}];1/pT_{trig}-1/pT_{ref} [GeV^{-1}]", 55, -5, 5   );
  // h_etares = new TProfile("Res_eta", "Eta residual;#eta;#eta_{trig}-#eta_{ref}", 25,   -2.5,    2.5);
  // h_phires = new TProfile("Res_phi", "Phi residual;#phi;#phi_{trig}-#phi_{ref}", 25,   -M_PI, M_PI);
  // h_d0res  = new TProfile("Res_d0", "d0 residual;d0 [mm];d0_{trig}-d0_{ref} [mm]",   50,  -10.,    10.);
  // h_z0res  = new TProfile("Res_z0", "z0 residual;z0 [mm];z0_{trig}-z0_{ref} [mm]",   50, -225.,   225.);

  h_pTres  = new TProfile("Res_pT",  "pT residual;#eta_{ref};#Delta pT [Gev]",  25,   -2.5,  2.5   );
  h_ipTres = new TProfile("Res_ipT", "ipT residual;#eta_{ref};#Delta pT^{-1} [Gev^{-1}]", 25, -2.5, 2.5); 
  h_etares = new TProfile("Res_eta", "Eta residual;#eta_{ref};#Delta #eta",     25,   -2.5,  2.5);
  h_phires = new TProfile("Res_phi", "Phi residual;#eta_{ref};#Delta #phi",     25,   -2.5,  2.5);
  h_d0res  = new TProfile("Res_d0",  "d0 residual;#eta_{ref};#Delta d0 [mm]",   25,   -2.5,  2.5);
  h_z0res  = new TProfile("Res_z0",  "z0 residual;#eta_{ref};#Delta z0 [mm]",   25,   -2.5,  2.5);

  addHistogram(h_pTres);
  addHistogram(h_ipTres);
  addHistogram(h_etares);
  addHistogram(h_phires);
  addHistogram(h_d0res);
  addHistogram(h_z0res);

  /// residuals

  h_trkpT_residual  = new TH1F("residual_pT" , "track pT residual",  201, -100.0,    100.0  );
  h_trkipT_residual = new TH1F("residual_ipT", "track ipT residual",  55,   -5.5,      5.5  );
  h_trkphi_residual = new TH1F("residual_phi", "track Phi residual",  50,   -0.02,     0.02 );
  h_trketa_residual = new TH1F("residual_eta", "track Eta residual",  50,   -0.02,     0.02 );
  h_trkd0_residual  = new TH1F("residual_d0" , "track d0 residual ", 251,   -2.5,      2.5  );
  h_trkz0_residual  = new TH1F("residual_z0" , "track z0 residual",  401,  -20.0,     20.0  );

  h_trkdd0_residual  = new TH1F("residual_dd0" , "track sigma d0 residual ", 251,   -0.5,     0.5  );
  h_trkdz0_residual  = new TH1F("residual_dz0" , "track sigma z0 residual",  401,   -1.0,     1.0  );

  addHistogram(h_trkpT_residual);
  addHistogram(h_trkipT_residual);
  addHistogram(h_trkphi_residual);
  addHistogram(h_trketa_residual);
  addHistogram(h_trkd0_residual);
  addHistogram(h_trkz0_residual);

  addHistogram(h_trkdd0_residual);
  addHistogram(h_trkdz0_residual);

  h_npix     = new TH1F("npix",     "npix",     26,  -0.5,     25.5  );
  h_npix_rec = new TH1F("npix_rec", "npix_rec", 26,  -0.5,     25.5  );

  h_nsct     = new TH1F("nsct",     "nsct",     31,  -0.5,     30.5  );
  h_nsct_rec = new TH1F("nsct_rec", "nsct_rec", 31,  -0.5,     30.5  );

  h_nsihits     = new TH1F("nsiHits",     "nsiHits",     41,  -0.5,     40.5  );
  h_nsihits_rec = new TH1F("nsiHits_rec", "nsiHits_rec", 41,  -0.5,     40.5  );

  h_ntrt     = new TH1F("ntrt",     "ntrt",      91,  -0.5,     91.5  );
  h_ntrt_rec = new TH1F("ntrt_rec", "ntrt_rec",  91,  -0.5,     91.5  );

  addHistogram( h_npix );
  addHistogram( h_nsct );
  addHistogram( h_nsihits );
  addHistogram( h_ntrt );

  addHistogram( h_npix_rec );
  addHistogram( h_nsct_rec );
  addHistogram( h_nsihits_rec );
  addHistogram( h_ntrt_rec );

  /// miscelaneous histograms

  h_d0vsphi       = new TProfile( "d0_vs_phi_prof", "d0 vs phi_prof",  25, -M_PI, M_PI );
  h_d0vsphi_rec   = new TProfile( "d0_vs_phi_rec_prof", "d0 vs phi_rec_prof",  25, -M_PI, M_PI );

  addHistogram( h_d0vsphi ); 
  addHistogram( h_d0vsphi_rec );
 
  //  h2d_d0vsphi     = 0; // new     TH2D( "d0_vs_phi",      "d0 vs phi",       25, -M_PI, M_PI, 50, -2.6, 2.6 );
  //  h2d_d0vsphi_rec = 0; // new     TH2D( "d0_vs_phi_rec",  "d0 vs phi rec",   25, -M_PI, M_PI, 50, -2.6, 2.6 );

  //  addHistogram( h2d_d0vsphi ); 
  //  addHistogram( h2d_d0vsphi_rec ); 

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

extern TIDARoiDescriptor* agroi;


void Analysis_Tier0::execute(const std::vector<TIDA::Track*>& referenceTracks,
			     const std::vector<TIDA::Track*>& testTracks,
			     TrackAssociator* associator ) { 
  
  /// Loop over reference tracks
  std::vector<TIDA::Track*>::const_iterator  reference    = referenceTracks.begin();
  std::vector<TIDA::Track*>::const_iterator  referenceEnd = referenceTracks.end();

  /// fill number of times this analysis was called - presumably 
  /// the number of passed RoIs for this chain 
  h_chain->Fill( 0.5 );

  if ( m_eventid != event()->event_number() ) { 
    /// if the event number has changed, this is a new event
    /// so update the event counts
    m_eventid = event()->event_number(); 
    h_chain->Fill( 1.5 );
  }

  h_ntrk->Fill( referenceTracks.size() );
  h_ntrk_rec->Fill( testTracks.size() );

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

    h_total_efficiency->Fill(0.5, eff_weight );

    h_pTeff->Fill( std::fabs(referencePT)*0.001, eff_weight );
    h_z0eff->Fill( referenceZ0, eff_weight );
    h_d0eff->Fill( referenceD0, eff_weight );
    h_etaeff->Fill( referenceEta, eff_weight );
    h_phieff->Fill( referencePhi, eff_weight );
    h_nVtxeff->Fill( m_nVtx, eff_weight );

    h_lbeff->Fill( event()->lumi_block(), eff_weight );

    h_trkpT->Fill( std::fabs(referencePT)*0.001 );
    h_trketa->Fill( referenceEta );
    h_trkphi->Fill( referencePhi );
    h_trkd0->Fill( referenceD0 );
    h_trkz0->Fill( referenceZ0 );

    h_trkdd0->Fill( referenceDD0 );
    h_trkdz0->Fill( referenceDZ0 );

    if ( referenceDD0!=0 )  h_trkd0sig->Fill( referenceD0/referenceDD0 );

    h_npixvseta->Fill( referenceEta,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    h_nsctvseta->Fill( referenceEta,  (*reference)->sctHits() ); 
    h_ntrtvseta->Fill( referenceEta,  (*reference)->strawHits() ); 


    h_npixvsphi->Fill( referencePhi,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    h_nsctvsphi->Fill( referencePhi,  (*reference)->sctHits() ); 
    h_ntrtvsphi->Fill( referencePhi,  (*reference)->strawHits() ); 

    h_npixvsd0->Fill( referenceD0,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    h_nsctvsd0->Fill( referenceD0,  (*reference)->sctHits() ); 

    h_npixvspT->Fill( std::fabs(referencePT)*0.001,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    h_nsctvspT->Fill( std::fabs(referencePT)*0.001,  (*reference)->sctHits() ); 


    h_npix->Fill(  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    h_nsct->Fill(  (*reference)->sctHits() ); 
    h_nsihits->Fill(  (*reference)->siHits() ); 
    h_ntrt->Fill(  (*reference)->strawHits() ); 
   

    h_nsihits_lb->Fill( event()->lumi_block(), (*reference)->siHits() ); 
 
    h_d0vsphi->Fill(referencePhi, referenceD0 );
 
    h_chain->Fill(2.5);

    for ( size_t ilayer=0 ; ilayer<32 ; ilayer++ ) { 
      if ( (*reference)->hitPattern()&(1<<ilayer) ) m_h_layer_rec->Fill( ilayer );
    } 
    
    if (test) {

      h_chain->Fill(3.5);

      /// NB: do we want to fill the actual *trigger* quantities, or the 
      /// offline quantities for the *matched* tracks?

      /// residual profiles vs the reference variable      
      // h_pTres->Fill( referencePT*0.001, (test->pT() - referencePT)*0.001 );
      // h_ipTres->Fill( 1000/referencePT, (1000/test->pT() - 1000/referencePT) );
      // h_etares->Fill( referenceEta, test->eta() - referenceEta );
      // h_phires->Fill( referencePhi, phi(test->phi() - referencePhi) );
      // h_d0res->Fill( referenceD0, test->a0() - referenceD0 );
      // h_z0res->Fill( referenceZ0, test->z0() - referenceZ0  );

      /// residual profiles vs eta - the more easy to understand
      h_pTres->Fill( referenceEta, (test->pT() - referencePT)*0.001 );
      h_ipTres->Fill( referenceEta, (1000/test->pT() - 1000/referencePT) );
      h_etares->Fill( referenceEta, test->eta() - referenceEta );
      h_phires->Fill( referenceEta, phi(test->phi() - referencePhi) );
      h_d0res->Fill( referenceEta, test->a0() - referenceD0 );
      h_z0res->Fill( referenceEta, test->z0() - referenceZ0  );

      h_trkvtx_x_lb->Fill( event()->lumi_block(), beamTestx() );
      h_trkvtx_y_lb->Fill( event()->lumi_block(), beamTesty() );
      h_trkvtx_z_lb->Fill( event()->lumi_block(), beamTestz() );

      for ( size_t ilayer=0 ; ilayer<32 ; ilayer++ ) { 
	if ( test->hitPattern()&(1<<ilayer) ) m_h_layer_rec->Fill( ilayer );
      } 

      //      std::cout << "SUTT beam x " << beamTestx() << " " << "\tx " << beamTesty() << " " <<  "\ty " << beamTestz() << std::endl;

#if 0
      /// reference tracks values for tracks with a reference track match (not test track values) 
      h_trkpT_rec->Fill( referencePT*0.001 );
      h_trketa_rec->Fill( referenceEta );
      h_trkphi_rec->Fill( referencePhi );
      h_trkd0_rec->Fill( referenceD0 );
      h_trkz0_rec->Fill( referenceZ0 );
     
#endif

      /// test track distributions for test tracks with a reference track match 
      h_trkpT_rec->Fill( std::fabs(test->pT())*0.001 );
      h_trketa_rec->Fill( test->eta() );
      h_trkphi_rec->Fill( test->phi() );
      h_trkd0_rec->Fill( test->a0() );
      h_trkz0_rec->Fill( test->z0() );

      h_trkdd0_rec->Fill( test->da0() );
      h_trkdz0_rec->Fill( test->dz0() );

      if ( test->da0()!=0 )  h_trkd0sig_rec->Fill( test->a0()/test->da0() );


      /// 1d residual distributions 
      h_trkpT_residual->Fill( (test->pT() - referencePT)*0.001 );
      h_trkipT_residual->Fill( (1000/test->pT() - 1000/referencePT) );
      h_trketa_residual->Fill( test->eta() - referenceEta );
      h_trkphi_residual->Fill( phi(test->phi() - referencePhi) );

      h_trkd0_residual->Fill(  test->a0() - referenceD0 );
      h_trkz0_residual->Fill( test->z0() - referenceZ0  );

      h_trkdd0_residual->Fill( test->da0() - referenceDD0 );
      h_trkdz0_residual->Fill( test->dz0() - referenceDZ0  );

      h_npixvseta_rec->Fill( referenceEta, int((test->pixelHits()+0.5)*0.5) ); 
      h_nsctvseta_rec->Fill( referenceEta, test->sctHits() ); 

      h_npixvsphi_rec->Fill( referencePhi, int((test->pixelHits()+0.5)*0.5) ); 
      h_nsctvsphi_rec->Fill( referencePhi, test->sctHits() ); 

      h_npixvsd0_rec->Fill( referenceD0, int((test->pixelHits()+0.5)*0.5) ); 
      h_nsctvsd0_rec->Fill( referenceD0, test->sctHits() ); 

      h_npixvspT_rec->Fill( std::fabs(referencePT)*0.001,  int((test->pixelHits()+0.5)*0.5) ); 
      h_nsctvspT_rec->Fill( std::fabs(referencePT)*0.001,  test->sctHits() ); 

      h_npix_rec->Fill(  int((test->pixelHits()+0.5)*0.5) ); 
      h_nsct_rec->Fill(  test->sctHits() ); 
      h_nsihits_rec->Fill(  test->siHits() ); 

      h_nsihits_lb_rec->Fill( event()->lumi_block(), test->siHits() ); 
    
 
      h_ntrt_rec->Fill(  test->strawHits() ); 

      h_ntrtvseta_rec->Fill( referenceEta, test->strawHits() ); 
      h_ntrtvsphi_rec->Fill( referencePhi, test->strawHits() ); 

      h_d0vsphi_rec->Fill( test->phi(), test->a0() );

    }
    else { 
      //  std::cout << "Analysis: " << name() << " " << roi();
      //  if ( roi() ) std::cout << "\t" << *roi();
      //  std::cout << std::endl; 

      //      std::cout << "about to crash ? " << std::endl;

      if ( roi() ) { 

	double deta = 0.5*(roi()->etaPlus() - roi()->etaMinus()); 
	
	double Dzed = 0.5*(roi()->zedPlus() - roi()->zedMinus());
	double dzed = referenceZ0 - roi()->zed();
	
	h_trketaroi->Fill( (referenceEta - roi()->eta())/deta );

	h_trketazroi->Fill( (referenceEta - roi()->eta())*dzed/Dzed );
      
      }

      //      std::cout << "not crashed yet" << std::endl;


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





