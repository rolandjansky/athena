/**
 **     @file    Analysis_ResolutionCosmics.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_ResolutionCosmics.h"



Analysis_ResolutionCosmics::Analysis_ResolutionCosmics(const std::string& name) : TrackAnalysis(name) {

  // Create resolution histograms
  h_res_eta   = new TH1D(std::string(mname+"-Res-eta").c_str(), std::string(mname+" eta resolution").c_str(), 50, -0.05, 0.05);
  h_res_phi   = new TH1D(std::string(mname+"-Res-phi").c_str(), std::string(mname+" phi resolution").c_str(), 50, -0.05, 0.05);
  h_res_z0    = new TH1D(std::string(mname+"-Res-z0").c_str(),  std::string(mname+" z0 resolution").c_str(), 50, -3.0, 3.0);
  h_res_d0    = new TH1D(std::string(mname+"-Res-d0").c_str(),  std::string(mname+" d0 resolution").c_str(), 50, -3.0, 3.0);
  h_res_invpT = new TH1D(std::string(mname+"-Res-pT").c_str(),  std::string(mname+" inv-pT resolution").c_str(), 100, -0.0004, 0.0004);
  addHistogram(h_res_eta);
  addHistogram(h_res_phi);
  addHistogram(h_res_z0);
  addHistogram(h_res_d0);
  addHistogram(h_res_invpT);
  
  // Create pull histograms
  h_pull_eta   = new TH1D(std::string(mname+"-Pull-eta").c_str(), std::string(mname+" eta pull").c_str(), 50, -20.0, 20.0);
  h_pull_phi   = new TH1D(std::string(mname+"-Pull-phi").c_str(), std::string(mname+" phi pull").c_str(), 50, -20.0, 20.0);
  h_pull_z0    = new TH1D(std::string(mname+"-Pull-z0").c_str(),  std::string(mname+" z0 pull").c_str(), 50, -20.0, 20.0);
  h_pull_d0    = new TH1D(std::string(mname+"-Pull-d0").c_str(),  std::string(mname+" d0 pull").c_str(), 50, -20.0, 20.0);
  h_pull_invpT = new TH1D(std::string(mname+"-Pull-pT").c_str(),  std::string(mname+" inv-pT pull").c_str(), 50, -20.0, 20.0);
  addHistogram(h_pull_eta);
  addHistogram(h_pull_phi);
  addHistogram(h_pull_z0);
  addHistogram(h_pull_d0);
  addHistogram(h_pull_invpT);

}



void Analysis_ResolutionCosmics::initialise() {

}



void Analysis_ResolutionCosmics::execute(const std::vector<TIDA::Track*>& referenceTracks,
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
    
    // Find matched tracks
    const TIDA::Track* test = associator->matched(*reference);

    // Fill histograms
    if(test) {

      // Get test parameters
      double testEta = -test->eta();
      double testPhi = phi(test->phi()+M_PI);
      double testZ0  = test->z0();
      double testD0  = -test->a0();
      double testPT  = -test->pT();
	
      // Skip problematic tracks
      if(referencePT==0 || testPT==0) continue;
      
      // Get errors
      double eeta   = sqrt( test->deta()*test->deta() + (*reference)->deta()*(*reference)->deta() );
      double ephi   = sqrt( test->dphi()*test->dphi() + (*reference)->dphi()*(*reference)->dphi() );
      double ez0    = sqrt( test->dz0() *test->dz0()  + (*reference)->dz0() *(*reference)->dz0() );
      double ed0    = sqrt( test->da0() *test->da0()  + (*reference)->da0() *(*reference)->da0() );
      double einvpT = sqrt( test->dpT() *test->dpT()  + (*reference)->dpT() *(*reference)->dpT() );
      
      // Fill resolution plots
      h_res_eta->Fill(fabs(referenceEta)-fabs(testEta));
      h_res_phi->Fill(phi(referencePhi-testPhi));
      h_res_z0->Fill(fabs(referenceZ0)-fabs(testZ0));
      h_res_d0->Fill(fabs(referenceD0)-fabs(testD0));
      h_res_invpT->Fill(fabs(1.0/referencePT)-fabs(1.0/testPT));
      
      // Fill pull plots
      if(eeta!=0)   h_pull_eta->Fill((fabs(referenceEta)-fabs(testEta))/eeta);           else h_pull_eta->Fill(-1000.0);
      if(ephi!=0)   h_pull_phi->Fill((phi(referencePhi-testPhi))/ephi);                  else h_pull_phi->Fill(-1000.0);
      if(ez0!=0)    h_pull_z0->Fill((fabs(referenceZ0)-fabs(testZ0))/ez0);               else h_pull_z0->Fill(-1000.0);
      if(ed0!=0)    h_pull_d0->Fill((fabs(referenceD0)-fabs(testD0))/ed0);               else h_pull_d0->Fill(-1000.0);
      if(einvpT!=0) h_pull_invpT->Fill((fabs(1.0/referencePT)-fabs(1.0/testPT))/einvpT); else h_pull_invpT->Fill(-1000.0);

    }
  }
}



void Analysis_ResolutionCosmics::finalise() {

} 



double Analysis_ResolutionCosmics::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


