/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Nov  6 11:20:13 2009 by ROOT version 5.22/00d
// from TTree csc_4d_segment/CSC 4D segments
// found on file: csc_clusters.root
//////////////////////////////////////////////////////////

#ifndef Segment4dAccessor_h
#define Segment4dAccessor_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class Segment4dAccessor {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           run;
   Int_t           evt;
   Int_t           nentry;
   Int_t           nchamber;
   Int_t           nseg[32];   //[nchamber]
   Int_t           nhit[2000];   //[nentry]
   Double_t        gx[2000];   //[nentry]
   Double_t        gy[2000];   //[nentry]
   Double_t        gz[2000];   //[nentry]
   Double_t        y[2000];   //[nentry]
   Double_t        x[2000];   //[nentry]
   Double_t        ay[2000];   //[nentry]
   Double_t        ax[2000];   //[nentry]
   Double_t        dy[2000];   //[nentry]
   Double_t        dx[2000];   //[nentry]
   Double_t        day[2000];   //[nentry]
   Double_t        dax[2000];   //[nentry]
   Double_t        eyx[2000];   //[nentry]
   Double_t        eyay[2000];   //[nentry]
   Double_t        eyax[2000];   //[nentry]
   Double_t        exay[2000];   //[nentry]
   Double_t        exax[2000];   //[nentry]
   Double_t        eayax[2000];   //[nentry]
   Double_t        chsq[2000];   //[nentry]
   Int_t           phase[2000];   //[nentry]
   Int_t           ihit[2000][8];   //[nentry]
   Int_t           str0[2000][8];   //[nentry]
   Int_t           nstr[2000][8];   //[nentry]
   Int_t           pstr[2000][8];   //[nentry]
   Int_t           sfit[2000][8];   //[nentry]
   Double_t        qsum[2000][8];   //[nentry]
   Double_t        qleft[2000][8];   //[nentry]
   Double_t        qright[2000][8];   //[nentry]
   Double_t        qpeak[2000][8];   //[nentry]
   Double_t        dqleft[2000][8];   //[nentry]
   Double_t        dqright[2000][8];   //[nentry]
   Double_t        dqpeak[2000][8];   //[nentry]
   Double_t        tpeak[2000][8];   //[nentry]
   Double_t        pos[2000][8];   //[nentry]
   Double_t        dpos[2000][8];   //[nentry]
   Double_t        res[2000][8];   //[nentry]
   Double_t        dres[2000][8];   //[nentry]
   Double_t        rs[2000][8];   //[nentry]
   Double_t        drs[2000][8];   //[nentry]
   Double_t        posAsSid[2000][8];   //[nentry]
   Double_t        posInStr[2000][8];   //[nentry]
   Double_t        refInStr[2000][8];   //[nentry]
   Int_t           istation[2000];   //[nentry]
   Int_t           zsec[2000];   //[nentry]
   Int_t           phisec[2000];   //[nentry]
   Int_t           sector[2000];   //[nentry]
   Int_t           hitmap[2000];   //[nentry]

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_evt;   //!
   TBranch        *b_nentry;   //!
   TBranch        *b_nchamber;   //!
   TBranch        *b_nseg;   //!
   TBranch        *b_nhit;   //!
   TBranch        *b_gx;   //!
   TBranch        *b_gy;   //!
   TBranch        *b_gz;   //!
   TBranch        *b_y;   //!
   TBranch        *b_x;   //!
   TBranch        *b_ay;   //!
   TBranch        *b_ax;   //!
   TBranch        *b_dy;   //!
   TBranch        *b_dx;   //!
   TBranch        *b_day;   //!
   TBranch        *b_dax;   //!
   TBranch        *b_eyx;   //!
   TBranch        *b_eyay;   //!
   TBranch        *b_eyax;   //!
   TBranch        *b_exay;   //!
   TBranch        *b_exax;   //!
   TBranch        *b_eayax;   //!
   TBranch        *b_chsq;   //!
   TBranch        *b_phase;   //!
   TBranch        *b_ihit;   //!
   TBranch        *b_str0;   //!
   TBranch        *b_nstr;   //!
   TBranch        *b_pstr;   //!
   TBranch        *b_sfit;   //!
   TBranch        *b_qsum;   //!
   TBranch        *b_qleft;   //!
   TBranch        *b_qright;   //!
   TBranch        *b_qpeak;   //!
   TBranch        *b_dqleft;   //!
   TBranch        *b_dqright;   //!
   TBranch        *b_dqpeak;   //!
   TBranch        *b_tpeak;   //!
   TBranch        *b_pos;   //!
   TBranch        *b_dpos;   //!
   TBranch        *b_res;   //!
   TBranch        *b_dres;   //!
   TBranch        *b_rs;   //!
   TBranch        *b_drs;   //!
   TBranch        *b_posAsSid;   //!
   TBranch        *b_posInStr;   //!
   TBranch        *b_refInStr;   //!
   TBranch        *b_istation;   //!
   TBranch        *b_zsec;   //!
   TBranch        *b_phisec;   //!
   TBranch        *b_sector;   //!
   TBranch        *b_hitmap;   //!

   Segment4dAccessor(TTree *tree=0);
   virtual ~Segment4dAccessor();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Segment4dAccessor_cxx
Segment4dAccessor::Segment4dAccessor(TTree *tree) :
   fChain(NULL), b_run(NULL), b_evt(NULL), b_nentry(NULL), b_nchamber(NULL), b_nseg(NULL),
   b_nhit(NULL), b_gx(NULL), b_gy(NULL), b_gz(NULL), b_y(NULL), b_x(NULL),
   b_ay(NULL), b_ax(NULL), b_dy(NULL), b_dx(NULL), b_day(NULL), b_dax(NULL),
   b_eyx(NULL), b_eyay(NULL), b_eyax(NULL), b_exay(NULL), b_exax(NULL), b_eayax(NULL),
   b_chsq(NULL), b_phase(NULL), b_ihit(NULL), b_str0(NULL), b_nstr(NULL), b_pstr(NULL),
   b_sfit(NULL), b_qsum(NULL), b_qleft(NULL), b_qright(NULL), b_qpeak(NULL),
   b_dqleft(NULL), b_dqright(NULL), b_dqpeak(NULL), b_tpeak(NULL), b_pos(NULL),
   b_dpos(NULL), b_res(NULL), b_dres(NULL), b_rs(NULL), b_drs(NULL), b_posAsSid(NULL),
   b_posInStr(NULL), b_refInStr(NULL), b_istation(NULL), b_zsec(NULL), b_phisec(NULL),
   b_sector(NULL), b_hitmap(NULL)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("csc_clusters.root");
      if (!f) {
         new TFile("csc_clusters.root");
      }
      tree = (TTree*)gDirectory->Get("csc_4d_segment");

   }
   Init(tree);
}

Segment4dAccessor::~Segment4dAccessor()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Segment4dAccessor::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Segment4dAccessor::LoadTree(Long64_t entry)
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

void Segment4dAccessor::Init(TTree *tree)
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
   fChain->SetBranchAddress("nhit", nhit, &b_nhit);
   fChain->SetBranchAddress("gx", gx, &b_gx);
   fChain->SetBranchAddress("gy", gy, &b_gy);
   fChain->SetBranchAddress("gz", gz, &b_gz);
   fChain->SetBranchAddress("y", y, &b_y);
   fChain->SetBranchAddress("x", x, &b_x);
   fChain->SetBranchAddress("ay", ay, &b_ay);
   fChain->SetBranchAddress("ax", ax, &b_ax);
   fChain->SetBranchAddress("dy", dy, &b_dy);
   fChain->SetBranchAddress("dx", dx, &b_dx);
   fChain->SetBranchAddress("day", day, &b_day);
   fChain->SetBranchAddress("dax", dax, &b_dax);
   fChain->SetBranchAddress("eyx", eyx, &b_eyx);
   fChain->SetBranchAddress("eyay", eyay, &b_eyay);
   fChain->SetBranchAddress("eyax", eyax, &b_eyax);
   fChain->SetBranchAddress("exay", exay, &b_exay);
   fChain->SetBranchAddress("exax", exax, &b_exax);
   fChain->SetBranchAddress("eayax", eayax, &b_eayax);
   fChain->SetBranchAddress("chsq", chsq, &b_chsq);
   fChain->SetBranchAddress("phase", phase, &b_phase);
   fChain->SetBranchAddress("ihit", ihit, &b_ihit);
   fChain->SetBranchAddress("str0", str0, &b_str0);
   fChain->SetBranchAddress("nstr", nstr, &b_nstr);
   fChain->SetBranchAddress("pstr", pstr, &b_pstr);
   fChain->SetBranchAddress("sfit", sfit, &b_sfit);
   fChain->SetBranchAddress("qsum", qsum, &b_qsum);
   fChain->SetBranchAddress("qleft", qleft, &b_qleft);
   fChain->SetBranchAddress("qright", qright, &b_qright);
   fChain->SetBranchAddress("qpeak", qpeak, &b_qpeak);
   fChain->SetBranchAddress("dqleft", dqleft, &b_dqleft);
   fChain->SetBranchAddress("dqright", dqright, &b_dqright);
   fChain->SetBranchAddress("dqpeak", dqpeak, &b_dqpeak);
   fChain->SetBranchAddress("tpeak", tpeak, &b_tpeak);
   fChain->SetBranchAddress("pos", pos, &b_pos);
   fChain->SetBranchAddress("dpos", dpos, &b_dpos);
   fChain->SetBranchAddress("res", res, &b_res);
   fChain->SetBranchAddress("dres", dres, &b_dres);
   fChain->SetBranchAddress("rs", rs, &b_rs);
   fChain->SetBranchAddress("drs", drs, &b_drs);
   fChain->SetBranchAddress("posAsSid", posAsSid, &b_posAsSid);
   fChain->SetBranchAddress("posInStr", posInStr, &b_posInStr);
   fChain->SetBranchAddress("refInStr", refInStr, &b_refInStr);
   fChain->SetBranchAddress("istation", istation, &b_istation);
   fChain->SetBranchAddress("zsec", zsec, &b_zsec);
   fChain->SetBranchAddress("phisec", phisec, &b_phisec);
   fChain->SetBranchAddress("sector", sector, &b_sector);
   fChain->SetBranchAddress("hitmap", hitmap, &b_hitmap);
   Notify();
}

Bool_t Segment4dAccessor::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Segment4dAccessor::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Segment4dAccessor::Cut(Long64_t /*entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Segment4dAccessor_cxx
