/** @file Analysis_Resolution.h */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Resolution.h"


Analysis_Resolution::Analysis_Resolution(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut) {

  // Create resolution histograms
  h_res_eta   = new TH1D(std::string(mname+"-Res-eta").c_str(), std::string(mname+" eta resolution").c_str(), 100, -0.02, 0.02);
  h_res_phi   = new TH1D(std::string(mname+"-Res-phi").c_str(), std::string(mname+" phi resolution").c_str(), 100, -0.01, 0.01);
  h_res_z0    = new TH1D(std::string(mname+"-Res-z0").c_str(),  std::string(mname+" z0 resolution").c_str(), 100, -3.0, 3.0);
  h_res_d0    = new TH1D(std::string(mname+"-Res-d0").c_str(),  std::string(mname+" d0 resolution").c_str(), 100, -3.0, 3.0);
  h_res_invpT = new TH1D(std::string(mname+"-Res-pT").c_str(),  std::string(mname+" inv-pT resolution").c_str(), 100, -0.0004, 0.0004);
  addHistogram(h_res_eta);
  addHistogram(h_res_phi);
  addHistogram(h_res_z0);
  addHistogram(h_res_d0);
  addHistogram(h_res_invpT);
  
  // Create pull histograms
  h_pull_eta   = new TH1D(std::string(mname+"-Pull-eta").c_str(), std::string(mname+" eta pull").c_str(), 100, -20.0, 20.0);
  h_pull_phi   = new TH1D(std::string(mname+"-Pull-phi").c_str(), std::string(mname+" phi pull").c_str(), 100, -20.0, 20.0);
  h_pull_z0    = new TH1D(std::string(mname+"-Pull-z0").c_str(),  std::string(mname+" z0 pull").c_str(), 100, -20.0, 20.0);
  h_pull_d0    = new TH1D(std::string(mname+"-Pull-d0").c_str(),  std::string(mname+" d0 pull").c_str(), 100, -20.0, 20.0);
  h_pull_invpT = new TH1D(std::string(mname+"-Pull-pT").c_str(),  std::string(mname+" inv-pT pull").c_str(), 100, -20.0, 20.0);
  addHistogram(h_pull_eta);
  addHistogram(h_pull_phi);
  addHistogram(h_pull_z0);
  addHistogram(h_pull_d0);
  addHistogram(h_pull_invpT);

  //Create more specific resolution histograms
  h_res_d0VsPt  = new TH2D(std::string(mname+"-Res-d0VsPt").c_str(), std::string(mname+" d0VsPt resolution").c_str(), 
			   50, -3.0, 3.0, 500, -30000.0, 30000.0);
  h_res_z0VsPt  = new TH2D(std::string(mname+"-z0VsPt").c_str(), std::string(mname+" z0VsPt resolution").c_str(), 
			   50, -3.0, 3.0, 500, -30000.0, 30000.0);
  h_res_d0VsD0  = new TH2D(std::string(mname+"-Res-d0VsD0").c_str(),std::string(mname+" d0VsD0 resolution").c_str(), 
			   50, -3.0, 3.0, 500, -300.0, 300.0);
  h_res_z0VsEta = new TH2D(std::string(mname+"-Res-z0VsEta").c_str(), std::string(mname+" z0VsEta resolution").c_str(), 
			   50, -3.0, 3.0, 500, -3.0, 3.0);
  h_res_d0VsPixelHits_withBLayer    = new TH2D(std::string(mname+"-Res-d0VsPixel-withBLayer").c_str(), 
					       std::string(mname+" resolution vs pixelHits (w b-layer)").c_str(), 50, -3, 3, 10, 0, 10);
  h_res_d0VsPixelHits_withoutBLayer = new TH2D(std::string(mname+"-Res-d0VsPixel-withoutBLayer").c_str(),
					       std::string(mname+" resolution vs pixelHits (w/o b-layer)").c_str(), 50, -3, 3, 10, 0, 10);
  addHistogram(h_res_d0VsPt);
  addHistogram(h_res_z0VsPt);
  addHistogram(h_res_d0VsD0);
  addHistogram(h_res_z0VsEta);
  addHistogram(h_res_d0VsPixelHits_withBLayer);
  addHistogram(h_res_d0VsPixelHits_withoutBLayer);

  //Create more specific pull histograms
  h_pull_d0VsEta  = new TH2D(std::string(mname+"-Pull-d0VsEta").c_str(), std::string(mname+" d0VsEta pull").c_str(), 
			     50, -5.0, 5.0, 500, -3.0, 3.0);
  h_pull_z0VsEta  = new TH2D(std::string(mname+"-Pull-z0VsEta").c_str(), std::string(mname+" z0VsEta pull").c_str(), 
			     50, -5.0, 5.0, 500, -3.0, 3.0);
  h_pull_d0VsPixelHits_withoutBLayer = new TH2D(std::string(mname+"-Pull-d0VsPixelHits_withoutBLayer").c_str(),
						std::string(mname+" d0VsPixelHits pull (without b-layer)").c_str(), 50, -5.0, 5.0, 10, 0.0, 10.0);
  h_pull_z0VsPixelHits_withoutBLayer = new TH2D(std::string(mname+"-Pull-z0VsPixelHits_withoutBLayer").c_str(),
						std::string(mname+" z0VsPixelHits pull (without b-layer)").c_str(), 50, -5.0, 5.0, 10, 0.0, 10.0);
  h_pull_d0VsPixelHits_withBLayer = new TH2D(std::string(mname+"-Pull-d0VsPixelHits_withBLayer").c_str(),
					     std::string(mname+" d0VsPixelHits pull (with b-layer)").c_str(), 50, -5.0, 5.0, 10, 0.0, 10.0);
  h_pull_z0VsPixelHits_withBLayer = new TH2D(std::string(mname+"-Pull-z0VsPixelHits_withBLayer").c_str(),
					     std::string(mname+" z0VsPixelHits pull (with b-layer)").c_str(), 50, -5.0, 5.0, 10, 0.0, 10.0);
  addHistogram(h_pull_d0VsEta);
  addHistogram(h_pull_z0VsEta);
  addHistogram(h_pull_d0VsPixelHits_withoutBLayer);
  addHistogram(h_pull_z0VsPixelHits_withoutBLayer);
  addHistogram(h_pull_d0VsPixelHits_withBLayer);
  addHistogram(h_pull_z0VsPixelHits_withBLayer);  
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

      double referenceBLayerHits = (*reference)->bLayerHits();
      double referencePixelHits  = (*reference)->pixelHits();

      //Fill resolution 2D plots
      if (referenceBLayerHits>=1) {
	h_res_d0VsPt->Fill(fabs(referenceD0)-fabs(testD0),referencePT);
	h_res_z0VsPt->Fill(fabs(referenceZ0)-fabs(testZ0),referencePT);
      }

      if (referencePT <= 10000 || testPT <= 10000) continue;
      
      if (referenceBLayerHits>=1) {
	h_res_d0VsD0->Fill(fabs(referenceD0)-fabs(testD0),referenceD0);
	h_res_z0VsEta->Fill(fabs(referenceZ0)-fabs(testZ0),referenceEta);
      }

      if (referenceBLayerHits>=1)
	h_res_d0VsPixelHits_withBLayer->Fill(fabs(referenceD0)-fabs(testD0),referencePixelHits);
      else     
	h_res_d0VsPixelHits_withoutBLayer->Fill(fabs(referenceD0)-fabs(testD0),referencePixelHits);

      //Fill pull 2D plots
      if (referenceBLayerHits>=1) {
	h_pull_d0VsEta->Fill(fabs(referenceD0)-fabs(testD0),referenceEta);
	h_pull_z0VsEta->Fill(fabs(referenceZ0)-fabs(testZ0),referenceEta);
      }

      if (referenceBLayerHits>=1) {
	if(ed0!=0) h_pull_d0VsPixelHits_withBLayer->Fill((fabs(referenceD0)-fabs(testD0))/ed0,referencePixelHits); 
	//	else       h_pull_d0VsPixelHits_withBLayer->Fill(-1000.0);
	if(ez0!=0) h_pull_z0VsPixelHits_withBLayer->Fill((fabs(referenceZ0)-fabs(testZ0))/ez0,referencePixelHits); 
	//	else       h_pull_z0VsPixelHits_withBLayer->Fill(-1000.0);
      } else {
	if(ed0!=0) h_pull_d0VsPixelHits_withoutBLayer->Fill((fabs(referenceD0)-fabs(testD0))/ed0,referencePixelHits); 
	// else       h_pull_d0VsPixelHits_withoutBLayer->Fill(-1000.0);
	if(ez0!=0) h_pull_z0VsPixelHits_withoutBLayer->Fill((fabs(referenceZ0)-fabs(testZ0))/ez0,referencePixelHits); 
	//	else       h_pull_z0VsPixelHits_withoutBLayer->Fill(-1000.0);
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


