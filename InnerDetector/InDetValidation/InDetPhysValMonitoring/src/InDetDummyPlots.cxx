/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDummyPlots.h"
#include "xAODTruth/TruthVertex.h"
#include "InDetPhysValMonitoringUtilities.h"
#include <cmath>
using namespace IDPVM;

InDetDummyPlots::InDetDummyPlots(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_lepton_disappearance_radius{},
  m_low_Pt_lepton_frac{},
  m_nOut_of_lepdeath{},
  m_brem_spectrum{},
  m_energy_remaining{},
  m_energy_remaining_vs_eta{},
  m_energy_remaining_vs_prodR_low_eta{},
  m_energy_remaining_vs_prodR_medium_eta{},
  m_energy_remaining_vs_prodR_high_eta{},
  m_primary_photon_eta_vs_conversion_radius{},
  m_primary_photon_pt_vs_decay_radius{},
  m_primary_photon_pt{},
  m_brem_photon_eta_vs_radius_of_conversion{},
  m_brem_photon_pt_vs_radius_of_origin_of_conversion{} {
  // nop
}

void
InDetDummyPlots::initializePlots() {
  book(m_lepton_disappearance_radius, "lepton_disappearance_radius");
  book(m_low_Pt_lepton_frac, "low_Pt_lepton_frac");
  book(m_nOut_of_lepdeath, "nOut_of_lepdeath");
  book(m_brem_spectrum, "brem_spectrum");
  book(m_energy_remaining, "energy_remaining");
  book(m_energy_remaining_vs_eta, "energy_remaining_vs_eta");
  book(m_energy_remaining_vs_prodR_low_eta, "energy_remaining_vs_prodR_low_eta");
  book(m_energy_remaining_vs_prodR_medium_eta, "energy_remaining_vs_prodR_medium_eta");
  book(m_energy_remaining_vs_prodR_high_eta, "energy_remaining_vs_prodR_high_eta");
  book(m_primary_photon_eta_vs_conversion_radius, "primary_photon_eta_vs_conversion_radius");
  book(m_primary_photon_pt_vs_decay_radius, "primary_photon_pt_vs_decay_radius");
  book(m_primary_photon_pt, "primary_photon_pt");
  book(m_brem_photon_eta_vs_radius_of_conversion, "brem_photon_eta_vs_radius_of_conversion");
  book(m_brem_photon_pt_vs_radius_of_origin_of_conversion, "brem_photon_pt_vs_radius_of_origin_of_conversion");
}

void
InDetDummyPlots::lepton_fill(const xAOD::TruthParticle& truth, float weight) {
  double R = truth.auxdata<float>("prodR");
  double eta = truth.eta();
  double abseta = fabs(eta);

  fillHisto(m_low_Pt_lepton_frac, R, weight);
  if (truth.hasProdVtx()) {
    const xAOD::TruthVertex* vtx = truth.prodVtx();
    double prod_rad = vtx->perp();
    // double prod_z = vtx->z(); unused
    if (truth.hasDecayVtx()) {
      const xAOD::TruthVertex* decay_vtx = truth.decayVtx();
      double decay_rad = decay_vtx->perp();
      int nOut = decay_vtx->nOutgoingParticles();
      if (prod_rad < 150) {
        fillHisto(m_lepton_disappearance_radius, decay_rad);
      }
      fillHisto(m_nOut_of_lepdeath, nOut);
      for (int i = 0; i < nOut; i++) {
        const xAOD::TruthParticle* out = decay_vtx->outgoingParticle(i);
        double brem_e = out->e() * 0.001; // convert MeV to GeV
        double log_e = std::log10(brem_e);
        if (out->pdgId() == 22) {
          fillHisto(m_brem_spectrum, decay_rad, log_e);
        }
      }
    }
    double ECal(1000);            //Inner radius of the ECal, not sure this is correct
    double e_init = truth.e();    //initial electron/positron total energy
    double e_lost(0);             //energy the electron/positron has lost so far
    
    std::vector<const xAOD::TruthParticle*> pers_truth; //Not sure if this is right
    pers_truth.push_back(&truth);
   
    std::vector<double> decays;

    std::cout<<"Tarkin: You may fire when ready \n";
    if(prod_rad < ECal){
      do {
	if(pers_truth.back()->hasDecayVtx()){
	  const xAOD::TruthVertex * decay = pers_truth.back()->decayVtx();
	  double dec_rad = decay->perp();
	  decays.push_back(dec_rad);
	  int temp_nOut = decay->nOutgoingParticles();
	  std::vector<const xAOD::TruthParticle*> outs;
	  for(int i=0; i<temp_nOut; i++){
	    const xAOD::TruthParticle * temp_out = decay->outgoingParticle(i);
	    outs.push_back(temp_out);
	  }
	  if(temp_nOut == 2){
	    if(outs[0]->pdgId() == 22){
	      e_lost += outs[0]->e();
	      pers_truth.push_back(outs[1]);  //Adding electron/positron to pers_truth vector
	    }else if(outs[1]->pdgId() == 22){
	      e_lost += outs[1]->e();
	      pers_truth.push_back(outs[0]);
	    }else{
	      break;
	    }
	  }else{
	    break;
	  }
	  outs.clear();
	}else{
	  break;
	}
      }while(decays.back() < ECal);  
    }
    double e_left = ((e_init - e_lost)/e_init);  //fraction of origina electron/positron energy left when it reaches the ECal
    std::cout<<"Fraction of electron/positron energy remaining: "<<e_left<<"\n";
    fillHisto(m_energy_remaining, prod_rad, e_left);
    fillHisto(m_energy_remaining_vs_eta, eta, e_left);
    if(abseta < 1.0){
      fillHisto(m_energy_remaining_vs_prodR_low_eta, prod_rad, e_left);
    }else if(abseta < 1.8){
      fillHisto(m_energy_remaining_vs_prodR_medium_eta, prod_rad, e_left);
    }else{
      fillHisto(m_energy_remaining_vs_prodR_high_eta, prod_rad, e_left);
    }

  }
}

void
InDetDummyPlots::prim_photon_fill(const xAOD::TruthParticle& truth) {
  double eta = truth.eta();
  double pt = truth.pt() * 0.001; // convert MeV to GeV
  double log_pt = std::log10(pt);

  fillHisto(m_primary_photon_pt, pt);
  if (truth.hasDecayVtx()) {
    const xAOD::TruthVertex* vtx = truth.decayVtx();
    double decay_rad = vtx->perp();
    fillHisto(m_primary_photon_eta_vs_conversion_radius, decay_rad, eta);
    fillHisto(m_primary_photon_pt_vs_decay_radius, decay_rad, log_pt);
  }
}

void
InDetDummyPlots::brem_photon_fill(const xAOD::TruthParticle& truth) {
  double eta = truth.eta();
  double pt = truth.pt() * 0.001; // convert MeV to GeV
  double log_pt = std::log10(pt);
  const xAOD::TruthVertex* vtx = truth.prodVtx();

  // double prod_rad = vtx->perp(); unused
  if (truth.hasDecayVtx()) {
    double decay_rad = vtx->perp();
    fillHisto(m_brem_photon_eta_vs_radius_of_conversion, decay_rad, eta);
    fillHisto(m_brem_photon_pt_vs_radius_of_origin_of_conversion, decay_rad, log_pt);
  }
}

void
InDetDummyPlots::finalizePlots() {
}
