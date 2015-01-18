/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 15 09:04:51 2007 by ROOT version 5.10/00e
// from TTree csc_simseg/CSC simulated segments
// found on file: /afs/usatlas.bnl.gov/users/dladams/data/csc_simseg.root
//////////////////////////////////////////////////////////

#ifndef SimsegAccessor_h
#define SimsegAccessor_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class SimsegAccessor {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           run;
   Int_t           evt;
   Int_t           nentry;
   Float_t         y[2000];   //[nentry]
   Float_t         z[2000];   //[nentry]
   Float_t         dydx[2000];   //[nentry]
   Float_t         dzdx[2000];   //[nentry]
   Float_t         axy[2000];   //[nentry]
   Float_t         axz[2000];   //[nentry]
   Float_t         chsqy[2000];   //[nentry]
   Float_t         chsqz[2000];   //[nentry]
   Float_t         time[2000];   //[nentry]
   Int_t           gid[2000];   //[nentry]
   Int_t           pdgid[2000];   //[nentry]
   Float_t         eta[2000];   //[nentry]
   Float_t         pt[2000];   //[nentry]
   Int_t           istation[2000];   //[nentry]
   Int_t           phisec[2000];   //[nentry]
   Int_t           zsec[2000];   //[nentry]

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_nentry;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!
   TBranch        *b_dydx;   //!
   TBranch        *b_dzdx;   //!
   TBranch        *b_axy;   //!
   TBranch        *b_axz;   //!
   TBranch        *b_chsqy;   //!
   TBranch        *b_chsqz;   //!
   TBranch        *b_time;   //!
   TBranch        *b_gid;   //!
   TBranch        *b_pdgid;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_istation;   //!
   TBranch        *b_phisec;   //!
   TBranch        *b_zsec;   //!

   SimsegAccessor(TTree *tree=0);
   virtual ~SimsegAccessor();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef SimsegAccessor_cxx
SimsegAccessor::SimsegAccessor(TTree *tree) :
  fChain(NULL), b_run(NULL), b_evt(NULL), b_nentry(NULL), b_y(NULL), b_z(NULL), b_dydx(NULL), b_dzdx(NULL),
  b_axy(NULL), b_axz(NULL), b_chsqy(NULL), b_chsqz(NULL), b_time(NULL), b_gid(NULL), b_pdgid(NULL),
  b_eta(NULL), b_pt(NULL), b_istation(NULL), b_phisec(NULL), b_zsec(NULL)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/afs/usatlas.bnl.gov/users/dladams/data/csc_simseg.root");
      if (!f) {
         new TFile("/afs/usatlas.bnl.gov/users/dladams/data/csc_simseg.root");
      }
      tree = (TTree*)gDirectory->Get("csc_simseg");

   }
   Init(tree);
}

SimsegAccessor::~SimsegAccessor()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SimsegAccessor::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SimsegAccessor::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() != TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void SimsegAccessor::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses of the tree
   // will be set. It is normaly not necessary to make changes to the
   // generated code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running with PROOF.

   // Set branch addresses
   if (tree == 0) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("run",&run);
   fChain->SetBranchAddress("evt",&evt);
   fChain->SetBranchAddress("nentry",&nentry);
   fChain->SetBranchAddress("y",y);
   fChain->SetBranchAddress("z",z);
   fChain->SetBranchAddress("dydx",dydx);
   fChain->SetBranchAddress("dzdx",dzdx);
   fChain->SetBranchAddress("axy",axy);
   fChain->SetBranchAddress("axz",axz);
   fChain->SetBranchAddress("chsqy",chsqy);
   fChain->SetBranchAddress("chsqz",chsqz);
   fChain->SetBranchAddress("time",time);
   fChain->SetBranchAddress("gid",gid);
   fChain->SetBranchAddress("pdgid",pdgid);
   fChain->SetBranchAddress("eta",eta);
   fChain->SetBranchAddress("pt",pt);
   fChain->SetBranchAddress("istation",istation);
   fChain->SetBranchAddress("phisec",phisec);
   fChain->SetBranchAddress("zsec",zsec);
   Notify();
}

Bool_t SimsegAccessor::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. Typically here the branch pointers
   // will be retrieved. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed.

   // Get branch pointers
   b_run = fChain->GetBranch("run");
   b_evt = fChain->GetBranch("evt");
   b_nentry = fChain->GetBranch("nentry");
   b_y = fChain->GetBranch("y");
   b_z = fChain->GetBranch("z");
   b_dydx = fChain->GetBranch("dydx");
   b_dzdx = fChain->GetBranch("dzdx");
   b_axy = fChain->GetBranch("axy");
   b_axz = fChain->GetBranch("axz");
   b_chsqy = fChain->GetBranch("chsqy");
   b_chsqz = fChain->GetBranch("chsqz");
   b_time = fChain->GetBranch("time");
   b_gid = fChain->GetBranch("gid");
   b_pdgid = fChain->GetBranch("pdgid");
   b_eta = fChain->GetBranch("eta");
   b_pt = fChain->GetBranch("pt");
   b_istation = fChain->GetBranch("istation");
   b_phisec = fChain->GetBranch("phisec");
   b_zsec = fChain->GetBranch("zsec");

   return kTRUE;
}

void SimsegAccessor::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SimsegAccessor::Cut(Long64_t)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef SimsegAccessor_cxx
