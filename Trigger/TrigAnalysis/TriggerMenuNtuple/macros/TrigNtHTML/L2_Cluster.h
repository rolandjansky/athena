/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Dec  3 18:25:05 2009 by ROOT version 5.22/00d
// from TTree L2_Cluster/L2 Trigger EMCluster
// found on file: trigntupleTTBAR.root
//////////////////////////////////////////////////////////

#ifndef L2_Cluster_h
#define L2_Cluster_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class L2_Cluster {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          TrigEMCluster_nClus;
   vector<float>   *TrigEMCluster_energy;
   vector<float>   *TrigEMCluster_et;
   vector<vector<double> > *TrigEMCluster_energyInSample;
   vector<float>   *TrigEMCluster_eta;
   vector<float>   *TrigEMCluster_phi;
   vector<float>   *TrigEMCluster_e237;
   vector<float>   *TrigEMCluster_e277;
   vector<float>   *TrigEMCluster_fracs1;
   vector<float>   *TrigEMCluster_weta2;
   vector<float>   *TrigEMCluster_ehad1;
   vector<float>   *TrigEMCluster_Eta1;
   vector<float>   *TrigEMCluster_emaxs1;
   vector<float>   *TrigEMCluster_e2tsts1;
   vector<vector<string> > *TrigEMCluster_trigChain;
   vector<vector<int> > *TrigEMCluster_trigDec;

   // List of branches
   TBranch        *b_TrigEMCluster_nClus;   //!
   TBranch        *b_TrigEMCluster_energy;   //!
   TBranch        *b_TrigEMCluster_et;   //!
   TBranch        *b_TrigEMCluster_energyInSample;   //!
   TBranch        *b_TrigEMCluster_eta;   //!
   TBranch        *b_TrigEMCluster_phi;   //!
   TBranch        *b_TrigEMCluster_e237;   //!
   TBranch        *b_TrigEMCluster_e277;   //!
   TBranch        *b_TrigEMCluster_fracs1;   //!
   TBranch        *b_TrigEMCluster_weta2;   //!
   TBranch        *b_TrigEMCluster_ehad1;   //!
   TBranch        *b_TrigEMCluster_Eta1;   //!
   TBranch        *b_TrigEMCluster_emaxs1;   //!
   TBranch        *b_TrigEMCluster_e2tsts1;   //!
   TBranch        *b_TrigEMCluster_trigChain;   //!
   TBranch        *b_TrigEMCluster_trigDec;   //!

   L2_Cluster(TTree *tree=0);
   virtual ~L2_Cluster();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef L2_Cluster_cxx
L2_Cluster::L2_Cluster(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("trigntupleTTBAR.root");
      if (!f) {
         f = new TFile("trigntupleTTBAR.root");
         f->cd("trigntupleTTBAR.root:/EGamma");
      }
      tree = (TTree*)gDirectory->Get("L2_Cluster");

   }
   Init(tree);
}

L2_Cluster::~L2_Cluster()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t L2_Cluster::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t L2_Cluster::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void L2_Cluster::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TrigEMCluster_energy = 0;
   TrigEMCluster_et = 0;
   TrigEMCluster_energyInSample = 0;
   TrigEMCluster_eta = 0;
   TrigEMCluster_phi = 0;
   TrigEMCluster_e237 = 0;
   TrigEMCluster_e277 = 0;
   TrigEMCluster_fracs1 = 0;
   TrigEMCluster_weta2 = 0;
   TrigEMCluster_ehad1 = 0;
   TrigEMCluster_Eta1 = 0;
   TrigEMCluster_emaxs1 = 0;
   TrigEMCluster_e2tsts1 = 0;
   TrigEMCluster_trigChain = 0;
   TrigEMCluster_trigDec = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("TrigEMCluster_nClus", &TrigEMCluster_nClus, &b_TrigEMCluster_nClus);
   fChain->SetBranchAddress("TrigEMCluster_energy", &TrigEMCluster_energy, &b_TrigEMCluster_energy);
   fChain->SetBranchAddress("TrigEMCluster_et", &TrigEMCluster_et, &b_TrigEMCluster_et);
   fChain->SetBranchAddress("TrigEMCluster_energyInSample", &TrigEMCluster_energyInSample, &b_TrigEMCluster_energyInSample);
   fChain->SetBranchAddress("TrigEMCluster_eta", &TrigEMCluster_eta, &b_TrigEMCluster_eta);
   fChain->SetBranchAddress("TrigEMCluster_phi", &TrigEMCluster_phi, &b_TrigEMCluster_phi);
   fChain->SetBranchAddress("TrigEMCluster_e237", &TrigEMCluster_e237, &b_TrigEMCluster_e237);
   fChain->SetBranchAddress("TrigEMCluster_e277", &TrigEMCluster_e277, &b_TrigEMCluster_e277);
   fChain->SetBranchAddress("TrigEMCluster_fracs1", &TrigEMCluster_fracs1, &b_TrigEMCluster_fracs1);
   fChain->SetBranchAddress("TrigEMCluster_weta2", &TrigEMCluster_weta2, &b_TrigEMCluster_weta2);
   fChain->SetBranchAddress("TrigEMCluster_ehad1", &TrigEMCluster_ehad1, &b_TrigEMCluster_ehad1);
   fChain->SetBranchAddress("TrigEMCluster_Eta1", &TrigEMCluster_Eta1, &b_TrigEMCluster_Eta1);
   fChain->SetBranchAddress("TrigEMCluster_emaxs1", &TrigEMCluster_emaxs1, &b_TrigEMCluster_emaxs1);
   fChain->SetBranchAddress("TrigEMCluster_e2tsts1", &TrigEMCluster_e2tsts1, &b_TrigEMCluster_e2tsts1);
   fChain->SetBranchAddress("TrigEMCluster_trigChain", &TrigEMCluster_trigChain, &b_TrigEMCluster_trigChain);
   fChain->SetBranchAddress("TrigEMCluster_trigDec", &TrigEMCluster_trigDec, &b_TrigEMCluster_trigDec);
   Notify();
}

Bool_t L2_Cluster::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void L2_Cluster::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t L2_Cluster::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef L2_Cluster_cxx
