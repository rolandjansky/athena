/**
 **     @file    AnalysisR3_Tier0.cxx
 **
 **     @author  mark sutton
 **     @date    $Id: AnalysisR3_Tier0.cxx   Thu 18 May 2017 15:35:34 CEST 
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/AnalysisR3_Tier0.h"
#include "TrigInDetAnalysisExample/VtxR3Analysis.h"

#include "TrigInDetAnalysisExample/ChainString.h"

AnalysisR3_Tier0::AnalysisR3_Tier0(const std::string& name,
                               double /*pTCut*/,
                               double /*etaCut*/,
                               double /*d0Cut*/,
                               double /*z0Cut*/)
  : TrackAnalysis(name),
    // m_pTCut(pTCut), /// left in commented for development purposes
    // m_etaCut(etaCut),
    // m_d0Cut(d0Cut),
    // m_z0Cut(z0Cut),
    m_debug(false),
    m_eventid(0),
    m_monTool(0)
{}

void AnalysisR3_Tier0::initialise() {
  if ( m_monTool ) initialise_R3();
}

void AnalysisR3_Tier0::initialise_R2() { }

void AnalysisR3_Tier0::initialise_R3() {

  ChainString cname = name();

  //  std::cout << "AnalysisR3_Tier0::initialise() name " << name() << std::endl; 

#if 0
  /// here for development purposes ...
  std::cout << "\nAnalysisR3_Tier0:: chain specification: " << cname << "\t" << cname.raw() << std::endl;
  std::cout << "\tchain: " << cname.head()    << std::endl;
  std::cout << "\tkey:   " << cname.tail()    << std::endl;
  std::cout << "\troi:   " << cname.roi()     << std::endl;
  std::cout << "\tvtx:   " << cname.vtx()     << std::endl;
  std::cout << "\tte:    " << cname.element() << std::endl;
#endif

  m_debug = false;

  m_hchain = new TIDA::Histogram<float>( monTool(),  "Chain" );

  /// Limit the bins - to only the first 77 bins - so a range up to ~ 1000
  /// leave the previous selection commented for the time being

  m_hntrk = new TIDA::Histogram<float>( monTool(),  "reftrk_N" );

  /// reference track distributions

  m_htrkpT  = new TIDA::Histogram<float>( monTool(), "reftrk_pT" );
  m_htrkphi = new TIDA::Histogram<float>( monTool(), "reftrk_phi" );
  m_htrketa = new TIDA::Histogram<float>( monTool(), "reftrk_eta" );
  if (name().find("LRT")!=std::string::npos || name().find("lrt")!=std::string::npos) { 
    m_htrkd0  = new TIDA::Histogram<float>( monTool(), "reftrk_d0" );
  } else { 
    m_htrkd0  = new TIDA::Histogram<float>( monTool(), "reftrk_d0" );
  }      
  m_htrkz0  = new TIDA::Histogram<float>( monTool(), "reftrk_z0" );

  /// the error estimates are always positive ...
  m_htrkdd0  = new TIDA::Histogram<float>( monTool(), "reftrk_dd0" );
  m_htrkdz0  = new TIDA::Histogram<float>( monTool(), "reftrk_dz0" );

  m_htrkd0sig = new TIDA::Histogram<float>( monTool(), "reftrk_d0sig" );


  /// test track distributions

  /// Limit the bins - to only the first 77 bins - so a range up to ~ 1000
  /// leave the previous selection commented for the time being
  //  m_hntrk_rec = new TIDA::Histogram<float>( monTool(),  "testtrk_N", "Test tracks", 100, vnbins );
  m_hntrk_rec = new TIDA::Histogram<float>( monTool(),  "testtrk_N" );


  //  m_htrkpT_rec  = new TIDA::Histogram<float>( monTool(), "testtrk_pT" , "Test track pT",  25,    0.,   100.);
  m_htrkpT_rec  = new TIDA::Histogram<float>( monTool(), "testtrk_pT" );
  m_htrkphi_rec = new TIDA::Histogram<float>( monTool(), "testtrk_phi" );
  m_htrketa_rec = new TIDA::Histogram<float>( monTool(), "testtrk_eta" );
  if (name().find("LRT")!=std::string::npos || name().find("lrt")!=std::string::npos) { 
    m_htrkd0_rec  = new TIDA::Histogram<float>( monTool(), "testtrk_d0" );
  } else { 
    m_htrkd0_rec  = new TIDA::Histogram<float>( monTool(), "testtrk_d0" );
  }  
  m_htrkz0_rec  = new TIDA::Histogram<float>( monTool(), "testtrk_z0" );

  m_htrkdd0_rec  = new TIDA::Histogram<float>( monTool(), "testtrk_dd0" );
  m_htrkdz0_rec  = new TIDA::Histogram<float>( monTool(), "testtrk_dz0" );

  m_htrkd0sig_rec = new TIDA::Histogram<float>( monTool(), "testtrk_d0sig" );



  /// trigger tracking efficiencies


  m_htotal_efficiency = new TIDA::Histogram<float>( monTool(), "Eff_overall" );

  m_hpTeff    = new TIDA::Histogram<float>( monTool(),  "Eff_pT" );
  m_hetaeff   = new TIDA::Histogram<float>( monTool(),  "Eff_Eta" );
  m_hphieff   = new TIDA::Histogram<float>( monTool(),  "Eff_Phi" );
  if (name().find("LRT")!=std::string::npos || name().find("lrt")!=std::string::npos) { 
    m_hd0eff    = new TIDA::Histogram<float>( monTool(),  "Eff_d0" );
  } else { 
    m_hd0eff    = new TIDA::Histogram<float>( monTool(),  "Eff_d0" );
  } 
  m_hz0eff    = new TIDA::Histogram<float>( monTool(),  "Eff_z0" );
  m_hnVtxeff  = new TIDA::Histogram<float>( monTool(),  "Eff_nVtx" );
  
  
  m_hlbeff = new TIDA::Histogram<float>( monTool(),  "Eff_lb" );


  m_htrkvtx_x_lb = new TIDA::Histogram<float>( monTool(),  "trkvtx_x_vs_lb" );
  m_htrkvtx_y_lb = new TIDA::Histogram<float>( monTool(),  "trkvtx_y_vs_lb" );
  m_htrkvtx_z_lb = new TIDA::Histogram<float>( monTool(),  "trkvtx_z_vs_lb" );

  /// han config too stufid to deal with spaces in histogram names
  m_hnpixvseta     = new TIDA::Histogram<float>( monTool(), "npix_vs_eta" );
  m_hnpixvseta_rec = new TIDA::Histogram<float>( monTool(), "npix_vs_eta_rec" );

  m_hnsctvseta     = new TIDA::Histogram<float>( monTool(), "nsct_vs_eta" );
  m_hnsctvseta_rec = new TIDA::Histogram<float>( monTool(), "nsct_vs_eta_rec" );

  m_hntrtvseta     = new TIDA::Histogram<float>( monTool(), "ntrt_vs_eta" );
  m_hntrtvseta_rec = new TIDA::Histogram<float>( monTool(), "ntrt_vs_eta_rec" );

  m_hnpixvsphi     = new TIDA::Histogram<float>( monTool(), "npix_vs_phi" );
  m_hnpixvsphi_rec = new TIDA::Histogram<float>( monTool(), "npix_vs_phi_rec" );

  m_hnsctvsphi     = new TIDA::Histogram<float>( monTool(), "nsct_vs_phi" );
  m_hnsctvsphi_rec = new TIDA::Histogram<float>( monTool(), "nsct_vs_phi_rec" );

  m_hntrtvsphi     = new TIDA::Histogram<float>( monTool(), "ntrt_vs_phi" );
  m_hntrtvsphi_rec = new TIDA::Histogram<float>( monTool(), "ntrt_vs_phi_rec" );
  
  if (name().find("LRT")!=std::string::npos || name().find("lrt")!=std::string::npos) {
    m_hnpixvsd0     = new TIDA::Histogram<float>( monTool(), "npix_vs_d0" );
    m_hnpixvsd0_rec = new TIDA::Histogram<float>( monTool(), "npix_vs_d0_rec" );
    
    m_hnsctvsd0     = new TIDA::Histogram<float>( monTool(), "nsct_vs_d0" );
    m_hnsctvsd0_rec = new TIDA::Histogram<float>( monTool(), "nsct_vs_d0_rec" );
  } else {
    m_hnpixvsd0     = new TIDA::Histogram<float>( monTool(), "npix_vs_d0" );
    m_hnpixvsd0_rec = new TIDA::Histogram<float>( monTool(), "npix_vs_d0_rec" );
    
    m_hnsctvsd0     = new TIDA::Histogram<float>( monTool(), "nsct_vs_d0" );
    m_hnsctvsd0_rec = new TIDA::Histogram<float>( monTool(), "nsct_vs_d0_rec" );
  }  
  
  m_hnpixvspT     = new TIDA::Histogram<float>( monTool(), "npix_vs_pT" );
  m_hnpixvspT_rec = new TIDA::Histogram<float>( monTool(), "npix_vs_pT_rec" );

  m_hnsctvspT     = new TIDA::Histogram<float>( monTool(), "nsct_vs_pT" );
  m_hnsctvspT_rec = new TIDA::Histogram<float>( monTool(), "nsct_vs_pT_rec" );
  
  
  m_hnsihits_lb     = new TIDA::Histogram<float>( monTool(),  "nsihits_lb" );
  m_hnsihits_lb_rec = new TIDA::Histogram<float>( monTool(),  "nsihits_lb_rec" );
  
  
  m_hlayer_rec  = new TIDA::Histogram<float>( monTool(), "layer_rec" );
  m_hlayer      = new TIDA::Histogram<float>( monTool(), "layer" );

  /// trigger tracking differential resolutions


  m_hpTres  = new TIDA::Histogram<float>( monTool(), "Res_pT" );
  m_hipTres = new TIDA::Histogram<float>( monTool(), "Res_ipT" );
  m_hetares = new TIDA::Histogram<float>( monTool(), "Res_eta" );
  m_hphires = new TIDA::Histogram<float>( monTool(), "Res_phi" );
  m_hd0res  = new TIDA::Histogram<float>( monTool(), "Res_d0" );
  m_hz0res  = new TIDA::Histogram<float>( monTool(), "Res_z0" );


  /// residuals

  m_htrkpT_residual  = new TIDA::Histogram<float>( monTool(), "residual_pT" );
  m_htrkipT_residual = new TIDA::Histogram<float>( monTool(), "residual_ipT" );
  m_htrkphi_residual = new TIDA::Histogram<float>( monTool(), "residual_phi" );
  m_htrketa_residual = new TIDA::Histogram<float>( monTool(), "residual_eta" );
  m_htrkd0_residual  = new TIDA::Histogram<float>( monTool(), "residual_d0" );
  m_htrkz0_residual  = new TIDA::Histogram<float>( monTool(), "residual_z0" );

  m_htrkdd0_residual  = new TIDA::Histogram<float>( monTool(), "residual_dd0" );
  m_htrkdz0_residual  = new TIDA::Histogram<float>( monTool(), "residual_dz0" );



  m_hnpix     = new TIDA::Histogram<float>( monTool(), "npix" );
  m_hnpix_rec = new TIDA::Histogram<float>( monTool(), "npix_rec" );

  m_hnsct     = new TIDA::Histogram<float>( monTool(), "nsct" );
  m_hnsct_rec = new TIDA::Histogram<float>( monTool(), "nsct_rec" );

  m_hnsihits     = new TIDA::Histogram<float>( monTool(), "nsiHits" );
  m_hnsihits_rec = new TIDA::Histogram<float>( monTool(), "nsiHits_rec" );

  m_hntrt     = new TIDA::Histogram<float>( monTool(), "ntrt" );
  m_hntrt_rec = new TIDA::Histogram<float>( monTool(), "ntrt_rec" );


  /// miscelaneous histograms

  m_hd0vsphi       = new TIDA::Histogram<float>( monTool(),  "d0_vs_phi_prof" );
  m_hd0vsphi_rec   = new TIDA::Histogram<float>( monTool(),  "d0_vs_phi_rec_prof" );

  m_vtxanal = 0;

#if 1

  /// vertex analyses if required ...
  //  if ( name().find("vtx")!=std::string::npos || name().find("Vtx")!=std::string::npos ||
  //       name().find("vx")!=std::string::npos  || name().find("Vx")!=std::string::npos ) { 

  if ( ChainString(name()).vtx()!="" ) { 

    m_vtxanal = new VtxR3Analysis("VTX");

    m_vtxanal->set_monTool( monTool() );

    /// is this needed ? 
    store().insert( m_vtxanal, "VTX" );

    /// initialise the vtx analysis
    m_vtxanal->initialise();

  }

#endif

}

extern TIDARoiDescriptor* agroi;

void AnalysisR3_Tier0::execute(const std::vector<TIDA::Track*>& ,
			       const std::vector<TIDA::Track*>& ,
			       TrackAssociator*  ) 
{ }


void AnalysisR3_Tier0::execute(const std::vector<TIDA::Track*>& referenceTracks,
			       const std::vector<TIDA::Track*>& testTracks,
			       TrackAssociator* associator, 
			       const TIDA::Event* tevt )
{ 
  
  /// Loop over reference tracks
  std::vector<TIDA::Track*>::const_iterator  reference    = referenceTracks.begin();
  std::vector<TIDA::Track*>::const_iterator  referenceEnd = referenceTracks.end();

  /// fill number of times this analysis was called - presumably 
  /// the number of passed RoIs for this chain 
  m_hchain->Fill( 0.5, 1 );

  

  //  if ( m_eventid != event()->event_number() ) { 
  if ( m_eventid != tevt->event_number() ) { 
    /// if the event number has changed, this is a new event
    /// so update the event counts
    //    m_eventid = event()->event_number(); 
    m_eventid = tevt->event_number(); 
    m_hchain->Fill( 1.5, 1 );
  }

  m_hntrk->Fill( referenceTracks.size() );
  m_hntrk_rec->Fill( testTracks.size() );

  /// fil the number of offline tracks
  m_hchain->Fill(4.5, testTracks.size() );

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

    float     eff_weight = 0;
    if (test) eff_weight = 1;

    m_htotal_efficiency->Fill(0.5, eff_weight );

    m_hpTeff->Fill( std::fabs(referencePT)*0.001, eff_weight );
    m_hz0eff->Fill( referenceZ0, eff_weight );
    m_hd0eff->Fill( referenceD0, eff_weight );
    m_hetaeff->Fill( referenceEta, eff_weight );
    m_hphieff->Fill( referencePhi, eff_weight );
    m_hnVtxeff->Fill( m_nVtx, eff_weight );

    //    m_hlbeff->Fill( event()->lumi_block(), eff_weight );
    m_hlbeff->Fill( tevt->lumi_block(), eff_weight );

    m_htrkpT->Fill( std::fabs(referencePT)*0.001 );
    m_htrketa->Fill( referenceEta );
    m_htrkphi->Fill( referencePhi );
    m_htrkd0->Fill( referenceD0 );
    m_htrkz0->Fill( referenceZ0 );

    m_htrkdd0->Fill( referenceDD0 );
    m_htrkdz0->Fill( referenceDZ0 );

    if ( referenceDD0!=0 )  m_htrkd0sig->Fill( referenceD0/referenceDD0 );

    m_hnpixvseta->Fill( referenceEta,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_hnsctvseta->Fill( referenceEta,  (*reference)->sctHits() ); 
    m_hntrtvseta->Fill( referenceEta,  (*reference)->strawHits() ); 


    m_hnpixvsphi->Fill( referencePhi,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_hnsctvsphi->Fill( referencePhi,  (*reference)->sctHits() ); 
    m_hntrtvsphi->Fill( referencePhi,  (*reference)->strawHits() ); 

    m_hnpixvsd0->Fill( referenceD0,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_hnsctvsd0->Fill( referenceD0,  (*reference)->sctHits() ); 

    m_hnpixvspT->Fill( std::fabs(referencePT)*0.001,  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_hnsctvspT->Fill( std::fabs(referencePT)*0.001,  (*reference)->sctHits() ); 


    m_hnpix->Fill(  int(((*reference)->pixelHits()+0.5)*0.5) ); 
    m_hnsct->Fill(  (*reference)->sctHits() ); 
    m_hnsihits->Fill(  (*reference)->siHits() ); 
    m_hntrt->Fill(  (*reference)->strawHits() ); 
   

    //    m_hnsihits_lb->Fill( event()->lumi_block(), (*reference)->siHits() ); 
    m_hnsihits_lb->Fill( tevt->lumi_block(), (*reference)->siHits() ); 
 
    m_hd0vsphi->Fill(referencePhi, referenceD0 );
 
    /// fil the number of offline tracks
    m_hchain->Fill(2.5, 1);


    for ( size_t ilayer=0 ; ilayer<32 ; ilayer++ ) { 
      if ( (*reference)->hitPattern()&(1<<ilayer) ) m_hlayer->Fill( ilayer );
    } 
    
    if (test) {

      m_hchain->Fill(3.5, 1);

      /// NB: do we want to fill the actual *trigger* quantities, or the 
      /// offline quantities for the *matched* tracks?

      /// residual profiles vs the reference variable      
      // m_hpTres->Fill( referencePT*0.001, (test->pT() - referencePT)*0.001 );
      // m_hipTres->Fill( 1000/referencePT, (1000/test->pT() - 1000/referencePT) );
      // m_hetares->Fill( referenceEta, test->eta() - referenceEta );
      // m_hphires->Fill( referencePhi, phi(test->phi() - referencePhi) );
      // m_hd0res->Fill( referenceD0, test->a0() - referenceD0 );
      // m_hz0res->Fill( referenceZ0, test->z0() - referenceZ0  );

      /// residual profiles vs eta - the more easy to understand
      m_hpTres->Fill( referenceEta, (test->pT() - referencePT)*0.001 );
      m_hipTres->Fill( referenceEta, (1000/test->pT() - 1000/referencePT) );
      m_hetares->Fill( referenceEta, test->eta() - referenceEta );
      m_hphires->Fill( referenceEta, phi(test->phi() - referencePhi) );
      m_hd0res->Fill( referenceEta, test->a0() - referenceD0 );
      m_hz0res->Fill( referenceEta, test->z0() - referenceZ0  );

      //    m_htrkvtx_x_lb->Fill( event()->lumi_block(), beamTestx() );
      //    m_htrkvtx_y_lb->Fill( event()->lumi_block(), beamTesty() );
      //    m_htrkvtx_z_lb->Fill( event()->lumi_block(), beamTestz() );

      m_htrkvtx_x_lb->Fill( tevt->lumi_block(), beamTestx() );
      m_htrkvtx_y_lb->Fill( tevt->lumi_block(), beamTesty() );
      m_htrkvtx_z_lb->Fill( tevt->lumi_block(), beamTestz() );

      for ( size_t ilayer=0 ; ilayer<32 ; ilayer++ ) { 
	if ( test->hitPattern()&(1<<ilayer) ) m_hlayer_rec->Fill( ilayer );
      } 

      //      std::cout << "SUTT beam x " << beamTestx() << " " << "\tx " << beamTesty() << " " <<  "\ty " << beamTestz() << std::endl;

#if 0
      /// reference tracks values for tracks with a reference track match (not test track values) 
      m_htrkpT_rec->Fill( referencePT*0.001 );
      m_htrketa_rec->Fill( referenceEta );
      m_htrkphi_rec->Fill( referencePhi );
      m_htrkd0_rec->Fill( referenceD0 );
      m_htrkz0_rec->Fill( referenceZ0 );
     
#endif

      /// test track distributions for test tracks with a reference track match 
      m_htrkpT_rec->Fill( std::fabs(test->pT())*0.001 );
      m_htrketa_rec->Fill( test->eta() );
      m_htrkphi_rec->Fill( test->phi() );
      m_htrkd0_rec->Fill( test->a0() );
      m_htrkz0_rec->Fill( test->z0() );

      m_htrkdd0_rec->Fill( test->da0() );
      m_htrkdz0_rec->Fill( test->dz0() );

      if ( test->da0()!=0 )  m_htrkd0sig_rec->Fill( test->a0()/test->da0() );


      /// 1d residual distributions 
      m_htrkpT_residual->Fill( (test->pT() - referencePT)*0.001 );
      m_htrkipT_residual->Fill( (1000/test->pT() - 1000/referencePT) );
      m_htrketa_residual->Fill( test->eta() - referenceEta );
      m_htrkphi_residual->Fill( phi(test->phi() - referencePhi) );

      m_htrkd0_residual->Fill(  test->a0() - referenceD0 );
      m_htrkz0_residual->Fill( test->z0() - referenceZ0  );

      m_htrkdd0_residual->Fill( test->da0() - referenceDD0 );
      m_htrkdz0_residual->Fill( test->dz0() - referenceDZ0  );

      m_hnpixvseta_rec->Fill( referenceEta, int((test->pixelHits()+0.5)*0.5) ); 
      m_hnsctvseta_rec->Fill( referenceEta, test->sctHits() ); 

      m_hnpixvsphi_rec->Fill( referencePhi, int((test->pixelHits()+0.5)*0.5) ); 
      m_hnsctvsphi_rec->Fill( referencePhi, test->sctHits() ); 

      m_hnpixvsd0_rec->Fill( referenceD0, int((test->pixelHits()+0.5)*0.5) ); 
      m_hnsctvsd0_rec->Fill( referenceD0, test->sctHits() ); 

      m_hnpixvspT_rec->Fill( std::fabs(referencePT)*0.001,  int((test->pixelHits()+0.5)*0.5) ); 
      m_hnsctvspT_rec->Fill( std::fabs(referencePT)*0.001,  test->sctHits() ); 

      m_hnpix_rec->Fill(  int((test->pixelHits()+0.5)*0.5) ); 
      m_hnsct_rec->Fill(  test->sctHits() ); 
      m_hnsihits_rec->Fill(  test->siHits() ); 

      //      m_hnsihits_lb_rec->Fill( event()->lumi_block(), test->siHits() ); 
      m_hnsihits_lb_rec->Fill( tevt->lumi_block(), test->siHits() ); 
    
 
      m_hntrt_rec->Fill(  test->strawHits() ); 

      m_hntrtvseta_rec->Fill( referenceEta, test->strawHits() ); 
      m_hntrtvsphi_rec->Fill( referencePhi, test->strawHits() ); 

      m_hd0vsphi_rec->Fill( test->phi(), test->a0() );

    }
    
  }
}




void AnalysisR3_Tier0::execute_vtx(const std::vector<TIDA::Vertex*>& vtx0, 
				   const std::vector<TIDA::Vertex*>& vtx1,
				   const TIDA::Event* tevt ) { 
  if ( m_vtxanal ) m_vtxanal->execute( vtx0, vtx1, tevt );
}


void AnalysisR3_Tier0::finalise() {
  if ( m_vtxanal ) m_vtxanal->finalise();
} 



double AnalysisR3_Tier0::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}





