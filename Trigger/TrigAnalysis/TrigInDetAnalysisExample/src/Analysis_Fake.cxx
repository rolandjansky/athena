/** @file Analysis_Fake.cxx */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Fake.h"



Analysis_Fake::Analysis_Fake(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut) {                                   

  // Create integral efficiency histograms
  m_h_integralD = new TH1D(std::string(m_name+"-denominator-integral").c_str(), std::string(m_name+" integral fake denominator").c_str(), 1, 0.0, 1.0);
  m_h_integralN = new TH1D(std::string(m_name+"-numerator-integral").c_str(),   std::string(m_name+" integral fake numerator").c_str(),   1, 0.0, 1.0);
  m_h_integral  = new TH1D(std::string(m_name+"-integral").c_str(),             std::string(m_name+" integral fake").c_str(),             1, 0.0, 1.0);
  addHistogram(m_h_integralD);
  addHistogram(m_h_integralN);
  addHistogram(m_h_integral);
    
  // Create differential efficiency histograms
  m_h_etaD = new TH1D(std::string(m_name+"-denominator-eta").c_str(), std::string(m_name+" eta fake denominator").c_str(), 100, -2.5, 2.5);
  m_h_phiD = new TH1D(std::string(m_name+"-denominator-phi").c_str(), std::string(m_name+" phi fake denominator").c_str(), 100, -3.142, 3.142);
  m_h_z0D  = new TH1D(std::string(m_name+"-denominator-z0").c_str(),  std::string( m_name+" z0 fake denominator").c_str(),  100, -200, 200);
  m_h_d0D  = new TH1D(std::string(m_name+"-denominator-d0").c_str(),  std::string( m_name+" d0 fake denominator").c_str(),  100, -20, 20);
  m_h_pTD  = new TH1D(std::string(m_name+"-denominator-pT").c_str(),  std::string( m_name+" pT fake denominator").c_str(),  100, -10000, 10000);
  addHistogram(m_h_etaD);
  addHistogram(m_h_phiD);
  addHistogram(m_h_z0D);
  addHistogram(m_h_d0D);
  addHistogram(m_h_pTD);
  
  m_h_etaN = new TH1D(std::string(m_name+"-numerator-eta").c_str(), std::string(m_name+" eta fake numerator").c_str(), 100, -2.5, 2.5);
  m_h_phiN = new TH1D(std::string(m_name+"-numerator-phi").c_str(), std::string(m_name+" phi fake numerator").c_str(), 100, -3.142, 3.142);
  m_h_z0N  = new TH1D(std::string(m_name+"-numerator-z0").c_str(),  std::string(m_name +" z0 fake numerator").c_str(),  100, -200, 200);
  m_h_d0N  = new TH1D(std::string(m_name+"-numerator-d0").c_str(),  std::string(m_name +" d0 fake numerator").c_str(),  100, -20, 20);
  m_h_pTN  = new TH1D(std::string(m_name+"-numerator-pT").c_str(),  std::string(m_name +" pT fake numerator").c_str(),  100, -10000, 10000);
  addHistogram(m_h_etaN);
  addHistogram(m_h_phiN);
  addHistogram(m_h_z0N);
  addHistogram(m_h_d0N);
  addHistogram(m_h_pTN);
  
  m_h_eta = new TH1D(std::string(m_name+"-eta").c_str(), std::string(m_name+" eta efficiency").c_str(), 100, -2.5, 2.5);
  m_h_phi = new TH1D(std::string(m_name+"-phi").c_str(), std::string(m_name+" phi efficiency").c_str(), 100, -3.142, 3.142);
  m_h_z0  = new TH1D(std::string(m_name+"-z0").c_str(),  std::string(m_name+" z0 efficiency").c_str(),  100, -200, 200);
  m_h_d0  = new TH1D(std::string(m_name+"-d0").c_str(),  std::string(m_name+" d0 efficiency").c_str(),  100, -20, 20);
  m_h_pT  = new TH1D(std::string(m_name+"-pT").c_str(),  std::string(m_name+" pT efficiency").c_str(),  100, -10000, 10000);
  addHistogram(m_h_eta);
  addHistogram(m_h_phi);
  addHistogram(m_h_z0);
  addHistogram(m_h_d0);
  addHistogram(m_h_pT);
  
}  



void Analysis_Fake::initialise() {
  
}



void Analysis_Fake::execute(const std::vector<TIDA::Track*>& ,
			    const std::vector<TIDA::Track*>& testTracks,
			    TrackAssociator* associator) {
  
  // Loop over reference tracks
  // std::vector<TIDA::Track*>::const_iterator reference, referenceEnd=referenceTracks.end();
  std::vector<TIDA::Track*>::const_iterator test, testEnd=testTracks.end();
  for(test=testTracks.begin(); test!=testEnd; test++) {
  
    
    // Get test parameters
    double testEta = (*test)->eta();
    double testPhi = phi((*test)->phi());
    double testZ0  = (*test)->z0();
    double testD0  = (*test)->a0();
    double testPT  = (*test)->pT();
    
    // Fill denominator
    bool passesEta = (fabs(testEta)<m_etaCut);
    bool passesPT  = (fabs(testPT)>m_pTCut);
    bool passesD0  = (fabs(testD0)<m_d0Cut);
    bool passesZ0  = (fabs(testZ0)<m_z0Cut);
    if(passesPT&&passesD0&&passesZ0)            m_h_etaD->Fill(testEta);
    if(passesPT&&passesD0&&passesZ0&&passesEta) m_h_phiD->Fill(testPhi);
    if(passesPT&&passesD0&&passesEta)           m_h_z0D->Fill(testZ0);
    if(passesPT&&passesZ0&&passesEta)           m_h_d0D->Fill(testD0);
    if(passesD0&&passesZ0&&passesEta)           m_h_pTD->Fill(fabs(testPT));
    if(passesPT&&passesD0&&passesZ0&&passesEta) m_h_integralD->Fill(0.5);
    
    // Find matched tracks
    const TIDA::Track* ref = associator->revmatched(*test);

    // Fill histograms
    if(!ref) {

      // Fill numerator
      if(passesPT&&passesD0&&passesZ0)            m_h_etaN->Fill(testEta);
      if(passesPT&&passesD0&&passesZ0&&passesEta) m_h_phiN->Fill(testPhi);
      if(passesPT&&passesD0&&passesEta)           m_h_z0N->Fill(testZ0);  
      if(passesPT&&passesZ0&&passesEta)           m_h_d0N->Fill(testD0);  
      if(passesD0&&passesZ0&&passesEta)           m_h_pTN->Fill(fabs(testPT));  
      if(passesPT&&passesD0&&passesZ0&&passesEta) m_h_integralN->Fill(0.5);    
      
    }
  }
}



void Analysis_Fake::finalise() {

  m_h_phiD->Sumw2(); m_h_phiN->Sumw2(); m_h_phi->Sumw2(); m_h_phi->Divide(m_h_phiN, m_h_phiD, 1.0, 1.0, "B");
  m_h_etaD->Sumw2(); m_h_etaN->Sumw2(); m_h_eta->Sumw2(); m_h_eta->Divide(m_h_etaN, m_h_etaD, 1.0, 1.0, "B");
  m_h_z0D->Sumw2();  m_h_z0N->Sumw2();  m_h_z0->Sumw2();  m_h_z0->Divide(m_h_z0N, m_h_z0D, 1.0, 1.0, "B");
  m_h_d0D->Sumw2();  m_h_d0N->Sumw2();  m_h_d0->Sumw2();  m_h_d0->Divide(m_h_d0N, m_h_d0D, 1.0, 1.0, "B");
  m_h_pTD->Sumw2();  m_h_pTN->Sumw2();  m_h_pT->Sumw2();  m_h_pT->Divide(m_h_pTN, m_h_pTD, 1.0, 1.0, "B");
  m_h_integralD->Sumw2();  m_h_integralN->Sumw2();  m_h_integral->Sumw2();  m_h_integral->Divide(m_h_integralN, m_h_integralD, 1.0, 1.0, "B");

} 



double Analysis_Fake::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


