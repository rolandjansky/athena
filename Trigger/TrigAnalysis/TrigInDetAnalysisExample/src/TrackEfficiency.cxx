//
//   @file    TrackEfficiency.cxx         
//   
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: TrackEfficiency.cxx, v0.0   Mon 19 Jan 2009 17:25:51 GMT sutt $


#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/TrackEfficiency.h"

// #include "Resplot/Effplot.h"
// #include "Resplot/Resplot.h"

#include <cmath>


void TrackEfficiency::initialise() {
    // book the histograms

#if 0
  double limits[23] = { 0,     500,   1000,  1500,  2000,  2500, 
			3000,  4000,  5000,  6000,  7000,  8000, 
			10000, 12000, 14000, 16000, 20000, 25000, 
			30000, 35000, 40000, 50000, 60000 };
#endif


#if 0
  double limits[13] = { 0,     1000,  2000,  3000,  5000,  7000,  
  			10000, 14000, 20000, 30000, 40000, 60000, 100000 };

  double _limits[24] = { 0,     1000,  2000,  3000,  5000,  7000, 8000, 8500, 9000, 9500,    
			 10000, 10500, 11000, 11500, 12000, 12500, 13000, 13500, 14000, 
			 20000, 30000, 40000, 60000, 100000 };
#endif

  double _limits[18] = { 0,     1000,  2000,  3000,  5000,  7000, 
			 8000,  9000, 10000, 11000, 12000, 13000, 
			 14000, 20000, 30000, 40000, 60000, 100000 };

  std::vector<double> limits;
  for ( int i=0 ; i<18 ; i++ ) limits.push_back(_limits[i]);

#if 0
  // efficiencies
  effvsPT  = new Effplot( "efficiency vs pt",  limits );
  effvseta = new Effplot( "efficiency vs eta", 8, -3,  3);
  effvsphi = new Effplot( "efficiency vs phi", 8, -M_PI, M_PI );
  //  effvsd0  = new Effplot( "efficiency vs d0", 20, -100, 100 );
  effvsd0  = new Effplot( "efficiency vs d0", 200, -200, 200 );
 
  // resolutions
  iptvipt  = new Resplot( "resolution ipt vs ipt",  20, -0.2, 0.2,   21, -0.01, 0.01);

  ptvpt   = new Resplot( "resolution pt vs pt",   limits,   41, -10,   10   );
  phivpt  = new Resplot( "resolution phi vs pt",  limits,   41, -0.01, 0.01 );
  etavpt  = new Resplot( "resolution eta vs pt",  limits,   41, -0.01,  0.01  );
  z0vpt   = new Resplot( "resolution z0 vs pt",   limits,   41, -1,    1  );
  d0vpt   = new Resplot( "resolution d0 vs pt",   limits,   61, -0.3,  0.3  );

  ptveta   = new Resplot( "resolution pt vs eta",   12, -3, 3,   41, -10, 10);
  iptveta  = new Resplot( "resolution ipt vs eta",  12, -3, 3,   41, -0.01, 0.01);
  phiveta  = new Resplot( "resolution phi vs eta",  12, -3, 3,   41, -0.01, 0.01);
  etaveta  = new Resplot( "resolution eta vs eta",  12, -3, 3,   41, -0.01, 0.01);
  z0veta   = new Resplot( "resolution z vs eta",    12, -3, 3,   41, -1, 1);

  z0vz0  = new Resplot( "resolution z vs z",    40, -200, 200,  41, -1, 1);
  d0vd0  = new Resplot( "resolution d0 vs d0",  20, -5,  5,     61, -0.3, 0.3); 
#endif

} 



void TrackEfficiency::execute( const std::vector<TIDA::Track*>& reftracks,
			       const std::vector<TIDA::Track*>& /*testtracks*/,
			       TrackAssociator* matcher ) 
{ 
  std::cout << "execute() " << name() << std::endl;

  //    for ( int i=0 ; i<reftracks.size() ; i++ )  std::cout << i << " " << *reftracks[i]  << " ref"  << std::endl;
  //    for ( int i=0 ; i<testtracks.size() ; i++ ) std::cout << i << " " << *testtracks[i] << " tst"  << std::endl;
  
  // for an efficiency, we want at most 1 matching reco track
  // for each true track
  // for a purity, we want at most 1 true track for each 
  // reconstructed track
  // for a resolution we only want to consider matched pairs
  // of tracks
  
  // this is an efficiency
  for ( unsigned i=reftracks.size() ; i-- ;  ) { 
    
     // get the matched reco track for this true track if
     // there is one - returns NULL if no matching track
    
     //      std::cout << i << " " << *reftracks[i] << std::endl;
    
     const TIDA::Track* matchedreco = matcher->matched(reftracks[i]); 
      
     std::cout << "\t\tSUTT Analysis " << name() << "\t" << i << " " << *reftracks[i] << " -> ";
  
     if ( matchedreco )  {

	std::cout << *matchedreco << std::endl;
	
	// convert pt in MeV to GeV
	double pTt = reftracks[i]->pT()/1000; // convert to GeV
	double pTr = matchedreco->pT()/1000;   // convert to GeV

#if 0
	effvsPT->Fill(  reftracks[i]->pT() );
	effvseta->Fill( reftracks[i]->eta() );
	effvsphi->Fill( reftracks[i]->phi() );
	effvsd0->Fill( reftracks[i]->a0() );


	// fill the plots
	iptvipt->Fill( 1/pTt, 1/pTt-1/pTr );
	ptvpt->Fill( reftracks[i]->pT(), pTt-pTr );

	phivpt->Fill( reftracks[i]->pT(), reftracks[i]->phi()-matchedreco->phi() );
	etavpt->Fill( reftracks[i]->pT(), reftracks[i]->eta()-matchedreco->eta() );
	z0vpt->Fill( reftracks[i]->pT(),  reftracks[i]->z0()-matchedreco->z0() );
	d0vpt->Fill( reftracks[i]->pT(), reftracks[i]->a0()-matchedreco->a0() );

	ptveta->Fill( reftracks[i]->eta(), pTt-pTr); 
	iptveta->Fill( reftracks[i]->eta(), 1/pTt-1/pTr); 
	phiveta->Fill( reftracks[i]->eta(), reftracks[i]->phi()-matchedreco->phi() );
	etaveta->Fill( reftracks[i]->eta(), reftracks[i]->eta()-matchedreco->eta() );
	z0veta->Fill( reftracks[i]->eta(), reftracks[i]->z0()-matchedreco->z0() );

	z0vz0->Fill( reftracks[i]->z0(), reftracks[i]->z0()-matchedreco->z0() );
	d0vd0->Fill( reftracks[i]->a0(), reftracks[i]->a0()-matchedreco->a0() );
#endif

	std::cout << "SUTT pt res " 
		  << "\tpt="    << reftracks[i]->pT() 
		  << "\tpTr="   << pTt 
		  << "\t1/pTt=" << 1/pTt 
		  << "\tDpT="   << pTt-pTr  
		  << "\tD1/pT=" << 1/pTt-1/pTr
		  << std::endl;

     }
     else { 
	std::cout << " NULL" << std::endl;
#if 0

        effvsPT->FillTrueOnly(  reftracks[i]->pT() );
	effvseta->FillTrueOnly( reftracks[i]->eta() );
	effvsphi->FillTrueOnly( reftracks[i]->phi() );
	effvsd0->FillTrueOnly( reftracks[i]->a0() );
#endif

     }      
  }



}



void TrackEfficiency::finalise() {
  // sort out all the histograms and write them out
  std::cout << "SUTT TrackEfficiency::finalise() " << name() << std::endl;

#if 0
  gDirectory->pwd();
  Directory d(name());
  d.push();
  effvsPT->Finalise();
  effvseta->Finalise(Effplot::FitQuad);
  effvsphi->Finalise(Effplot::FitQuad);
  effvsd0->Finalise(Effplot::FitQuad);
  effvsPT->Write();
  effvseta->Write();
  effvsphi->Write();
  effvsd0->Write();

  iptvipt->Finalise();
  ptvpt->Finalise();

  phivpt->Finalise();
  etavpt->Finalise();
  z0vpt->Finalise();
  d0vpt->Finalise();
  
  ptveta->Finalise();
  iptveta->Finalise();
  phiveta->Finalise();
  etaveta->Finalise();
  z0veta->Finalise();
  
  z0vz0->Finalise();
  d0vd0->Finalise();

  // maybe put a Write() call into Finalise() ???
  iptvipt->Write();
  ptvpt->Write();

  phivpt->Write();
  etavpt->Write();
  z0vpt->Write();
  d0vpt->Write();
  
  ptveta->Write();
  iptveta->Write();
  phiveta->Write();
  etaveta->Write();
  z0veta->Write();
  
  z0vz0->Write();
  d0vd0->Write();


  d.pop();

#endif
} 

