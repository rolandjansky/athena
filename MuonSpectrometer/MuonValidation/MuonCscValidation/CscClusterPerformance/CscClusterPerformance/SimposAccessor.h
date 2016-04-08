/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 22 15:00:24 2009 by ROOT version 5.22/00d
// from TTree csc_simpos/CSC simulation positions
// found on file: csc_simpos.root
//////////////////////////////////////////////////////////

#ifndef SimposAccessor_h
#define SimposAccessor_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class SimposAccessor {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           run;
   Int_t           evt;
   Int_t           nentry;
   Float_t         y[200];   //[nentry]
   Float_t         z[200];   //[nentry]
   Float_t         dx[200];   //[nentry]
   Float_t         dydx[200];   //[nentry]
   Float_t         dzdx[200];   //[nentry]
   Float_t         time[200];   //[nentry]
   Int_t           gid[200];   //[nentry]
   Int_t           pdgid[200];   //[nentry]
   Int_t           istation[200];   //[nentry]
   Int_t           phisec[200];   //[nentry]
   Int_t           zsec[200];   //[nentry]
   Int_t           sector[200];   //[nentry]
   Int_t           wlay[200];   //[nentry]
   Float_t         eta[200];   //[nentry]
   Float_t         pt[200];   //[nentry]
   Float_t         phi0[200];   //[nentry]
   Float_t         d0[200];   //[nentry]
   Float_t         z0[200];   //[nentry]
   Float_t         vpr[200];   //[nentry]
   Float_t         vpphi[200];   //[nentry]
   Float_t         vpz[200];   //[nentry]
   Float_t         ver[200];   //[nentry]
   Float_t         vephi[200];   //[nentry]
   Float_t         vez[200];   //[nentry]

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_nentry;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!
   TBranch        *b_dx;   //!
   TBranch        *b_dydx;   //!
   TBranch        *b_dzdx;   //!
   TBranch        *b_time;   //!
   TBranch        *b_gid;   //!
   TBranch        *b_pdgid;   //!
   TBranch        *b_istation;   //!
   TBranch        *b_phisec;   //!
   TBranch        *b_zsec;   //!
   TBranch        *b_sector;   //!
   TBranch        *b_wlay;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_phi0;   //!
   TBranch        *b_d0;   //!
   TBranch        *b_z0;   //!
   TBranch        *b_vpr;   //!
   TBranch        *b_vpphi;   //!
   TBranch        *b_vpz;   //!
   TBranch        *b_ver;   //!
   TBranch        *b_vephi;   //!
   TBranch        *b_vez;   //!

   SimposAccessor(TTree *tree=0);
   virtual ~SimposAccessor();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef SimposAccessor_cxx
SimposAccessor::SimposAccessor(TTree *tree) :
   fChain(NULL), b_run(NULL), b_evt(NULL), b_nentry(NULL), b_y(NULL), b_z(NULL), b_dx(NULL),
   b_dydx(NULL), b_dzdx(NULL), b_time(NULL), b_gid(NULL), b_pdgid(NULL),
   b_istation(NULL), b_phisec(NULL), b_zsec(NULL), b_sector(NULL), b_wlay(NULL),
   b_eta(NULL), b_pt(NULL), b_phi0(NULL), b_d0(NULL), b_z0(NULL), b_vpr(NULL),
   b_vpphi(NULL), b_vpz(NULL), b_ver(NULL), b_vephi(NULL), b_vez(NULL)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("csc_simpos.root");
      if (!f) {
         new TFile("csc_simpos.root");
      }
      tree = (TTree*)gDirectory->Get("csc_simpos");

   }
   Init(tree);
}

SimposAccessor::~SimposAccessor()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SimposAccessor::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SimposAccessor::LoadTree(Long64_t entry)
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

void SimposAccessor::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("nentry", &nentry, &b_nentry);
   fChain->SetBranchAddress("y", y, &b_y);
   fChain->SetBranchAddress("z", z, &b_z);
   fChain->SetBranchAddress("dx", dx, &b_dx);
   fChain->SetBranchAddress("dydx", dydx, &b_dydx);
   fChain->SetBranchAddress("dzdx", dzdx, &b_dzdx);
   fChain->SetBranchAddress("time", time, &b_time);
   fChain->SetBranchAddress("gid", gid, &b_gid);
   fChain->SetBranchAddress("pdgid", pdgid, &b_pdgid);
   fChain->SetBranchAddress("istation", istation, &b_istation);
   fChain->SetBranchAddress("phisec", phisec, &b_phisec);
   fChain->SetBranchAddress("zsec", zsec, &b_zsec);
   fChain->SetBranchAddress("sector", sector, &b_sector);
   fChain->SetBranchAddress("wlay", wlay, &b_wlay);
   fChain->SetBranchAddress("eta", eta, &b_eta);
   fChain->SetBranchAddress("pt", pt, &b_pt);
   fChain->SetBranchAddress("phi0", phi0, &b_phi0);
   fChain->SetBranchAddress("d0", d0, &b_d0);
   fChain->SetBranchAddress("z0", z0, &b_z0);
   fChain->SetBranchAddress("vpr", vpr, &b_vpr);
   fChain->SetBranchAddress("vpphi", vpphi, &b_vpphi);
   fChain->SetBranchAddress("vpz", vpz, &b_vpz);
   fChain->SetBranchAddress("ver", ver, &b_ver);
   fChain->SetBranchAddress("vephi", vephi, &b_vephi);
   fChain->SetBranchAddress("vez", vez, &b_vez);
   Notify();
}

Bool_t SimposAccessor::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void SimposAccessor::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SimposAccessor::Cut(Long64_t /*entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef SimposAccessor_cxx
