/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Nov  5 23:56:49 2009 by ROOT version 5.22/00d
// from TTree csc_calib/calibration Constants
// found on file: /susyraid1/wpark/DATA/Cosmics/11042009/cosmic125032.20k_0xSigma.root
//////////////////////////////////////////////////////////

#ifndef CalibAccessor_h
#define CalibAccessor_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class CalibAccessor {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           measphi;
   Int_t           wlay;
   Int_t           istation;
   Int_t           zsec;
   Int_t           phisec;
   Int_t           sector;
   Int_t           nchan;
   Double_t        ped[192];   //[nchan]
   Double_t        pedADC[192];   //[nchan]
   Double_t        sigma[192];   //[nchan]
   Double_t        rms[192];   //[nchan]
   Double_t        sigmaADC[192];   //[nchan]
   Double_t        rmsADC[192];   //[nchan]
   Double_t        f001[192];   //[nchan]
   Double_t        f001ADC[192];   //[nchan]
   Double_t        thr[192];   //[nchan]
   Int_t           stat[192];   //[nchan]
   Int_t           address[192];   //[nchan]

   // List of branches
   TBranch        *b_measphi;   //!
   TBranch        *b_wlay;   //!
   TBranch        *b_istation;   //!
   TBranch        *b_zsec;   //!
   TBranch        *b_phisec;   //!
   TBranch        *b_sector;   //!
   TBranch        *b_nchan;   //!
   TBranch        *b_ped;   //!
   TBranch        *b_pedADC;   //!
   TBranch        *b_sigma;   //!
   TBranch        *b_rms;   //!
   TBranch        *b_sigmaADC;   //!
   TBranch        *b_rmsADC;   //!
   TBranch        *b_f001;   //!
   TBranch        *b_f001ADC;   //!
   TBranch        *b_thr;   //!
   TBranch        *b_stat;   //!
   TBranch        *b_address;   //!

   CalibAccessor(TTree *tree=0);
   virtual ~CalibAccessor();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef CalibAccessor_cxx
CalibAccessor::CalibAccessor(TTree *tree) :
   fChain(NULL), b_measphi(NULL), b_wlay(NULL), b_istation(NULL), b_zsec(NULL), b_phisec(NULL),
   b_sector(NULL), b_nchan(NULL), b_ped(NULL), b_pedADC(NULL), b_sigma(NULL),
   b_rms(NULL), b_sigmaADC(NULL), b_rmsADC(NULL), b_f001(NULL), b_f001ADC(NULL),
   b_thr(NULL), b_stat(NULL), b_address(NULL)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/susyraid1/wpark/DATA/Cosmics/11042009/cosmic125032.20k_0xSigma.root");
      if (!f) {
         new TFile("/susyraid1/wpark/DATA/Cosmics/11042009/cosmic125032.20k_0xSigma.root");
      }
      tree = (TTree*)gDirectory->Get("csc_calib");

   }
   Init(tree);
}

CalibAccessor::~CalibAccessor()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CalibAccessor::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CalibAccessor::LoadTree(Long64_t entry)
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

void CalibAccessor::Init(TTree *tree)
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

   fChain->SetBranchAddress("measphi", &measphi, &b_measphi);
   fChain->SetBranchAddress("wlay", &wlay, &b_wlay);
   fChain->SetBranchAddress("istation", &istation, &b_istation);
   fChain->SetBranchAddress("zsec", &zsec, &b_zsec);
   fChain->SetBranchAddress("phisec", &phisec, &b_phisec);
   fChain->SetBranchAddress("sector", &sector, &b_sector);
   fChain->SetBranchAddress("nchan", &nchan, &b_nchan);
   fChain->SetBranchAddress("ped", ped, &b_ped);
   fChain->SetBranchAddress("pedADC", pedADC, &b_pedADC);
   fChain->SetBranchAddress("sigma", sigma, &b_sigma);
   fChain->SetBranchAddress("rms", rms, &b_rms);
   fChain->SetBranchAddress("sigmaADC", sigmaADC, &b_sigmaADC);
   fChain->SetBranchAddress("rmsADC", rmsADC, &b_rmsADC);
   fChain->SetBranchAddress("f001", f001, &b_f001);
   fChain->SetBranchAddress("f001ADC", f001ADC, &b_f001ADC);
   fChain->SetBranchAddress("thr", thr, &b_thr);
   fChain->SetBranchAddress("stat", stat, &b_stat);
   fChain->SetBranchAddress("address", address, &b_address);
   Notify();
}

Bool_t CalibAccessor::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CalibAccessor::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CalibAccessor::Cut(Long64_t /*entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef CalibAccessor_cxx
