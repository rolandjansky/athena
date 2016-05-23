//
//   @file    ConfVtxAnalysis.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: ConfVtxAnalysis.cxx, v0.0   Sun  9 Aug 2015 21:53:46 CEST sutt $


#include "ConfVtxAnalysis.h"

#include "TrigInDetAnalysisUtils/VertexMatcher.h"


ConfVtxAnalysis::ConfVtxAnalysis( const std::string& n ) : VertexAnalysis( n ), m_initialised(false), mdir(0) { } 

void ConfVtxAnalysis::initialise() { 

  m_initialised = true;

  //  std::cout << "ConfVtxAnalysis::initialise() " << name() << std::endl;


  mdir = new TIDDirectory(name());
  mdir->push();

  hnvtx   = new TH1F( "nvtx", ";number of vertices",   100, -0.5,  100.5   );
  hzed    = new TH1F( "zed",   ";vtx z [mm]",       200, -300,   300   );
  hntrax  = new TH1F( "ntrax", ";number of tracks", 201,   -0.5, 200.5 );


  hnvtx_rec  = new TH1F( "nvtx_rec",  ";number of vertices",   100, -0.5,  100.5   );
  hzed_rec   = new TH1F( "zed_rec",   ";vtx z [mm]",       200, -300,   300   );
  hntrax_rec = new TH1F( "ntrax_rec", ";number of tracks", 201,   -0.5, 200.5 );

  hzed_res = new TH1F( "zed_res", "Delta z [mm]", 400, -10, 10 );

  rdz_vs_zed    = new Resplot( "rdz_vs_zed",   100, -300,    300,    400, -20, 20 ); 
  rdz_vs_ntrax  = new Resplot( "rdz_vs_ntrax", 200,   -0.5,  200.5,  400, -20, 20 ); 
  rdz_vs_nvtx   = new Resplot( "rdz_vs_nvtx",  51,    -0.125, 50.125,  400, -20, 20 ); 
  //  rdz_vs_mu     = new Resplot( "rdz_vs_mu",    30,     0,    30,    400, -20, 20 ); 

  eff_zed   = new Efficiency( hzed,   "zed_eff" );
  eff_ntrax = new Efficiency( hntrax, "ntrax_eff" );
  eff_nvtx  = new Efficiency( hnvtx,  "nvtx_eff" );
  //  eff_mu    = new Efficiency( hmu, "mu_eff" );
 
  //  double ntrax[10] = { 0, 2, 5, 10, 15, 20, 30, 15, 100 }; 

  mdir->pop();


}


void ConfVtxAnalysis::execute( const std::vector<TIDA::Vertex*>& vtx0,
			       const std::vector<TIDA::Vertex*>& vtx1 ) { 


  if ( !m_initialised ) return;

  //  if ( vtx1.size()<2 ) return;

#if 0
    std::cout << "ConfVtxAnalysis::execute() " << name()
	      << "\tvtx0.size() " << vtx0.size()
	      << "\tvtx1.size() " << vtx1.size()
	      << std::endl;
#endif


    VertexMatcher m("vtx_matcher", 10 );

    m.match( vtx0, vtx1 );

    hnvtx->Fill( vtx0.size() );
    hnvtx_rec->Fill( vtx1.size() );
    
    for ( unsigned i=0 ; i<vtx0.size() ; i++ ) { 

      if ( i>0 ) continue;

      //      std::cout << i << "\tref z " << vtx0[i]->z();

      hzed->Fill( vtx0[i]->z() );
      hntrax->Fill( vtx0[i]->Ntracks() );

      const TIDA::Vertex* mv = m.matched( vtx0[i] ); 
      if ( mv ) { 
	//	std::cout << "\ttest z " << mv->z() << "  : delta z " << (mv->z()-vtx0[i]->z()) << std::endl;
      
	/// ah ha ! can fill some silly old histograms here 
	/// ...
	
	hzed_rec->Fill( mv->z() );
	hntrax_rec->Fill( mv->Ntracks() );

	hzed_res->Fill( mv->z() - vtx0[i]->z() );

       
	rdz_vs_zed->Fill(   vtx0[i]->z(),       mv->z() - vtx0[i]->z() );
	rdz_vs_ntrax->Fill( vtx0[i]->Ntracks(), mv->z() - vtx0[i]->z() );
	rdz_vs_nvtx->Fill( vtx0.size(),  mv->z() - vtx0[i]->z() ); /// this isn't really legitimate

	eff_zed->Fill( vtx0[i]->z() );
	eff_ntrax->Fill( vtx0[i]->Ntracks() );
	eff_nvtx->Fill( vtx0.size() );

      }
      else {
	//	std::cout << "\t" << "------" << std::endl;
 
	eff_zed->FillDenom( vtx0[i]->z() );
	eff_ntrax->FillDenom( vtx0[i]->Ntracks() );
	eff_nvtx->FillDenom( vtx0.size() );
	
      }
      
    }

}



  
void ConfVtxAnalysis::finalise() { 

  std::cout << "ConfVtxAnalysis::finalise() " << name() << std::endl;
 
  if ( !m_initialised ) return;;

  mdir->push();

  std::cout << "ConfVtxAnalysis::finalise() " << name() << std::endl;
 
  gDirectory->pwd();

  hnvtx->Write();
  hzed->Write();
  hntrax->Write();

  hnvtx_rec->Write();
  hzed_rec->Write();
  hntrax_rec->Write();

  hzed_res->Write();

  rdz_vs_zed->Finalise( Resplot::FitNull95 );    rdz_vs_zed->Write();
  rdz_vs_ntrax->Finalise( Resplot::FitNull95 );  rdz_vs_ntrax->Write();
  rdz_vs_nvtx->Finalise( Resplot::FitNull95 );   rdz_vs_nvtx->Write();

  eff_zed->finalise();   eff_zed->Bayes()->Write( (eff_zed->name()+"_tg").c_str() );
  eff_ntrax->finalise(); eff_ntrax->Bayes()->Write( (eff_ntrax->name()+"_tg").c_str() );
  eff_nvtx->finalise();  eff_nvtx->Bayes()->Write( (eff_nvtx->name()+"_tg").c_str() );

  mdir->pop();

}

