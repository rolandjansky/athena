/**
 **     @file    Analysis_Fake.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Fake.h"



Analysis_Fake::Analysis_Fake(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut) {                                   

  // Create integral efficiency histograms
  h_integralD = new TH1D(std::string(mname+"-denominator-integral").c_str(), std::string(mname+" integral fake denominator").c_str(), 1, 0.0, 1.0);
  h_integralN = new TH1D(std::string(mname+"-numerator-integral").c_str(),   std::string(mname+" integral fake numerator").c_str(),   1, 0.0, 1.0);
  h_integral  = new TH1D(std::string(mname+"-integral").c_str(),             std::string(mname+" integral fake").c_str(),             1, 0.0, 1.0);
  addHistogram(h_integralD);
  addHistogram(h_integralN);
  addHistogram(h_integral);
    
  // Create differential efficiency histograms
  h_etaD = new TH1D(std::string(mname+"-denominator-eta").c_str(), std::string(mname+" eta fake denominator").c_str(), 100, -2.5, 2.5);
  h_phiD = new TH1D(std::string(mname+"-denominator-phi").c_str(), std::string(mname+" phi fake denominator").c_str(), 100, -3.142, 3.142);
  h_z0D  = new TH1D(std::string(mname+"-denominator-z0").c_str(),  std::string( mname+" z0 fake denominator").c_str(),  100, -200, 200);
  h_d0D  = new TH1D(std::string(mname+"-denominator-d0").c_str(),  std::string( mname+" d0 fake denominator").c_str(),  100, -20, 20);
  h_pTD  = new TH1D(std::string(mname+"-denominator-pT").c_str(),  std::string( mname+" pT fake denominator").c_str(),  100, -10000, 10000);
  addHistogram(h_etaD);
  addHistogram(h_phiD);
  addHistogram(h_z0D);
  addHistogram(h_d0D);
  addHistogram(h_pTD);
  
  h_etaN = new TH1D(std::string(mname+"-numerator-eta").c_str(), std::string(mname+" eta fake numerator").c_str(), 100, -2.5, 2.5);
  h_phiN = new TH1D(std::string(mname+"-numerator-phi").c_str(), std::string(mname+" phi fake numerator").c_str(), 100, -3.142, 3.142);
  h_z0N  = new TH1D(std::string(mname+"-numerator-z0").c_str(),  std::string(mname +" z0 fake numerator").c_str(),  100, -200, 200);
  h_d0N  = new TH1D(std::string(mname+"-numerator-d0").c_str(),  std::string(mname +" d0 fake numerator").c_str(),  100, -20, 20);
  h_pTN  = new TH1D(std::string(mname+"-numerator-pT").c_str(),  std::string(mname +" pT fake numerator").c_str(),  100, -10000, 10000);
  addHistogram(h_etaN);
  addHistogram(h_phiN);
  addHistogram(h_z0N);
  addHistogram(h_d0N);
  addHistogram(h_pTN);
  
  h_eta = new TH1D(std::string(mname+"-eta").c_str(), std::string(mname+" eta efficiency").c_str(), 100, -2.5, 2.5);
  h_phi = new TH1D(std::string(mname+"-phi").c_str(), std::string(mname+" phi efficiency").c_str(), 100, -3.142, 3.142);
  h_z0  = new TH1D(std::string(mname+"-z0").c_str(),  std::string(mname+" z0 efficiency").c_str(),  100, -200, 200);
  h_d0  = new TH1D(std::string(mname+"-d0").c_str(),  std::string(mname+" d0 efficiency").c_str(),  100, -20, 20);
  h_pT  = new TH1D(std::string(mname+"-pT").c_str(),  std::string(mname+" pT efficiency").c_str(),  100, -10000, 10000);
  addHistogram(h_eta);
  addHistogram(h_phi);
  addHistogram(h_z0);
  addHistogram(h_d0);
  addHistogram(h_pT);
  
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
    if(passesPT&&passesD0&&passesZ0)            h_etaD->Fill(testEta);
    if(passesPT&&passesD0&&passesZ0&&passesEta) h_phiD->Fill(testPhi);
    if(passesPT&&passesD0&&passesEta)           h_z0D->Fill(testZ0);
    if(passesPT&&passesZ0&&passesEta)           h_d0D->Fill(testD0);
    if(passesD0&&passesZ0&&passesEta)           h_pTD->Fill(fabs(testPT));
    if(passesPT&&passesD0&&passesZ0&&passesEta) h_integralD->Fill(0.5);
    
    // Find matched tracks
    const TIDA::Track* ref = associator->revmatched(*test);

    // Fill histograms
    if(!ref) {

      // Fill numerator
      if(passesPT&&passesD0&&passesZ0)            h_etaN->Fill(testEta);
      if(passesPT&&passesD0&&passesZ0&&passesEta) h_phiN->Fill(testPhi);
      if(passesPT&&passesD0&&passesEta)           h_z0N->Fill(testZ0);  
      if(passesPT&&passesZ0&&passesEta)           h_d0N->Fill(testD0);  
      if(passesD0&&passesZ0&&passesEta)           h_pTN->Fill(fabs(testPT));  
      if(passesPT&&passesD0&&passesZ0&&passesEta) h_integralN->Fill(0.5);    
      
    }
  }
}



void Analysis_Fake::finalise() {

  h_phiD->Sumw2(); h_phiN->Sumw2(); h_phi->Sumw2(); h_phi->Divide(h_phiN, h_phiD, 1.0, 1.0, "B");
  h_etaD->Sumw2(); h_etaN->Sumw2(); h_eta->Sumw2(); h_eta->Divide(h_etaN, h_etaD, 1.0, 1.0, "B");
  h_z0D->Sumw2();  h_z0N->Sumw2();  h_z0->Sumw2();  h_z0->Divide(h_z0N, h_z0D, 1.0, 1.0, "B");
  h_d0D->Sumw2();  h_d0N->Sumw2();  h_d0->Sumw2();  h_d0->Divide(h_d0N, h_d0D, 1.0, 1.0, "B");
  h_pTD->Sumw2();  h_pTN->Sumw2();  h_pT->Sumw2();  h_pT->Divide(h_pTN, h_pTD, 1.0, 1.0, "B");
  h_integralD->Sumw2();  h_integralN->Sumw2();  h_integral->Sumw2();  h_integral->Divide(h_integralN, h_integralD, 1.0, 1.0, "B");

} 



double Analysis_Fake::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


