//
//   @file    VtxAnalysis.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: VtxAnalysis.cxx, v0.0   Sun  9 Aug 2015 21:53:46 CEST sutt $


#include "TrigInDetAnalysisExample/VtxAnalysis.h"

#include "TrigInDetAnalysisUtils/VertexMatcher.h"


VtxAnalysis::VtxAnalysis( const std::string& n ) : VertexAnalysis( n ), m_initialised(false), mdir(0) { } 


void VtxAnalysis::initialise() { 

  m_initialised = true;

  //  std::cout << "VtxAnalysis::initialise() " << name() << std::endl;


  mdir = new TIDDirectory(name());
  mdir->push();

  hnvtx   = new TH1F( "nvtx", ";number of vertices",   100, -0.5,  100.5   );
  hzed    = new TH1F( "zed",   ";vtx z [mm]",       200, -300,   300   );
  hntrax  = new TH1F( "ntrax", ";number of tracks", 201,   -0.5, 200.5 );

  addHistogram( hnvtx );
  addHistogram( hzed );
  addHistogram( hntrax );


  hnvtx_rec  = new TH1F( "nvtx_rec",  ";number of vertices",   100, -0.5,  100.5   );
  hzed_rec   = new TH1F( "zed_rec",   ";vtx z [mm]",       200, -300,   300   );
  hntrax_rec = new TH1F( "ntrax_rec", ";number of tracks", 201,   -0.5, 200.5 );

  addHistogram( hnvtx_rec );
  addHistogram( hzed_rec );
  addHistogram( hntrax_rec );

  hzed_res = new TH1F( "zed_res", "Delta z [mm]", 400, -10, 10 );

  addHistogram( hzed_res );

  rdz_vs_zed    = new TProfile(   "rdz_vs_zed",   "rdz_vs_zed; vtx z [mm];z residual [mm]",          100,   -300,    300 ); 
  rdz_vs_ntrax  = new TProfile( "rdz_vs_ntrax", "rdz_vs_ntrax;number of tracks;z residual [mm]",     200,   -0.5,  200.5 ); 
  rdz_vs_nvtx   = new TProfile(  "rdz_vs_nvtx",  "rdz_vs_nvtx;number of vertices;z residual [mm]",    51, -0.125, 50.125 );

  addHistogram( rdz_vs_zed );
  addHistogram( rdz_vs_ntrax );
  addHistogram( rdz_vs_nvtx );
 
  //  rdz_vs_mu     = new TProfile( "rdz_vs_mu",    30,     0,    30,    400, -20, 20 ); 

  eff_zed   = new TProfile( "zed_eff", "zed_eff;efficiency [%];offline vtx z [mm]",     200, -300,   300   );
  eff_ntrax = new TProfile( "ntrax_eff", "ntrax_eff;number of tracks;efficiency [%]",   201,   -0.5, 200.5 );
  eff_nvtx  = new TProfile( "nvtx_eff",  "nvtx_eff;number of vertices;efficiency [%]",  100, -0.5,  100.5   );
  //  eff_mu    = new Efficiency( hmu, "mu_eff" );
 
  addHistogram( eff_zed );
  addHistogram( eff_ntrax );
  addHistogram( eff_nvtx );

  mdir->pop();

}


void VtxAnalysis::execute( const std::vector<TIDA::Vertex*>& vtx0,
			       const std::vector<TIDA::Vertex*>& vtx1 ) { 


  if ( !m_initialised ) return;

  //  if ( vtx1.size()<2 ) return;

#if 1
    std::cout << "VtxAnalysis::execute() " << name()
	      << "\tvtx0.size() " << vtx0.size()
	      << "\tvtx1.size() " << vtx1.size()
	      << std::endl;
#endif


    VertexMatcher m("vtx_matcher", 10 );

    m.match( vtx0, vtx1 );

    hnvtx->Fill( vtx0.size() );
    hnvtx_rec->Fill( vtx1.size() );
    
    //    for ( unsigned i=0 ; i<vtx0.size() ; i++ ) { 
    for ( unsigned i=0 ; i<vtx0.size() ; i++ ) { 

      if ( i>0 ) break;

      std::cout << i << "\tref z " << vtx0[i]->z();

      hzed->Fill( vtx0[i]->z() );
      hntrax->Fill( vtx0[i]->Ntracks() );

      const TIDA::Vertex* mv = m.matched( vtx0[i] ); 

      if ( mv ) { 

	std::cout << "\ttest z " << mv->z() << "  : delta z " << (mv->z()-vtx0[i]->z()) << std::endl;
      
	/// ah ha ! can fill some silly old histograms here 
	/// ...
	
	hzed_rec->Fill( mv->z() );
	hntrax_rec->Fill( mv->Ntracks() );

	hzed_res->Fill( mv->z() - vtx0[i]->z() );
       
	rdz_vs_zed->Fill(   vtx0[i]->z(),       mv->z() - vtx0[i]->z() );
	rdz_vs_ntrax->Fill( vtx0[i]->Ntracks(), mv->z() - vtx0[i]->z() );
	rdz_vs_nvtx->Fill( vtx0.size(),  mv->z() - vtx0[i]->z() ); /// this isn't really legitimate

	eff_zed->Fill( vtx0[i]->z(), 1 );
	eff_ntrax->Fill( vtx0[i]->Ntracks(), 1 );
	eff_nvtx->Fill( vtx0.size(), 1 );

      }
      else {
	std::cout << "\t" << "------" << std::endl;
 
	eff_zed->Fill( vtx0[i]->z(), 0 );
	eff_ntrax->Fill( vtx0[i]->Ntracks(), 0 );
	eff_nvtx->Fill( vtx0.size(), 0 );
	
      }
      
    }

}



  
void VtxAnalysis::finalise() { 
  std::cout << "VtxAnalysis::finalise() " << name() << std::endl;
}

