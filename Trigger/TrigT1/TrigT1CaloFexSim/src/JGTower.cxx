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
  float test_dR = TMath::Sqrt(dPhi*dPhi + dEta*dEta);
  if(acceptEqual)
    return test_dR <= dR;
  else
    return test_dR <  dR;
}

float deltaPhi(float phi1,float phi2){
  //https://root.cern.ch/doc/master/TLorentzVector_8h_source.html#l00456 
  return TVector2::Phi_mpi_pi(phi1-phi2);
}

int GFEX_pFPGA_Int(std::string in){
  if(in=="A") return 1; 
  else if(in=="B") return 2; 
  else if(in=="C") return 3; 

  return -1; 
}

int GFEX_iEta(float eta){
  
  float etaBins [] = {0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0,2.2,2.4,2.5,2.7,2.9,3.1,3.2,3.5,4.0,4.45,4.9};
  int N = sizeof(etaBins)/sizeof(float)-1;
  TH1F* h_eta = new TH1F("h_eta", "", N, etaBins);

  float eta_ = fabs(eta);
  int ieta = h_eta->FindBin(eta_);

  delete h_eta;

  return ieta;
}

std::string GFEX_pFPGA(float eta){
  if(eta <=0 &&eta > -2.5) return "A";
  else if(eta > 0 &&eta <=2.5) return "B";
  else if(fabs(eta)>2.5) return "C";

  return "NOTANFPGA"; 
}

float GTowerArea(float eta){
  /* 
     Returns the gTower area in units of 0.2x0.2 towers
     There are irregular bins from 2.4-2.5 and 3.1-3.2, but the bins from 
     3.1-3.2 has A = 1 
   */ 
  float abs_eta = fabs(eta); 
  if(abs_eta < 2.4) return 1; 
  else if(abs_eta >= 2.4 && abs_eta < 2.5) return 0.5; 
  else if(abs_eta >=2.5 && abs_eta < 3.1) return 1.; 
  else if(abs_eta >= 3.1 && abs_eta < 3.2) return 1.; 
  else if(abs_eta >= 3.2) return 4.; 
  
  return -1; 

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

