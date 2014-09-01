/** @file Analysis_Tier0.cxx */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Tier0.h"



Analysis_Tier0::Analysis_Tier0(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut) {                                   ;}

void Analysis_Tier0::initialise() {

  h_total_efficiency = new TProfile ("Eff_overall",  "total efficiency",  1, 0., 1.);
  h_pTeff = new TProfile ("Eff_pT",  "pT efficiency",  100, 0., 100.);
  h_etaeff = new TProfile("Eff_Eta", "eta efficiency",  100, -2.5, 2.5);
  h_phieff = new TProfile("Eff_Phi", "phi  efficiency",  100, -3.142, 3.142);
  h_d0eff = new TProfile ("Eff_d0",  "d0 efficiency",  100, -5., 5.);
  h_z0eff = new TProfile ("Eff_z0",  "z0 efficiency",  100, -200., 200.);
  h_nVtxeff = new TProfile ("Eff_nVtx",  "nVtx efficiency",  30, 0., 30.);
  addHistogram(h_total_efficiency);
  addHistogram(h_pTeff);
  addHistogram(h_etaeff);
  addHistogram(h_phieff);
  addHistogram(h_z0eff);
  addHistogram(h_d0eff);
  addHistogram(h_nVtxeff);

  h_pTres = new TProfile("Res_pT", "pT residual", 100, 0., 100.);
  h_etares = new TProfile("Res_eta", "Eta residual", 100, -2.5, 2.5);
  h_phires = new TProfile("Res_phi", "Phi residual", 100, -3.142, 3.142);
  h_d0res = new TProfile("Res_d0", "d0 residual", 100, -5., 5.);
  h_z0res = new TProfile("Res_z0", "z0 residual", 100, -200., 200.);
  addHistogram(h_pTres);
  addHistogram(h_etares);
  addHistogram(h_phires);
  addHistogram(h_d0res);
  addHistogram(h_z0res);


  h_trkpT  = new TH1D("reftrk_pT" , "Reference track pT", 100, 0., 100.);
  h_trkphi = new TH1D("reftrk_phi", "Reference track Phi", 100, -3.142, 3.142);
  h_trketa = new TH1D("reftrk_eta", "Reference track Eta", 100, -2.5, 2.5) ;
  h_trkd0  = new TH1D("reftrk_d0" , "Reference track d0", 100, -5., 5.);
  h_trkz0  = new TH1D("reftrk_z0" , "Reference track z0", 100, -200., 200.);
  addHistogram(h_trkpT);
  addHistogram(h_trkphi);
  addHistogram(h_trketa);
  addHistogram(h_trkd0);
  addHistogram(h_trkz0);
  
}



void Analysis_Tier0::execute(const std::vector<TrigInDetAnalysis::Track*>& referenceTracks,
			     const std::vector<TrigInDetAnalysis::Track*>& /*testTracks*/,
				  TrackAssociator* associator) {
  
  // Loop over reference tracks
  std::vector<TrigInDetAnalysis::Track*>::const_iterator reference, referenceEnd=referenceTracks.end();
  for(reference=referenceTracks.begin(); reference!=referenceEnd; reference++) {
    
    // Get reference parameters
    double referenceEta = (*reference)->eta();
    double referencePhi = phi((*reference)->phi());
    double referenceZ0  = (*reference)->z0();
    double referenceD0  = (*reference)->a0();
    double referencePT  = fabs((*reference)->pT());
    
    // Find matched tracks
    const TrigInDetAnalysis::Track* test = associator->matched(*reference);

    double eff_weight(0.);
    if(test) eff_weight = 1.0;

    h_total_efficiency->Fill(0.5, eff_weight);
    h_pTeff->Fill(referencePT/1000., eff_weight);
    h_z0eff->Fill(referenceZ0, eff_weight);
    h_d0eff->Fill(referenceD0, eff_weight);
    h_etaeff->Fill(referenceEta, eff_weight);
    h_phieff->Fill(referencePhi, eff_weight);
    h_nVtxeff->Fill(m_nVtx,eff_weight);

    h_trkpT->Fill(referencePT/1000.);
    h_trketa->Fill(referenceEta);
    h_trkphi->Fill(referencePhi);
    h_trkd0->Fill(referenceD0);
    h_trkz0->Fill(referenceZ0);
    
    if(test){
      h_pTres->Fill(referencePT/1000.,(referencePT/1000.)-(fabs(test->pT()/1000.)));
      h_etares->Fill(referenceEta,referenceEta-test->eta() );
      h_phires->Fill(referencePhi,referencePhi-test->phi() );
      h_d0res->Fill(referenceD0,referenceD0-test->a0() );
      h_z0res->Fill(referenceZ0,referenceZ0-test->z0() );
    }

  }
}



void Analysis_Tier0::finalise() {

} 



double Analysis_Tier0::phi(double p) {
  if(p < -M_PI) p += 2*M_PI;
  if(p >  M_PI) p -= 2*M_PI;
  return p;
}


