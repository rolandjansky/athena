/**
 **     @file    ConfVtxAnalysis.cxx
 **
 **     @author  mark sutton
 **     @date    Sun  9 Aug 2015 21:53:46 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include "ConfVtxAnalysis.h"

#include "TrigInDetAnalysisUtils/VertexMatcher.h"
#include "TrigInDetAnalysis/TIDAEvent.h"


ConfVtxAnalysis::ConfVtxAnalysis( const std::string& n ) : 
  VertexAnalysis( n ), m_initialised(false), m_finalised(false), mdir(0) { } 


extern TIDA::Event* gevent;


void ConfVtxAnalysis::initialise() { 
  
  //  std::cerr << "ConfVtxAnalysis::initialise() " << name() << std::endl;

  if ( m_initialised ) { 
    std::cerr << "ConfVtxAnalysis::initialise() " << name() << " already initialised" << std::endl;
    return;
  }

  m_initialised = true;
  m_finalised   = false;

  //  std::cout << "ConfVtxAnalysis::initialise() " << name() << std::endl;

  mdir = new TIDDirectory(name());
  mdir->push();

#if 0
  double vnbins[41] = {
    0.5,	1.5,	2.5,	3.5,	4.5,	5.5,	6.5,	7.5,	8.5,	9.5,
    10.5,	11.5,	12.5,	13.5,	14.5,	15.5,	16.5,	17.5,	18.5,	19.5,
    21.5,	22.5,	24.5,	25.5,	27.5,	29.5,
    31.5,	33.5,	35.5,	38.5,
    40.5,	43.5,	46.5,
    50.5,	53.5,	57.5,
    61.5,	65.5,	69.5,
    74.5,
    80.5 };
#endif

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



  
  hnvtx   = new TH1F( "nvtx", ";number of vertices",   100, -0.5,  100.5   );
  hzed    = new TH1F( "zed",   ";vtx z [mm]",          200, -300,   300   );
  //  hntrax  = new TH1F( "ntrax", ";number of tracks", 201,   -0.5, 200.5 );
  hntrax  = new TH1F( "ntrax", ";number of tracks", 80,  vnbins );
  hmu     = new TH1F( "mu",    ";<mu>",         81, -0.5, 80.5   );
  hlb     = new TH1F( "lb",    ";lumi block",  301, -0.5, 3009.5   );


  hnvtx_rec  = new TH1F( "nvtx_rec",  ";number of vertices",  100, -0.5,  100.5   );
  hzed_rec   = new TH1F( "zed_rec",   ";vtx z [mm]",          200, -300,   300   );
  hntrax_rec = new TH1F( "ntrax_rec", ";number of tracks",     80, vnbins );

  hzed_res = new TH1F( "zed_res", "Delta z [mm]", 400, -10, 10 );

  rdz_vs_zed    = new Resplot( "rdz_vs_zed",   100, -300,    300,    1600, -20, 20 ); 
  rdz_vs_ntrax  = new Resplot( "rdz_vs_ntrax", 201,   -0.5,  200.5,  1600, -20, 20 ); 
  rdz_vs_nvtx   = new Resplot( "rdz_vs_nvtx",  81,    -0.5,   80.5,  1600, -20, 20 ); 
  rdz_vs_mu     = new Resplot( "rdz_vs_mu",    30,     0,     30,    1600, -20, 20 ); 

  eff_zed   = new Efficiency( hzed,   "zed_eff" );
  eff_ntrax = new Efficiency( hntrax, "ntrax_eff" );
  eff_nvtx  = new Efficiency( hnvtx,  "nvtx_eff" );
  eff_mu    = new Efficiency( hmu, "mu_eff" );
  eff_lb    = new Efficiency( hlb, "lb_eff" );
 
  //  double ntrax[10] = { 0, 2, 5, 10, 15, 20, 30, 15, 100 }; 

  rdz_vs_lb    = new Resplot( "rdz_vs_lb",  301, -0.5, 3009.5,  800, -20, 20 ); 
  rdx_vs_lb    = new Resplot( "rdx_vs_lb",  301, -0.5, 3009.5,  300,  -3,  3 ); 
  rdy_vs_lb    = new Resplot( "rdy_vs_lb",  301, -0.5, 3009.5,  300,  -3,  3 ); 

  mdir->pop();


}

template<typename T> 
std::ostream& operator<<( std::ostream& s, const std::vector<T*>& v) { 
  for ( size_t i=0 ; i<v.size() ; i++ ) { 
    if ( v[i] )  s << "\t" << *v[i] << "\n";
    else         s << "\t" << "----\n";
  }
  return s;
} 

void ConfVtxAnalysis::execute( const std::vector<TIDA::Vertex*>& vtx0,
			       const std::vector<TIDA::Vertex*>& vtx1,
			       const TIDA::Event* tevt ) { 


  if ( !m_initialised ) return;

  //  if ( vtx1.size()<2 ) return;

#if 0
    std::cout << "ConfVtxAnalysis::execute() " << name()
	      << "\tvtx0.size() " << vtx0.size()
	      << "\tvtx1.size() " << vtx1.size()
	      << std::endl;

    std::cout << "\tref:  " << vtx0 << std::endl;
    std::cout << "\ttest: " << vtx1 << std::endl;
#endif


    VertexMatcher m("vtx_matcher", 10 );

    m.match( vtx0, vtx1 );

    hnvtx->Fill( vtx0.size() );
    hnvtx_rec->Fill( vtx1.size() );
    
    // std::cout << "gevent " << gevent << std::endl;

    /// pass in a parameter now, rather than using a global
    //    double mu = gevent->mu();
    //    double lb = gevent->lumi_block();
    double mu = tevt->mu();
    double lb = tevt->lumi_block();

    for ( unsigned i=0 ; i<vtx0.size() ; i++ ) { 

      //      if ( i>0 ) break;

      //      std::cout << i << "\tref z " << vtx0[i]->z();

      hzed->Fill( vtx0[i]->z() );
      hntrax->Fill( vtx0[i]->Ntracks() );

      hlb->Fill( lb );
      hmu->Fill( mu );
      
      const TIDA::Vertex* mv = m.matched( vtx0[i] ); 
      
      //      std::cout << "\tvtx match: " << i << " " << mv << std::endl;

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
	rdz_vs_mu->Fill( mu,  mv->z() - vtx0[i]->z() ); /// this isn't really legitimate

	eff_zed->Fill( vtx0[i]->z() );
	eff_ntrax->Fill( vtx0[i]->Ntracks() );
	eff_nvtx->Fill( vtx0.size() );

	eff_mu->Fill( mu );
	eff_lb->Fill( lb );

	//	std::cout << "found vtx ref vertex size " << vtx0.size() << "\tonline " << vtx1.size() << std::endl;
	//	std::cout << "\tref:  " << *vtx0[i] << std::endl;
	//	for ( unsigned iv=0 ; iv<vtx1.size() ; iv++ ) if ( vtx1[iv] ) std::cout << "\t" << iv << " :  " << *vtx1[iv] << std::endl;
	
	/// what about beam tilts etc? where are these defined with respect to ?
	rdz_vs_lb->Fill( lb, mv->z() - vtx0[i]->z() ); 
	rdx_vs_lb->Fill( lb, mv->x() - vtx0[i]->x() ); 
	rdy_vs_lb->Fill( lb, mv->y() - vtx0[i]->y() ); 

      }
      else {
	//	std::cout << "\t" << "------" << std::endl;
 
	//	std::cout << "missing vtx ref vertex size " << vtx0.size() << "\tonline " << vtx1.size() << std::endl;
	//	std::cout << "\tref:  " << *vtx0[i] << std::endl;
	//	for ( unsigned iv=0 ; iv<vtx1.size() ; iv++ ) if ( vtx1[iv] ) std::cout << "\t" << iv << " :  " << *vtx1[iv] << std::endl;

#if 0
	static int vtxcount=0;

	if ( vtxcount<100 ) { 
	  std::cout << "ConfVtxAnalysis::execute() " << name() << "\tnomatch\n"
		    << "\tvtx0.size() " << vtx0.size()
		    << "\tvtx1.size() " << vtx1.size()
		    << std::endl;
	  
	  std::cout << "\tref:  " << vtx0 << std::endl;
	  std::cout << "\ttest: " << vtx1 << std::endl;
	  
	  vtxcount++;

	  //	  std::cout << *gevent << std::endl;
	}

	
#endif


	eff_zed->FillDenom( vtx0[i]->z() );
	eff_ntrax->FillDenom( vtx0[i]->Ntracks() );
	eff_nvtx->FillDenom( vtx0.size() );

	eff_mu->FillDenom( mu );
	eff_lb->FillDenom( lb );
	
      }
      
    }

}



  
void ConfVtxAnalysis::finalise() { 

  if ( !m_initialised ) return;
  if (  m_finalised )   return;

  std::cout << "ConfVtxAnalysis::finalise() " << name() << std::endl;

  m_finalised = true;

  mdir->push();

  hnvtx->Write();
  hzed->Write();
  hntrax->Write();

  hnvtx_rec->Write();
  hzed_rec->Write();
  hntrax_rec->Write();

  hmu->Write();
  hlb->Write();

  hzed_res->Write();

  std::cout << "finalising resplots" << std::endl;

  rdz_vs_zed->Finalise( Resplot::FitNull95 );    rdz_vs_zed->Write();
  rdz_vs_ntrax->Finalise( Resplot::FitNull95 );  rdz_vs_ntrax->Write();
  rdz_vs_nvtx->Finalise( Resplot::FitNull95 );   rdz_vs_nvtx->Write();

  rdz_vs_mu->Finalise( Resplot::FitNull95 );   rdz_vs_mu->Write();

  eff_zed->finalise();   eff_zed->Bayes()->Write( (eff_zed->name()+"_tg").c_str() );
  eff_ntrax->finalise(); eff_ntrax->Bayes()->Write( (eff_ntrax->name()+"_tg").c_str() );
  eff_nvtx->finalise();  eff_nvtx->Bayes()->Write( (eff_nvtx->name()+"_tg").c_str() );

  eff_mu->finalise();  eff_mu->Bayes()->Write( (eff_mu->name()+"_tg").c_str() );
  eff_lb->finalise();  eff_lb->Bayes()->Write( (eff_lb->name()+"_tg").c_str() );


  rdx_vs_lb->Finalise( Resplot::FitNull95 );    rdx_vs_lb->Write();
  rdy_vs_lb->Finalise( Resplot::FitNull95 );    rdy_vs_lb->Write();
  rdz_vs_lb->Finalise( Resplot::FitNull95 );    rdz_vs_lb->Write();


  mdir->pop();

}

