
/** @file Analysis_Distribution.h */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Distribution.h"



Analysis_Distribution::Analysis_Distribution(const std::string& name) : TrackAnalysis(name) {

  // Create track parameter histograms
  m_h_eta = new TH1D(std::string(m_name+"-eta").c_str(), std::string(m_name+" eta distribution").c_str(), 100, -3, 3);
  m_h_phi = new TH1D(std::string(m_name+"-phi").c_str(), std::string(m_name+" phi distribution").c_str(), 100, -3.15, 3.15);
  m_h_z0  = new TH1D(std::string(m_name+"-z0").c_str(),  std::string(m_name+" z0 distribution").c_str(),  100, -200.0, 200.0);
  m_h_d0  = new TH1D(std::string(m_name+"-d0").c_str(),  std::string(m_name+" d0 distribution").c_str(),  100, -5.0, 5.0);
  m_h_pT  = new TH1D(std::string(m_name+"-pT").c_str(),  std::string(m_name+" pT distribution").c_str(),  100, -10000, 10000);
  addHistogram(m_h_eta);
  addHistogram(m_h_phi);
  addHistogram(m_h_z0);
  addHistogram(m_h_d0);
  addHistogram(m_h_pT);
  
  // Create track hit histograms
  m_h_blay    = new TH1D(std::string(m_name+"-blay").c_str(),    std::string(m_name+" blayer").c_str(),  2, -0.5, 1.5);
  m_h_pixel   = new TH1D(std::string(m_name+"-pixel").c_str(),   std::string(m_name+" pixel").c_str(),   10, -0.5, 9.5);
  m_h_sct     = new TH1D(std::string(m_name+"-sct").c_str(),     std::string(m_name+" sct").c_str(),     12, -0.5, 11.5);
  m_h_silicon = new TH1D(std::string(m_name+"-silicon").c_str(), std::string(m_name+" silicon").c_str(), 20, -0.5, 19.5);
  m_h_silicon_match = new TH1D(std::string(m_name+"-silicon_match").c_str(), std::string(m_name+" silicon_match").c_str(), 20, -0.5, 19.5);
  m_h_straw   = new TH1D(std::string(m_name+"-straw").c_str(),   std::string(m_name+" straw").c_str(),   40, -0.5, 39.5);
  m_h_tr      = new TH1D(std::string(m_name+"-tr").c_str(),      std::string(m_name+" tr").c_str(),      40, -0.5, 39.5);
  addHistogram(m_h_blay);
  addHistogram(m_h_pixel);
  addHistogram(m_h_sct);
  addHistogram(m_h_silicon);
  addHistogram(m_h_silicon_match);
  addHistogram(m_h_straw);
  addHistogram(m_h_tr);

}



void Analysis_Distribution::initialise() {

}



void Analysis_Distribution::execute(const std::vector<TIDA::Track*>& referenceTracks,
					 const std::vector<TIDA::Track*>& testTracks,
				    TrackAssociator* /*associator*/) {

  

  // Loop over test tracks
  std::vector<TIDA::Track*>::const_iterator test, testEnd=testTracks.end();
  std::vector<TIDA::Track*>::const_iterator ref, refEnd=referenceTracks.end();
  for(test=testTracks.begin(); test!=testEnd; test++) {
    
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
    m_h_eta->Fill(testEta);
    m_h_phi->Fill(testPhi);
    m_h_z0->Fill(testZ0);
    m_h_d0->Fill(testD0);
    m_h_pT->Fill(testPT);

    // Fill hit distribution plots
    m_h_blay->Fill(testBlay);
    m_h_pixel->Fill(testPixel);
    m_h_sct->Fill(testSct);
    m_h_silicon->Fill(testSilicon);
    m_h_straw->Fill(testStraw);
    m_h_tr->Fill(testTr);

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
