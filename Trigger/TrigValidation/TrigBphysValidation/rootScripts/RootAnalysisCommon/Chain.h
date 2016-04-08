/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Apr 29 09:49:26 2010 by ROOT version 5.26/00
// from TTree L2_MU4_DiMu_FS_noOS/L2_MU4_DiMu_FS_noOS
// found on file: ../run/bphys.10.root
//////////////////////////////////////////////////////////

#ifndef Chain_h
#define Chain_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>

class Chain {
  public :
    TTree          *fChain;

   // Declaration of leaf types
    Int_t           RunNumber;
    Int_t           LumiBlock;
    Int_t           EventNumber;
    vector<bool>    *All_trk1_valid;
    vector<double>  *All_trk1_pt;
    vector<double>  *All_trk1_eta;
    vector<double>  *All_trk1_phi;
    vector<double>  *All_trk1_d0;
    vector<double>  *All_trk1_z0;
    vector<double>  *All_trk1_charge;
    vector<double>  *All_trk1_dEta;
    vector<double>  *All_trk1_dPhi;
    vector<double>  *All_trk1_mdtHits;
    vector<double>  *All_trk1_rpcHits;
    vector<double>  *All_trk1_tgcHits;
    vector<bool>    *All_trk2_valid;
    vector<double>  *All_trk2_pt;
    vector<double>  *All_trk2_eta;
    vector<double>  *All_trk2_phi;
    vector<double>  *All_trk2_d0;
    vector<double>  *All_trk2_z0;
    vector<double>  *All_trk2_charge;
    vector<double>  *All_trk2_dEta;
    vector<double>  *All_trk2_dPhi;
    vector<double>  *All_trk2_mdtHits;
    vector<double>  *All_trk2_rpcHits;
    vector<double>  *All_trk2_tgcHits;
    vector<double>  *All_mass;
    vector<double>  *All_fitMass;
    vector<double>  *All_chi2;
    vector<double>  *All_vtx_x;
    vector<double>  *All_vtx_y;
    vector<double>  *All_vtx_z;
    vector<bool>    *Matched_trk1_valid;
    vector<double>  *Matched_trk1_pt;
    vector<double>  *Matched_trk1_eta;
    vector<double>  *Matched_trk1_phi;
    vector<double>  *Matched_trk1_d0;
    vector<double>  *Matched_trk1_z0;
    vector<double>  *Matched_trk1_charge;
    vector<double>  *Matched_trk1_dEta;
    vector<double>  *Matched_trk1_dPhi;
    vector<double>  *Matched_trk1_mdtHits;
    vector<double>  *Matched_trk1_rpcHits;
    vector<double>  *Matched_trk1_tgcHits;
    vector<bool>    *Matched_trk2_valid;
    vector<double>  *Matched_trk2_pt;
    vector<double>  *Matched_trk2_eta;
    vector<double>  *Matched_trk2_phi;
    vector<double>  *Matched_trk2_d0;
    vector<double>  *Matched_trk2_z0;
    vector<double>  *Matched_trk2_charge;
    vector<double>  *Matched_trk2_dEta;
    vector<double>  *Matched_trk2_dPhi;
    vector<double>  *Matched_trk2_mdtHits;
    vector<double>  *Matched_trk2_rpcHits;
    vector<double>  *Matched_trk2_tgcHits;
    vector<double>  *Matched_mass;
    vector<double>  *Matched_fitMass;
    vector<double>  *Matched_chi2;
    vector<double>  *Matched_vtx_x;
    vector<double>  *Matched_vtx_y;
    vector<double>  *Matched_vtx_z;
    vector<bool>    *AllTracks_valid;
    vector<double>  *AllTracks_pt;
    vector<double>  *AllTracks_eta;
    vector<double>  *AllTracks_phi;
    vector<double>  *AllTracks_d0;
    vector<double>  *AllTracks_z0;
    vector<double>  *AllTracks_charge;
    vector<double>  *AllTracks_dEta;
    vector<double>  *AllTracks_dPhi;
    vector<bool>    *MatchedTracks_valid;
    vector<double>  *MatchedTracks_pt;
    vector<double>  *MatchedTracks_eta;
    vector<double>  *MatchedTracks_phi;
    vector<double>  *MatchedTracks_d0;
    vector<double>  *MatchedTracks_z0;
    vector<double>  *MatchedTracks_charge;
    vector<double>  *MatchedTracks_dEta;
    vector<double>  *MatchedTracks_dPhi;

   //constructor
    Chain(TTree *tree,
          std::string chain,
          bool doMatchedTracks=true,
          bool doAllTracks=true)
    {
      fChain = tree;
      Init(chain+"_", doMatchedTracks, doAllTracks);
    }

    Int_t    GetEntry(Long64_t entry);
    void     Init(std::string chain,bool doMatchedTracks,bool doAllTracks);
};


Int_t Chain::GetEntry(Long64_t entry)
{
// Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

void Chain::Init(std::string chain,
                 bool doMatchedTracks,
                 bool doAllTracks)
{
   // Set object pointer
  All_trk1_valid = 0;
  All_trk1_pt = 0;
  All_trk1_eta = 0;
  All_trk1_phi = 0;
  All_trk1_d0 = 0;
  All_trk1_z0 = 0;
  All_trk1_charge = 0;
  All_trk1_dEta = 0;
  All_trk1_dPhi = 0;
  All_trk1_mdtHits = 0;
  All_trk1_rpcHits = 0;
  All_trk1_tgcHits = 0;
  All_trk2_valid = 0;
  All_trk2_pt = 0;
  All_trk2_eta = 0;
  All_trk2_phi = 0;
  All_trk2_d0 = 0;
  All_trk2_z0 = 0;
  All_trk2_charge = 0;
  All_trk2_dEta = 0;
  All_trk2_dPhi = 0;
  All_trk2_mdtHits = 0;
  All_trk2_rpcHits = 0;
  All_trk2_tgcHits = 0;
  All_mass = 0;
  All_fitMass = 0;
  All_chi2 = 0;
  All_vtx_x = 0;
  All_vtx_y = 0;
  All_vtx_z = 0;
  Matched_trk1_valid = 0;
  Matched_trk1_pt = 0;
  Matched_trk1_eta = 0;
  Matched_trk1_phi = 0;
  Matched_trk1_d0 = 0;
  Matched_trk1_z0 = 0;
  Matched_trk1_charge = 0;
  Matched_trk1_dEta = 0;
  Matched_trk1_dPhi = 0;
  Matched_trk1_mdtHits = 0;
  Matched_trk1_rpcHits = 0;
  Matched_trk1_tgcHits = 0;
  Matched_trk2_valid = 0;
  Matched_trk2_pt = 0;
  Matched_trk2_eta = 0;
  Matched_trk2_phi = 0;
  Matched_trk2_d0 = 0;
  Matched_trk2_z0 = 0;
  Matched_trk2_charge = 0;
  Matched_trk2_dEta = 0;
  Matched_trk2_dPhi = 0;
  Matched_trk2_mdtHits = 0;
  Matched_trk2_rpcHits = 0;
  Matched_trk2_tgcHits = 0;
  Matched_mass = 0;
  Matched_fitMass = 0;
  Matched_chi2 = 0;
  Matched_vtx_x = 0;
  Matched_vtx_y = 0;
  Matched_vtx_z = 0;
  AllTracks_valid = 0;
  AllTracks_pt = 0;
  AllTracks_eta = 0;
  AllTracks_phi = 0;
  AllTracks_d0 = 0;
  AllTracks_z0 = 0;
  AllTracks_charge = 0;
  AllTracks_dEta = 0;
  AllTracks_dPhi = 0;
  MatchedTracks_valid = 0;
  MatchedTracks_pt = 0;
  MatchedTracks_eta = 0;
  MatchedTracks_phi = 0;
  MatchedTracks_d0 = 0;
  MatchedTracks_z0 = 0;
  MatchedTracks_charge = 0;
  MatchedTracks_dEta = 0;
  MatchedTracks_dPhi = 0;

   // Set branch addresses
  fChain->SetBranchAddress("RunNumber",   &RunNumber  );
  fChain->SetBranchAddress("LumiBlock",   &LumiBlock  );
  fChain->SetBranchAddress("EventNumber", &EventNumber);
  fChain->SetBranchAddress((chain+"All_trk1_valid").c_str(), &All_trk1_valid);
  fChain->SetBranchAddress((chain+"All_trk1_pt").c_str(), &All_trk1_pt);
  fChain->SetBranchAddress((chain+"All_trk1_eta").c_str(), &All_trk1_eta);
  fChain->SetBranchAddress((chain+"All_trk1_phi").c_str(), &All_trk1_phi);
  fChain->SetBranchAddress((chain+"All_trk1_d0").c_str(), &All_trk1_d0);
  fChain->SetBranchAddress((chain+"All_trk1_z0").c_str(), &All_trk1_z0);
  fChain->SetBranchAddress((chain+"All_trk1_charge").c_str(), &All_trk1_charge);
  fChain->SetBranchAddress((chain+"All_trk1_dEta").c_str(), &All_trk1_dEta);
  fChain->SetBranchAddress((chain+"All_trk1_dPhi").c_str(), &All_trk1_dPhi);
  fChain->SetBranchAddress((chain+"All_trk1_mdtHits").c_str(), &All_trk1_mdtHits);
  fChain->SetBranchAddress((chain+"All_trk1_rpcHits").c_str(), &All_trk1_rpcHits);
  fChain->SetBranchAddress((chain+"All_trk1_tgcHits").c_str(), &All_trk1_tgcHits);
  fChain->SetBranchAddress((chain+"All_trk2_valid").c_str(), &All_trk2_valid);
  fChain->SetBranchAddress((chain+"All_trk2_pt").c_str(), &All_trk2_pt);
  fChain->SetBranchAddress((chain+"All_trk2_eta").c_str(), &All_trk2_eta);
  fChain->SetBranchAddress((chain+"All_trk2_phi").c_str(), &All_trk2_phi);
  fChain->SetBranchAddress((chain+"All_trk2_d0").c_str(), &All_trk2_d0);
  fChain->SetBranchAddress((chain+"All_trk2_z0").c_str(), &All_trk2_z0);
  fChain->SetBranchAddress((chain+"All_trk2_charge").c_str(), &All_trk2_charge);
  fChain->SetBranchAddress((chain+"All_trk2_dEta").c_str(), &All_trk2_dEta);
  fChain->SetBranchAddress((chain+"All_trk2_dPhi").c_str(), &All_trk2_dPhi);
  fChain->SetBranchAddress((chain+"All_trk2_mdtHits").c_str(), &All_trk2_mdtHits);
  fChain->SetBranchAddress((chain+"All_trk2_rpcHits").c_str(), &All_trk2_rpcHits);
  fChain->SetBranchAddress((chain+"All_trk2_tgcHits").c_str(), &All_trk2_tgcHits);
  fChain->SetBranchAddress((chain+"All_mass").c_str(), &All_mass);
  fChain->SetBranchAddress((chain+"All_fitMass").c_str(), &All_fitMass);
  fChain->SetBranchAddress((chain+"All_chi2").c_str(), &All_chi2);
  fChain->SetBranchAddress((chain+"All_vtx_x").c_str(), &All_vtx_x);
  fChain->SetBranchAddress((chain+"All_vtx_y").c_str(), &All_vtx_y);
  fChain->SetBranchAddress((chain+"All_vtx_z").c_str(), &All_vtx_z);
  fChain->SetBranchAddress((chain+"Matched_trk1_valid").c_str(), &Matched_trk1_valid);
  fChain->SetBranchAddress((chain+"Matched_trk1_pt").c_str(), &Matched_trk1_pt);
  fChain->SetBranchAddress((chain+"Matched_trk1_eta").c_str(), &Matched_trk1_eta);
  fChain->SetBranchAddress((chain+"Matched_trk1_phi").c_str(), &Matched_trk1_phi);
  fChain->SetBranchAddress((chain+"Matched_trk1_d0").c_str(), &Matched_trk1_d0);
  fChain->SetBranchAddress((chain+"Matched_trk1_z0").c_str(), &Matched_trk1_z0);
  fChain->SetBranchAddress((chain+"Matched_trk1_charge").c_str(), &Matched_trk1_charge);
  fChain->SetBranchAddress((chain+"Matched_trk1_dEta").c_str(), &Matched_trk1_dEta);
  fChain->SetBranchAddress((chain+"Matched_trk1_dPhi").c_str(), &Matched_trk1_dPhi);
  fChain->SetBranchAddress((chain+"Matched_trk1_mdtHits").c_str(), &Matched_trk1_mdtHits);
  fChain->SetBranchAddress((chain+"Matched_trk1_rpcHits").c_str(), &Matched_trk1_rpcHits);
  fChain->SetBranchAddress((chain+"Matched_trk1_tgcHits").c_str(), &Matched_trk1_tgcHits);
  fChain->SetBranchAddress((chain+"Matched_trk2_valid").c_str(), &Matched_trk2_valid);
  fChain->SetBranchAddress((chain+"Matched_trk2_pt").c_str(), &Matched_trk2_pt);
  fChain->SetBranchAddress((chain+"Matched_trk2_eta").c_str(), &Matched_trk2_eta);
  fChain->SetBranchAddress((chain+"Matched_trk2_phi").c_str(), &Matched_trk2_phi);
  fChain->SetBranchAddress((chain+"Matched_trk2_d0").c_str(), &Matched_trk2_d0);
  fChain->SetBranchAddress((chain+"Matched_trk2_z0").c_str(), &Matched_trk2_z0);
  fChain->SetBranchAddress((chain+"Matched_trk2_charge").c_str(), &Matched_trk2_charge);
  fChain->SetBranchAddress((chain+"Matched_trk2_dEta").c_str(), &Matched_trk2_dEta);
  fChain->SetBranchAddress((chain+"Matched_trk2_dPhi").c_str(), &Matched_trk2_dPhi);
  fChain->SetBranchAddress((chain+"Matched_trk2_mdtHits").c_str(), &Matched_trk2_mdtHits);
  fChain->SetBranchAddress((chain+"Matched_trk2_rpcHits").c_str(), &Matched_trk2_rpcHits);
  fChain->SetBranchAddress((chain+"Matched_trk2_tgcHits").c_str(), &Matched_trk2_tgcHits);
  fChain->SetBranchAddress((chain+"Matched_mass").c_str(), &Matched_mass);
  fChain->SetBranchAddress((chain+"Matched_fitMass").c_str(), &Matched_fitMass);
  fChain->SetBranchAddress((chain+"Matched_chi2").c_str(), &Matched_chi2);
  fChain->SetBranchAddress((chain+"Matched_vtx_x").c_str(), &Matched_vtx_x);
  fChain->SetBranchAddress((chain+"Matched_vtx_y").c_str(), &Matched_vtx_y);
  fChain->SetBranchAddress((chain+"Matched_vtx_z").c_str(), &Matched_vtx_z);

  if(doAllTracks) {
    fChain->SetBranchAddress((chain+"AllTracks_valid").c_str(), &AllTracks_valid);
    fChain->SetBranchAddress((chain+"AllTracks_pt").c_str(), &AllTracks_pt);
    fChain->SetBranchAddress((chain+"AllTracks_eta").c_str(), &AllTracks_eta);
    fChain->SetBranchAddress((chain+"AllTracks_phi").c_str(), &AllTracks_phi);
    fChain->SetBranchAddress((chain+"AllTracks_d0").c_str(), &AllTracks_d0);
    fChain->SetBranchAddress((chain+"AllTracks_z0").c_str(), &AllTracks_z0);
    fChain->SetBranchAddress((chain+"AllTracks_charge").c_str(), &AllTracks_charge);
    fChain->SetBranchAddress((chain+"AllTracks_dEta").c_str(), &AllTracks_dEta);
    fChain->SetBranchAddress((chain+"AllTracks_dPhi").c_str(), &AllTracks_dPhi);
  }

  if(doMatchedTracks) {
    fChain->SetBranchAddress((chain+"MatchedTracks_valid").c_str(), &MatchedTracks_valid);
    fChain->SetBranchAddress((chain+"MatchedTracks_pt").c_str(), &MatchedTracks_pt);
    fChain->SetBranchAddress((chain+"MatchedTracks_eta").c_str(), &MatchedTracks_eta);
    fChain->SetBranchAddress((chain+"MatchedTracks_phi").c_str(), &MatchedTracks_phi);
    fChain->SetBranchAddress((chain+"MatchedTracks_d0").c_str(), &MatchedTracks_d0);
    fChain->SetBranchAddress((chain+"MatchedTracks_z0").c_str(), &MatchedTracks_z0);
    fChain->SetBranchAddress((chain+"MatchedTracks_charge").c_str(), &MatchedTracks_charge);
    fChain->SetBranchAddress((chain+"MatchedTracks_dEta").c_str(), &MatchedTracks_dEta);
    fChain->SetBranchAddress((chain+"MatchedTracks_dPhi").c_str(), &MatchedTracks_dPhi);
  }
}

#endif
