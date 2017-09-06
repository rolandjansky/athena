/** @file Analysis_Missed.h */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Missed.h"


Analysis_Missed::Analysis_Missed(const std::string& name) : TrackAnalysis(name) {

  // Create track number histograms
  m_h_ntrack     = new TH1D(std::string(m_name+"-ntracks").c_str(),     std::string(m_name+" n tracks").c_str(),          10, -0.5, 9.5);
  m_h_ntrackfrac = new TH1D(std::string(m_name+"-ntracksfrac").c_str(), std::string(m_name+" n tracks fraction").c_str(), 20, -0.5, 1.5);
  addHistogram(m_h_ntrack);
  addHistogram(m_h_ntrackfrac);

  // Create track parameter histograms
  m_h_eta  = new TH1D(std::string(m_name+"-eta").c_str(),  std::string(m_name+" eta distribution").c_str(),  100, -3, 3);
  m_h_phi  = new TH1D(std::string(m_name+"-phi").c_str(),  std::string(m_name+" phi distribution").c_str(),  100, -3.15, 3.15);
  m_h_z0   = new TH1D(std::string(m_name+"-z0").c_str(),   std::string(m_name+" z0 distribution").c_str(),   100, -200.0, 200.0);
  m_h_d0   = new TH1D(std::string(m_name+"-d0").c_str(),   std::string(m_name+" d0 distribution").c_str(),   100, -5.0, 5.0);
  m_h_pT   = new TH1D(std::string(m_name+"-pT").c_str(),   std::string(m_name+" pT distribution").c_str(),   100, -10000, 10000);
  m_h_chi2 = new TH1D(std::string(m_name+"-chi2").c_str(), std::string(m_name+" chi2 distribution").c_str(), 1000, -10, 990);
  addHistogram(m_h_eta);
  addHistogram(m_h_phi);
  addHistogram(m_h_z0);
  addHistogram(m_h_d0);
  addHistogram(m_h_pT);
  addHistogram(m_h_chi2);
  
  // Create track hit histograms
  m_h_blay      = new TH1D(std::string(m_name+"-blay").c_str(),         std::string(m_name+" blayer").c_str(),        2, -0.5, 1.5);
  m_h_pixel     = new TH1D(std::string(m_name+"-pixel").c_str(),        std::string(m_name+" pixel").c_str(),         10, -0.5, 9.5);
  m_h_sct       = new TH1D(std::string(m_name+"-sct").c_str(),          std::string(m_name+" sct").c_str(),           12, -0.5, 11.5);
  m_h_silicon   = new TH1D(std::string(m_name+"-silicon").c_str(),      std::string(m_name+" silicon").c_str(),       20, -0.5, 19.5);
  m_h_straw     = new TH1D(std::string(m_name+"-straw").c_str(),        std::string(m_name+" straw").c_str(),         40, -0.5, 39.5);
  m_h_tr        = new TH1D(std::string(m_name+"-tr").c_str(),           std::string(m_name+" tr").c_str(),            40, -0.5, 39.5);
  m_h_patt      = new TH1D(std::string(m_name+"-pattern").c_str(),      std::string(m_name+" pattern").c_str(),       19, -0.5, 18.5);
  m_h_firstpatt = new TH1D(std::string(m_name+"-firstpattern").c_str(), std::string(m_name+" first pattern").c_str(), 19, -0.5, 18.5);
  addHistogram(m_h_blay);
  addHistogram(m_h_pixel);
  addHistogram(m_h_sct);
  addHistogram(m_h_silicon);
  addHistogram(m_h_straw);
  addHistogram(m_h_tr);
  addHistogram(m_h_patt);
  addHistogram(m_h_firstpatt);
  
  // Create 2D histograms
  
  m_h_etaphi  = new TH2D(std::string(m_name+"-etaphi").c_str(),  std::string(m_name+" etaphi").c_str(),  100, -3.0, 3.0, 100, -3.15, 3.15);  
  m_h_etapatt = new TH2D(std::string(m_name+"-etapatt").c_str(), std::string(m_name+" etapatt").c_str(), 19, -0.5, 18.5, 100, -3.0, 3.0);  
  m_h_phipatt = new TH2D(std::string(m_name+"-phipatt").c_str(), std::string(m_name+" phipatt").c_str(), 19, -0.5, 18.5, 100, -3.15, 3.15);  
  addHistogram(m_h_etaphi);
  addHistogram(m_h_etapatt);
  addHistogram(m_h_phipatt);
  
}


void Analysis_Missed::initialise() {

}



void Analysis_Missed::execute(const std::vector<TIDA::Track*>& referenceTracks,
					 const std::vector<TIDA::Track*>& testTracks,
					 TrackAssociator* /*associator*/) {


  // Get authors
  int mainAuthor=-1; int countMain=0;
  int testAuthor=-1; int countTest=0;
  std::vector<TIDA::Track*>::const_iterator test,  testEnd=testTracks.end();
  for(test=testTracks.begin(); test!=testEnd; test++) {
    // Check main author
    if(mainAuthor==-1) mainAuthor=(*test)->author();
    // Check test author
    if(testAuthor==-1&&((*test)->author()!=mainAuthor)) testAuthor=(*test)->author();
    // Count tracks
    if((*test)->author()==mainAuthor) countMain++;
    if((*test)->author()==testAuthor) countTest++;
  }

  // Fill track number histograms
  if(testAuthor<mainAuthor) {
    m_h_ntrack->Fill((double)(countMain));
    m_h_ntrackfrac->Fill((double)(countMain-countTest)/((double)countMain));
  }

  if(mainAuthor==-1 || testAuthor==-1) return;

  // Loop over test tracks
  std::vector<TIDA::Track*>::const_iterator test2, test2End=testTracks.end();
  std::vector<TIDA::Track*>::const_iterator ref,   refEnd=referenceTracks.end();
  for(test=testTracks.begin(); test!=testEnd; test++) {
    
    // Check pT
    if(fabs((*test)->pT())<2000) continue;
    if(fabs((*test)->eta())>1.5) continue;

    // Check author
    if((*test)->author()!=mainAuthor) continue;

    // Get test parameters
    double testEta  = (*test)->eta();
    double testPhi  = phi((*test)->phi());
    double testZ0   = (*test)->z0();
    double testD0   = (*test)->a0();
    double testPT   = (*test)->pT();
    double testChi2 = (*test)->chi2();

    // Get test hits
    double testBlay    = (*test)->bLayerHits();
    double testPixel   = (*test)->pixelHits();
    double testSct     = (*test)->sctHits();
    double testSilicon = (*test)->siHits();
    double testStraw   = (*test)->strawHits();
    double testTr      = (*test)->trHits();

    unsigned int testPattern = (*test)->hitPattern();

    // Matches
    bool matchOffline=false;
    bool matchOnline=false;

    // Loop over reference tracks
    for(ref=referenceTracks.begin(); ref!=refEnd; ref++){
      // Get ref parameters
      double refEta = (*ref)->eta();
      double refPhi = phi((*ref)->phi());
      // Check matching
      if(deltaR(testEta, refEta, testPhi, refPhi)<0.05) matchOffline=true;
    }
    if(!matchOffline) continue;
    
    // Loop over test tracks
    for(test2=testTracks.begin(); test2!=test2End; test2++) {
      // Check author
      if((*test2)->author()!=testAuthor) continue;
      // Get ref parameters
      double test2Eta = (*test2)->eta();
      double test2Phi = phi((*test2)->phi());
      // Check matching
      if(deltaR(testEta, test2Eta, testPhi, test2Phi)<0.05) matchOnline=true;
    }
    
    if(!matchOnline) {
      
      std::cout << "MISSING TRACK FOUND!" << std::endl;

      // Fill parameter distribution plots
      m_h_etaphi->Fill(testEta,testPhi);
      m_h_eta->Fill(testEta);
      m_h_phi->Fill(testPhi);
      m_h_z0->Fill(testZ0);
      m_h_d0->Fill(testD0);
      m_h_pT->Fill(testPT);
      m_h_chi2->Fill(testChi2);
      
      // Fill hit distribution plots
      m_h_blay->Fill(testBlay);
      m_h_pixel->Fill(testPixel);
      m_h_sct->Fill(testSct);
      m_h_silicon->Fill(testSilicon);
      m_h_straw->Fill(testStraw);
      m_h_tr->Fill(testTr);

      // Fill pattern plots
      bool first=true;
      for(int i=0;i<19;i++) {
	if((testPattern&(int)pow(2,i))!=0) {
	  m_h_patt->Fill(i); 
	  if(first){m_h_firstpatt->Fill(i);first=false;}
	  m_h_etapatt->Fill(i,testEta);     
	  m_h_phipatt->Fill(i,testPhi);     
	}
      }
    }
  }
}



void Analysis_Missed::finalise() {
 
} 



double Analysis_Missed::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}

double Analysis_Missed::deltaR(double eta1, double eta2, double phi1, double phi2){
  return sqrt((eta1-eta2)*(eta1-eta2) + (phi1-phi2)*(phi1-phi2));
}
