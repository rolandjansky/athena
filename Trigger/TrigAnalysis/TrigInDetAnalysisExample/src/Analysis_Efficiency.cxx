/** @file Analysis_Efficiency.cxx */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Efficiency.h"



Analysis_Efficiency::Analysis_Efficiency(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut) {                                   

  // Create integral efficiency histograms
  m_h_integralD = new TH1D(std::string(m_name+"-denominator-integral").c_str(), std::string(m_name+" integral efficiency denominator").c_str(), 1, 0.0, 1.0);
  m_h_integralN = new TH1D(std::string(m_name+"-numerator-integral").c_str(),   std::string(m_name+" integral efficiency numerator").c_str(),   1, 0.0, 1.0);
  m_h_integral  = new TH1D(std::string(m_name+"-integral").c_str(),             std::string(m_name+" integral efficiency").c_str(),             1, 0.0, 1.0);
  addHistogram(m_h_integralD);
  addHistogram(m_h_integralN);
  addHistogram(m_h_integral);
    
  // Create differential efficiency histograms
  m_h_etaD = new TH1D(std::string(m_name+"-denominator-eta").c_str(), std::string(m_name+" eta efficiency denominator").c_str(), 100, -2.5, 2.5);
  m_h_phiD = new TH1D(std::string(m_name+"-denominator-phi").c_str(), std::string(m_name+" phi efficiency denominator").c_str(), 100, -3.142, 3.142);
  m_h_z0D  = new TH1D(std::string(m_name+"-denominator-z0").c_str(),  std::string(m_name+" z0 efficiency denominator").c_str(),  100, -200, 200);
  m_h_d0D  = new TH1D(std::string(m_name+"-denominator-d0").c_str(),  std::string(m_name+" d0 efficiency denominator").c_str(),  100, -20, 20);
  m_h_pTD  = new TH1D(std::string(m_name+"-denominator-pT").c_str(),  std::string(m_name+" pT efficiency denominator").c_str(),  100, -10000, 10000);
  addHistogram(m_h_etaD);
  addHistogram(m_h_phiD);
  addHistogram(m_h_z0D);
  addHistogram(m_h_d0D);
  addHistogram(m_h_pTD);
  
  m_h_etaN = new TH1D(std::string(m_name+"-numerator-eta").c_str(), std::string(m_name+" eta efficiency numerator").c_str(), 100, -2.5, 2.5);
  m_h_phiN = new TH1D(std::string(m_name+"-numerator-phi").c_str(), std::string(m_name+" phi efficiency numerator").c_str(), 100, -3.142, 3.142);
  m_h_z0N  = new TH1D(std::string(m_name+"-numerator-z0").c_str(),  std::string(m_name+" z0 efficiency numerator").c_str(),  100, -200, 200);
  m_h_d0N  = new TH1D(std::string(m_name+"-numerator-d0").c_str(),  std::string(m_name+" d0 efficiency numerator").c_str(),  100, -20, 20);
  m_h_pTN  = new TH1D(std::string(m_name+"-numerator-pT").c_str(),  std::string(m_name+" pT efficiency numerator").c_str(),  100, -10000, 10000);
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



void Analysis_Efficiency::initialise() {
  
}



void Analysis_Efficiency::execute(const std::vector<TIDA::Track*>& referenceTracks,
				  const std::vector<TIDA::Track*>& /*testTracks*/,
				  TrackAssociator* associator) {
  
  // Loop over reference tracks
  std::vector<TIDA::Track*>::const_iterator reference, referenceEnd=referenceTracks.end();
  for(reference=referenceTracks.begin(); reference!=referenceEnd; reference++) {
    
    // Get reference parameters
    double referenceEta = (*reference)->eta();
    double referencePhi = phi((*reference)->phi());
    double referenceZ0  = (*reference)->z0();
    double referenceD0  = (*reference)->a0();
    double referencePT  = (*reference)->pT();
    
    // Fill denominator
    bool passesEta = (fabs(referenceEta)<m_etaCut);
    bool passesPT  = (fabs(referencePT)>m_pTCut);
    bool passesD0  = (fabs(referenceD0)<m_d0Cut);
    bool passesZ0  = (fabs(referenceZ0)<m_z0Cut);
    if(passesPT&&passesD0&&passesZ0)            m_h_etaD->Fill(referenceEta);
    if(passesPT&&passesD0&&passesZ0&&passesEta) m_h_phiD->Fill(referencePhi);
    if(passesPT&&passesD0&&passesEta)           m_h_z0D->Fill(referenceZ0);
    if(passesPT&&passesZ0&&passesEta)           m_h_d0D->Fill(referenceD0);
    if(passesD0&&passesZ0&&passesEta)           m_h_pTD->Fill(referencePT);
    if(passesPT&&passesD0&&passesZ0&&passesEta) m_h_integralD->Fill(0.5);
    
    // Find matched tracks
    const TIDA::Track* test = associator->matched(*reference);

    // Fill histograms
    if(test ) {
        
      // Fill denominator
      if(passesPT&&passesD0&&passesZ0)            m_h_etaN->Fill(referenceEta);
      if(passesPT&&passesD0&&passesZ0&&passesEta) m_h_phiN->Fill(referencePhi);
      if(passesPT&&passesD0&&passesEta)           m_h_z0N->Fill(referenceZ0);  
      if(passesPT&&passesZ0&&passesEta)           m_h_d0N->Fill(referenceD0);  
      if(passesD0&&passesZ0&&passesEta)           m_h_pTN->Fill(referencePT);  
      if(passesPT&&passesD0&&passesZ0&&passesEta) m_h_integralN->Fill(0.5);    
      
    }
  }
}



void Analysis_Efficiency::finalise() {

  m_h_phiD->Sumw2(); m_h_phiN->Sumw2(); m_h_phi->Sumw2(); m_h_phi->Divide(m_h_phiN, m_h_phiD, 1.0, 1.0, "B");
  m_h_etaD->Sumw2(); m_h_etaN->Sumw2(); m_h_eta->Sumw2(); m_h_eta->Divide(m_h_etaN, m_h_etaD, 1.0, 1.0, "B");
  m_h_z0D->Sumw2();  m_h_z0N->Sumw2();  m_h_z0->Sumw2();  m_h_z0->Divide(m_h_z0N, m_h_z0D, 1.0, 1.0, "B");
  m_h_d0D->Sumw2();  m_h_d0N->Sumw2();  m_h_d0->Sumw2();  m_h_d0->Divide(m_h_d0N, m_h_d0D, 1.0, 1.0, "B");
  m_h_pTD->Sumw2();  m_h_pTN->Sumw2();  m_h_pT->Sumw2();  m_h_pT->Divide(m_h_pTN, m_h_pTD, 1.0, 1.0, "B");
  m_h_integralD->Sumw2();  m_h_integralN->Sumw2();  m_h_integral->Sumw2();  m_h_integral->Divide(m_h_integralN, m_h_integralD, 1.0, 1.0, "B");

} 



double Analysis_Efficiency::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


