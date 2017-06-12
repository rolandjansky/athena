/** @file Analysis_Resolution.h */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Resolution.h"


Analysis_Resolution::Analysis_Resolution(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut) {

  // Create resolution histograms
  m_h_res_eta   = new TH1D(std::string(m_name+"-Res-eta").c_str(), std::string(m_name+" eta resolution").c_str(), 100, -0.02, 0.02);
  m_h_res_phi   = new TH1D(std::string(m_name+"-Res-phi").c_str(), std::string(m_name+" phi resolution").c_str(), 100, -0.01, 0.01);
  m_h_res_z0    = new TH1D(std::string(m_name+"-Res-z0").c_str(),  std::string(m_name+" z0 resolution").c_str(), 100, -3.0, 3.0);
  m_h_res_d0    = new TH1D(std::string(m_name+"-Res-d0").c_str(),  std::string(m_name+" d0 resolution").c_str(), 100, -3.0, 3.0);
  m_h_res_invpT = new TH1D(std::string(m_name+"-Res-pT").c_str(),  std::string(m_name+" inv-pT resolution").c_str(), 100, -0.0004, 0.0004);
  addHistogram(m_h_res_eta);
  addHistogram(m_h_res_phi);
  addHistogram(m_h_res_z0);
  addHistogram(m_h_res_d0);
  addHistogram(m_h_res_invpT);
  
  // Create pull histograms
  m_h_pull_eta   = new TH1D(std::string(m_name+"-Pull-eta").c_str(), std::string(m_name+" eta pull").c_str(), 100, -20.0, 20.0);
  m_h_pull_phi   = new TH1D(std::string(m_name+"-Pull-phi").c_str(), std::string(m_name+" phi pull").c_str(), 100, -20.0, 20.0);
  m_h_pull_z0    = new TH1D(std::string(m_name+"-Pull-z0").c_str(),  std::string(m_name+" z0 pull").c_str(), 100, -20.0, 20.0);
  m_h_pull_d0    = new TH1D(std::string(m_name+"-Pull-d0").c_str(),  std::string(m_name+" d0 pull").c_str(), 100, -20.0, 20.0);
  m_h_pull_invpT = new TH1D(std::string(m_name+"-Pull-pT").c_str(),  std::string(m_name+" inv-pT pull").c_str(), 100, -20.0, 20.0);
  addHistogram(m_h_pull_eta);
  addHistogram(m_h_pull_phi);
  addHistogram(m_h_pull_z0);
  addHistogram(m_h_pull_d0);
  addHistogram(m_h_pull_invpT);

  //Create more specific resolution histograms
  m_h_res_d0VsPt  = new TH2D(std::string(m_name+"-Res-d0VsPt").c_str(), std::string(m_name+" d0VsPt resolution").c_str(), 
			   50, -3.0, 3.0, 500, -30000.0, 30000.0);
  m_h_res_z0VsPt  = new TH2D(std::string(m_name+"-z0VsPt").c_str(), std::string(m_name+" z0VsPt resolution").c_str(), 
			   50, -3.0, 3.0, 500, -30000.0, 30000.0);
  m_h_res_d0VsD0  = new TH2D(std::string(m_name+"-Res-d0VsD0").c_str(),std::string(m_name+" d0VsD0 resolution").c_str(), 
			   50, -3.0, 3.0, 500, -300.0, 300.0);
  m_h_res_z0VsEta = new TH2D(std::string(m_name+"-Res-z0VsEta").c_str(), std::string(m_name+" z0VsEta resolution").c_str(), 
			   50, -3.0, 3.0, 500, -3.0, 3.0);
  m_h_res_d0VsPixelHits_withBLayer    = new TH2D(std::string(m_name+"-Res-d0VsPixel-withBLayer").c_str(), 
					       std::string(m_name+" resolution vs pixelHits (w b-layer)").c_str(), 50, -3, 3, 10, 0, 10);
  m_h_res_d0VsPixelHits_withoutBLayer = new TH2D(std::string(m_name+"-Res-d0VsPixel-withoutBLayer").c_str(),
					       std::string(m_name+" resolution vs pixelHits (w/o b-layer)").c_str(), 50, -3, 3, 10, 0, 10);
  addHistogram(m_h_res_d0VsPt);
  addHistogram(m_h_res_z0VsPt);
  addHistogram(m_h_res_d0VsD0);
  addHistogram(m_h_res_z0VsEta);
  addHistogram(m_h_res_d0VsPixelHits_withBLayer);
  addHistogram(m_h_res_d0VsPixelHits_withoutBLayer);

  //Create more specific pull histograms
  m_h_pull_d0VsEta  = new TH2D(std::string(m_name+"-Pull-d0VsEta").c_str(), std::string(m_name+" d0VsEta pull").c_str(), 
			     50, -5.0, 5.0, 500, -3.0, 3.0);
  m_h_pull_z0VsEta  = new TH2D(std::string(m_name+"-Pull-z0VsEta").c_str(), std::string(m_name+" z0VsEta pull").c_str(), 
			     50, -5.0, 5.0, 500, -3.0, 3.0);
  m_h_pull_d0VsPixelHits_withoutBLayer = new TH2D(std::string(m_name+"-Pull-d0VsPixelHits_withoutBLayer").c_str(),
						std::string(m_name+" d0VsPixelHits pull (without b-layer)").c_str(), 50, -5.0, 5.0, 10, 0.0, 10.0);
  m_h_pull_z0VsPixelHits_withoutBLayer = new TH2D(std::string(m_name+"-Pull-z0VsPixelHits_withoutBLayer").c_str(),
						std::string(m_name+" z0VsPixelHits pull (without b-layer)").c_str(), 50, -5.0, 5.0, 10, 0.0, 10.0);
  m_h_pull_d0VsPixelHits_withBLayer = new TH2D(std::string(m_name+"-Pull-d0VsPixelHits_withBLayer").c_str(),
					     std::string(m_name+" d0VsPixelHits pull (with b-layer)").c_str(), 50, -5.0, 5.0, 10, 0.0, 10.0);
  m_h_pull_z0VsPixelHits_withBLayer = new TH2D(std::string(m_name+"-Pull-z0VsPixelHits_withBLayer").c_str(),
					     std::string(m_name+" z0VsPixelHits pull (with b-layer)").c_str(), 50, -5.0, 5.0, 10, 0.0, 10.0);
  addHistogram(m_h_pull_d0VsEta);
  addHistogram(m_h_pull_z0VsEta);
  addHistogram(m_h_pull_d0VsPixelHits_withoutBLayer);
  addHistogram(m_h_pull_z0VsPixelHits_withoutBLayer);
  addHistogram(m_h_pull_d0VsPixelHits_withBLayer);
  addHistogram(m_h_pull_z0VsPixelHits_withBLayer);  
}



void Analysis_Resolution::initialise() {

}



void Analysis_Resolution::execute(const std::vector<TIDA::Track*>& referenceTracks,
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
    
    if (fabs(referencePT)<m_pTCut) continue;
    if (fabs(referenceEta)>m_etaCut) continue;
    if (fabs(referenceD0)>m_d0Cut) continue;
    if (fabs(referenceZ0)>m_z0Cut) continue;

    // Find matched tracks
    const TIDA::Track* test=0;
    test = associator->matched(*reference);

    // Fill histograms
    if(test) {
      
      // Get test parameters
      double testEta = test->eta();
      double testPhi = phi(test->phi());
      double testZ0  = test->z0();
      double testD0  = test->a0();
      double testPT  = test->pT();

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

      double referenceBLayerHits = (*reference)->bLayerHits();
      double referencePixelHits  = (*reference)->pixelHits();

      //Fill resolution 2D plots
      if (referenceBLayerHits>=1) {
	m_h_res_d0VsPt->Fill(fabs(referenceD0)-fabs(testD0),referencePT);
	m_h_res_z0VsPt->Fill(fabs(referenceZ0)-fabs(testZ0),referencePT);
      }

      if (referencePT <= 10000 || testPT <= 10000) continue;
      
      if (referenceBLayerHits>=1) {
	m_h_res_d0VsD0->Fill(fabs(referenceD0)-fabs(testD0),referenceD0);
	m_h_res_z0VsEta->Fill(fabs(referenceZ0)-fabs(testZ0),referenceEta);
      }

      if (referenceBLayerHits>=1)
	m_h_res_d0VsPixelHits_withBLayer->Fill(fabs(referenceD0)-fabs(testD0),referencePixelHits);
      else     
	m_h_res_d0VsPixelHits_withoutBLayer->Fill(fabs(referenceD0)-fabs(testD0),referencePixelHits);

      //Fill pull 2D plots
      if (referenceBLayerHits>=1) {
	m_h_pull_d0VsEta->Fill(fabs(referenceD0)-fabs(testD0),referenceEta);
	m_h_pull_z0VsEta->Fill(fabs(referenceZ0)-fabs(testZ0),referenceEta);
      }

      if (referenceBLayerHits>=1) {
	if(ed0!=0) m_h_pull_d0VsPixelHits_withBLayer->Fill((fabs(referenceD0)-fabs(testD0))/ed0,referencePixelHits); 
	//	else       m_h_pull_d0VsPixelHits_withBLayer->Fill(-1000.0);
	if(ez0!=0) m_h_pull_z0VsPixelHits_withBLayer->Fill((fabs(referenceZ0)-fabs(testZ0))/ez0,referencePixelHits); 
	//	else       m_h_pull_z0VsPixelHits_withBLayer->Fill(-1000.0);
      } else {
	if(ed0!=0) m_h_pull_d0VsPixelHits_withoutBLayer->Fill((fabs(referenceD0)-fabs(testD0))/ed0,referencePixelHits); 
	// else       m_h_pull_d0VsPixelHits_withoutBLayer->Fill(-1000.0);
	if(ez0!=0) m_h_pull_z0VsPixelHits_withoutBLayer->Fill((fabs(referenceZ0)-fabs(testZ0))/ez0,referencePixelHits); 
	//	else       m_h_pull_z0VsPixelHits_withoutBLayer->Fill(-1000.0);
      }    
    }
  }
}
 


void Analysis_Resolution::finalise() {

} 



double Analysis_Resolution::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


