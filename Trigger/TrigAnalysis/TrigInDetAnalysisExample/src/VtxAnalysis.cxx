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


VtxAnalysis::VtxAnalysis( const std::string& n ) : VertexAnalysis( n ), m_initialised(false), m_dir(0) { } 


void VtxAnalysis::initialise() { 

  m_initialised = true;

  //  std::cout << "VtxAnalysis::initialise() " << name() << std::endl;


  m_dir = new TIDDirectory(name());
  m_dir->push();


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




  m_hnvtx   = new TH1F( "vx_nvtx", ";number of vertices",   100, -0.5,  100.5   );
  m_hzed    = new TH1F( "vx_zed",   ";vtx z [mm]",          100, -250,   250   );
  m_hntrax  = new TH1F( "vx_ntrax", ";number of tracks",    100,   0.5, 200.5 );

  addHistogram( m_hnvtx );
  addHistogram( m_hzed );
  addHistogram( m_hntrax );


  m_hnvtx_rec  = new TH1F( "vx_nvtx_rec",  ";number of vertices",   100, -0.5,  100.5   );
  m_hzed_rec   = new TH1F( "vx_zed_rec",   ";vtx z [mm]",           100, -250,   250   );
  m_hntrax_rec = new TH1F( "vx_ntrax_rec", ";number of tracks",     100,   0.5, 200.5 );

  addHistogram( m_hnvtx_rec );
  addHistogram( m_hzed_rec );
  addHistogram( m_hntrax_rec );

  m_hzed_res = new TH1F( "vx_zed_res", "Delta z [mm]", 400, -10, 10 );

  addHistogram( m_hzed_res );

  m_rdz_vs_zed    = new TProfile( "vx_rdz_vs_zed",   "m_rdz_vs_zed; vtx z [mm];z residual [mm]",           100, -250,   250 ); 
  m_rdz_vs_ntrax  = new TProfile( "vx_rdz_vs_ntrax", "m_rdz_vs_ntrax;number of tracks;z residual [mm]",     80, vnbins );
  m_rdz_vs_nvtx   = new TProfile( "vx_rdz_vs_nvtx",  "m_rdz_vs_nvtx;number of vertices;z residual [mm]",    51, -0.125, 50.125 );

  addHistogram( m_rdz_vs_zed );
  addHistogram( m_rdz_vs_ntrax );
  addHistogram( m_rdz_vs_nvtx );
 
  //  rdz_vs_mu     = new TProfile( "rdz_vs_mu",    30,     0,    30,    400, -20, 20 ); 

  m_eff_zed   = new TProfile( "vx_zed_eff", "zed_eff;efficiency;offline vtx z [mm]",       50, -250,  250   );
  m_eff_ntrax = new TProfile( "vx_ntrax_eff", "ntrax_eff;number of tracks;efficiency",    80, vnbins );
  m_eff_nvtx  = new TProfile( "vx_nvtx_eff",  "nvtx_eff;number of vertices;efficiency",  100, -0.5,  100.5   );
  m_eff_mu    = new TProfile( "vx_mu_eff",     "lb_eff;<#mu>;efficiency",                 61, -0.5,  60.5   );
  m_eff_lb    = new TProfile( "vx_lb_eff",     "lb_eff;lumi block;efficiency",           151, -0.5,  3019.5   );
 
  addHistogram( m_eff_zed );
  addHistogram( m_eff_ntrax );
  addHistogram( m_eff_nvtx );
  addHistogram( m_eff_mu );
  addHistogram( m_eff_lb );

  m_dir->pop();

}


void VtxAnalysis::execute( const std::vector<TIDA::Vertex*>& vtx0,
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



  
void VtxAnalysis::finalise() { 
  //  std::cout << "VtxAnalysis::finalise() " << name() << std::endl;
}

