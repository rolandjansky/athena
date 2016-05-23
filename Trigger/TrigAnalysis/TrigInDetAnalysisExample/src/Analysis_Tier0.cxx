/** @file Analysis_Tier0.cxx */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Tier0.h"



Analysis_Tier0::Analysis_Tier0(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut), m_debug(false) {  }

void Analysis_Tier0::initialise() {

  //  std::cout << "Analysis_Tier0::initialise() name " << name() << std::endl; 

  m_debug = false;

  h_chain = new TH1D( "Chain", name().c_str(), 1, 0, 1 );

  /// archive the chain name

  addHistogram(h_chain);

  /// reference track distributions

  //  double pt_bins[25] = { 0, 1, 2, 3, 4, 8, 10, 12, 14, 16, 18, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100 }; 

  std::vector<double> ptbins;

  for ( int i=0 ; i<=25 ; i++ ) {

    double y = std::log10(1) + i*( std::log10(100)-std::log10(1) )/25;
    
    double x = std::pow(10, y);
    ptbins.push_back(x);

  }

  //  h_trkpT  = new TH1D("reftrk_pT" , "Reference track pT",  50,    0.,    100.);
  h_trkpT  = new TH1D("reftrk_pT" , "Reference track pT",  25,   &ptbins[0]   );
  h_trkphi = new TH1D("reftrk_phi", "Reference track Phi", 25,   -M_PI,   M_PI);
  h_trketa = new TH1D("reftrk_eta", "Reference track Eta", 25,   -2.5,     2.5) ;
  h_trkd0  = new TH1D("reftrk_d0" , "Reference track d0", 101,   -5.0,     5.0 );
  h_trkz0  = new TH1D("reftrk_z0" , "Reference track z0",  50,   -225.,    225.);

  h_trkdd0  = new TH1D("reftrk_dd0" , "Reference track sigma(d0)", 101,   -0.5,     0.5);
  h_trkdz0  = new TH1D("reftrk_dz0" , "Reference track sigma(z0)", 101,   -2.5,     2.5);

  h_trkd0sig = new TH1D("reftrk_d0sig" , "Reference track d0 significance", 101,   -5.,     5.);

  addHistogram(h_trkpT);
  addHistogram(h_trkphi);
  addHistogram(h_trketa);
  addHistogram(h_trkd0);
  addHistogram(h_trkz0);

  addHistogram(h_trkdd0);
  addHistogram(h_trkdz0);

  addHistogram(h_trkd0sig);

  /// test track distributions

  //  h_trkpT_rec  = new TH1D("testtrk_pT" , "Test track pT",  25,    0.,   100.);
  h_trkpT_rec  = new TH1D("testtrk_pT" , "Test track pT",  25,   &ptbins[0]   );
  h_trkphi_rec = new TH1D("testtrk_phi", "Test track Phi", 25,   -M_PI,  M_PI);
  h_trketa_rec = new TH1D("testtrk_eta", "Test track Eta", 25,   -2.5,    2.5) ;
  h_trkd0_rec  = new TH1D("testtrk_d0" , "Test track d0", 101,   -5.0,    5.0 );
  h_trkz0_rec  = new TH1D("testtrk_z0" , "Test track z0",  50,   -225.,   225.);

  h_trkdd0_rec  = new TH1D("testtrk_dd0" , "Test track sigma(d0)", 101,   -0.5,     0.5);
  h_trkdz0_rec  = new TH1D("testtrk_dz0" , "Test track sigma(z0)", 101,   -2.5,     2.5);

  h_trkd0sig_rec = new TH1D("testtrk_d0sig" , "Test track d0 significance", 101,   -5.0,     5.0);


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
  h_nVtxeff  = new TProfile( "Eff_nVtx",   "nVtx efficiency",   41,   -0.5,  40.5  );

  /// han config too stufid to deal with spaces in histogram names
  h_npixvseta     = new TProfile("npix_vs_eta",      "offline npix vs eta;offline #eta;# mean number of offline pixel hits", 30,   -2.5,  2.5);
  h_npixvseta_rec = new TProfile("npix_vs_eta_rec",  "trigger npix vs eta;trigger #eta;# mean number of trigger pixel hits", 30,   -2.5,  2.5);

  h_nsctvseta     = new TProfile("nsct_vs_eta",      "offline nsct vs eta;offline #eta;# mean number of offline sct hits", 30,   -2.5,  2.5);
  h_nsctvseta_rec = new TProfile("nsct_vs_eta_rec",  "trigger nsct vs eta;trigger #eta;# mean number of trigger sct hits", 30,   -2.5,  2.5);

  h_ntrtvseta     = new TProfile("ntrt_vs_eta",      "offline ntrt vs eta;offline #eta;# mean number of offline trt hits", 30,   -2.5, 2.5 );
  h_ntrtvseta_rec = new TProfile("ntrt_vs_eta_rec",  "trigger ntrt vs eta;trigger #eta;# mean number of trigger trt hits", 30,   -2.5, 2.5 );

  h_npixvsphi     = new TProfile("npix_vs_phi",      "offline npix vs phi;offline #phi;# mean number of offline pixel hits", 30,   -M_PI,  M_PI);
  h_npixvsphi_rec = new TProfile("npix_vs_phi_rec",  "trigger npix vs phi;trigger #phi;# mean number of trigger pixel hits", 30,   -M_PI,  M_PI);

  h_nsctvsphi     = new TProfile("nsct_vs_phi",      "offline nsct vs phi;offline #phi;# mean number of offline sct hits", 30,   -M_PI,  M_PI);
  h_nsctvsphi_rec = new TProfile("nsct_vs_phi_rec",  "trigger nsct vs phi;trigger #phi;# mean number of trigger sct hits", 30,   -M_PI,  M_PI);

  h_ntrtvsphi     = new TProfile("ntrt_vs_phi",      "offline ntrt vs phi;offline #phi;# mean number of offline trt hits", 30,   -M_PI, M_PI );
  h_ntrtvsphi_rec = new TProfile("ntrt_vs_phi_rec",  "trigger ntrt vs phi;trigger #phi;# mean number of trigger trt hits", 30,   -M_PI, M_PI );



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

  h_trkpT_residual  = new TH1D("residual_pT" , "track pT residual",  201, -100.0,    100.0  );
  h_trkipT_residual = new TH1D("residual_ipT", "track ipT residual",  55,   -5.5,      5.5  );
  h_trkphi_residual = new TH1D("residual_phi", "track Phi residual",  50,   -0.02,     0.02 );
  h_trketa_residual = new TH1D("residual_eta", "track Eta residual",  50,   -0.02,     0.02 );
  h_trkd0_residual  = new TH1D("residual_d0" , "track d0 residual ", 251,   -2.5,      2.5  );
  h_trkz0_residual  = new TH1D("residual_z0" , "track z0 residual",  401,  -20.0,     20.0  );

  h_trkdd0_residual  = new TH1D("residual_dd0" , "track sigma d0 residual ", 251,   -0.5,     0.5  );
  h_trkdz0_residual  = new TH1D("residual_dz0" , "track sigma z0 residual",  401,   -1.0,     1.0  );

  addHistogram(h_trkpT_residual);
  addHistogram(h_trkipT_residual);
  addHistogram(h_trkphi_residual);
  addHistogram(h_trketa_residual);
  addHistogram(h_trkd0_residual);
  addHistogram(h_trkz0_residual);

  addHistogram(h_trkdd0_residual);
  addHistogram(h_trkdz0_residual);

  h_npix     = new TH1D("npix",     "npix",     26,  -0.5,     25.5  );
  h_npix_rec = new TH1D("npix_rec", "npix_rec", 26,  -0.5,     25.5  );

  h_nsct     = new TH1D("nsct",     "nsct",     31,  -0.5,     30.5  );
  h_nsct_rec = new TH1D("nsct_rec", "nsct_rec", 31,  -0.5,     30.5  );

  h_nsihits     = new TH1D("nsiHits",     "nsiHits",     41,  -0.5,     40.5  );
  h_nsihits_rec = new TH1D("nsiHits_rec", "nsiHits_rec", 41,  -0.5,     40.5  );

  h_ntrt     = new TH1D("ntrt",     "ntrt",      91,  -0.5,     91.5  );
  h_ntrt_rec = new TH1D("ntrt_rec", "ntrt_rec",  91,  -0.5,     91.5  );

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
  h2d_d0vsphi     = 0; // new     TH2D( "d0_vs_phi",      "d0 vs phi",       25, -M_PI, M_PI, 50, -2.6, 2.6 );
  h2d_d0vsphi_rec = 0; // new     TH2D( "d0_vs_phi_rec",  "d0 vs phi rec",   25, -M_PI, M_PI, 50, -2.6, 2.6 );

  addHistogram( h_d0vsphi ); 
  //  addHistogram( h2d_d0vsphi ); 
  //  addHistogram( h2d_d0vsphi_rec ); 

  m_vtxanal = 0;

#if 0
  /// vertex analyses if required ...
  if ( name().find("vtx")!=std::string::npos || name().find("Vtx")!=std::string::npos ) { 
    m_vtxanal = new VtxAnalysis("VTX");
    store().insert( m_vtxanal, "VTX" );

    /// initialise the vtx analysis
    m_vtxanal->initialise();

    /// store the historams
    for ( unsigned i=0 ; i<m_vtxanal->objects().size() ; i++ ) addHistogram( m_vtxanal->objects()[i] );
    for ( unsigned i=0 ; i<m_vtxanal->profs().size() ; i++ )   addHistogram( m_vtxanal->profs()[i] );
  }
#endif

}


void Analysis_Tier0::execute(const std::vector<TIDA::Track*>& referenceTracks,
			     const std::vector<TIDA::Track*>& /*testTracks*/,
			     TrackAssociator* associator) {
  
  /// Loop over reference tracks
  std::vector<TIDA::Track*>::const_iterator  reference    = referenceTracks.begin();
  std::vector<TIDA::Track*>::const_iterator  referenceEnd = referenceTracks.end();

  /// fill number of times this analysis was called - presumably the number 
  /// of passed events for this chain 
  h_chain->Fill( 0.5 ) ;

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


    h_npix->Fill(  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    h_nsct->Fill(  (*reference)->sctHits() ); 
    h_nsihits->Fill(  (*reference)->siHits() ); 
    h_ntrt->Fill(  (*reference)->strawHits() ); 
   
 
    h_d0vsphi->Fill(referencePhi, referenceD0 );
    //   h2d_d0vsphi->Fill(referencePhi, referenceD0 );


    if(test){

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

      if ( test->da0()!=0 )  h_trkd0sig_rec->Fill( test->da0()/test->a0() );


      /// 1d residual distributions 
      h_trkpT_residual->Fill( (test->pT() - referencePT)*0.001 );
      h_trkipT_residual->Fill( (1000/test->pT() - 1000/referencePT) );
      h_trketa_residual->Fill( test->eta() - referenceEta );
      h_trkphi_residual->Fill( phi(test->phi() - referencePhi) );

      h_trkd0_residual->Fill(  test->a0() - referenceD0 );
      h_trkz0_residual->Fill( test->z0() - referenceZ0  );

      h_trkdd0_residual->Fill( test->da0() - referenceDD0 );
      h_trkdz0_residual->Fill( test->dz0() - referenceDZ0  );

      //      h2d_d0vsphi_rec->Fill( test->phi(), test->a0() );

      h_npixvseta_rec->Fill( referenceEta, int((test->pixelHits()+0.5)*0.5) ); 
      h_nsctvseta_rec->Fill( referenceEta, test->sctHits() ); 

      h_npixvsphi_rec->Fill( referencePhi, int((test->pixelHits()+0.5)*0.5) ); 
      h_nsctvsphi_rec->Fill( referencePhi, test->sctHits() ); 

      h_npix_rec->Fill(  int((test->pixelHits()+0.5)*0.5) ); 
      h_nsct_rec->Fill(  test->sctHits() ); 
      h_nsihits_rec->Fill(  test->siHits() ); 
    
      h_ntrt_rec->Fill(  test->strawHits() ); 

      h_ntrtvseta_rec->Fill( referenceEta, test->strawHits() ); 
      h_ntrtvsphi_rec->Fill( referencePhi, test->strawHits() ); 

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
				 const std::vector<TIDA::Vertex*>& vtx1 ) { 
  if ( m_vtxanal ) m_vtxanal->execute( vtx0, vtx1 );
}

void Analysis_Tier0::finalise() {
  if ( m_vtxanal ) m_vtxanal->finalise();
} 



double Analysis_Tier0::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


