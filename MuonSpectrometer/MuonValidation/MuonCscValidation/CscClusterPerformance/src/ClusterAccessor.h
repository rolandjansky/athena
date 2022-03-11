/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 22 15:00:24 2009 by ROOT version 5.22/00d
// from TTree csc_cluster/CSC clusters
// found on file: csc_clusters.root
//////////////////////////////////////////////////////////

#ifndef ClusterAccessor_h
#define ClusterAccessor_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class ClusterAccessor {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           run;
   Int_t           evt;
   Int_t           nentry;
   Float_t         pos[800];   //[nentry]
   Float_t         error[800];   //[nentry]
   Float_t         x[800];   //[nentry]
   Float_t         y[800];   //[nentry]
   Float_t         z[800];   //[nentry]
   Int_t           nstrip[800];   //[nentry]
   Float_t         qstrip[800][10];   //[nentry]
   Float_t         tstrip[800][10];   //[nentry]
   Float_t         qpeak[800];   //[nentry]
   Float_t         speak[800];   //[nentry]
   Float_t         qsum[800];   //[nentry]
   Float_t         qleft[800];   //[nentry]
   Float_t         sleft[800];   //[nentry]
   Float_t         qright[800];   //[nentry]
   Float_t         sright[800];   //[nentry]
   Float_t         tpeak[800];   //[nentry]
   Int_t           sfit[800];   //[nentry]
   Int_t           measphi[800];   //[nentry]
   Int_t           wlay[800];   //[nentry]
   Int_t           istation[800];   //[nentry]
   Int_t           zsec[800];   //[nentry]
   Int_t           phisec[800];   //[nentry]
   Int_t           sector[800];   //[nentry]
   Float_t         pitch[800];   //[nentry]
   Int_t           srefit[800][10];   //[nentry]
   Float_t         posrefit[800][10];   //[nentry]
   Float_t         errrefit[800][10];   //[nentry]
   Float_t         maxstrip[800];   //[nentry]
   Int_t           strip0[800];   //[nentry]
   Int_t           pstrip[800];   //[nentry]
   Float_t         scor1[800];   //[nentry]
   Float_t         scor2[800];   //[nentry]
   Float_t         scor[800];   //[nentry]
   Float_t         dscor1[800];   //[nentry]
   Float_t         dscor2[800];   //[nentry]
   Float_t         dscor[800];   //[nentry]
   Float_t         scordiff[800];   //[nentry]
   Float_t         dscordiff[800];   //[nentry]

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_nentry;   //!
   TBranch        *b_pos;   //!
   TBranch        *b_error;   //!
   TBranch        *b_x;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!
   TBranch        *b_nstrip;   //!
   TBranch        *b_qstrip;   //!
   TBranch        *b_tstrip;   //!
   TBranch        *b_qpeak;   //!
   TBranch        *b_speak;   //!
   TBranch        *b_qsum;   //!
   TBranch        *b_qleft;   //!
   TBranch        *b_sleft;   //!
   TBranch        *b_qright;   //!
   TBranch        *b_sright;   //!
   TBranch        *b_tpeak;   //!
   TBranch        *b_sfit;   //!
   TBranch        *b_measphi;   //!
   TBranch        *b_wlay;   //!
   TBranch        *b_istation;   //!
   TBranch        *b_zsec;   //!
   TBranch        *b_phisec;   //!
   TBranch        *b_sector;   //!
   TBranch        *b_pitch;   //!
   TBranch        *b_srefit;   //!
   TBranch        *b_posrefit;   //!
   TBranch        *b_errrefit;   //!
   TBranch        *b_maxstrip;   //!
   TBranch        *b_strip0;   //!
   TBranch        *b_pstrip;   //!
   TBranch        *b_scor1;   //!
   TBranch        *b_scor2;   //!
   TBranch        *b_scor;   //!
   TBranch        *b_dscor1;   //!
   TBranch        *b_dscor2;   //!
   TBranch        *b_dscor;   //!
   TBranch        *b_scordiff;   //!
   TBranch        *b_dscordiff;   //!

   ClusterAccessor(TTree *tree=0);
   virtual ~ClusterAccessor();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ClusterAccessor_cxx
ClusterAccessor::ClusterAccessor(TTree *tree) :
   fChain(NULL), b_run(NULL), b_evt(NULL), b_nentry(NULL), b_pos(NULL), b_error(NULL),
   b_x(NULL), b_y(NULL), b_z(NULL), b_nstrip(NULL), b_qstrip(NULL), b_tstrip(NULL), b_qpeak(NULL),
   b_speak(NULL), b_qsum(NULL), b_qleft(NULL), b_sleft(NULL), b_qright(NULL), b_sright(NULL),
   b_tpeak(NULL), b_sfit(NULL), b_measphi(NULL), b_wlay(NULL), b_istation(NULL), b_zsec(NULL),
   b_phisec(NULL), b_sector(NULL), b_pitch(NULL), b_srefit(NULL), b_posrefit(NULL), b_errrefit(NULL),
   b_maxstrip(NULL), b_strip0(NULL), b_pstrip(NULL), b_scor1(NULL), b_scor2(NULL), b_scor(NULL),
   b_dscor1(NULL), b_dscor2(NULL), b_dscor(NULL), b_scordiff(NULL), b_dscordiff(NULL)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("csc_clusters.root");
      if (!f) {
         new TFile("csc_clusters.root");
      }
      tree = (TTree*)gDirectory->Get("csc_cluster");

   }
   Init(tree);
}

ClusterAccessor::~ClusterAccessor()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ClusterAccessor::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ClusterAccessor::LoadTree(Long64_t entry)
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

void ClusterAccessor::Init(TTree *tree)
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
   fChain->SetBranchAddress("pos", pos, &b_pos);
   fChain->SetBranchAddress("error", error, &b_error);
   fChain->SetBranchAddress("x", x, &b_x);
   fChain->SetBranchAddress("y", y, &b_y);
   fChain->SetBranchAddress("z", z, &b_z);
   fChain->SetBranchAddress("nstrip", nstrip, &b_nstrip);
   fChain->SetBranchAddress("qstrip", qstrip, &b_qstrip);
   fChain->SetBranchAddress("tstrip", tstrip, &b_tstrip);
   fChain->SetBranchAddress("qpeak", qpeak, &b_qpeak);
   fChain->SetBranchAddress("speak", speak, &b_speak);
   fChain->SetBranchAddress("qsum", qsum, &b_qsum);
   fChain->SetBranchAddress("qleft", qleft, &b_qleft);
   fChain->SetBranchAddress("sleft", sleft, &b_sleft);
   fChain->SetBranchAddress("qright", qright, &b_qright);
   fChain->SetBranchAddress("sright", sright, &b_sright);
   fChain->SetBranchAddress("tpeak", tpeak, &b_tpeak);
   fChain->SetBranchAddress("sfit", sfit, &b_sfit);
   fChain->SetBranchAddress("measphi", measphi, &b_measphi);
   fChain->SetBranchAddress("wlay", wlay, &b_wlay);
   fChain->SetBranchAddress("istation", istation, &b_istation);
   fChain->SetBranchAddress("zsec", zsec, &b_zsec);
   fChain->SetBranchAddress("phisec", phisec, &b_phisec);
   fChain->SetBranchAddress("sector", sector, &b_sector);
   fChain->SetBranchAddress("pitch", pitch, &b_pitch);
   fChain->SetBranchAddress("srefit", srefit, &b_srefit);
   fChain->SetBranchAddress("posrefit", posrefit, &b_posrefit);
   fChain->SetBranchAddress("errrefit", errrefit, &b_errrefit);
   fChain->SetBranchAddress("maxstrip", maxstrip, &b_maxstrip);
   fChain->SetBranchAddress("strip0", strip0, &b_strip0);
   fChain->SetBranchAddress("pstrip", pstrip, &b_pstrip);
   fChain->SetBranchAddress("scor1", scor1, &b_scor1);
   fChain->SetBranchAddress("scor2", scor2, &b_scor2);
   fChain->SetBranchAddress("scor", scor, &b_scor);
   fChain->SetBranchAddress("dscor1", dscor1, &b_dscor1);
   fChain->SetBranchAddress("dscor2", dscor2, &b_dscor2);
   fChain->SetBranchAddress("dscor", dscor, &b_dscor);
   fChain->SetBranchAddress("scordiff", scordiff, &b_scordiff);
   fChain->SetBranchAddress("dscordiff", dscordiff, &b_dscordiff);
   Notify();
}

Bool_t ClusterAccessor::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ClusterAccessor::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ClusterAccessor::Cut(Long64_t /*entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ClusterAccessor_cxx
