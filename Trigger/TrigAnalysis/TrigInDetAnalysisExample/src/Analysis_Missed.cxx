/** @file Analysis_Missed.h */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Missed.h"


Analysis_Missed::Analysis_Missed(const std::string& name) : TrackAnalysis(name) {

  // Create track number histograms
  h_ntrack     = new TH1D(std::string(mname+"-ntracks").c_str(),     std::string(mname+" n tracks").c_str(),          10, -0.5, 9.5);
  h_ntrackfrac = new TH1D(std::string(mname+"-ntracksfrac").c_str(), std::string(mname+" n tracks fraction").c_str(), 20, -0.5, 1.5);
  addHistogram(h_ntrack);
  addHistogram(h_ntrackfrac);

  // Create track parameter histograms
  h_eta  = new TH1D(std::string(mname+"-eta").c_str(),  std::string(mname+" eta distribution").c_str(),  100, -3, 3);
  h_phi  = new TH1D(std::string(mname+"-phi").c_str(),  std::string(mname+" phi distribution").c_str(),  100, -3.15, 3.15);
  h_z0   = new TH1D(std::string(mname+"-z0").c_str(),   std::string(mname+" z0 distribution").c_str(),   100, -200.0, 200.0);
  h_d0   = new TH1D(std::string(mname+"-d0").c_str(),   std::string(mname+" d0 distribution").c_str(),   100, -5.0, 5.0);
  h_pT   = new TH1D(std::string(mname+"-pT").c_str(),   std::string(mname+" pT distribution").c_str(),   100, -10000, 10000);
  h_chi2 = new TH1D(std::string(mname+"-chi2").c_str(), std::string(mname+" chi2 distribution").c_str(), 1000, -10, 990);
  addHistogram(h_eta);
  addHistogram(h_phi);
  addHistogram(h_z0);
  addHistogram(h_d0);
  addHistogram(h_pT);
  addHistogram(h_chi2);
  
  // Create track hit histograms
  h_blay      = new TH1D(std::string(mname+"-blay").c_str(),         std::string(mname+" blayer").c_str(),        2, -0.5, 1.5);
  h_pixel     = new TH1D(std::string(mname+"-pixel").c_str(),        std::string(mname+" pixel").c_str(),         10, -0.5, 9.5);
  h_sct       = new TH1D(std::string(mname+"-sct").c_str(),          std::string(mname+" sct").c_str(),           12, -0.5, 11.5);
  h_silicon   = new TH1D(std::string(mname+"-silicon").c_str(),      std::string(mname+" silicon").c_str(),       20, -0.5, 19.5);
  h_straw     = new TH1D(std::string(mname+"-straw").c_str(),        std::string(mname+" straw").c_str(),         40, -0.5, 39.5);
  h_tr        = new TH1D(std::string(mname+"-tr").c_str(),           std::string(mname+" tr").c_str(),            40, -0.5, 39.5);
  h_patt      = new TH1D(std::string(mname+"-pattern").c_str(),      std::string(mname+" pattern").c_str(),       19, -0.5, 18.5);
  h_firstpatt = new TH1D(std::string(mname+"-firstpattern").c_str(), std::string(mname+" first pattern").c_str(), 19, -0.5, 18.5);
  addHistogram(h_blay);
  addHistogram(h_pixel);
  addHistogram(h_sct);
  addHistogram(h_silicon);
  addHistogram(h_straw);
  addHistogram(h_tr);
  addHistogram(h_patt);
  addHistogram(h_firstpatt);
  
  // Create 2D histograms
  
  h_etaphi  = new TH2D(std::string(mname+"-etaphi").c_str(),  std::string(mname+" etaphi").c_str(),  100, -3.0, 3.0, 100, -3.15, 3.15);  
  h_etapatt = new TH2D(std::string(mname+"-etapatt").c_str(), std::string(mname+" etapatt").c_str(), 19, -0.5, 18.5, 100, -3.0, 3.0);  
  h_phipatt = new TH2D(std::string(mname+"-phipatt").c_str(), std::string(mname+" phipatt").c_str(), 19, -0.5, 18.5, 100, -3.15, 3.15);  
  addHistogram(h_etaphi);
  addHistogram(h_etapatt);
  addHistogram(h_phipatt);
  
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
    h_ntrack->Fill((double)(countMain));
    h_ntrackfrac->Fill((double)(countMain-countTest)/((double)countMain));
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
      h_etaphi->Fill(testEta,testPhi);
      h_eta->Fill(testEta);
      h_phi->Fill(testPhi);
      h_z0->Fill(testZ0);
      h_d0->Fill(testD0);
      h_pT->Fill(testPT);
      h_chi2->Fill(testChi2);
      
      // Fill hit distribution plots
      h_blay->Fill(testBlay);
      h_pixel->Fill(testPixel);
      h_sct->Fill(testSct);
      h_silicon->Fill(testSilicon);
      h_straw->Fill(testStraw);
      h_tr->Fill(testTr);

      // Fill pattern plots
      bool first=true;
      for(int i=0;i<19;i++) {
	if((testPattern&(int)pow(2,i))!=0) {
	  h_patt->Fill(i); 
	  if(first){h_firstpatt->Fill(i);first=false;}
	  h_etapatt->Fill(i,testEta);     
	  h_phipatt->Fill(i,testPhi);     
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
