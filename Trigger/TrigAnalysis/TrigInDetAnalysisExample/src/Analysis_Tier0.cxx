/** @file Analysis_Tier0.cxx */

#include <cmath>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysisExample/Analysis_Tier0.h"



Analysis_Tier0::Analysis_Tier0(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut) :
  TrackAnalysis(name), m_pTCut(pTCut), m_etaCut(etaCut), m_d0Cut(d0Cut), m_z0Cut(z0Cut) {                                   ;}

void Analysis_Tier0::initialise() {

  h_chain = new TH1D( "Chain", name().c_str(), 1, 0, 1 );

  addHistogram(h_chain);

  h_total_efficiency = new TProfile ("Eff_overall",  "total efficiency",  1, 0., 1.);

  h_pTeff   = new TProfile ("Eff_pT",  "pT efficiency",     25,    0.,  100.);
  h_etaeff  = new TProfile("Eff_Eta", "eta efficiency",     25,   -2.5,   2.5);
  h_phieff  = new TProfile("Eff_Phi", "phi  efficiency",    25,   -3.142, 3.142);
  h_d0eff   = new TProfile ("Eff_d0",  "d0 efficiency",     50,  -10.,   10.);
  h_z0eff   = new TProfile ("Eff_z0",  "z0 efficiency",     50, -225.,  225.);
  h_nVtxeff = new TProfile ("Eff_nVtx",  "nVtx efficiency", 41,   -0.5,  40.5);

  addHistogram(h_total_efficiency);
  addHistogram(h_pTeff);
  addHistogram(h_etaeff);
  addHistogram(h_phieff);
  addHistogram(h_z0eff);
  addHistogram(h_d0eff);
  addHistogram(h_nVtxeff);

  h_pTres  = new TProfile("Res_pT", "pT residual",   25,    0.,   100.);
  h_etares = new TProfile("Res_eta", "Eta residual", 25,   -2.5,    2.5);
  h_phires = new TProfile("Res_phi", "Phi residual", 25,   -3.142,  3.142);
  h_d0res  = new TProfile("Res_d0", "d0 residual",   50,  -10.,    10.);
  h_z0res  = new TProfile("Res_z0", "z0 residual",   50, -225.,   225.);

  addHistogram(h_pTres);
  addHistogram(h_etares);
  addHistogram(h_phires);
  addHistogram(h_d0res);
  addHistogram(h_z0res);


  h_trkpT  = new TH1D("reftrk_pT" , "Reference track pT",  25,    0.,    100.);
  h_trkphi = new TH1D("reftrk_phi", "Reference track Phi", 25,   -3.142,   3.142);
  h_trketa = new TH1D("reftrk_eta", "Reference track Eta", 25,   -2.5,     2.5) ;
  h_trkd0  = new TH1D("reftrk_d0" , "Reference track d0",  50,  -10.,     10.);
  h_trkz0  = new TH1D("reftrk_z0" , "Reference track z0",  50, -225.,    225.);

  addHistogram(h_trkpT);
  addHistogram(h_trkphi);
  addHistogram(h_trketa);
  addHistogram(h_trkd0);
  addHistogram(h_trkz0);


  h_trkpT_rec  = new TH1D("testtrk_pT" , "Test track pT",  25,    0.,   100.);
  h_trkphi_rec = new TH1D("testtrk_phi", "Test track Phi", 25,   -3.142,  3.142);
  h_trketa_rec = new TH1D("testtrk_eta", "Test track Eta", 25,   -2.5,    2.5) ;
  h_trkd0_rec  = new TH1D("testtrk_d0" , "Test track d0",  50,  -10.,    10.);
  h_trkz0_rec  = new TH1D("testtrk_z0" , "Test track z0",  50, -225.,   225.);

  addHistogram(h_trkpT_rec);
  addHistogram(h_trkphi_rec);
  addHistogram(h_trketa_rec);
  addHistogram(h_trkd0_rec);
  addHistogram(h_trkz0_rec);
  
}



void Analysis_Tier0::execute(const std::vector<TrigInDetAnalysis::Track*>& referenceTracks,
			     const std::vector<TrigInDetAnalysis::Track*>& /*testTracks*/,
			     TrackAssociator* associator) {
  
  // Loop over reference tracks
  std::vector<TrigInDetAnalysis::Track*>::const_iterator  reference    = referenceTracks.begin();
  std::vector<TrigInDetAnalysis::Track*>::const_iterator  referenceEnd = referenceTracks.end();

  for( ; reference!=referenceEnd ; reference++ ) {
    
    // Get reference parameters
    double referenceEta = (*reference)->eta();
    double referencePhi = phi((*reference)->phi());
    double referenceZ0  = (*reference)->z0();
    double referenceD0  = (*reference)->a0();
    double referencePT  = fabs((*reference)->pT());
    
    // Find matched tracks
    const TrigInDetAnalysis::Track* test = associator->matched(*reference);

    double    eff_weight = 0;
    if (test) eff_weight = 1;

    h_total_efficiency->Fill(0.5, eff_weight);

    h_pTeff->Fill(referencePT*0.001, eff_weight);
    h_z0eff->Fill(referenceZ0, eff_weight);
    h_d0eff->Fill(referenceD0, eff_weight);
    h_etaeff->Fill(referenceEta, eff_weight);
    h_phieff->Fill(referencePhi, eff_weight);
    h_nVtxeff->Fill(m_nVtx,eff_weight);

    h_trkpT->Fill(referencePT*0.001 );
    h_trketa->Fill(referenceEta);
    h_trkphi->Fill(referencePhi);
    h_trkd0->Fill(referenceD0);
    h_trkz0->Fill(referenceZ0);
    
    if(test){
      h_pTres->Fill( referencePT*0.001, (test->pT() - referencePT)*0.001 );
      h_etares->Fill( referenceEta, test->eta() - referenceEta );
      h_phires->Fill( referencePhi, phi(test->phi() - referencePhi) );
      h_d0res->Fill( referenceD0, test->a0() - referenceD0 );
      h_z0res->Fill( referenceZ0, test->z0() - referenceZ0  );

      /// reference tracks values for tracks with a reference track match (not test track values) 
      h_trkpT_rec->Fill(referencePT*0.001 );
      h_trketa_rec->Fill(referenceEta);
      h_trkphi_rec->Fill(referencePhi);
      h_trkd0_rec->Fill(referenceD0);
      h_trkz0_rec->Fill(referenceZ0);
      
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


