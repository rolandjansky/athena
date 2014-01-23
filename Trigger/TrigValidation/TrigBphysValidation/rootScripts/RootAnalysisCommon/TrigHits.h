/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jun 21 17:13:35 2010 by ROOT version 5.26/00
// from TTree Trigger/Trigger tree
// found on file: L2_MU4_DiMu_FS.root
//////////////////////////////////////////////////////////

#ifndef TrigHits_h
#define TrigHits_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class TrigHits {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           runNumber;
   Int_t           lumiBlock;
   Int_t           eventNumber;
   bool            passTrigDec;
   Double_t        trigMass;
   Double_t        chi2;
   Double_t        trk0_pt;
   Double_t        trk0_eta;
   Double_t        trk0_phi;
   Double_t        trk0_charge;
   Double_t        trk0_nMdt;
   Double_t        trk0_nRpc;
   Double_t        trk0_nTgc;
   Double_t        trk1_pt;
   Double_t        trk1_eta;
   Double_t        trk1_phi;
   Double_t        trk1_charge;
   Double_t        trk1_nMdt;
   Double_t        trk1_nRpc;
   Double_t        trk1_nTgc;
   vector<double>  *trk0_innerMdt_r;
   vector<double>  *trk0_innerMdt_z;
   vector<double>  *trk0_middleMdt_r;
   vector<double>  *trk0_middleMdt_z;
   vector<double>  *trk0_etaRpc_r;
   vector<double>  *trk0_etaRpc_z;
   vector<double>  *trk0_phiRpc_x;
   vector<double>  *trk0_phiRpc_y;
   vector<double>  *trk0_innerEtaTgc_r;
   vector<double>  *trk0_innerEtaTgc_z;
   vector<double>  *trk0_innerEtaTgc_phi;
   vector<double>  *trk0_innerPhiTgc;
   vector<double>  *trk0_middleEtaTgc_r;
   vector<double>  *trk0_middleEtaTgc_z;
   vector<double>  *trk0_middleEtaTgc_phi;
   vector<double>  *trk0_middlePhiTgc;
   vector<double>  *trk1_innerMdt_r;
   vector<double>  *trk1_innerMdt_z;
   vector<double>  *trk1_middleMdt_r;
   vector<double>  *trk1_middleMdt_z;
   vector<double>  *trk1_etaRpc_r;
   vector<double>  *trk1_etaRpc_z;
   vector<double>  *trk1_phiRpc_x;
   vector<double>  *trk1_phiRpc_y;
   vector<double>  *trk1_innerEtaTgc_r;
   vector<double>  *trk1_innerEtaTgc_z;
   vector<double>  *trk1_innerEtaTgc_phi;
   vector<double>  *trk1_innerPhiTgc;
   vector<double>  *trk1_middleEtaTgc_r;
   vector<double>  *trk1_middleEtaTgc_z;
   vector<double>  *trk1_middleEtaTgc_phi;
   vector<double>  *trk1_middlePhiTgc;

   // List of branches
   TBranch        *b_runNumber;   //!
   TBranch        *b_lumiBlock;   //!
   TBranch        *b_eventNumber;   //!
   TBranch        *b_trigMass;   //!
   TBranch        *b_trk0_pt;   //!
   TBranch        *b_trk0_eta;   //!
   TBranch        *b_trk0_phi;   //!
   TBranch        *b_trk0_charge;   //!
   TBranch        *b_trk1_pt;   //!
   TBranch        *b_trk1_eta;   //!
   TBranch        *b_trk1_phi;   //!
   TBranch        *b_trk1_charge;   //!
   TBranch        *b_trk0_innerMdt_r;   //!
   TBranch        *b_trk0_innerMdt_z;   //!
   TBranch        *b_trk0_middleMdt_r;   //!
   TBranch        *b_trk0_middleMdt_z;   //!
   TBranch        *b_trk0_etaRpc_r;   //!
   TBranch        *b_trk0_etaRpc_z;   //!
   TBranch        *b_trk0_phiRpc_x;   //!
   TBranch        *b_trk0_phiRpc_y;   //!
   TBranch        *b_trk0_innerEtaTgc_r;   //!
   TBranch        *b_trk0_innerEtaTgc_z;   //!
   TBranch        *b_trk0_innerEtaTgc_phi;   //!
   TBranch        *b_trk0_innerPhiTgc;   //!
   TBranch        *b_trk0_middleEtaTgc_r;   //!
   TBranch        *b_trk0_middleEtaTgc_z;   //!
   TBranch        *b_trk0_middleEtaTgc_phi;   //!
   TBranch        *b_trk0_middlePhiTgc;   //!
   TBranch        *b_trk1_innerMdt_r;   //!
   TBranch        *b_trk1_innerMdt_z;   //!
   TBranch        *b_trk1_middleMdt_r;   //!
   TBranch        *b_trk1_middleMdt_z;   //!
   TBranch        *b_trk1_etaRpc_r;   //!
   TBranch        *b_trk1_etaRpc_z;   //!
   TBranch        *b_trk1_phiRpc_x;   //!
   TBranch        *b_trk1_phiRpc_y;   //!
   TBranch        *b_trk1_innerEtaTgc_r;   //!
   TBranch        *b_trk1_innerEtaTgc_z;   //!
   TBranch        *b_trk1_innerEtaTgc_phi;   //!
   TBranch        *b_trk1_innerPhiTgc;   //!
   TBranch        *b_trk1_middleEtaTgc_r;   //!
   TBranch        *b_trk1_middleEtaTgc_z;   //!
   TBranch        *b_trk1_middleEtaTgc_phi;   //!
   TBranch        *b_trk1_middlePhiTgc;   //!

   TrigHits(TTree *tree=0);
   virtual ~TrigHits();
   void Init(TTree *tree);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);

   // inline functions
   double pt(int i)     { return i==0 ? trk0_pt : trk1_pt ; }
   double eta(int i)    { return i==0 ? trk0_eta : trk1_eta ; }
   double phi(int i)    { return i==0 ? trk0_phi : trk1_phi ; }
   double charge(int i) { return i==0 ? trk0_charge : trk1_charge ; }

   double nMdt(int i) { return i==0 ? trk0_nMdt : trk1_nMdt ; }
   double nRpc(int i) { return i==0 ? trk0_nRpc : trk1_nRpc ; }
   double nTgc(int i) { return i==0 ? trk0_nTgc : trk1_nTgc ; }

   vector<double>* innerMdt_r(int i)        { return i==0 ? trk0_innerMdt_r : trk1_innerMdt_r ; }
   vector<double>* innerMdt_z(int i)        { return i==0 ? trk0_innerMdt_z : trk1_innerMdt_z ; }
   vector<double>* middleMdt_r(int i)       { return i==0 ? trk0_middleMdt_r : trk1_middleMdt_r ; }
   vector<double>* middleMdt_z(int i)       { return i==0 ? trk0_middleMdt_z : trk1_middleMdt_z ; }
   vector<double>* etaRpc_r(int i)          { return i==0 ? trk0_etaRpc_r : trk1_etaRpc_r ; }
   vector<double>* etaRpc_z(int i)          { return i==0 ? trk0_etaRpc_z : trk1_etaRpc_z ; }
   vector<double>* phiRpc_x(int i)          { return i==0 ? trk0_phiRpc_x : trk1_phiRpc_x ; }
   vector<double>* phiRpc_y(int i)          { return i==0 ? trk0_phiRpc_y : trk1_phiRpc_y ; }
   vector<double>* innerEtaTgc_r(int i)     { return i==0 ? trk0_innerEtaTgc_r : trk1_innerEtaTgc_r ; }
   vector<double>* innerEtaTgc_z(int i)     { return i==0 ? trk0_innerEtaTgc_z : trk1_innerEtaTgc_z ; }
   vector<double>* innerEtaTgc_phi(int i)   { return i==0 ? trk0_innerEtaTgc_phi : trk1_innerEtaTgc_phi ; }
   vector<double>* innerPhiTgc(int i)       { return i==0 ? trk0_innerPhiTgc : trk1_innerPhiTgc ; }
   vector<double>* middleEtaTgc_r(int i)    { return i==0 ? trk0_middleEtaTgc_r : trk1_middleEtaTgc_r ; }
   vector<double>* middleEtaTgc_z(int i)    { return i==0 ? trk0_middleEtaTgc_z : trk1_middleEtaTgc_z ; }
   vector<double>* middleEtaTgc_phi(int i)  { return i==0 ? trk0_middleEtaTgc_phi : trk1_middleEtaTgc_phi ; }
   vector<double>* middlePhiTgc(int i)      { return i==0 ? trk0_middlePhiTgc : trk1_middlePhiTgc ; }

};

TrigHits::TrigHits(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("L2_MU4_DiMu_FS.root");
      if (!f) {
         f = new TFile("L2_MU4_DiMu_FS.root");
      }
      tree = (TTree*)gDirectory->Get("Trigger");

   }
   Init(tree);
}

TrigHits::~TrigHits()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrigHits::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TrigHits::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
   }
   return centry;
}

void TrigHits::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   trk0_innerMdt_r = 0;
   trk0_innerMdt_z = 0;
   trk0_middleMdt_r = 0;
   trk0_middleMdt_z = 0;
   trk0_etaRpc_r = 0;
   trk0_etaRpc_z = 0;
   trk0_phiRpc_x = 0;
   trk0_phiRpc_y = 0;
   trk0_innerEtaTgc_r = 0;
   trk0_innerEtaTgc_z = 0;
   trk0_innerEtaTgc_phi = 0;
   trk0_innerPhiTgc = 0;
   trk0_middleEtaTgc_r = 0;
   trk0_middleEtaTgc_z = 0;
   trk0_middleEtaTgc_phi = 0;
   trk0_middlePhiTgc = 0;
   trk1_innerMdt_r = 0;
   trk1_innerMdt_z = 0;
   trk1_middleMdt_r = 0;
   trk1_middleMdt_z = 0;
   trk1_etaRpc_r = 0;
   trk1_etaRpc_z = 0;
   trk1_phiRpc_x = 0;
   trk1_phiRpc_y = 0;
   trk1_innerEtaTgc_r = 0;
   trk1_innerEtaTgc_z = 0;
   trk1_innerEtaTgc_phi = 0;
   trk1_innerPhiTgc = 0;
   trk1_middleEtaTgc_r = 0;
   trk1_middleEtaTgc_z = 0;
   trk1_middleEtaTgc_phi = 0;
   trk1_middlePhiTgc = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
   fChain->SetBranchAddress("lumiBlock", &lumiBlock, &b_lumiBlock);
   fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
   fChain->SetBranchAddress("passTrigDec", &passTrigDec);
   fChain->SetBranchAddress("trigMass", &trigMass, &b_trigMass);
   fChain->SetBranchAddress("chi2", &chi2);
   fChain->SetBranchAddress("trk0_pt", &trk0_pt, &b_trk0_pt);
   fChain->SetBranchAddress("trk0_eta", &trk0_eta, &b_trk0_eta);
   fChain->SetBranchAddress("trk0_phi", &trk0_phi, &b_trk0_phi);
   fChain->SetBranchAddress("trk0_charge", &trk0_charge, &b_trk0_charge);
   fChain->SetBranchAddress("trk0_nMdt", &trk0_nMdt);
   fChain->SetBranchAddress("trk0_nRpc", &trk0_nRpc);
   fChain->SetBranchAddress("trk0_nTgc", &trk0_nTgc);
   fChain->SetBranchAddress("trk1_pt", &trk1_pt, &b_trk1_pt);
   fChain->SetBranchAddress("trk1_eta", &trk1_eta, &b_trk1_eta);
   fChain->SetBranchAddress("trk1_phi", &trk1_phi, &b_trk1_phi);
   fChain->SetBranchAddress("trk1_charge", &trk1_charge, &b_trk1_charge);
   fChain->SetBranchAddress("trk1_nMdt", &trk1_nMdt);
   fChain->SetBranchAddress("trk1_nRpc", &trk1_nRpc);
   fChain->SetBranchAddress("trk1_nTgc", &trk1_nTgc);

   fChain->SetBranchAddress("trk0_innerMdt_r", &trk0_innerMdt_r, &b_trk0_innerMdt_r);
   fChain->SetBranchAddress("trk0_innerMdt_z", &trk0_innerMdt_z, &b_trk0_innerMdt_z);
   fChain->SetBranchAddress("trk0_middleMdt_r", &trk0_middleMdt_r, &b_trk0_middleMdt_r);
   fChain->SetBranchAddress("trk0_middleMdt_z", &trk0_middleMdt_z, &b_trk0_middleMdt_z);
   fChain->SetBranchAddress("trk0_etaRpc_r", &trk0_etaRpc_r, &b_trk0_etaRpc_r);
   fChain->SetBranchAddress("trk0_etaRpc_z", &trk0_etaRpc_z, &b_trk0_etaRpc_z);
   fChain->SetBranchAddress("trk0_phiRpc_x", &trk0_phiRpc_x, &b_trk0_phiRpc_x);
   fChain->SetBranchAddress("trk0_phiRpc_y", &trk0_phiRpc_y, &b_trk0_phiRpc_y);
   fChain->SetBranchAddress("trk0_innerEtaTgc_r", &trk0_innerEtaTgc_r, &b_trk0_innerEtaTgc_r);
   fChain->SetBranchAddress("trk0_innerEtaTgc_z", &trk0_innerEtaTgc_z, &b_trk0_innerEtaTgc_z);
   fChain->SetBranchAddress("trk0_innerEtaTgc_phi", &trk0_innerEtaTgc_phi, &b_trk0_innerEtaTgc_phi);
   fChain->SetBranchAddress("trk0_innerPhiTgc", &trk0_innerPhiTgc, &b_trk0_innerPhiTgc);
   fChain->SetBranchAddress("trk0_middleEtaTgc_r", &trk0_middleEtaTgc_r, &b_trk0_middleEtaTgc_r);
   fChain->SetBranchAddress("trk0_middleEtaTgc_z", &trk0_middleEtaTgc_z, &b_trk0_middleEtaTgc_z);
   fChain->SetBranchAddress("trk0_middleEtaTgc_phi", &trk0_middleEtaTgc_phi, &b_trk0_middleEtaTgc_phi);
   fChain->SetBranchAddress("trk0_middlePhiTgc", &trk0_middlePhiTgc, &b_trk0_middlePhiTgc);
   fChain->SetBranchAddress("trk1_innerMdt_r", &trk1_innerMdt_r, &b_trk1_innerMdt_r);
   fChain->SetBranchAddress("trk1_innerMdt_z", &trk1_innerMdt_z, &b_trk1_innerMdt_z);
   fChain->SetBranchAddress("trk1_middleMdt_r", &trk1_middleMdt_r, &b_trk1_middleMdt_r);
   fChain->SetBranchAddress("trk1_middleMdt_z", &trk1_middleMdt_z, &b_trk1_middleMdt_z);
   fChain->SetBranchAddress("trk1_etaRpc_r", &trk1_etaRpc_r, &b_trk1_etaRpc_r);
   fChain->SetBranchAddress("trk1_etaRpc_z", &trk1_etaRpc_z, &b_trk1_etaRpc_z);
   fChain->SetBranchAddress("trk1_phiRpc_x", &trk1_phiRpc_x, &b_trk1_phiRpc_x);
   fChain->SetBranchAddress("trk1_phiRpc_y", &trk1_phiRpc_y, &b_trk1_phiRpc_y);
   fChain->SetBranchAddress("trk1_innerEtaTgc_r", &trk1_innerEtaTgc_r, &b_trk1_innerEtaTgc_r);
   fChain->SetBranchAddress("trk1_innerEtaTgc_z", &trk1_innerEtaTgc_z, &b_trk1_innerEtaTgc_z);
   fChain->SetBranchAddress("trk1_innerEtaTgc_phi", &trk1_innerEtaTgc_phi, &b_trk1_innerEtaTgc_phi);
   fChain->SetBranchAddress("trk1_innerPhiTgc", &trk1_innerPhiTgc, &b_trk1_innerPhiTgc);
   fChain->SetBranchAddress("trk1_middleEtaTgc_r", &trk1_middleEtaTgc_r, &b_trk1_middleEtaTgc_r);
   fChain->SetBranchAddress("trk1_middleEtaTgc_z", &trk1_middleEtaTgc_z, &b_trk1_middleEtaTgc_z);
   fChain->SetBranchAddress("trk1_middleEtaTgc_phi", &trk1_middleEtaTgc_phi, &b_trk1_middleEtaTgc_phi);
   fChain->SetBranchAddress("trk1_middlePhiTgc", &trk1_middlePhiTgc, &b_trk1_middlePhiTgc);
}
#endif // #ifdef TrigHits_cxx
