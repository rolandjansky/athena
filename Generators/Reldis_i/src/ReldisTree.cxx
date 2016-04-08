/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Reldis_i/ReldisTree.h"

ReldisTree::ReldisTree() {

  fChain   = 0;
  fCurrent = 0;

  Nleft     = 0;
  Eleft     = 0;
  Nright    = 0;
  Eright    = 0;
  Nleft_p   = 0;
  Nright_p  = 0;
  Nleft_pp  = 0;
  Nright_pp = 0;
  Nleft_pm  = 0;
  Nright_pm = 0;
  Nleft_p0  = 0;
  Nright_p0 = 0;
  Nleft_et  = 0;
  Nright_et = 0;
  Nleft_om  = 0;
  Nright_om = 0;

  for (int i=0; i<MAX_NUM_PART; i++) {

    Ipdg_l_n   [i] = 0;
    Pxl        [i] = 0;
    Pyl        [i] = 0;
    Pzl        [i] = 0;

    Ipdg_r_n   [i] = 0;
    Pxr        [i] = 0;
    Pyr        [i] = 0;
    Pzr        [i] = 0;

    Etaleft_p  [i] = 0;
    Ipdg_l_p   [i] = 0;
    Pxl_p      [i] = 0;
    Pyl_p      [i] = 0;
    Pzl_p      [i] = 0;

    Etaright_p [i] = 0;
    Ipdg_r_p   [i] = 0;
    Pxr_p      [i] = 0;
    Pyr_p      [i] = 0;
    Pzr_p      [i] = 0;

    Etaleft_pp [i] = 0;
    Ipdg_l_pp  [i] = 0;
    Pxl_pp     [i] = 0;
    Pyl_pp     [i] = 0;
    Pzl_pp     [i] = 0;

    Etaright_pp[i] = 0;
    Ipdg_r_pp  [i] = 0;
    Pxr_pp     [i] = 0;
    Pyr_pp     [i] = 0;
    Pzr_pp     [i] = 0;

    Etaleft_pm [i] = 0;
    Ipdg_l_pm  [i] = 0;
    Pxl_pm     [i] = 0;
    Pyl_pm     [i] = 0;
    Pzl_pm     [i] = 0;

    Etaright_pm[i] = 0;
    Ipdg_r_pm  [i] = 0;
    Pxr_pm     [i] = 0;
    Pyr_pm     [i] = 0;
    Pzr_pm     [i] = 0;

    Etaleft_p0 [i] = 0;
    Ipdg_l_p0  [i] = 0;
    Pxl_p0     [i] = 0;
    Pyl_p0     [i] = 0;
    Pzl_p0     [i] = 0;

    Etaright_p0[i] = 0;
    Ipdg_r_p0  [i] = 0;
    Pxr_p0     [i] = 0;
    Pyr_p0     [i] = 0;
    Pzr_p0     [i] = 0;

    Etaleft_et [i] = 0;
    Ipdg_l_et  [i] = 0;
    Pxl_et     [i] = 0;
    Pyl_et     [i] = 0;
    Pzl_et     [i] = 0;

    Etaright_et[i] = 0;
    Ipdg_r_et  [i] = 0;
    Pxr_et     [i] = 0;
    Pyr_et     [i] = 0;
    Pzr_et     [i] = 0;

    Etaleft_om [i] = 0;
    Ipdg_l_om  [i] = 0;
    Pxl_om     [i] = 0;
    Pyl_om     [i] = 0;
    Pzl_om     [i] = 0;

    Etaright_om[i] = 0;
    Ipdg_r_om  [i] = 0;
    Pxr_om     [i] = 0;
    Pyr_om     [i] = 0;
    Pzr_om     [i] = 0;
  }

  b_Nleft    = 0;
  b_Eleft    = 0;
  b_Ipdg_l_n = 0;
  b_Pxl      = 0;
  b_Pyl      = 0;
  b_Pzl      = 0;

  b_Nright   = 0;
  b_Eright   = 0;
  b_Ipdg_r_n = 0;
  b_Pxr      = 0;
  b_Pyr      = 0;
  b_Pzr      = 0;

  b_Nleft_p   = 0;
  b_Etaleft_p = 0;
  b_Ipdg_l_p  = 0;
  b_Pxl_p     = 0;
  b_Pyl_p     = 0;
  b_Pzl_p     = 0;

  b_Nright_p   = 0;
  b_Etaright_p = 0;
  b_Ipdg_r_p   = 0;
  b_Pxr_p      = 0;
  b_Pyr_p      = 0;
  b_Pzr_p      = 0;

  b_Nleft_pp   = 0;
  b_Etaleft_pp = 0;
  b_Ipdg_l_pp  = 0; 
  b_Pxl_pp     = 0;
  b_Pyl_pp     = 0;
  b_Pzl_pp     = 0;

  b_Nright_pp   = 0;
  b_Etaright_pp = 0;
  b_Ipdg_r_pp   = 0;
  b_Pxr_pp      = 0;
  b_Pyr_pp      = 0;
  b_Pzr_pp      = 0;

  b_Nleft_pm   = 0;
  b_Etaleft_pm = 0;
  b_Ipdg_l_pm  = 0;
  b_Pxl_pm     = 0;
  b_Pyl_pm     = 0;
  b_Pzl_pm     = 0;

  b_Nright_pm   = 0;
  b_Etaright_pm = 0;
  b_Ipdg_r_pm   = 0;
  b_Pxr_pm      = 0;
  b_Pyr_pm      = 0;
  b_Pzr_pm      = 0;

  b_Nleft_p0   = 0;
  b_Etaleft_p0 = 0;
  b_Ipdg_l_p0  = 0;
  b_Pxl_p0     = 0;
  b_Pyl_p0     = 0;
  b_Pzl_p0     = 0;

  b_Nright_p0   = 0;
  b_Etaright_p0 = 0;
  b_Ipdg_r_p0   = 0;
  b_Pxr_p0      = 0;
  b_Pyr_p0      = 0;
  b_Pzr_p0      = 0;

  b_Nleft_et   = 0;
  b_Etaleft_et = 0;
  b_Ipdg_l_et  = 0;
  b_Pxl_et     = 0;
  b_Pyl_et     = 0;
  b_Pzl_et     = 0;

  b_Nright_et   = 0;
  b_Etaright_et = 0;
  b_Ipdg_r_et   = 0;
  b_Pxr_et      = 0;
  b_Pyr_et      = 0;
  b_Pzr_et      = 0;

  b_Nleft_om   = 0;
  b_Etaleft_om = 0;
  b_Ipdg_l_om  = 0;
  b_Pxl_om     = 0;
  b_Pyl_om     = 0;
  b_Pzl_om     = 0;

  b_Nright_om   = 0;
  b_Etaright_om = 0;
  b_Ipdg_r_om   = 0;
  b_Pxr_om      = 0;
  b_Pyr_om      = 0;
  b_Pzr_om      = 0;
}

ReldisTree::~ReldisTree() {
  
  if (!fChain) return;
  
  delete fChain->GetCurrentFile();
}

Int_t ReldisTree::GetEntry(Long64_t entry) {

  if (!fChain) return 0;
  
  return fChain->GetEntry(entry);
}

Long64_t ReldisTree::LoadTree(Long64_t entry) {

  if (!fChain) return -5;

  Long64_t centry = fChain->LoadTree(entry);  

  if (centry < 0) return centry;
   
  if (!fChain->InheritsFrom(TChain::Class())) return centry;
   
  TChain* chain = (TChain*)fChain;
   
  if (chain->GetTreeNumber() != fCurrent)
    fCurrent = chain->GetTreeNumber();
   
  return centry;
}

Bool_t ReldisTree::Init(TString rootFileName, TString treeName) {
  
  TFile* file = new TFile(rootFileName);
  
  if (!file) return kFALSE; 
  
  TTree* tree = (TTree*)gDirectory->Get(treeName);
  
  if (!tree) {
    
    std::cout << treeName << " tree not found! " << std::endl;
    return kFALSE;
  }
  
  // Set branch addresses and branch pointers  
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("Nleft",       &Nleft,       &b_Nleft);
  fChain->SetBranchAddress("Eleft",       &Eleft,       &b_Eleft);
  fChain->SetBranchAddress("Ipdg_l_n",     Ipdg_l_n,    &b_Ipdg_l_n);
  fChain->SetBranchAddress("Pxl",          Pxl,         &b_Pxl);
  fChain->SetBranchAddress("Pyl",          Pyl,         &b_Pyl);
  fChain->SetBranchAddress("Pzl",          Pzl,         &b_Pzl);

  fChain->SetBranchAddress("Nright",      &Nright,      &b_Nright);
  fChain->SetBranchAddress("Eright",      &Eright,      &b_Eright);
  fChain->SetBranchAddress("Ipdg_r_n",     Ipdg_r_n,    &b_Ipdg_r_n);
  fChain->SetBranchAddress("Pxr",          Pxr,         &b_Pxr);
  fChain->SetBranchAddress("Pyr",          Pyr,         &b_Pyr);
  fChain->SetBranchAddress("Pzr",          Pzr,         &b_Pzr);

  fChain->SetBranchAddress("Nleft_p",     &Nleft_p,     &b_Nleft_p);
  fChain->SetBranchAddress("Etaleft_p",    Etaleft_p,   &b_Etaleft_p);
  fChain->SetBranchAddress("Ipdg_l_p",     Ipdg_l_p,    &b_Ipdg_l_p);
  fChain->SetBranchAddress("Pxl_p",        Pxl_p,       &b_Pxl_p);
  fChain->SetBranchAddress("Pyl_p",        Pyl_p,       &b_Pyl_p);
  fChain->SetBranchAddress("Pzl_p",        Pzl_p,       &b_Pzl_p);

  fChain->SetBranchAddress("Nright_p",    &Nright_p,    &b_Nright_p);
  fChain->SetBranchAddress("Etaright_p",   Etaright_p,  &b_Etaright_p);
  fChain->SetBranchAddress("Ipdg_r_p",     Ipdg_r_p,    &b_Ipdg_r_p);
  fChain->SetBranchAddress("Pxr_p",        Pxr_p,       &b_Pxr_p);
  fChain->SetBranchAddress("Pyr_p",        Pyr_p,       &b_Pyr_p);
  fChain->SetBranchAddress("Pzr_p",        Pzr_p,       &b_Pzr_p);

  fChain->SetBranchAddress("Nleft_pp",    &Nleft_pp,    &b_Nleft_pp);
  fChain->SetBranchAddress("Etaleft_pp",   Etaleft_pp,  &b_Etaleft_pp);
  fChain->SetBranchAddress("Ipdg_l_pp",    Ipdg_l_pp,   &b_Ipdg_l_pp);
  fChain->SetBranchAddress("Pxl_pp",       Pxl_pp,      &b_Pxl_pp);
  fChain->SetBranchAddress("Pyl_pp",       Pyl_pp,      &b_Pyl_pp);
  fChain->SetBranchAddress("Pzl_pp",       Pzl_pp,      &b_Pzl_pp);

  fChain->SetBranchAddress("Nright_pp",   &Nright_pp,   &b_Nright_pp);
  fChain->SetBranchAddress("Etaright_pp",  Etaright_pp, &b_Etaright_pp);
  fChain->SetBranchAddress("Ipdg_r_pp",    Ipdg_r_pp,   &b_Ipdg_r_pp);
  fChain->SetBranchAddress("Pxr_pp",       Pxr_pp,      &b_Pxr_pp);
  fChain->SetBranchAddress("Pyr_pp",       Pyr_pp,      &b_Pyr_pp);
  fChain->SetBranchAddress("Pzr_pp",       Pzr_pp,      &b_Pzr_pp);

  fChain->SetBranchAddress("Nleft_pm",    &Nleft_pm,    &b_Nleft_pm);
  fChain->SetBranchAddress("Etaleft_pm",   Etaleft_pm,  &b_Etaleft_pm);
  fChain->SetBranchAddress("Ipdg_l_pm",    Ipdg_l_pm,   &b_Ipdg_l_pm);
  fChain->SetBranchAddress("Pxl_pm",       Pxl_pm,      &b_Pxl_pm);
  fChain->SetBranchAddress("Pyl_pm",       Pyl_pm,      &b_Pyl_pm);
  fChain->SetBranchAddress("Pzl_pm",       Pzl_pm,      &b_Pzl_pm);

  fChain->SetBranchAddress("Nright_pm",   &Nright_pm,   &b_Nright_pm);
  fChain->SetBranchAddress("Etaright_pm",  Etaright_pm, &b_Etaright_pm);
  fChain->SetBranchAddress("Ipdg_r_pm",    Ipdg_r_pm,   &b_Ipdg_r_pm);
  fChain->SetBranchAddress("Pxr_pm",       Pxr_pm,      &b_Pxr_pm);
  fChain->SetBranchAddress("Pyr_pm",       Pyr_pm,      &b_Pyr_pm);
  fChain->SetBranchAddress("Pzr_pm",       Pzr_pm,      &b_Pzr_pm);

  fChain->SetBranchAddress("Nleft_p0",    &Nleft_p0,    &b_Nleft_p0);
  fChain->SetBranchAddress("Etaleft_p0",   Etaleft_p0,  &b_Etaleft_p0);
  fChain->SetBranchAddress("Ipdg_l_p0",    Ipdg_l_p0,   &b_Ipdg_l_p0);
  fChain->SetBranchAddress("Pxl_p0",       Pxl_p0,      &b_Pxl_p0);
  fChain->SetBranchAddress("Pyl_p0",       Pyl_p0,      &b_Pyl_p0);
  fChain->SetBranchAddress("Pzl_p0",       Pzl_p0,      &b_Pzl_p0);

  fChain->SetBranchAddress("Nright_p0",   &Nright_p0,   &b_Nright_p0);
  fChain->SetBranchAddress("Etaright_p0",  Etaright_p0, &b_Etaright_p0);
  fChain->SetBranchAddress("Ipdg_r_p0",    Ipdg_r_p0,   &b_Ipdg_r_p0);
  fChain->SetBranchAddress("Pxr_p0",       Pxr_p0,      &b_Pxr_p0);
  fChain->SetBranchAddress("Pyr_p0",       Pyr_p0,      &b_Pyr_p0);
  fChain->SetBranchAddress("Pzr_p0",       Pzr_p0,      &b_Pzr_p0);

  fChain->SetBranchAddress("Nleft_et",    &Nleft_et,    &b_Nleft_et);
  fChain->SetBranchAddress("Etaleft_et",   Etaleft_et,  &b_Etaleft_et);
  fChain->SetBranchAddress("Ipdg_l_et",    Ipdg_l_et,   &b_Ipdg_l_et);
  fChain->SetBranchAddress("Pxl_et",       Pxl_et,      &b_Pxl_et);
  fChain->SetBranchAddress("Pyl_et",       Pyl_et,      &b_Pyl_et);
  fChain->SetBranchAddress("Pzl_et",       Pzl_et,      &b_Pzl_et);

  fChain->SetBranchAddress("Nright_et",   &Nright_et,   &b_Nright_et);
  fChain->SetBranchAddress("Etaright_et",  Etaright_et, &b_Etaright_et);
  fChain->SetBranchAddress("Ipdg_r_et",    Ipdg_r_et,   &b_Ipdg_r_et);
  fChain->SetBranchAddress("Pxr_et",       Pxr_et,      &b_Pxr_et);
  fChain->SetBranchAddress("Pyr_et",       Pyr_et,      &b_Pyr_et);
  fChain->SetBranchAddress("Pzr_et",       Pzr_et,      &b_Pzr_et);

  fChain->SetBranchAddress("Nleft_om",    &Nleft_om,    &b_Nleft_om);
  fChain->SetBranchAddress("Etaleft_om",   Etaleft_om,  &b_Etaleft_om);
  fChain->SetBranchAddress("Ipdg_l_om",    Ipdg_l_om,   &b_Ipdg_l_om);
  fChain->SetBranchAddress("Pxl_om",       Pxl_om,      &b_Pxl_om);
  fChain->SetBranchAddress("Pyl_om",       Pyl_om,      &b_Pyl_om);
  fChain->SetBranchAddress("Pzl_om",       Pzl_om,      &b_Pzl_om);

  fChain->SetBranchAddress("Nright_om",   &Nright_om,   &b_Nright_om);
  fChain->SetBranchAddress("Etaright_om",  Etaright_om, &b_Etaright_om);
  fChain->SetBranchAddress("Ipdg_r_om",    Ipdg_r_om,   &b_Ipdg_r_om);
  fChain->SetBranchAddress("Pxr_om",       Pxr_om,      &b_Pxr_om);
  fChain->SetBranchAddress("Pyr_om",       Pyr_om,      &b_Pyr_om);
  fChain->SetBranchAddress("Pzr_om",       Pzr_om,      &b_Pzr_om);

  return kTRUE;
}
