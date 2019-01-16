/**
 **     @file    TrackDistributions.cxx
 **
 **     @author  mark sutton
 **     @date    Mon 19 Jan 2009 17:25:51 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/TrackEfficiency.h"
#include "TrigInDetAnalysisExample/TrackDistributions.h"

// #include "Resplot/Effplot.h"
// #include "Resplot/Resplot.h"

#include <cmath>


void TrackDistributions::initialise() {
    // book the histograms

  m_hchi2 = new TH1D(std::string(name()+"-chi2").c_str(),
			       std::string(name()+" chi2 distribution").c_str(), 100, 0.0, 50.0);
  m_heta  = new TH1D(std::string(name()+"-eta").c_str(),
			       std::string(name()+" eta distribution").c_str(), 100, -3.0, 3.0);
  m_hphi  = new TH1D(std::string(name()+"-phi").c_str(),
		     std::string(name()+" phi distribution").c_str(), 100, -3.1416, 3.1416);
  m_hz0   = new TH1D(std::string(name()+"-z0").c_str(),
		     std::string(name()+" z0 distribution").c_str(), 100, -1000.0, 1000.0);
  m_hd0   = new TH1D(std::string(name()+"-d0").c_str(),
		     std::string(name()+" d0 distribution").c_str(), 100, -500.0, 500.0);
  m_hpT   = new TH1D(std::string(name()+"-pT").c_str(),
			       std::string(name()+" pT distribution").c_str(), 100, -100000, 100000);
  m_hdeta = new TH1D(std::string(name()+"-deta").c_str(),
		     std::string(name()+" eta error distribution").c_str(), 100, 0, 0.2);
  m_hdphi = new TH1D(std::string(name()+"-dphi").c_str(),
		     std::string(name()+" phi error distribution").c_str(), 100, 0, 0.2);
  m_hdz0  = new TH1D(std::string(name()+"-dz0").c_str(),
		     std::string(name()+" z0 error distribution").c_str(), 100, 0, 1.0);
  m_hdd0  = new TH1D(std::string(name()+"-dd0").c_str(),
		     std::string(name()+" d0 error distribution").c_str(), 100, 0, 0.5);
  m_hdpT  = new TH1D(std::string(name()+"-dpT").c_str(),
		     std::string(name()+" pT error distribution").c_str(), 100, 0, 10000);
  m_hblayer  = new TH1D(std::string(name()+"-blayer").c_str(),
			std::string(name()+" blayer distribution").c_str(), 10, 0, 10.0);
  m_hpixel   = new TH1D(std::string(name()+"-pixel").c_str(),
			std::string(name()+" pixel distribution").c_str(), 10, 0, 10.0);
  m_hsct     = new TH1D(std::string(name()+"-sct").c_str(),
			std::string(name()+" sct distribution").c_str(), 10, 0, 10.0);
  m_hsilicon = new TH1D(std::string(name()+"-silicon").c_str(),
			std::string(name()+" silicon distribution").c_str(), 10, 0, 10.0);
  m_hstraw   = new TH1D(std::string(name()+"-straw").c_str(),
			std::string(name()+" straw distribution").c_str(), 40, 0, 40.0);
  m_htr      = new TH1D(std::string(name()+"-tr").c_str(),
			std::string(name()+" tr distribution").c_str(), 40, 0, 40.0);

  
} 



void TrackDistributions::execute( const std::vector<TIDA::Track*>& /*reftracks*/,
				  const std::vector<TIDA::Track*>& testtracks,
				  TrackAssociator* /*matcher*/ ) 
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
  
  // this is just filling the distributions of tracks
  // with no matching or nothin'

  for ( unsigned i=testtracks.size() ; i-- ;  ) { 
    
#if 0
   std::cout << "SUTT pt distribution " 
	      << "\tpt="    << reftracks[i]->pT() 
	      << "\tpTr="   << testtracks[i]->pT() 
	      << "\t1/pTt=" << 1/testpTt 
	      << "\tDpT="   << pTt-pTr  
	      << "\tD1/pT=" << 1/pTt-1/pTr
	      << std::endl;
#endif
    
   std::cout << "SUTT pt distribution " << *testtracks[i] << std::endl;

    m_hchi2->Fill( testtracks[i]->chi2() );

    m_heta->Fill( testtracks[i]->eta() );
    m_hphi->Fill( testtracks[i]->phi() );

    m_hz0->Fill( testtracks[i]->z0() );

    m_hd0->Fill( testtracks[i]->a0() );
    m_hpT->Fill( testtracks[i]->pT() );

    m_hdeta->Fill( testtracks[i]->deta() );
    m_hdphi->Fill( testtracks[i]->dphi() );

    m_hdz0->Fill( testtracks[i]->dz0() );
    m_hdd0->Fill( testtracks[i]->da0() );
    m_hdpT->Fill( testtracks[i]->dpT() );

    m_hblayer->Fill( testtracks[i]->bLayerHits() );
    m_hpixel->Fill( testtracks[i]->pixelHits() );
    m_hsct->Fill( testtracks[i]->sctHits() );
    m_hsilicon->Fill( testtracks[i]->siHits() );

    m_hstraw->Fill( testtracks[i]->strawHits() );
    m_htr->Fill( testtracks[i]->trHits() );

  
   
  }
  
}



void TrackDistributions::finalise() {
  // sort out all the histograms and write them out
  std::cout << "SUTT TrackDistributions::finalise() " << name() << std::endl;

#if 0
  gDirectory->pwd();
  Directory d(name());
  d.push();

  d.pop();

#endif
} 

