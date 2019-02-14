/**
 **     @file    Analysis_Efficiency.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Efficiency.h"



Analysis_Efficiency::Analysis_Efficiency(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut) {                                   

  // Create integral efficiency histograms
  h_integralD = new TH1D(std::string(mname+"-denominator-integral").c_str(), std::string(mname+" integral efficiency denominator").c_str(), 1, 0.0, 1.0);
  h_integralN = new TH1D(std::string(mname+"-numerator-integral").c_str(),   std::string(mname+" integral efficiency numerator").c_str(),   1, 0.0, 1.0);
  h_integral  = new TH1D(std::string(mname+"-integral").c_str(),             std::string(mname+" integral efficiency").c_str(),             1, 0.0, 1.0);
  addHistogram(h_integralD);
  addHistogram(h_integralN);
  addHistogram(h_integral);
    
  // Create differential efficiency histograms
  h_etaD = new TH1D(std::string(mname+"-denominator-eta").c_str(), std::string(mname+" eta efficiency denominator").c_str(), 100, -2.5, 2.5);
  h_phiD = new TH1D(std::string(mname+"-denominator-phi").c_str(), std::string(mname+" phi efficiency denominator").c_str(), 100, -3.142, 3.142);
  h_z0D  = new TH1D(std::string(mname+"-denominator-z0").c_str(),  std::string(mname+" z0 efficiency denominator").c_str(),  100, -200, 200);
  h_d0D  = new TH1D(std::string(mname+"-denominator-d0").c_str(),  std::string(mname+" d0 efficiency denominator").c_str(),  100, -20, 20);
  h_pTD  = new TH1D(std::string(mname+"-denominator-pT").c_str(),  std::string(mname+" pT efficiency denominator").c_str(),  100, -10000, 10000);
  addHistogram(h_etaD);
  addHistogram(h_phiD);
  addHistogram(h_z0D);
  addHistogram(h_d0D);
  addHistogram(h_pTD);
  
  h_etaN = new TH1D(std::string(mname+"-numerator-eta").c_str(), std::string(mname+" eta efficiency numerator").c_str(), 100, -2.5, 2.5);
  h_phiN = new TH1D(std::string(mname+"-numerator-phi").c_str(), std::string(mname+" phi efficiency numerator").c_str(), 100, -3.142, 3.142);
  h_z0N  = new TH1D(std::string(mname+"-numerator-z0").c_str(),  std::string(mname+" z0 efficiency numerator").c_str(),  100, -200, 200);
  h_d0N  = new TH1D(std::string(mname+"-numerator-d0").c_str(),  std::string(mname+" d0 efficiency numerator").c_str(),  100, -20, 20);
  h_pTN  = new TH1D(std::string(mname+"-numerator-pT").c_str(),  std::string(mname+" pT efficiency numerator").c_str(),  100, -10000, 10000);
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
    if(passesPT&&passesD0&&passesZ0)            h_etaD->Fill(referenceEta);
    if(passesPT&&passesD0&&passesZ0&&passesEta) h_phiD->Fill(referencePhi);
    if(passesPT&&passesD0&&passesEta)           h_z0D->Fill(referenceZ0);
    if(passesPT&&passesZ0&&passesEta)           h_d0D->Fill(referenceD0);
    if(passesD0&&passesZ0&&passesEta)           h_pTD->Fill(referencePT);
    if(passesPT&&passesD0&&passesZ0&&passesEta) h_integralD->Fill(0.5);
    
    // Find matched tracks
    const TIDA::Track* test = associator->matched(*reference);

    // Fill histograms
    if(test ) {
        
      // Fill denominator
      if(passesPT&&passesD0&&passesZ0)            h_etaN->Fill(referenceEta);
      if(passesPT&&passesD0&&passesZ0&&passesEta) h_phiN->Fill(referencePhi);
      if(passesPT&&passesD0&&passesEta)           h_z0N->Fill(referenceZ0);  
      if(passesPT&&passesZ0&&passesEta)           h_d0N->Fill(referenceD0);  
      if(passesD0&&passesZ0&&passesEta)           h_pTN->Fill(referencePT);  
      if(passesPT&&passesD0&&passesZ0&&passesEta) h_integralN->Fill(0.5);    
      
    }
  }
}



void Analysis_Efficiency::finalise() {

  h_phiD->Sumw2(); h_phiN->Sumw2(); h_phi->Sumw2(); h_phi->Divide(h_phiN, h_phiD, 1.0, 1.0, "B");
  h_etaD->Sumw2(); h_etaN->Sumw2(); h_eta->Sumw2(); h_eta->Divide(h_etaN, h_etaD, 1.0, 1.0, "B");
  h_z0D->Sumw2();  h_z0N->Sumw2();  h_z0->Sumw2();  h_z0->Divide(h_z0N, h_z0D, 1.0, 1.0, "B");
  h_d0D->Sumw2();  h_d0N->Sumw2();  h_d0->Sumw2();  h_d0->Divide(h_d0N, h_d0D, 1.0, 1.0, "B");
  h_pTD->Sumw2();  h_pTN->Sumw2();  h_pT->Sumw2();  h_pT->Divide(h_pTN, h_pTD, 1.0, 1.0, "B");
  h_integralD->Sumw2();  h_integralN->Sumw2();  h_integral->Sumw2();  h_integral->Divide(h_integralN, h_integralD, 1.0, 1.0, "B");

} 



double Analysis_Efficiency::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


