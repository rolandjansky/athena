/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ReldisTree_h
#define ReldisTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <iomanip>

class ReldisTree {
  
 public :
  
   static const int MAX_NUM_PART = 1000;
  
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Nleft;                     // "left side" NEUTRON multiplicity
   Float_t         Eleft;                     // "left side" NEUTRON energy total in GeV (ZDC energy)
   Int_t           Ipdg_l_n   [MAX_NUM_PART];
   Float_t         Pxl        [MAX_NUM_PART];
   Float_t         Pyl        [MAX_NUM_PART];
   Float_t         Pzl        [MAX_NUM_PART];

   Int_t           Nright;                    // "right side" NEUTRON multiplicity
   Float_t         Eright;                    // "right side" NEUTRON energy total in GeV (ZDC energy)
   Int_t           Ipdg_r_n   [MAX_NUM_PART];
   Float_t         Pxr        [MAX_NUM_PART];
   Float_t         Pyr        [MAX_NUM_PART];
   Float_t         Pzr        [MAX_NUM_PART];

   Int_t           Nleft_p;                   // "left side" PROTON multiplicity
   Float_t         Etaleft_p  [MAX_NUM_PART]; // "left side" PROTON pseudorapidity
   Int_t           Ipdg_l_p   [MAX_NUM_PART];
   Float_t         Pxl_p      [MAX_NUM_PART];
   Float_t         Pyl_p      [MAX_NUM_PART];
   Float_t         Pzl_p      [MAX_NUM_PART];

   Int_t           Nright_p;                  // "right side" PROTON multiplicity
   Float_t         Etaright_p [MAX_NUM_PART]; // "right side" PROTON pseudorapidity
   Int_t           Ipdg_r_p   [MAX_NUM_PART];
   Float_t         Pxr_p      [MAX_NUM_PART];
   Float_t         Pyr_p      [MAX_NUM_PART];
   Float_t         Pzr_p      [MAX_NUM_PART];

   Int_t           Nleft_pp;                  // "left side" PI PLUS multiplicity
   Float_t         Etaleft_pp [MAX_NUM_PART]; // "left side" PI PLUS pseudorapidity
   Int_t           Ipdg_l_pp  [MAX_NUM_PART];
   Float_t         Pxl_pp     [MAX_NUM_PART];
   Float_t         Pyl_pp     [MAX_NUM_PART];
   Float_t         Pzl_pp     [MAX_NUM_PART];

   Int_t           Nright_pp;                 // "right side" PI PLUS multiplicity
   Float_t         Etaright_pp[MAX_NUM_PART]; // "right side" PI PLUS pseudorapidity
   Int_t           Ipdg_r_pp  [MAX_NUM_PART];
   Float_t         Pxr_pp     [MAX_NUM_PART];
   Float_t         Pyr_pp     [MAX_NUM_PART];
   Float_t         Pzr_pp     [MAX_NUM_PART];

   Int_t           Nleft_pm;                  // "left side" PI MINUS multiplicity
   Float_t         Etaleft_pm [MAX_NUM_PART]; // "left side" PI MINUS pseudorapidity
   Int_t           Ipdg_l_pm  [MAX_NUM_PART];
   Float_t         Pxl_pm     [MAX_NUM_PART];
   Float_t         Pyl_pm     [MAX_NUM_PART];
   Float_t         Pzl_pm     [MAX_NUM_PART];

   Int_t           Nright_pm;                 // "right side" PI MINUS multiplicity
   Float_t         Etaright_pm[MAX_NUM_PART]; // "right side" PI MINUS pseudorapidity
   Int_t           Ipdg_r_pm  [MAX_NUM_PART];
   Float_t         Pxr_pm     [MAX_NUM_PART];
   Float_t         Pyr_pm     [MAX_NUM_PART];
   Float_t         Pzr_pm     [MAX_NUM_PART];

   Int_t           Nleft_p0;                  // "left side" PI ZERO multiplicity
   Float_t         Etaleft_p0 [MAX_NUM_PART]; // "left side" PI ZERO pseudorapidity
   Int_t           Ipdg_l_p0  [MAX_NUM_PART];
   Float_t         Pxl_p0     [MAX_NUM_PART];
   Float_t         Pyl_p0     [MAX_NUM_PART];
   Float_t         Pzl_p0     [MAX_NUM_PART];

   Int_t           Nright_p0;                 // "right side" PI ZERO multiplicity
   Float_t         Etaright_p0[MAX_NUM_PART]; // "right side" PI ZERO pseudorapidity
   Int_t           Ipdg_r_p0  [MAX_NUM_PART];
   Float_t         Pxr_p0     [MAX_NUM_PART];
   Float_t         Pyr_p0     [MAX_NUM_PART];
   Float_t         Pzr_p0     [MAX_NUM_PART];

   Int_t           Nleft_et;                  // "left side" ETA multiplicity
   Float_t         Etaleft_et [MAX_NUM_PART]; // "left side" ETA pseudorapidity
   Int_t           Ipdg_l_et  [MAX_NUM_PART];
   Float_t         Pxl_et     [MAX_NUM_PART];
   Float_t         Pyl_et     [MAX_NUM_PART];
   Float_t         Pzl_et     [MAX_NUM_PART];

   Int_t           Nright_et;                 // "right side" ETA multiplicity
   Float_t         Etaright_et[MAX_NUM_PART]; // "right side" ETA pseudorapidity
   Int_t           Ipdg_r_et  [MAX_NUM_PART];
   Float_t         Pxr_et     [MAX_NUM_PART];
   Float_t         Pyr_et     [MAX_NUM_PART];
   Float_t         Pzr_et     [MAX_NUM_PART];

   Int_t           Nleft_om;                  // "left side" OMEGA multiplicity
   Float_t         Etaleft_om [MAX_NUM_PART]; // "left side" OMEGA pseudorapidity
   Int_t           Ipdg_l_om  [MAX_NUM_PART];
   Float_t         Pxl_om     [MAX_NUM_PART];
   Float_t         Pyl_om     [MAX_NUM_PART];
   Float_t         Pzl_om     [MAX_NUM_PART];

   Int_t           Nright_om;                 // "right side" OMEGA multiplicity
   Float_t         Etaright_om[MAX_NUM_PART]; // "right side" OMEGA pseudorapidity
   Int_t           Ipdg_r_om  [MAX_NUM_PART];
   Float_t         Pxr_om     [MAX_NUM_PART];
   Float_t         Pyr_om     [MAX_NUM_PART];
   Float_t         Pzr_om     [MAX_NUM_PART];

   // List of branches
   TBranch        *b_Nleft;
   TBranch        *b_Eleft;
   TBranch        *b_Ipdg_l_n;
   TBranch        *b_Pxl;
   TBranch        *b_Pyl;
   TBranch        *b_Pzl;

   TBranch        *b_Nright;
   TBranch        *b_Eright;
   TBranch        *b_Ipdg_r_n;
   TBranch        *b_Pxr;
   TBranch        *b_Pyr;
   TBranch        *b_Pzr;

   TBranch        *b_Nleft_p;
   TBranch        *b_Etaleft_p;
   TBranch        *b_Ipdg_l_p;
   TBranch        *b_Pxl_p;
   TBranch        *b_Pyl_p;
   TBranch        *b_Pzl_p;

   TBranch        *b_Nright_p;
   TBranch        *b_Etaright_p;
   TBranch        *b_Ipdg_r_p;
   TBranch        *b_Pxr_p;
   TBranch        *b_Pyr_p;
   TBranch        *b_Pzr_p;

   TBranch        *b_Nleft_pp;
   TBranch        *b_Etaleft_pp;
   TBranch        *b_Ipdg_l_pp; 
   TBranch        *b_Pxl_pp;
   TBranch        *b_Pyl_pp;
   TBranch        *b_Pzl_pp;

   TBranch        *b_Nright_pp;
   TBranch        *b_Etaright_pp;
   TBranch        *b_Ipdg_r_pp;
   TBranch        *b_Pxr_pp;
   TBranch        *b_Pyr_pp;
   TBranch        *b_Pzr_pp;

   TBranch        *b_Nleft_pm;
   TBranch        *b_Etaleft_pm;
   TBranch        *b_Ipdg_l_pm;
   TBranch        *b_Pxl_pm;
   TBranch        *b_Pyl_pm;
   TBranch        *b_Pzl_pm;

   TBranch        *b_Nright_pm;
   TBranch        *b_Etaright_pm;
   TBranch        *b_Ipdg_r_pm;
   TBranch        *b_Pxr_pm;
   TBranch        *b_Pyr_pm;
   TBranch        *b_Pzr_pm;

   TBranch        *b_Nleft_p0;
   TBranch        *b_Etaleft_p0;
   TBranch        *b_Ipdg_l_p0;
   TBranch        *b_Pxl_p0;
   TBranch        *b_Pyl_p0;
   TBranch        *b_Pzl_p0;

   TBranch        *b_Nright_p0;
   TBranch        *b_Etaright_p0;
   TBranch        *b_Ipdg_r_p0;
   TBranch        *b_Pxr_p0;
   TBranch        *b_Pyr_p0;
   TBranch        *b_Pzr_p0;

   TBranch        *b_Nleft_et;
   TBranch        *b_Etaleft_et;
   TBranch        *b_Ipdg_l_et;
   TBranch        *b_Pxl_et;
   TBranch        *b_Pyl_et;
   TBranch        *b_Pzl_et;

   TBranch        *b_Nright_et;
   TBranch        *b_Etaright_et;
   TBranch        *b_Ipdg_r_et;
   TBranch        *b_Pxr_et;
   TBranch        *b_Pyr_et;
   TBranch        *b_Pzr_et;

   TBranch        *b_Nleft_om;
   TBranch        *b_Etaleft_om;
   TBranch        *b_Ipdg_l_om;
   TBranch        *b_Pxl_om;
   TBranch        *b_Pyl_om;
   TBranch        *b_Pzl_om;

   TBranch        *b_Nright_om;
   TBranch        *b_Etaright_om;
   TBranch        *b_Ipdg_r_om;
   TBranch        *b_Pxr_om;
   TBranch        *b_Pyr_om;
   TBranch        *b_Pzr_om;
   
   ReldisTree();
   virtual ~ReldisTree();
   
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual Bool_t   Init(TString rootFileName, TString treeName);
};

#endif
