/**
 **     @file    Analysis_Distribution.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/



#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Distribution.h"



Analysis_Distribution::Analysis_Distribution(const std::string& name) : TrackAnalysis(name) {

  // Create track parameter histograms
  h_eta = new TH1D(std::string(mname+"-eta").c_str(), std::string(mname+" eta distribution").c_str(), 100, -3, 3);
  h_phi = new TH1D(std::string(mname+"-phi").c_str(), std::string(mname+" phi distribution").c_str(), 100, -3.15, 3.15);
  h_z0  = new TH1D(std::string(mname+"-z0").c_str(),  std::string(mname+" z0 distribution").c_str(),  100, -200.0, 200.0);
  h_d0  = new TH1D(std::string(mname+"-d0").c_str(),  std::string(mname+" d0 distribution").c_str(),  100, -5.0, 5.0);
  h_pT  = new TH1D(std::string(mname+"-pT").c_str(),  std::string(mname+" pT distribution").c_str(),  100, -10000, 10000);
  addHistogram(h_eta);
  addHistogram(h_phi);
  addHistogram(h_z0);
  addHistogram(h_d0);
  addHistogram(h_pT);
  
  // Create track hit histograms
  h_blay    = new TH1D(std::string(mname+"-blay").c_str(),    std::string(mname+" blayer").c_str(),  2, -0.5, 1.5);
  h_pixel   = new TH1D(std::string(mname+"-pixel").c_str(),   std::string(mname+" pixel").c_str(),   10, -0.5, 9.5);
  h_sct     = new TH1D(std::string(mname+"-sct").c_str(),     std::string(mname+" sct").c_str(),     12, -0.5, 11.5);
  h_silicon = new TH1D(std::string(mname+"-silicon").c_str(), std::string(mname+" silicon").c_str(), 20, -0.5, 19.5);
  h_silicon_match = new TH1D(std::string(mname+"-silicon_match").c_str(), std::string(mname+" silicon_match").c_str(), 20, -0.5, 19.5);
  h_straw   = new TH1D(std::string(mname+"-straw").c_str(),   std::string(mname+" straw").c_str(),   40, -0.5, 39.5);
  h_tr      = new TH1D(std::string(mname+"-tr").c_str(),      std::string(mname+" tr").c_str(),      40, -0.5, 39.5);
  addHistogram(h_blay);
  addHistogram(h_pixel);
  addHistogram(h_sct);
  addHistogram(h_silicon);
  addHistogram(h_silicon_match);
  addHistogram(h_straw);
  addHistogram(h_tr);

}



void Analysis_Distribution::initialise() {

}



void Analysis_Distribution::execute(const std::vector<TIDA::Track*>& /*referenceTracks*/,
				    const std::vector<TIDA::Track*>& testTracks,
				    TrackAssociator* /*associator*/) {

  

  // Loop over test tracks
  std::vector<TIDA::Track*>::const_iterator test=testTracks.begin();
  std::vector<TIDA::Track*>::const_iterator testEnd=testTracks.end();

  for ( ; test!=testEnd ; test++ ) {
    
    // Get test parameters
    double testEta = (*test)->eta();
    double testPhi = phi((*test)->phi());
    double testZ0  = (*test)->z0();
    double testD0  = (*test)->a0();
    double testPT  = (*test)->pT();

    if (fabs(testPT)<1000) continue;

    // Get test hits
    double testBlay    = (*test)->bLayerHits();
    double testPixel   = (*test)->pixelHits();
    double testSct     = (*test)->sctHits();
    double testSilicon = (*test)->siHits();
    double testStraw   = (*test)->strawHits();
    double testTr      = (*test)->trHits();

    // Fill parameter distribution plots
    h_eta->Fill(testEta);
    h_phi->Fill(testPhi);
    h_z0->Fill(testZ0);
    h_d0->Fill(testD0);
    h_pT->Fill(testPT);

    // Fill hit distribution plots
    h_blay->Fill(testBlay);
    h_pixel->Fill(testPixel);
    h_sct->Fill(testSct);
    h_silicon->Fill(testSilicon);
    h_straw->Fill(testStraw);
    h_tr->Fill(testTr);

  }

}



void Analysis_Distribution::finalise() {

} 



double Analysis_Distribution::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}

double Analysis_Distribution::deltaR(double eta1, double eta2, double phi1, double phi2){
  return sqrt((eta1-eta2)*(eta1-eta2) + (phi1-phi2)*(phi1-phi2));
}
