/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloFexSim/JGTower.h"
#include "TMath.h"

// Default constructor, should never be used.
// This implementation exists only to print the warning and avoid undefined behavior.
JGTower::JGTower(){
  m_eta = -999;
  m_deta= -999;
  m_phi = -999;
  m_dphi= -999;
}

// Constructor
JGTower::JGTower(float eta, float deta, float phi, float dphi){
  m_eta = eta;
  m_deta = deta;
  m_phi = phi;
  m_dphi = dphi;
}

//Destructor
JGTower::~JGTower(){}

//Tools
bool inBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi){

  bool inEtaBox = fabs(eta1-eta2)<deta;
  if(phi2>TMath::Pi()) phi2 = phi2-2*TMath::Pi();
  float deltaPhi= fabs(phi1-phi2);
  if(deltaPhi>TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;
  bool inPhiBox = deltaPhi<dphi;

  return inEtaBox&&inPhiBox;
}

bool withinRadius(float eta1, float eta2, float phi1, float phi2, float dR, bool acceptEqual){
  //DeltaPhi call immediately below. This is now consistent with the correct definition in root: 
  //https://root.cern.ch/doc/master/TLorentzVector_8h_source.html#l00456
  float dPhi = deltaPhi(phi1, phi2);
  float dEta = eta1-eta2;
  float dR2 = dPhi*dPhi + dEta*dEta;
  if(acceptEqual)
    return dR2 <= dR*dR;
  else
    return dR2 < dR*dR;
}

float deltaPhi(float phi1,float phi2){
  //https://root.cern.ch/doc/master/TLorentzVector_8h_source.html#l00456 
  return TVector2::Phi_mpi_pi(phi1-phi2);
}

//The helpers
TowerHelper::TowerHelper(std::vector<TH2F*> & h_inputs){
 
   TH2F*h_in;
   for(auto& h:h_inputs){
      h_in = (TH2F*) h->Clone("h_in");
      h_bins.push_back(h_in);
   }

}

TowerHelper::~TowerHelper(){
   for(auto& h:h_bins){
      h->Delete();
   }
}

int TowerHelper::iEta(float eta){
   int x=0;
   for(auto& h:h_bins){
      int test = h->GetXaxis()->FindBin(eta);
      int nbin = ((TH1F*)h->ProjectionX())->GetSize()-2;

      if(test==0) continue;
      else if(test<nbin){
        x+=test;
        return x;
      }
      else x+=nbin;
   }
   x+=1;
  return x;
}

int TowerHelper::iPhi(float eta, float phi){
   int y=0;
   for(auto& h:h_bins){
      int test = h->GetXaxis()->FindBin(eta);
      int nbin = ((TH1F*)h->ProjectionX())->GetSize()-2;

      if(test==0 || test>nbin) continue;
      y=h->GetYaxis()->FindBin(phi);
   }
   return y;
}

