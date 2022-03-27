/**
 **     @file    VtxR3Analysis.cxx
 **
 **     @author  mark sutton
 **     @date    Sun  6 Feb 2022 21:53:46 CEST 
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysisExample/VtxR3Analysis.h"

#include "TrigInDetAnalysisUtils/VertexMatcher.h"


VtxR3Analysis::VtxR3Analysis( const std::string& n ) : VertexAnalysis( n ), m_initialised(false) { } 


void VtxR3Analysis::initialise() { 

  m_initialised = true;

  //  std::cout << "VtxAnalysis::initialise() " << name() << std::endl;


  //  mdir = new TIDDirectory(name());
  //  mdir->push();



  m_hnvtx   = new TIDA::Histogram<float>( monTool(),  "vx_nvtx" );
  m_hzed    = new TIDA::Histogram<float>( monTool(),  "vx_zed" );
  m_hntrax  = new TIDA::Histogram<float>( monTool(),  "vx_ntrax" );


  m_hnvtx_rec  = new TIDA::Histogram<float>( monTool(),  "vx_nvtx_rec" );
  m_hzed_rec   = new TIDA::Histogram<float>( monTool(),  "vx_zed_rec" );
  m_hntrax_rec = new TIDA::Histogram<float>( monTool(),  "vx_ntrax_rec" );
  
  m_hzed_res = new TIDA::Histogram<float>( monTool(),  "vx_zed_res" );

  m_rdz_vs_zed    = new TIDA::Histogram<float>( monTool(),  "vx_rdz_vs_zed" );
  m_rdz_vs_ntrax  = new TIDA::Histogram<float>( monTool(),  "vx_rdz_vs_ntrax" );
  m_rdz_vs_nvtx   = new TIDA::Histogram<float>( monTool(),  "vx_rdz_vs_nvtx" );
  
  m_eff_zed   = new TIDA::Histogram<float>( monTool(),  "vx_zed_eff" );
  m_eff_ntrax = new TIDA::Histogram<float>( monTool(),  "vx_ntrax_eff" );
  m_eff_nvtx  = new TIDA::Histogram<float>( monTool(),  "vx_nvtx_eff" );
  m_eff_mu    = new TIDA::Histogram<float>( monTool(),  "vx_mu_eff" );
  m_eff_lb    = new TIDA::Histogram<float>( monTool(),  "vx_lb_eff" );
  
  ///  mdir->pop();

}


void VtxR3Analysis::execute( const std::vector<TIDA::Vertex*>& vtx0,
			   const std::vector<TIDA::Vertex*>& vtx1,
			   const TIDA::Event* tevt ) { 

  //  std::cout << "VtxAnalysis::execute() initialised: " << m_initialised << "\tevent: " << tevt << std::endl;

  if ( !m_initialised ) return;

  //  if ( vtx1.size()<2 ) return;

#if 0
    std::cout << "VtxAnalysis::execute() " << name()
	      << "\tvtx0.size() " << vtx0.size()
	      << "\tvtx1.size() " << vtx1.size()
	      << std::endl;
#endif


    VertexMatcher m("vtx_matcher", 10 );

    m.match( vtx0, vtx1 );

    m_hnvtx->Fill( vtx0.size() );
    m_hnvtx_rec->Fill( vtx1.size() );
  
    //    for ( unsigned i=0 ; i<vtx0.size() ; i++ ) { 
    for ( unsigned i=0 ; i<vtx0.size() ; i++ ) { 

      if ( i>0 ) break;

      //      std::cout << i << "\tref z " << vtx0[i]->z();

      m_hzed->Fill( vtx0[i]->z() );
      m_hntrax->Fill( vtx0[i]->Ntracks() );

      const TIDA::Vertex* mv = m.matched( vtx0[i] ); 

      if ( mv ) { 

	//	std::cout << "\ttest z " << mv->z() << "  : delta z " << (mv->z()-vtx0[i]->z()) << std::endl;
      
	/// ah ha ! can fill some silly old histograms here 
	/// ...
	
	m_hzed_rec->Fill( mv->z() );
	m_hntrax_rec->Fill( mv->Ntracks() );

	m_hzed_res->Fill( mv->z() - vtx0[i]->z() );
       
	m_rdz_vs_zed->Fill(   vtx0[i]->z(),       mv->z() - vtx0[i]->z() );
	m_rdz_vs_ntrax->Fill( vtx0[i]->Ntracks(), mv->z() - vtx0[i]->z() );
	m_rdz_vs_nvtx->Fill( vtx0.size(),  mv->z() - vtx0[i]->z() ); /// this isn't really legitimate

	m_eff_zed->Fill( vtx0[i]->z(), 1 );
	m_eff_ntrax->Fill( vtx0[i]->Ntracks(), 1 );
	m_eff_nvtx->Fill( vtx0.size(), 1 );

	m_eff_mu->Fill( tevt->mu(), 1 );
	m_eff_lb->Fill( tevt->lumi_block(), 1 );

      }
      else {
	//	std::cout << "\t" << "------" << std::endl;
 
	m_eff_zed->Fill( vtx0[i]->z(), 0 );
	m_eff_ntrax->Fill( vtx0[i]->Ntracks(), 0 );
	m_eff_nvtx->Fill( vtx0.size(), 0 );
	
	m_eff_mu->Fill( tevt->mu(), 0 );
	m_eff_lb->Fill( tevt->lumi_block(), 0 );
      }
      
    }

}



  
void VtxR3Analysis::finalise() { 
  //  std::cout << "VtxAnalysis::finalise() " << name() << std::endl;
}

