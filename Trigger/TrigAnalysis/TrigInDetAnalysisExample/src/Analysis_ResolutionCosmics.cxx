/** @file Analysis_ResolutionCosmics.h */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_ResolutionCosmics.h"



Analysis_ResolutionCosmics::Analysis_ResolutionCosmics(const std::string& name) : TrackAnalysis(name) {

  // Create resolution histograms
  m_h_res_eta   = new TH1D(std::string(m_name+"-Res-eta").c_str(), std::string(m_name+" eta resolution").c_str(), 50, -0.05, 0.05);
  m_h_res_phi   = new TH1D(std::string(m_name+"-Res-phi").c_str(), std::string(m_name+" phi resolution").c_str(), 50, -0.05, 0.05);
  m_h_res_z0    = new TH1D(std::string(m_name+"-Res-z0").c_str(),  std::string(m_name+" z0 resolution").c_str(), 50, -3.0, 3.0);
  m_h_res_d0    = new TH1D(std::string(m_name+"-Res-d0").c_str(),  std::string(m_name+" d0 resolution").c_str(), 50, -3.0, 3.0);
  m_h_res_invpT = new TH1D(std::string(m_name+"-Res-pT").c_str(),  std::string(m_name+" inv-pT resolution").c_str(), 100, -0.0004, 0.0004);
  addHistogram(m_h_res_eta);
  addHistogram(m_h_res_phi);
  addHistogram(m_h_res_z0);
  addHistogram(m_h_res_d0);
  addHistogram(m_h_res_invpT);
  
  // Create pull histograms
  m_h_pull_eta   = new TH1D(std::string(m_name+"-Pull-eta").c_str(), std::string(m_name+" eta pull").c_str(), 50, -20.0, 20.0);
  m_h_pull_phi   = new TH1D(std::string(m_name+"-Pull-phi").c_str(), std::string(m_name+" phi pull").c_str(), 50, -20.0, 20.0);
  m_h_pull_z0    = new TH1D(std::string(m_name+"-Pull-z0").c_str(),  std::string(m_name+" z0 pull").c_str(), 50, -20.0, 20.0);
  m_h_pull_d0    = new TH1D(std::string(m_name+"-Pull-d0").c_str(),  std::string(m_name+" d0 pull").c_str(), 50, -20.0, 20.0);
  m_h_pull_invpT = new TH1D(std::string(m_name+"-Pull-pT").c_str(),  std::string(m_name+" inv-pT pull").c_str(), 50, -20.0, 20.0);
  addHistogram(m_h_pull_eta);
  addHistogram(m_h_pull_phi);
  addHistogram(m_h_pull_z0);
  addHistogram(m_h_pull_d0);
  addHistogram(m_h_pull_invpT);

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
      m_h_res_eta->Fill(fabs(referenceEta)-fabs(testEta));
      m_h_res_phi->Fill(phi(referencePhi-testPhi));
      m_h_res_z0->Fill(fabs(referenceZ0)-fabs(testZ0));
      m_h_res_d0->Fill(fabs(referenceD0)-fabs(testD0));
      m_h_res_invpT->Fill(fabs(1.0/referencePT)-fabs(1.0/testPT));
      
      // Fill pull plots
      if(eeta!=0)   m_h_pull_eta->Fill((fabs(referenceEta)-fabs(testEta))/eeta);           else m_h_pull_eta->Fill(-1000.0);
      if(ephi!=0)   m_h_pull_phi->Fill((phi(referencePhi-testPhi))/ephi);                  else m_h_pull_phi->Fill(-1000.0);
      if(ez0!=0)    m_h_pull_z0->Fill((fabs(referenceZ0)-fabs(testZ0))/ez0);               else m_h_pull_z0->Fill(-1000.0);
      if(ed0!=0)    m_h_pull_d0->Fill((fabs(referenceD0)-fabs(testD0))/ed0);               else m_h_pull_d0->Fill(-1000.0);
      if(einvpT!=0) m_h_pull_invpT->Fill((fabs(1.0/referencePT)-fabs(1.0/testPT))/einvpT); else m_h_pull_invpT->Fill(-1000.0);

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


