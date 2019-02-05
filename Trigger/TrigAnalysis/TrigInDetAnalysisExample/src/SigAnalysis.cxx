/**
 **     @file    SigAnalysis.cxx
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:41:07 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysisExample/SigAnalysis.h"

void SigAnalysis::initialise() { 

  std::cout << "SigAnalysis::initialise() " << name() << std::endl;
  
  //+++ pT ranges
  //  double tmp_maxPt    = 50000.;
  double tmp_absResPt = 0.0005;

  const int pTResBins = 100;

  //+++ Eta ranges
  double tmp_maxEta    = 3.;
  double tmp_absResEta = 0.04; // 0.0005;

  //+++ Phi ranges
  double tmp_maxPhi    = 3.142;
  double tmp_absResPhi = 0.02; // 0.0001;


  const int etaBins    = 60;
  const int etaResBins = 300;

  const int phiBins    = 36;
  const int phiResBins = 100;

  const int    zBins = 50;      
  const double zMax  = 400;

  const int    zresBins = 100;      
  const double zresMax  = 10;

  const int    d0Bins = 100;
  const double d0Max  = 10;

  const int    d0resBins = 100;      
  const double d0resMax  = 10;

  // beamspot corrected position

  const int    a0Bins = 100;
  const double a0Max  = 10;

  const int    a0resBins = 100;      
  const double a0resMax  = 5;

  //+++ Book histograms

  // calculate a logarithmic binning in pt
  
  const int  ptnbins = 20;  
  // const int  ptnbins = 30;  
  double     ptbinlims[ptnbins+1];
  for ( int i=0 ; i<=ptnbins ; i++ ) {     ptbinlims[i] = std::pow(10, 2.0*i/ptnbins+2); }
  // for ( int i=0 ; i<=ptnbins ; i++ ) {     ptbinlims[i] = std::pow(10, 2.3*i/ptnbins+2); }



  TDirectory* dir = gDirectory;

  std::cout << "SigAnalysis::initialize() Directory " << gDirectory->GetName() << " " << name() << std::endl;

  m_dir = new TIDDirectory(name());
  m_dir->push();

  std::cout << "SigAnalysis::initialize() Directory " << gDirectory->GetName() << " package directory, " << name() << std::endl;

  Efficiency* heff[8];  
  Efficiency* hpurity[6]; 

  // "reference" quantities
  addHistogram(  new TH1F(  "pT",   "pT",   ptnbins,   ptbinlims ) );
  addHistogram(  new TH1F( "eta",  "eta",   etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram(  new TH1F( "phi",  "phi",   phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  addHistogram(  new TH1F(  "z0",   "z0",     zBins,        -zMax,       zMax ) );
  addHistogram(  new TH1F(  "d0",   "d0",    d0Bins,       -d0Max,      d0Max ) );
  addHistogram(  new TH1F(  "a0",   "a0",    a0Bins,       -a0Max,      a0Max ) );
  
  // efficienies and purities
  heff[0]    = new Efficiency( find("pT"),  "pT_eff"  );
  heff[1]    = new Efficiency( find("eta"), "eta_eff" );
  heff[2]    = new Efficiency( find("phi"), "phi_eff" );
  heff[3]    = new Efficiency( find("z0"),  "z0_eff"  );
  heff[4]    = new Efficiency( find("d0"),  "d0_eff"  );
  heff[5]    = new Efficiency( find("a0"),  "a0_eff"  );
  
  heff[6]    = new Efficiency( find("pT"), "pTm_eff" );
  heff[7]    = new Efficiency( find("pT"), "pTp_eff" );

  eff_pt  = heff[0];
  eff_eta = heff[1];
  eff_phi = heff[2];
  eff_z0  = heff[3];
  eff_d0  = heff[4];
  eff_a0  = heff[5];

  eff_ptm = heff[6];
  eff_ptp = heff[7];
  
  hpurity[0] = new Efficiency( find("pT"),  "pT_pur"  );
  hpurity[1] = new Efficiency( find("eta"), "eta_pur" );
  hpurity[2] = new Efficiency( find("phi"), "phi_pur" );
  hpurity[3] = new Efficiency( find("z0"),  "z0_pur"  );
  hpurity[4] = new Efficiency( find("d0"),  "d0_pur"  );
  hpurity[5] = new Efficiency( find("a0"),  "a0_pur"  );

  purity_pt  = hpurity[0];
  purity_eta = hpurity[1];
  purity_phi = hpurity[2];
  purity_z0  = hpurity[3];
  purity_d0  = hpurity[4];
  purity_a0  = hpurity[5];

  // "test" quantities
  addHistogram(    new TH1F(  "pT_rec",   "pT_rec",   ptnbins,    ptbinlims ) );
  addHistogram(    new TH1F( "eta_rec",  "eta_rec",   etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram(    new TH1F( "phi_rec",  "phi_rec",   phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  addHistogram(    new TH1F(  "z0_rec",   "z0_rec",     zBins,        -zMax,       zMax ) );
  addHistogram(    new TH1F(  "d0_rec",   "d0_rec",    d0Bins,       -d0Max,      d0Max ) );
  addHistogram(    new TH1F(  "a0_rec",   "a0_rec",    a0Bins,       -a0Max,      a0Max ) );
    
  // resolutions
  addHistogram(    new TH1F(  "pT_res",   "pT_res",    pTResBins,   -tmp_absResPt,  tmp_absResPt ) );
  addHistogram(    new TH1F( "eta_res",  "eta_res",   etaResBins,  -tmp_absResEta, tmp_absResEta ) );
  addHistogram(    new TH1F( "phi_res",  "phi_res",   phiResBins,  -tmp_absResPhi, tmp_absResPhi ) );
  addHistogram(    new TH1F(  "z0_res",   "z0_res",     zresBins,        -zresMax,       zresMax ) );
  addHistogram(    new TH1F(  "d0_res",   "d0_res",    d0resBins,       -0.5*d0resMax,      0.5*d0resMax ) );
  addHistogram(    new TH1F(  "a0_res",   "a0_res",    a0resBins,       -0.5*a0resMax,      0.5*a0resMax ) );
  
  // hit occupancies
			       
  int   NHits = 40;
  int Ntracks = 50;

  addHistogram( new TH1F( "nsct",     "nsct",     NHits, -0.5, float(NHits-0.5) ) );
  addHistogram( new TH1F( "nsct_rec", "nsct_rec", NHits, -0.5, float(NHits-0.5) ) );

  addHistogram( new TH1F( "npix",     "npix",     NHits, -0.5, float(NHits-0.5) ) );
  addHistogram( new TH1F( "npix_rec", "npix_rec", NHits, -0.5, float(NHits-0.5) ) );

  addHistogram( new TH1F( "ntrt",     "ntrt",     NHits, -0.5, float(NHits-0.5) ) );
  addHistogram( new TH1F( "ntrt_rec", "ntrt_rec", NHits, -0.5, float(NHits-0.5) ) );

  addHistogram( new TH1F( "nstraw",     "nstraw",     NHits*4, -0.5, float(4*NHits-0.5) ) );
  addHistogram( new TH1F( "nstraw_rec", "nstraw_rec", NHits*4, -0.5, float(4*NHits-0.5) ) );

  addHistogram( new TH1F( "ntracks",     "ntracks",     Ntracks, -0.5, float(Ntracks+0.5) ) );
  addHistogram( new TH1F( "ntracks_rec", "ntracks_rec", Ntracks, -0.5, float(Ntracks+0.5) ) );
  

  // beam offset fitting histos
  h2  = new TH2F( "d0vphi",       "d0vphi",        phiBins, -3.142, 3.142, d0Bins, -d0Max, d0Max );  
  h2r = new TH2F( "d0vphi_rec",   "d0vphi_rec",    phiBins, -3.142, 3.142, d0Bins, -d0Max, d0Max );  
  h2m = new TH2F( "d0vphi_match", "d0vphi_match",  phiBins, -3.142, 3.142, d0Bins, -d0Max, d0Max );  

  m_dir->pop();

  dir->cd();

  //  std::cout << "initialize() Directory " << gDirectory->GetName() << " on leaving" << std::endl;
  
}
  


void SigAnalysis::finalise() { 
  std::cout << "SigAnalysis::finalise() " << name() << "\tNreco " << Nreco << " tracks" << std::endl;

  //  if ( Nreco==0 ) return;

  m_dir->push();

  std::cout << "SigAnalysis::finalise() " << gDirectory->GetName() << std::endl;

  //  std::map<std::string, TH1F*>::iterator hitr=m_histos.begin();
  //  std::map<std::string, TH1F*>::iterator hend=m_histos.end();
  //  for ( ; hitr!=hend ; hitr++ ) hitr->second->Write();     

  //  std::cout << "DBG >" << eff_pt->Hist()->GetName() << "< DBG" << std::endl;

  Efficiency* heff[8] = { eff_pt, eff_eta, eff_phi, eff_z0, eff_d0, eff_a0, eff_ptm, eff_ptp };
  for ( int i=8 ; i-- ; ) { heff[i]->finalise();  } // heff[i]->Hist()->Write(); } 

  //  std::cout << "DBG >" << purity_pt->Hist()->GetName() << "< DBG" << std::endl;

  Efficiency* hpurity[6] = { purity_pt, purity_eta, purity_phi, purity_z0, purity_d0, purity_a0 };
  for ( int i=6 ; i-- ; ) { hpurity[i]->finalise();  } //  hpurity[i]->Hist()->Write(); } 

  m_dir->pop();

}


void SigAnalysis::execute(const std::vector<TIDA::Track*>& reftracks,
			  const std::vector<TIDA::Track*>& testtracks,
			  TrackAssociator* matcher ) 
{
  if ( m_print ) std::cout << "SigAnalysis::execute() \t " << name() 
			   << "\tref "  <<  reftracks.size() 
			   << "\ttest " << testtracks.size() << std::endl;

  //  std::cout << "\tx " << m_xBeamReference << "\ty " << m_yBeamReference 
  //   	        << "\tx " << m_xBeamTest      << "\ty " << m_yBeamTest << std::endl;
	    
  
  //  std::cout << "SigAnalysis (resolutions really) filling " << std::endl;

  // should have these as a class variable   
  static std::string  varName[10] = { "pT", "eta", "phi", "z0", "d0", "a0", "nsct", "npix", "ntrt", "nstraw" };  

  //  std::cout << "SigAnalysis ref size " << reftracks.size() << "\ttest size " << testtracks.size() << std::endl; 
  
  std::map<std::string, TH1F*>::iterator hmitr = m_histos.find("ntracks");
  if ( hmitr!=m_histos.end() )   hmitr->second->Fill( reftracks.size() );
  
  hmitr = m_histos.find("ntracks_rec");
  if ( hmitr!=m_histos.end() )   hmitr->second->Fill( testtracks.size() );

  bool dump = false;
  
  for ( int i=reftracks.size() ; i-- ; ) { 
     
    double pTt  = reftracks[i]->pT(); 
    double z0t  = reftracks[i]->z0(); 
    double etat = reftracks[i]->eta(); 
    double phit = reftracks[i]->phi(); 
    double d0t  = reftracks[i]->a0(); 
    // this will be changed when we know the beam spot position
    //   double a0t  =  reftracks[i]->a0() + sin(phit)*m_xBeam - cos(phit)*m_yBeam; 
    double a0t  =  reftracks[i]->a0() + sin(phit)*m_xBeamReference - cos(phit)*m_yBeamReference; 

    double nsctt = reftracks[i]->sctHits(); 
    double npixt = reftracks[i]->pixelHits(); 

    double ntrtt   = reftracks[i]->trHits(); 
    double nstrawt = reftracks[i]->strawHits(); 

    //    std::cout << "Fill h2 " << " " << h2m << " " << *reftracks[i] << std::endl; 

    h2->Fill( phit, d0t );

    const TIDA::Track* matchedreco = matcher->matched(reftracks[i]); 
    
    //    std::cout << "\t\tSigAnalysis " << name() << "\t" << i << " " << *reftracks[i] << " -> ";        

    // raw reference track distributions 
    double vpart[10] = { std::fabs(pTt), etat, phit, z0t, d0t, a0t, nsctt, npixt, ntrtt, nstrawt };
    for ( int it=0 ; it<10 ; it++ ) { 
      // std::string hname = varName[it];
      // std::map<std::string, TH1F*>::iterator hmitr = m_histos.find(hname);
      //  if ( hmitr!=m_histos.end() )   hmitr->second->Fill( vpart[it] );
      
      if ( TH1F* hptr = find( varName[it] ) ) hptr->Fill( vpart[it] ); 
      else std::cerr << "hmmm histo " << varName[it] << " not found" << std::endl;
	   
    }  
    
    
    if ( matchedreco )  {
       
       // efficiency histos
       eff_pt->Fill(std::fabs(pTt));
       eff_z0->Fill(z0t);
       eff_eta->Fill(etat);
       eff_phi->Fill(phit);
       eff_d0->Fill(d0t);
       eff_a0->Fill(a0t);
       
       // signed pT
       if ( pTt<0 ) eff_ptm->Fill(std::fabs(pTt));
       else         eff_ptp->Fill(std::fabs(pTt));
       
       // residual histos
       double pTr  = matchedreco->pT();  
       double z0r  = matchedreco->z0(); 
       double etar = matchedreco->eta();
       double phir = matchedreco->phi();
       double d0r  = matchedreco->a0(); 
       double a0r  = matchedreco->a0() + sin(phir)*m_xBeamTest - cos(phir)*m_yBeamTest; // this will be changed when we know the beam spot position

       if ( h2m ) h2m->Fill( phit, d0t );
      
       //       if ( m_print ) std::cout << "SigAnalysis::execute() \t " << name() << "\t" << i << " " 
       if ( m_print ) std::cout << "SigAnalysis::execute() \t\t" << i << " " 
				<< *reftracks[i] << " -> " << *matchedreco << "\t" 
				<< pTr << " " << pTt << " " << d0r << " " << d0t << std::endl;
    
       double vres[6] = { 1/pTt-1/pTr, etat-etar, phit-phir, z0t-z0r, d0t-d0r, a0t-a0r };
       for ( int it=0 ; it<6 ; it++ ) { 
	 if ( TH1F* hptr = find(varName[it]+"_res") ) hptr->Fill( vres[it] ); 
	 else std::cerr << "hmmm histo " << varName[it]+"_res" << " not found" << std::endl;
      }  

       // in this loop over the reference tracks, could fill efficiency 
       // histograms

     }
     else { 
       // fill efficiencies with unmatched histos
       //       std::cout << "NULL" << std::endl;
       eff_pt->FillDenom(std::fabs(pTt));
       eff_z0->FillDenom(z0t);
       eff_eta->FillDenom(etat);
       eff_phi->FillDenom(phit);
       eff_d0->FillDenom(d0t);
       eff_a0->FillDenom(a0t);

       // signed pT
       if ( pTt<0 ) eff_ptm->FillDenom(std::fabs(pTt));
       else         eff_ptp->FillDenom(std::fabs(pTt));

       if ( std::fabs(pTt)>4000 ) dump = true; 
     }
     
  }


  // for fake/purity histograms, loop over the test tracks
  // and get the corresponding matched reference tracks from the 
  // reverse map in the TrackAscociator class  - revmatched() 

  static int icount = 0;

  //  if ( icount%1000 ) std::cout << "chain " << name() << "\t " << Nreco << " tracks" << std::endl;  
  // if ( icount%1000 ) 
  if ( m_print ) std::cout << "SigAnalysis::execute() \t " << name() << "\t " << icount << " events\t " << testtracks.size() << " tracks (" << Nreco << ")" << "\n---------------" << std::endl;  

  icount++;
  
  Nreco += testtracks.size();

  for ( int i=testtracks.size() ; i-- ; ) { 
     
    //    std::cout << "\t\tSigAnalysis purity " << name() << "\t" << i << " " << *testtracks[i] << " -> ";

    //    double pTr  = std::fabs(testtracks[i]->pT()); 
    double pTr  = testtracks[i]->pT(); 
    double etar = testtracks[i]->eta(); 
    double phir = testtracks[i]->phi(); 
    double z0r  = testtracks[i]->z0(); 
    double d0r  = testtracks[i]->a0(); 
    double a0r  = testtracks[i]->a0() + sin(phir)*m_xBeamTest - cos(phir)*m_yBeamTest; // this will be changed when we know the beam spot position
    //    double a0rp = testtracks[i]->a0() - sin(phir)*m_xBeam - cos(phir)*m_yBeam; // this will be changed when we know the beam spot position

    //    std::cout << "d0 " << d0r << "\tphi " << phir << "\tx " << m_xBeamTest << "\ty " << m_yBeamTest << std::endl;

    double nsctr = testtracks[i]->sctHits(); 
    double npixr = testtracks[i]->pixelHits(); 

    double ntrtr   = testtracks[i]->trHits(); 
    double nstrawr = testtracks[i]->strawHits(); 

    if ( h2r ) h2r->Fill( phir, d0r );

    const TIDA::Track* matchedref = matcher->revmatched(testtracks[i]); 

    //    if ( matchedref )  std::cout << *matchedref << std::endl;
    //    else               std::cout << "NULL" << std::endl;     
        
    // raw test track distributions 
    double vpart[10] = { std::fabs(pTr), etar, phir, z0r, d0r, a0r, nsctr, npixr, ntrtr, nstrawr };
    for ( int it=0 ; it<10 ; it++ ) { 
      // std::string hname = name()+"_"+varName[it]+"_rec";
      //      std::string hname = varName[it]+"_rec";
      //      std::map<std::string, TH1F*>::iterator hmitr = m_histos.find(hname);
      //      if ( hmitr!=m_histos.end() )   hmitr->second->Fill( vpar[it] );
      //      else std::cerr << "hmmm histo " << hname << " not found" << std::endl;
      if ( TH1F* hptr = find(varName[it]+"_rec") ) hptr->Fill( vpart[it] ); 
      else std::cerr << "hmmm histo " << varName[it]+"_rec" << " not found" << std::endl;
    }  
    
    // purities
    if ( matchedref )  {

      //       std::cout << *matchedref << std::endl;
       
       purity_pt->Fill(std::fabs(pTr));
       purity_z0->Fill(z0r);
       purity_eta->Fill(etar);
       purity_phi->Fill(phir);
       purity_d0->Fill(d0r);
       purity_a0->Fill(a0r);
       
    }
    else { 
      //       std::cout << "NULL" << std::endl;
       purity_pt->FillDenom(std::fabs(pTr));
       purity_z0->FillDenom(z0r);
       purity_eta->FillDenom(etar);
       purity_phi->FillDenom(phir);
       purity_d0->FillDenom(d0r);
       purity_a0->FillDenom(a0r);
     }

  }

  if ( dump && m_print ) { 
    
     if ( m_print ) std::cout << "SigAnalysis::execute() missed a high pT track - dumping tracks" << std::endl;
      
    for ( int i=reftracks.size() ; i-- ; ) {

      if ( std::fabs( reftracks[i]->pT() ) > 1000 ) { 
	std::cout << "\t dump " << *reftracks[i];
	const TIDA::Track* matchedreco = matcher->matched(reftracks[i]); 
	if ( matchedreco ) std::cout << " <--> " << *matchedreco << std::endl;
	else               std::cout << std::endl;
      }

    }

    for ( int i=testtracks.size() ; i-- ; ) {     
      const TIDA::Track* matchedref = matcher->revmatched(testtracks[i]); 
      if ( matchedref==0 ) std::cout << "\t\t\t\t\t " << *testtracks[i] << std::endl;      
    }

  }

  //  std::cout << "SigAnalysis::execute() exiting" << std::endl;
    
}



