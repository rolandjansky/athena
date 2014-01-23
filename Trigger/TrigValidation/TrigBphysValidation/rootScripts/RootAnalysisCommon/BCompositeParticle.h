/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jun  9 15:06:36 2010 by ROOT version 5.26/00
// from TTree JPSI/JPSI
// found on file: /home/scheiric/bulk/data/output_JpsiOfficial/2010-06-09/user.JamesCatmore.155697.f261_m496.muonstream_StandardGRL_prelim_muon_yellowPlus_7TeV-v02-pass1.staco.UKI-NORTHGRID-SHEF-HEP_SCRATCHDISK.bphys.root
//////////////////////////////////////////////////////////

#ifndef BCompositeParticle_h
#define BCompositeParticle_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>

class BCompositeParticle {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // smart getter methods
   TVector3               getTrackMom(int i);                           // get i-th original track momentum
   TLorentzVector         getTrackLorentzMom(int i, double mass);       // get i-th original track 4-momentum

   TVector3               getRefTrackMom(int i);                        // get i-th refitted track momentum
   TLorentzVector         getRefTrackLorentzMom(int i, double mass);    // get i-th refitted track 4-momentum

   TVector3               getTrueTrackMom(int i);                       // get i-th original track momentum
   TLorentzVector         getTrueTrackLorentzMom(int i);                // get i-th original track 4-momentum

   TVector3               getVertex();                                  // get reconstructed vertex position
   TVector3               getTrueVertex();                              // get true vertex position (if filled)
   std::vector<TVector3>  getPrimVertex();                              // get primary vertex position (if filled)
   TVector3               getTruePrimVertex();                          // get true primary vertex position (if filled)

   TVector3               getRefMom();                                  // get refitted momentum
   TLorentzVector         getRefLorentzMom();                           // get refitted 4-momentum
   TLorentzVector         getRefLorentzMom(double mass);                // get refitted 4-momentum taking external mass of the paricle

   std::vector<int>       getParticlesInEvent(int runNum, int evtNum);  // get entry numbers of particles in given event

   bool                   findTrack(int i, TrackBag tracks, Track& track);  // find corresponding Track object
   bool                   findMuon(int i, MuonBag muons, Muon& muon);       // find corresponding Track object

   std::string            m_name;

   // Declaration of leaf types
   Int_t           eventNumber;
   Int_t           lumiBlock;
   Int_t           runNumber;
   vector<double>  *PV_X;
   vector<double>  *PV_Y;
   vector<double>  *PV_Z;
   vector<int>     *PV_type;
   vector<int>     *PV_index;
   Double_t        TRUEPV_X;
   Double_t        TRUEPV_Y;
   Double_t        TRUEPV_Z;
   Int_t           charge;
   Bool_t          isSignal;
   Double_t        VTX_pt;
   Double_t        VTX_ptError;
   Int_t           index;
   Double_t        VTX_xposition;
   Double_t        VTX_yposition;
   Double_t        VTX_zposition;
   Double_t        VTX_xxPosCov;
   Double_t        VTX_xyPosCov;
   Double_t        VTX_xzPosCov;
   Double_t        VTX_yyPosCov;
   Double_t        VTX_yzPosCov;
   Double_t        VTX_zzPosCov;
   Double_t        VTX_chi2;
   Int_t           VTX_NDF;
   Double_t        VTX_mass;
   Double_t        VTX_massError;
   Double_t        VTX_px;
   Double_t        VTX_py;
   Double_t        VTX_pz;
   Double_t        VTX_rapidity;
   vector<double>  *VTX_refTrksPx;
   vector<double>  *VTX_refTrksPy;
   vector<double>  *VTX_refTrksPz;
   vector<double>  *VTX_lxy;
   vector<double>  *VTX_lxyError;
   vector<double>  *VTX_tau;
   vector<double>  *VTX_tauError;
   vector<double>  *VTX_refPVx;
   vector<double>  *VTX_refPVy;
   vector<double>  *VTX_refPVz;
   vector<double>  *TRKS_px;
   vector<double>  *TRKS_py;
   vector<double>  *TRKS_pz;
   vector<double>  *TRKS_d0;
   vector<double>  *TRKS_z0;
   vector<int>     *TRKS_index;
   vector<double>  *TRUE_TRKS_px;
   vector<double>  *TRUE_TRKS_py;
   vector<double>  *TRUE_TRKS_pz;
   vector<double>  *TRUE_TRKS_e;
   vector<double>  *MUONS_pt;
   vector<double>  *MUONS_eta;
   vector<int>     *MUONS_index;
   Double_t        TRUTH_px;
   Double_t        TRUTH_py;
   Double_t        TRUTH_pz;
   Double_t        TRUTH_e;
   Int_t           TRUTH_pdgID;
   Double_t        TRUTH_vtxX;
   Double_t        TRUTH_vtxY;
   Double_t        TRUTH_vtxZ;

   // List of branches
   TBranch        *b_eventNumber;   //!
   TBranch        *b_lumiBlock;   //!
   TBranch        *b_runNumber;   //!
   TBranch        *b_PV_X;   //!
   TBranch        *b_PV_Y;   //!
   TBranch        *b_PV_Z;   //!
   TBranch        *b_PV_type;   //!
   TBranch        *b_PV_index;   //!
   TBranch        *b_TRUEPV_X;   //!
   TBranch        *b_TRUEPV_Y;   //!
   TBranch        *b_TRUEPV_Z;   //!
   TBranch        *b_charge;   //!
   TBranch        *b_isSignal;   //!
   TBranch        *b_VTX_pt;   //!
   TBranch        *b_VTX_ptError;   //!
   TBranch        *b_index;   //!
   TBranch        *b_VTX_xposition;   //!
   TBranch        *b_VTX_yposition;   //!
   TBranch        *b_VTX_zposition;   //!
   TBranch        *b_VTX_xxPosCov;   //!
   TBranch        *b_VTX_xyPosCov;   //!
   TBranch        *b_VTX_xzPosCov;   //!
   TBranch        *b_VTX_yyPosCov;   //!
   TBranch        *b_VTX_yzPosCov;   //!
   TBranch        *b_VTX_zzPosCov;   //!
   TBranch        *b_VTX_chi2;   //!
   TBranch        *b_VTX_NDF;   //!
   TBranch        *b_VTX_mass;   //!
   TBranch        *b_VTX_massError;   //!
   TBranch        *b_VTX_px;   //!
   TBranch        *b_VTX_py;   //!
   TBranch        *b_VTX_pz;   //!
   TBranch        *b_VTX_rapidity;   //!
   TBranch        *b_VTX_refTrksPx;   //!
   TBranch        *b_VTX_refTrksPy;   //!
   TBranch        *b_VTX_refTrksPz;   //!
   TBranch        *b_VTX_lxy;   //!
   TBranch        *b_VTX_lxyError;   //!
   TBranch        *b_VTX_tau;   //!
   TBranch        *b_VTX_tauError;   //!
   TBranch        *b_VTX_refPVx;   //!
   TBranch        *b_VTX_refPVy;   //!
   TBranch        *b_VTX_refPVz;   //!
   TBranch        *b_TRKS_px;   //!
   TBranch        *b_TRKS_py;   //!
   TBranch        *b_TRKS_pz;   //!
   TBranch        *b_TRKS_d0;   //!
   TBranch        *b_TRKS_z0;   //!
   TBranch        *b_TRKS_index;   //!
   TBranch        *b_TRUE_TRKS_px;   //!
   TBranch        *b_TRUE_TRKS_py;   //!
   TBranch        *b_TRUE_TRKS_pz;   //!
   TBranch        *b_TRUE_TRKS_e;   //!
   TBranch        *b_MUONS_pt;   //!
   TBranch        *b_MUONS_eta;   //!
   TBranch        *b_MUONS_index;   //!
   TBranch        *b_TRUTH_px;   //!
   TBranch        *b_TRUTH_py;   //!
   TBranch        *b_TRUTH_pz;   //!
   TBranch        *b_TRUTH_e;   //!
   TBranch        *b_TRUTH_pdgID;   //!
   TBranch        *b_TRUTH_vtxX;   //!
   TBranch        *b_TRUTH_vtxY;   //!
   TBranch        *b_TRUTH_vtxZ;   //!

   BCompositeParticle(TTree *tree, std::string name);
   Int_t    GetEntry(Long64_t entry);
   void     Init(TTree *tree, std::string name);
};

BCompositeParticle::BCompositeParticle(TTree *tree, std::string name) : m_name(name)
{
  Init(tree,name);
}

Int_t BCompositeParticle::GetEntry(Long64_t entry)
{
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

void BCompositeParticle::Init(TTree *tree, std::string name)
{
   // Set object pointer
  PV_X = 0;
  PV_Y = 0;
  PV_Z = 0;
  PV_type = 0;
  PV_index = 0;
  VTX_refTrksPx = 0;
  VTX_refTrksPy = 0;
  VTX_refTrksPz = 0;
  VTX_lxy = 0;
  VTX_lxyError = 0;
  VTX_tau = 0;
  VTX_tauError = 0;
  VTX_refPVx = 0;
  VTX_refPVy = 0;
  VTX_refPVz = 0;
  TRKS_px = 0;
  TRKS_py = 0;
  TRKS_pz = 0;
  TRKS_d0 = 0;
  TRKS_z0 = 0;
  TRKS_index = 0;
  TRUE_TRKS_px = 0;
  TRUE_TRKS_py = 0;
  TRUE_TRKS_pz = 0;
  TRUE_TRKS_e = 0;
  MUONS_pt = 0;
  MUONS_eta = 0;
  MUONS_index = 0;

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetBranchAddress((name+"_eventNumber").c_str(), &eventNumber, &b_eventNumber);
   fChain->SetBranchAddress((name+"_lumiBlock").c_str(), &lumiBlock, &b_lumiBlock);
   fChain->SetBranchAddress((name+"_runNumber").c_str(), &runNumber, &b_runNumber);
   fChain->SetBranchAddress((name+"_PV_X").c_str(), &PV_X, &b_PV_X);
   fChain->SetBranchAddress((name+"_PV_Y").c_str(), &PV_Y, &b_PV_Y);
   fChain->SetBranchAddress((name+"_PV_Z").c_str(), &PV_Z, &b_PV_Z);
   fChain->SetBranchAddress((name+"_PV_type").c_str(), &PV_type, &b_PV_type);
   fChain->SetBranchAddress((name+"_PV_index").c_str(), &PV_index, &b_PV_index);
   fChain->SetBranchAddress((name+"_TRUEPV_X").c_str(), &TRUEPV_X, &b_TRUEPV_X);
   fChain->SetBranchAddress((name+"_TRUEPV_Y").c_str(), &TRUEPV_Y, &b_TRUEPV_Y);
   fChain->SetBranchAddress((name+"_TRUEPV_Z").c_str(), &TRUEPV_Z, &b_TRUEPV_Z);
   fChain->SetBranchAddress((name+"_charge").c_str(), &charge, &b_charge);
   fChain->SetBranchAddress((name+"_isSignal").c_str(), &isSignal, &b_isSignal);
   fChain->SetBranchAddress((name+"_VTX_pt").c_str(), &VTX_pt, &b_VTX_pt);
   fChain->SetBranchAddress((name+"_VTX_ptError").c_str(), &VTX_ptError, &b_VTX_ptError);
   fChain->SetBranchAddress((name+"_index").c_str(), &index, &b_index);
   fChain->SetBranchAddress((name+"_VTX_xposition").c_str(), &VTX_xposition, &b_VTX_xposition);
   fChain->SetBranchAddress((name+"_VTX_yposition").c_str(), &VTX_yposition, &b_VTX_yposition);
   fChain->SetBranchAddress((name+"_VTX_zposition").c_str(), &VTX_zposition, &b_VTX_zposition);
   fChain->SetBranchAddress((name+"_VTX_xxPosCov").c_str(), &VTX_xxPosCov, &b_VTX_xxPosCov);
   fChain->SetBranchAddress((name+"_VTX_xyPosCov").c_str(), &VTX_xyPosCov, &b_VTX_xyPosCov);
   fChain->SetBranchAddress((name+"_VTX_xzPosCov").c_str(), &VTX_xzPosCov, &b_VTX_xzPosCov);
   fChain->SetBranchAddress((name+"_VTX_yyPosCov").c_str(), &VTX_yyPosCov, &b_VTX_yyPosCov);
   fChain->SetBranchAddress((name+"_VTX_yzPosCov").c_str(), &VTX_yzPosCov, &b_VTX_yzPosCov);
   fChain->SetBranchAddress((name+"_VTX_zzPosCov").c_str(), &VTX_zzPosCov, &b_VTX_zzPosCov);
   fChain->SetBranchAddress((name+"_VTX_chi2").c_str(), &VTX_chi2, &b_VTX_chi2);
   fChain->SetBranchAddress((name+"_VTX_NDF").c_str(), &VTX_NDF, &b_VTX_NDF);
   fChain->SetBranchAddress((name+"_VTX_mass").c_str(), &VTX_mass, &b_VTX_mass);
   fChain->SetBranchAddress((name+"_VTX_massError").c_str(), &VTX_massError, &b_VTX_massError);
   fChain->SetBranchAddress((name+"_VTX_px").c_str(), &VTX_px, &b_VTX_px);
   fChain->SetBranchAddress((name+"_VTX_py").c_str(), &VTX_py, &b_VTX_py);
   fChain->SetBranchAddress((name+"_VTX_pz").c_str(), &VTX_pz, &b_VTX_pz);
   fChain->SetBranchAddress((name+"_VTX_rapidity").c_str(), &VTX_rapidity, &b_VTX_rapidity);
   fChain->SetBranchAddress((name+"_VTX_refTrksPx").c_str(), &VTX_refTrksPx, &b_VTX_refTrksPx);
   fChain->SetBranchAddress((name+"_VTX_refTrksPy").c_str(), &VTX_refTrksPy, &b_VTX_refTrksPy);
   fChain->SetBranchAddress((name+"_VTX_refTrksPz").c_str(), &VTX_refTrksPz, &b_VTX_refTrksPz);
   fChain->SetBranchAddress((name+"_VTX_lxy").c_str(), &VTX_lxy, &b_VTX_lxy);
   fChain->SetBranchAddress((name+"_VTX_lxyError").c_str(), &VTX_lxyError, &b_VTX_lxyError);
   fChain->SetBranchAddress((name+"_VTX_tau").c_str(), &VTX_tau, &b_VTX_tau);
   fChain->SetBranchAddress((name+"_VTX_tauError").c_str(), &VTX_tauError, &b_VTX_tauError);
   fChain->SetBranchAddress((name+"_VTX_refPVx").c_str(), &VTX_refPVx, &b_VTX_refPVx);
   fChain->SetBranchAddress((name+"_VTX_refPVy").c_str(), &VTX_refPVy, &b_VTX_refPVy);
   fChain->SetBranchAddress((name+"_VTX_refPVz").c_str(), &VTX_refPVz, &b_VTX_refPVz);
   fChain->SetBranchAddress((name+"_TRKS_px").c_str(), &TRKS_px, &b_TRKS_px);
   fChain->SetBranchAddress((name+"_TRKS_py").c_str(), &TRKS_py, &b_TRKS_py);
   fChain->SetBranchAddress((name+"_TRKS_pz").c_str(), &TRKS_pz, &b_TRKS_pz);
   fChain->SetBranchAddress((name+"_TRKS_d0").c_str(), &TRKS_d0, &b_TRKS_d0);
   fChain->SetBranchAddress((name+"_TRKS_z0").c_str(), &TRKS_z0, &b_TRKS_z0);
   fChain->SetBranchAddress((name+"_TRKS_index").c_str(), &TRKS_index, &b_TRKS_index);
   fChain->SetBranchAddress((name+"_TRUE_TRKS_px").c_str(), &TRUE_TRKS_px, &b_TRUE_TRKS_px);
   fChain->SetBranchAddress((name+"_TRUE_TRKS_py").c_str(), &TRUE_TRKS_py, &b_TRUE_TRKS_py);
   fChain->SetBranchAddress((name+"_TRUE_TRKS_pz").c_str(), &TRUE_TRKS_pz, &b_TRUE_TRKS_pz);
   fChain->SetBranchAddress((name+"_TRUE_TRKS_e").c_str(), &TRUE_TRKS_e, &b_TRUE_TRKS_e);
   fChain->SetBranchAddress((name+"_MUONS_pt").c_str(), &MUONS_pt, &b_MUONS_pt);
   fChain->SetBranchAddress((name+"_MUONS_eta").c_str(), &MUONS_eta, &b_MUONS_eta);
   fChain->SetBranchAddress((name+"_MUONS_index").c_str(), &MUONS_index, &b_MUONS_index);
   fChain->SetBranchAddress((name+"_TRUTH_px").c_str(), &TRUTH_px, &b_TRUTH_px);
   fChain->SetBranchAddress((name+"_TRUTH_py").c_str(), &TRUTH_py, &b_TRUTH_py);
   fChain->SetBranchAddress((name+"_TRUTH_pz").c_str(), &TRUTH_pz, &b_TRUTH_pz);
   fChain->SetBranchAddress((name+"_TRUTH_e").c_str(), &TRUTH_e, &b_TRUTH_e);
   fChain->SetBranchAddress((name+"_TRUTH_pdgID").c_str(), &TRUTH_pdgID, &b_TRUTH_pdgID);
   fChain->SetBranchAddress((name+"_TRUTH_vtxX").c_str(), &TRUTH_vtxX, &b_TRUTH_vtxX);
   fChain->SetBranchAddress((name+"_TRUTH_vtxY").c_str(), &TRUTH_vtxY, &b_TRUTH_vtxY);
   fChain->SetBranchAddress((name+"_TRUTH_vtxZ").c_str(), &TRUTH_vtxZ, &b_TRUTH_vtxZ);

}

//*****************************************
TVector3 BCompositeParticle::getTrackMom(int i) {
  TVector3 vect(0,0,0);
  if(i >= (int)TRKS_px->size() ||
     TRKS_px->size() != TRKS_py->size() ||
     TRKS_px->size() != TRKS_pz->size())
  {
    std::cout << "ERROR in BCompositeParticle::getTracks: momentum components have different length" << std::endl;
    return vect;
  }

  vect.SetXYZ(TRKS_px->at(i), TRKS_py->at(i), TRKS_pz->at(i));
  return vect;
}
//*****************************************
TLorentzVector BCompositeParticle::getTrackLorentzMom(int i, double mass) {
  TLorentzVector vect(0,0,0,0);
  if(i >= (int)TRKS_px->size() ||
     TRKS_px->size() != TRKS_py->size() ||
     TRKS_px->size() != TRKS_pz->size())
  {
    std::cout << "ERROR in BCompositeParticle::getTracks: momentum components have different length" << std::endl;
    return vect;
  }

  vect.SetXYZM(TRKS_px->at(i), TRKS_py->at(i), TRKS_pz->at(i), mass);
  return vect;
}
//*****************************************
TVector3 BCompositeParticle::getRefTrackMom(int i) {
  TVector3 vect(0,0,0);
  if(i >= (int)VTX_refTrksPx->size() ||
     VTX_refTrksPx->size() != VTX_refTrksPy->size() ||
     VTX_refTrksPx->size() != VTX_refTrksPz->size())
  {
    std::cout << "ERROR in BCompositeParticle::getRefTracks: momentum components have different length" << std::endl;
    return vect;
  }

  vect.SetXYZ(VTX_refTrksPx->at(i), VTX_refTrksPy->at(i), VTX_refTrksPz->at(i));
  return vect;
}
//*****************************************
TLorentzVector BCompositeParticle::getRefTrackLorentzMom(int i, double mass) {
  TLorentzVector vect(0,0,0,0);
  if(i >= (int)TRKS_px->size() ||
     VTX_refTrksPx->size() != VTX_refTrksPy->size() ||
     VTX_refTrksPx->size() != VTX_refTrksPz->size())
  {
    std::cout << "ERROR in BCompositeParticle::getRefTracks: momentum components have different length" << std::endl;
    return vect;
  }

  vect.SetXYZM(VTX_refTrksPx->at(i), VTX_refTrksPy->at(i), VTX_refTrksPz->at(i), mass);
  return vect;
}
//*****************************************
TVector3 BCompositeParticle::getTrueTrackMom(int i) {
  TVector3 vect(0,0,0);
  if(i >= (int)TRUE_TRKS_px->size() ||
     TRUE_TRKS_px->size() != TRUE_TRKS_py->size() ||
     TRUE_TRKS_px->size() != TRUE_TRKS_pz->size())
  {
    std::cout << "ERROR in BCompositeParticle::getTrueTrack: momentum components have different length" << std::endl;
    return vect;
  }

  vect.SetXYZ(TRUE_TRKS_px->at(i), TRUE_TRKS_py->at(i), TRUE_TRKS_pz->at(i));
  return vect;
}
//*****************************************
TLorentzVector BCompositeParticle::getTrueTrackLorentzMom(int i) {
  TLorentzVector vect(0,0,0,0);
  if(i >= (int)TRUE_TRKS_px->size() ||
     TRUE_TRKS_px->size() != TRUE_TRKS_py->size() ||
     TRUE_TRKS_px->size() != TRUE_TRKS_pz->size() ||
     TRUE_TRKS_px->size() != TRUE_TRKS_e->size())
  {
    std::cout << "ERROR in BCompositeParticle::getTrueTrack: momentum components have different length" << std::endl;
    return vect;
  }

  vect.SetPxPyPzE(TRUE_TRKS_px->at(i), TRUE_TRKS_py->at(i), TRUE_TRKS_pz->at(i), TRUE_TRKS_e->at(i));
  return vect;
}
//*****************************************
TVector3 BCompositeParticle::getVertex() {
  TVector3 vect(VTX_xposition, VTX_yposition, VTX_zposition);
  return vect;
}
//*****************************************
TVector3 BCompositeParticle::getTrueVertex() {
  TVector3 vect(TRUTH_vtxX, TRUTH_vtxY, TRUTH_vtxZ);
  return vect;
}
//*****************************************
std::vector<TVector3> BCompositeParticle::getPrimVertex() {
  std::vector<TVector3> pv;
  for(int i=0; i<(int)VTX_refPVx->size(); ++i) {
    pv.push_back(TVector3(VTX_refPVx->at(i), VTX_refPVy->at(i), VTX_refPVz->at(i)));
  }

  return pv;
}
//*****************************************
TVector3 BCompositeParticle::getTruePrimVertex() {
  TVector3 vect(TRUEPV_X, TRUEPV_Y, TRUEPV_Z);
  return vect;
}
//*****************************************
TVector3 BCompositeParticle::getRefMom() {
  TVector3 vect(VTX_px, VTX_py, VTX_pz);
  return vect;
}
//*****************************************
TLorentzVector BCompositeParticle::getRefLorentzMom() {
  TLorentzVector vect;
  vect.SetXYZM(VTX_px, VTX_py, VTX_pz, VTX_mass);
  return vect;
}
//*****************************************
TLorentzVector BCompositeParticle::getRefLorentzMom(double mass) {
  TLorentzVector vect;
  vect.SetXYZM(VTX_px, VTX_py, VTX_pz, mass);
  return vect;
}
//*****************************************
std::vector<int> BCompositeParticle::getParticlesInEvent(int run, int event) {

  std::vector<int> particles;

  // Get the entry corresponding to the run and event
  Int_t index = fChain->GetEntryNumberWithIndex(run,event);
  if (index==-1) { // Trap in case wrong event/run
    std::cout << "ERROR: run/event pairing " << run << "/" << event << " is not found in the BCompositeParticle ntuple " << m_name << std::endl;
    std::cout << "Please check! Returning an empty vector of tracks!" << std::endl;
    return particles;
  }

  //NOTE index retrieved by GetEntryNumberWithIndex is not neccessary the one of the first entry
  //Thus we have to loop back to check:

  //read entry and loop back until we reach a different event or a beginning of the tree
  for(fChain->GetEntry(index); runNumber == run && eventNumber==event && index > 0; fChain->GetEntry(--index)) {
    //std::cout << "index = " << index << " run = " << run << "(" << runNumber << ") event = " <<  event << "(" << eventNumber << ")" << std::endl;
  }

  //now index is either 0 or it is the first entry before the one we want
  //so let's take the one we want:
  if(runNumber != run || eventNumber!=event) ++index;

  //loop over all the entries in this event
  for(fChain->GetEntry(index); runNumber == run && eventNumber==event && index<fChain->GetEntries(); fChain->GetEntry(++index)) {
    //std::cout << "index = " << index << " run = " << run << "(" << runNumber << ") event = " <<  event << "(" << eventNumber << ")" << std::endl;
    particles.push_back( index );
  }

  return particles;

}
//*****************************************
bool BCompositeParticle::findTrack(int i, TrackBag tracks, Track& track) {

  int trkIndex = TRKS_index->at(i);

  //loop through tracks
  TrackBag::iterator trkItr = tracks.begin();
  for(; trkItr!=tracks.end(); ++trkItr) {
    if ( (*trkItr).index()==trkIndex) {
      track = (*trkItr);
      return true;
    }
  }
  return false;
}
//*****************************************
bool BCompositeParticle::findMuon(int i, MuonBag muons, Muon& muon) {

  int trkIndex = TRKS_index->at(i);

  //loop through muons
  MuonBag::iterator muonItr = muons.begin();
  for(; muonItr!=muons.end(); ++muonItr) {
    if ( (*muonItr).inDetTrackIndex()==trkIndex) {
      muon = (*muonItr);
      return true;
    }
  }

  return false;
}
//*****************************************

#endif
