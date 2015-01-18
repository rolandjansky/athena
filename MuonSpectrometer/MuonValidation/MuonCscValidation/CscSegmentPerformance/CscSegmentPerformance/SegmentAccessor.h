/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Nov  6 2000:33:13 2009 by ROOT version 5.22/00d
// from TTree csc_segment/CSC segments
// found on file: /susyraid1/home/wpark/atlas/AtlasOffline-15.5.1/MuonSpectrometer/MuonValidation/MuonCscValidation/CscClusterValidation/run/csc_clusters.root
//////////////////////////////////////////////////////////

#ifndef SegmentAccessor_h
#define SegmentAccessor_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>
using std::cout;
using std::endl;

class SegmentAccessor {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           run;
   Int_t           evt;
   Int_t           nentry;
   Int_t           nchamber;
   Int_t           nseg[32];   //[nchamber]
   Int_t           iseg[2000];   //[nentry]
   Int_t           nspoil[2000];   //[nentry]
   Int_t           nunspoil[2000];   //[nentry]
   Double_t        x[2000];   //[nentry]
   Double_t        y[2000];   //[nentry]
   Double_t        z[2000];   //[nentry]
   Double_t        s0[2000];   //[nentry]
   Double_t        s1[2000];   //[nentry]
   Double_t        sa[2000];   //[nentry]
   Double_t        d0[2000];   //[nentry]
   Double_t        d1[2000];   //[nentry]
   Double_t        da[2000];   //[nentry]
   Double_t        d01[2000];   //[nentry]
   Double_t        d0a[2000];   //[nentry]
   Double_t        chsq[2000];   //[nentry]
   Int_t           phase[2000];   //[nentry]
   Int_t           str0[2000][4];   //[nentry]
   Int_t           nstr[2000][4];   //[nentry]
   Int_t           pstr[2000][4];   //[nentry]
   Double_t        qsum[2000][4];   //[nentry]
   Double_t        tstr[2000][4];   //[nentry]
   Double_t        qleft[2000][4];   //[nentry]
   Double_t        qright[2000][4];   //[nentry]
   Double_t        qpeak[2000][4];   //[nentry]
   Int_t           sfit[2000][4];   //[nentry]
   Double_t        pos[2000][4];   //[nentry]
   Double_t        dpos[2000][4];   //[nentry]
   Double_t        res[2000][4];   //[nentry]
   Double_t        dres[2000][4];   //[nentry]
   Double_t        posAsSid[2000][4];   //[nentry]
   Double_t        posInStr[2000][4];   //[nentry]
   Double_t        refInStr[2000][4];   //[nentry]
   Double_t        rs2[2000];   //[nentry]
   Double_t        rs3[2000];   //[nentry]
   Double_t        drs2[2000];   //[nentry]
   Double_t        drs3[2000];   //[nentry]
   Int_t           measphi[2000];   //[nentry]
   Int_t           istation[2000];   //[nentry]
   Int_t           zsec[2000];   //[nentry]
   Int_t           phisec[2000];   //[nentry]
   Int_t           sector[2000];   //[nentry]
   Int_t           hitmap[2000];   //[nentry]
   Int_t           spoilmap[2000];   //[nentry]

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_nentry;   //!
   TBranch        *b_nchamber;   //!
   TBranch        *b_nseg;   //!
   TBranch        *b_iseg;   //!
   TBranch        *b_nspoil;   //!
   TBranch        *b_nunspoil;   //!
   TBranch        *b_x;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!
   TBranch        *b_s0;   //!
   TBranch        *b_s1;   //!
   TBranch        *b_sa;   //!
   TBranch        *b_d0;   //!
   TBranch        *b_d1;   //!
   TBranch        *b_da;   //!
   TBranch        *b_d01;   //!
   TBranch        *b_d0a;   //!
   TBranch        *b_chsq;   //!
   TBranch        *b_phase;   //!
   TBranch        *b_str0;   //!
   TBranch        *b_nstr;   //!
   TBranch        *b_pstr;   //!
   TBranch        *b_qsum;   //!
   TBranch        *b_tstr;   //!
   TBranch        *b_qleft;   //!
   TBranch        *b_qright;   //!
   TBranch        *b_qpeak;   //!
   TBranch        *b_sfit;   //!
   TBranch        *b_pos;   //!
   TBranch        *b_dpos;   //!
   TBranch        *b_res;   //!
   TBranch        *b_dres;   //!
   TBranch        *b_posAsSid;   //!
   TBranch        *b_posInStr;   //!
   TBranch        *b_refInStr;   //!
   TBranch        *b_rs2;   //!
   TBranch        *b_rs3;   //!
   TBranch        *b_drs2;   //!
   TBranch        *b_drs3;   //!
   TBranch        *b_measphi;   //!
   TBranch        *b_istation;   //!
   TBranch        *b_zsec;   //!
   TBranch        *b_phisec;   //!
   TBranch        *b_sector;   //!
   TBranch        *b_hitmap;   //!
   TBranch        *b_spoilmap;   //!

   SegmentAccessor(TTree *tree=0);
   virtual ~SegmentAccessor();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef SegmentAccessor_cxx
SegmentAccessor::SegmentAccessor(TTree *tree) :
   fChain(NULL), b_run(NULL), b_evt(NULL), b_nentry(NULL), b_nchamber(NULL), b_nseg(NULL), b_iseg(NULL),
   b_nspoil(NULL), b_nunspoil(NULL), b_x(NULL), b_y(NULL), b_z(NULL), b_s0(NULL), b_s1(NULL),
   b_sa(NULL), b_d0(NULL), b_d1(NULL), b_da(NULL), b_d01(NULL), b_d0a(NULL), b_chsq(NULL),
   b_phase(NULL), b_str0(NULL), b_nstr(NULL), b_pstr(NULL), b_qsum(NULL), b_tstr(NULL),
   b_qleft(NULL), b_qright(NULL), b_qpeak(NULL), b_sfit(NULL), b_pos(NULL), b_dpos(NULL),
   b_res(NULL), b_dres(NULL), b_posAsSid(NULL), b_posInStr(NULL), b_refInStr(NULL), b_rs2(NULL),
   b_rs3(NULL), b_drs2(NULL), b_drs3(NULL), b_measphi(NULL), b_istation(NULL), b_zsec(NULL),
   b_phisec(NULL), b_sector(NULL), b_hitmap(NULL), b_spoilmap(NULL)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/susyraid1/home/wpark/atlas/AtlasOffline-15.5.1/MuonSpectrometer/MuonValidation/MuonCscValidation/CscClusterValidation/run/csc_clusters.root");
      if (!f) {
         new TFile("/susyraid1/home/wpark/atlas/AtlasOffline-15.5.1/MuonSpectrometer/MuonValidation/MuonCscValidation/CscClusterValidation/run/csc_clusters.root");
      }
      tree = (TTree*)gDirectory->Get("csc_segment");

   }
   Init(tree);
}

SegmentAccessor::~SegmentAccessor()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SegmentAccessor::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SegmentAccessor::LoadTree(Long64_t entry)
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

void SegmentAccessor::Init(TTree *tree)
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
   fChain->SetBranchAddress("nchamber", &nchamber, &b_nchamber);
   fChain->SetBranchAddress("nseg", nseg, &b_nseg);
   fChain->SetBranchAddress("iseg", iseg, &b_iseg);
   fChain->SetBranchAddress("nspoil", nspoil, &b_nspoil);
   fChain->SetBranchAddress("nunspoil", nunspoil, &b_nunspoil);
   fChain->SetBranchAddress("x", x, &b_x);
   fChain->SetBranchAddress("y", y, &b_y);
   fChain->SetBranchAddress("z", z, &b_z);
   fChain->SetBranchAddress("s0", s0, &b_s0);
   fChain->SetBranchAddress("s1", s1, &b_s1);
   fChain->SetBranchAddress("sa", sa, &b_sa);
   fChain->SetBranchAddress("d0", d0, &b_d0);
   fChain->SetBranchAddress("d1", d1, &b_d1);
   fChain->SetBranchAddress("da", da, &b_da);
   fChain->SetBranchAddress("d01", d01, &b_d01);
   fChain->SetBranchAddress("d0a", d0a, &b_d0a);
   fChain->SetBranchAddress("chsq", chsq, &b_chsq);
   fChain->SetBranchAddress("phase", phase, &b_phase);
   fChain->SetBranchAddress("str0", str0, &b_str0);
   fChain->SetBranchAddress("nstr", nstr, &b_nstr);
   fChain->SetBranchAddress("pstr", pstr, &b_pstr);
   fChain->SetBranchAddress("qsum", qsum, &b_qsum);
   fChain->SetBranchAddress("tstr", tstr, &b_tstr);
   fChain->SetBranchAddress("qleft", qleft, &b_qleft);
   fChain->SetBranchAddress("qright", qright, &b_qright);
   fChain->SetBranchAddress("qpeak", qpeak, &b_qpeak);
   fChain->SetBranchAddress("sfit", sfit, &b_sfit);
   fChain->SetBranchAddress("pos", pos, &b_pos);
   fChain->SetBranchAddress("dpos", dpos, &b_dpos);
   fChain->SetBranchAddress("res", res, &b_res);
   fChain->SetBranchAddress("dres", dres, &b_dres);
   fChain->SetBranchAddress("posAsSid", posAsSid, &b_posAsSid);
   fChain->SetBranchAddress("posInStr", posInStr, &b_posInStr);
   fChain->SetBranchAddress("refInStr", refInStr, &b_refInStr);
   fChain->SetBranchAddress("rs2", rs2, &b_rs2);
   fChain->SetBranchAddress("rs3", rs3, &b_rs3);
   fChain->SetBranchAddress("drs2", drs2, &b_drs2);
   fChain->SetBranchAddress("drs3", drs3, &b_drs3);
   fChain->SetBranchAddress("measphi", measphi, &b_measphi);
   fChain->SetBranchAddress("istation", istation, &b_istation);
   fChain->SetBranchAddress("zsec", zsec, &b_zsec);
   fChain->SetBranchAddress("phisec", phisec, &b_phisec);
   fChain->SetBranchAddress("sector", sector, &b_sector);
   fChain->SetBranchAddress("hitmap", hitmap, &b_hitmap);
   fChain->SetBranchAddress("spoilmap", spoilmap, &b_spoilmap);
   Notify();
}

Bool_t SegmentAccessor::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void SegmentAccessor::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SegmentAccessor::Cut(Long64_t /*entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef SegmentAccessor_cxx
