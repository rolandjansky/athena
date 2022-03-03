/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRec_h1000.h"

TileRec_h1000::TileRec_h1000(TTree *tree)
  : fChain(0),
    fCurrent(0),
    b_EvTime(0),
    b_Run(0),
    b_Evt(0),
    b_Trig(0),
    b_L1ID(0),
    b_L1Type(0),
    b_EvType(0),
    b_EvBCID(0),
    b_FrBCID(0),
    b_MuBackHit(0),
    b_MuBackSum(0),
    b_MuBack(0),
    b_MuCalib(0),
    b_Ecal(0),
    b_LasFlag(0),
    b_Las0(0),
    b_Las1(0),
    b_Las2(0),
    b_Las3(0),
    b_LasExtra(0),
    b_Cispar(0),
    b_S1cou(0),
    b_S2cou(0),
    b_S3cou(0),
    b_MuTag(0),
    b_Cher1(0),
    b_Cher2(0),
    b_MuHalo(0),
    b_MuVeto(0),
    b_S2extra(0),
    b_S3extra(0),
    b_SC1(0),
    b_SC2(0),
    b_pu(0),
    b_tof(0),
    b_btdc1(0),
    b_btdc2(0),
    b_XchN2(0),
    b_YchN2(0),
    b_XchN1(0),
    b_YchN1(0),
    b_Xcha0(0),
    b_Ycha0(0),
    b_Xcha1(0),
    b_Ycha1(0),
    b_Xcha2(0),
    b_Ycha2(0),
    b_Ximp(0),
    b_Yimp(0),
    b_EvtC0(0),
    b_BCIDC0(0),
    b_rodBCIDC0(0),
    b_SizeC0(0),
    b_DMUMaskC0(0),
    b_SlinkCRCC0(0),
    b_GainC0(0),
    b_ErrC0(0),
    b_SampleC0(0),
    b_feCRCC0(0),
    b_rodCRCC0(0),
    b_EfitC0(0),
    b_TfitC0(0),
    b_PedfitC0(0),
    b_Chi2C0(0),
    b_EvtC1(0),
    b_BCIDC1(0),
    b_rodBCIDC1(0),
    b_SizeC1(0),
    b_DMUMaskC1(0),
    b_SlinkCRCC1(0),
    b_GainC1(0),
    b_ErrC1(0),
    b_SampleC1(0),
    b_feCRCC1(0),
    b_rodCRCC1(0),
    b_EfitC1(0),
    b_TfitC1(0),
    b_PedfitC1(0),
    b_Chi2C1(0),
    b_EvtC2(0),
    b_BCIDC2(0),
    b_rodBCIDC2(0),
    b_SizeC2(0),
    b_DMUMaskC2(0),
    b_SlinkCRCC2(0),
    b_GainC2(0),
    b_ErrC2(0),
    b_SampleC2(0),
    b_feCRCC2(0),
    b_rodCRCC2(0),
    b_EfitC2(0),
    b_TfitC2(0),
    b_PedfitC2(0),
    b_Chi2C2(0),
    b_EvtA0(0),
    b_BCIDA0(0),
    b_rodBCIDA0(0),
    b_SizeA0(0),
    b_DMUMaskA0(0),
    b_SlinkCRCA0(0),
    b_GainA0(0),
    b_ErrA0(0),
    b_SampleA0(0),
    b_feCRCA0(0),
    b_rodCRCA0(0),
    b_EfitA0(0),
    b_TfitA0(0),
    b_PedfitA0(0),
    b_Chi2A0(0),
    b_EvtA1(0),
    b_BCIDA1(0),
    b_rodBCIDA1(0),
    b_SizeA1(0),
    b_DMUMaskA1(0),
    b_SlinkCRCA1(0),
    b_GainA1(0),
    b_ErrA1(0),
    b_SampleA1(0),
    b_feCRCA1(0),
    b_rodCRCA1(0),
    b_EfitA1(0),
    b_TfitA1(0),
    b_PedfitA1(0),
    b_Chi2A1(0),
    b_EvtA2(0),
    b_BCIDA2(0),
    b_rodBCIDA2(0),
    b_SizeA2(0),
    b_DMUMaskA2(0),
    b_SlinkCRCA2(0),
    b_GainA2(0),
    b_ErrA2(0),
    b_SampleA2(0),
    b_feCRCA2(0),
    b_rodCRCA2(0),
    b_EfitA2(0),
    b_TfitA2(0),
    b_PedfitA2(0),
    b_Chi2A2(0),
    b_LarSmp(0),
    b_BarSmp(0),
    b_ExtSmp(0),
    b_GapSmp(0)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("cbnt_RecExTB_Combined_1207_2102095.0.root");
      if (!f) {
         f = new TFile("cbnt_RecExTB_Combined_1207_2102095.0.root");
         f->cd("cbnt_RecExTB_Combined_1207_2102095.0.root:/TileRec");
      }
      tree = (TTree*)gDirectory->Get("h1000");

   }
   TileRec_h1000::Init(tree);
}

TileRec_h1000::~TileRec_h1000()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TileRec_h1000::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TileRec_h1000::LoadTree(Long64_t entry)
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

void TileRec_h1000::Init(TTree *tree)
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

   fChain->SetBranchAddress("EvTime", &EvTime, &b_EvTime);
   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Evt", &Evt, &b_Evt);
   fChain->SetBranchAddress("Trig", &Trig, &b_Trig);
   fChain->SetBranchAddress("L1ID", L1ID, &b_L1ID);
   fChain->SetBranchAddress("L1Type", L1Type, &b_L1Type);
   fChain->SetBranchAddress("EvType", EvType, &b_EvType);
   fChain->SetBranchAddress("EvBCID", EvBCID, &b_EvBCID);
   fChain->SetBranchAddress("FrBCID", FrBCID, &b_FrBCID);
   fChain->SetBranchAddress("MuBackHit", &MuBackHit, &b_MuBackHit);
   fChain->SetBranchAddress("MuBackSum", &MuBackSum, &b_MuBackSum);
   fChain->SetBranchAddress("MuBack", MuBack, &b_MuBack);
   fChain->SetBranchAddress("MuCalib", MuCalib, &b_MuCalib);
   fChain->SetBranchAddress("Ecal", Ecal, &b_Ecal);
   fChain->SetBranchAddress("LasFlag", &LasFlag, &b_LasFlag);
   fChain->SetBranchAddress("Las0", &Las0, &b_Las0);
   fChain->SetBranchAddress("Las1", &Las1, &b_Las1);
   fChain->SetBranchAddress("Las2", &Las2, &b_Las2);
   fChain->SetBranchAddress("Las3", &Las3, &b_Las3);
   fChain->SetBranchAddress("LasExtra", LasExtra, &b_LasExtra);
   fChain->SetBranchAddress("Cispar", Cispar, &b_Cispar);
   fChain->SetBranchAddress("S1cou", &S1cou, &b_S1cou);
   fChain->SetBranchAddress("S2cou", &S2cou, &b_S2cou);
   fChain->SetBranchAddress("S3cou", &S3cou, &b_S3cou);
   fChain->SetBranchAddress("MuTag", &MuTag, &b_MuTag);
   fChain->SetBranchAddress("Cher1", &Cher1, &b_Cher1);
   fChain->SetBranchAddress("Cher2", &Cher2, &b_Cher2);
   fChain->SetBranchAddress("MuHalo", &MuHalo, &b_MuHalo);
   fChain->SetBranchAddress("MuVeto", &MuVeto, &b_MuVeto);
   fChain->SetBranchAddress("S2extra", &S2extra, &b_S2extra);
   fChain->SetBranchAddress("S3extra", &S3extra, &b_S3extra);
   fChain->SetBranchAddress("SC1", &SC1, &b_SC1);
   fChain->SetBranchAddress("SC2", &SC2, &b_SC2);
   fChain->SetBranchAddress("pu", &pu, &b_pu);
   fChain->SetBranchAddress("tof", tof, &b_tof);
   fChain->SetBranchAddress("btdc1", btdc1, &b_btdc1);
   fChain->SetBranchAddress("btdc2", btdc2, &b_btdc2);
   fChain->SetBranchAddress("XchN2", &XchN2, &b_XchN2);
   fChain->SetBranchAddress("YchN2", &YchN2, &b_YchN2);
   fChain->SetBranchAddress("XchN1", &XchN1, &b_XchN1);
   fChain->SetBranchAddress("YchN1", &YchN1, &b_YchN1);
   fChain->SetBranchAddress("Xcha0", &Xcha0, &b_Xcha0);
   fChain->SetBranchAddress("Ycha0", &Ycha0, &b_Ycha0);
   fChain->SetBranchAddress("Xcha1", &Xcha1, &b_Xcha1);
   fChain->SetBranchAddress("Ycha1", &Ycha1, &b_Ycha1);
   fChain->SetBranchAddress("Xcha2", &Xcha2, &b_Xcha2);
   fChain->SetBranchAddress("Ycha2", &Ycha2, &b_Ycha2);
   fChain->SetBranchAddress("Ximp", &Ximp, &b_Ximp);
   fChain->SetBranchAddress("Yimp", &Yimp, &b_Yimp);
   fChain->SetBranchAddress("EvtC0", &EvtC0, &b_EvtC0);
   fChain->SetBranchAddress("BCIDC0", BCIDC0, &b_BCIDC0);
   fChain->SetBranchAddress("rodBCIDC0", &rodBCIDC0, &b_rodBCIDC0);
   fChain->SetBranchAddress("SizeC0", &SizeC0, &b_SizeC0);
   fChain->SetBranchAddress("DMUMaskC0", DMUMaskC0, &b_DMUMaskC0);
   fChain->SetBranchAddress("SlinkCRCC0", SlinkCRCC0, &b_SlinkCRCC0);
   fChain->SetBranchAddress("GainC0", GainC0, &b_GainC0);
   fChain->SetBranchAddress("ErrC0", ErrC0, &b_ErrC0);
   fChain->SetBranchAddress("SampleC0", SampleC0, &b_SampleC0);
   fChain->SetBranchAddress("feCRCC0", feCRCC0, &b_feCRCC0);
   fChain->SetBranchAddress("rodCRCC0", rodCRCC0, &b_rodCRCC0);
   fChain->SetBranchAddress("EfitC0", EfitC0, &b_EfitC0);
   fChain->SetBranchAddress("TfitC0", TfitC0, &b_TfitC0);
   fChain->SetBranchAddress("PedfitC0", PedfitC0, &b_PedfitC0);
   fChain->SetBranchAddress("Chi2C0", Chi2C0, &b_Chi2C0);
   fChain->SetBranchAddress("EvtC1", &EvtC1, &b_EvtC1);
   fChain->SetBranchAddress("BCIDC1", BCIDC1, &b_BCIDC1);
   fChain->SetBranchAddress("rodBCIDC1", &rodBCIDC1, &b_rodBCIDC1);
   fChain->SetBranchAddress("SizeC1", &SizeC1, &b_SizeC1);
   fChain->SetBranchAddress("DMUMaskC1", DMUMaskC1, &b_DMUMaskC1);
   fChain->SetBranchAddress("SlinkCRCC1", SlinkCRCC1, &b_SlinkCRCC1);
   fChain->SetBranchAddress("GainC1", GainC1, &b_GainC1);
   fChain->SetBranchAddress("ErrC1", ErrC1, &b_ErrC1);
   fChain->SetBranchAddress("SampleC1", SampleC1, &b_SampleC1);
   fChain->SetBranchAddress("feCRCC1", feCRCC1, &b_feCRCC1);
   fChain->SetBranchAddress("rodCRCC1", rodCRCC1, &b_rodCRCC1);
   fChain->SetBranchAddress("EfitC1", EfitC1, &b_EfitC1);
   fChain->SetBranchAddress("TfitC1", TfitC1, &b_TfitC1);
   fChain->SetBranchAddress("PedfitC1", PedfitC1, &b_PedfitC1);
   fChain->SetBranchAddress("Chi2C1", Chi2C1, &b_Chi2C1);
   fChain->SetBranchAddress("EvtC2", &EvtC2, &b_EvtC2);
   fChain->SetBranchAddress("BCIDC2", BCIDC2, &b_BCIDC2);
   fChain->SetBranchAddress("rodBCIDC2", &rodBCIDC2, &b_rodBCIDC2);
   fChain->SetBranchAddress("SizeC2", &SizeC2, &b_SizeC2);
   fChain->SetBranchAddress("DMUMaskC2", DMUMaskC2, &b_DMUMaskC2);
   fChain->SetBranchAddress("SlinkCRCC2", SlinkCRCC2, &b_SlinkCRCC2);
   fChain->SetBranchAddress("GainC2", GainC2, &b_GainC2);
   fChain->SetBranchAddress("ErrC2", ErrC2, &b_ErrC2);
   fChain->SetBranchAddress("SampleC2", SampleC2, &b_SampleC2);
   fChain->SetBranchAddress("feCRCC2", feCRCC2, &b_feCRCC2);
   fChain->SetBranchAddress("rodCRCC2", rodCRCC2, &b_rodCRCC2);
   fChain->SetBranchAddress("EfitC2", EfitC2, &b_EfitC2);
   fChain->SetBranchAddress("TfitC2", TfitC2, &b_TfitC2);
   fChain->SetBranchAddress("PedfitC2", PedfitC2, &b_PedfitC2);
   fChain->SetBranchAddress("Chi2C2", Chi2C2, &b_Chi2C2);
   fChain->SetBranchAddress("EvtA0", &EvtA0, &b_EvtA0);
   fChain->SetBranchAddress("BCIDA0", BCIDA0, &b_BCIDA0);
   fChain->SetBranchAddress("rodBCIDA0", &rodBCIDA0, &b_rodBCIDA0);
   fChain->SetBranchAddress("SizeA0", &SizeA0, &b_SizeA0);
   fChain->SetBranchAddress("DMUMaskA0", DMUMaskA0, &b_DMUMaskA0);
   fChain->SetBranchAddress("SlinkCRCA0", SlinkCRCA0, &b_SlinkCRCA0);
   fChain->SetBranchAddress("GainA0", GainA0, &b_GainA0);
   fChain->SetBranchAddress("ErrA0", ErrA0, &b_ErrA0);
   fChain->SetBranchAddress("SampleA0", SampleA0, &b_SampleA0);
   fChain->SetBranchAddress("feCRCA0", feCRCA0, &b_feCRCA0);
   fChain->SetBranchAddress("rodCRCA0", rodCRCA0, &b_rodCRCA0);
   fChain->SetBranchAddress("EfitA0", EfitA0, &b_EfitA0);
   fChain->SetBranchAddress("TfitA0", TfitA0, &b_TfitA0);
   fChain->SetBranchAddress("PedfitA0", PedfitA0, &b_PedfitA0);
   fChain->SetBranchAddress("Chi2A0", Chi2A0, &b_Chi2A0);
   fChain->SetBranchAddress("EvtA1", &EvtA1, &b_EvtA1);
   fChain->SetBranchAddress("BCIDA1", BCIDA1, &b_BCIDA1);
   fChain->SetBranchAddress("rodBCIDA1", &rodBCIDA1, &b_rodBCIDA1);
   fChain->SetBranchAddress("SizeA1", &SizeA1, &b_SizeA1);
   fChain->SetBranchAddress("DMUMaskA1", DMUMaskA1, &b_DMUMaskA1);
   fChain->SetBranchAddress("SlinkCRCA1", SlinkCRCA1, &b_SlinkCRCA1);
   fChain->SetBranchAddress("GainA1", GainA1, &b_GainA1);
   fChain->SetBranchAddress("ErrA1", ErrA1, &b_ErrA1);
   fChain->SetBranchAddress("SampleA1", SampleA1, &b_SampleA1);
   fChain->SetBranchAddress("feCRCA1", feCRCA1, &b_feCRCA1);
   fChain->SetBranchAddress("rodCRCA1", rodCRCA1, &b_rodCRCA1);
   fChain->SetBranchAddress("EfitA1", EfitA1, &b_EfitA1);
   fChain->SetBranchAddress("TfitA1", TfitA1, &b_TfitA1);
   fChain->SetBranchAddress("PedfitA1", PedfitA1, &b_PedfitA1);
   fChain->SetBranchAddress("Chi2A1", Chi2A1, &b_Chi2A1);
   fChain->SetBranchAddress("EvtA2", &EvtA2, &b_EvtA2);
   fChain->SetBranchAddress("BCIDA2", BCIDA2, &b_BCIDA2);
   fChain->SetBranchAddress("rodBCIDA2", &rodBCIDA2, &b_rodBCIDA2);
   fChain->SetBranchAddress("SizeA2", &SizeA2, &b_SizeA2);
   fChain->SetBranchAddress("DMUMaskA2", DMUMaskA2, &b_DMUMaskA2);
   fChain->SetBranchAddress("SlinkCRCA2", SlinkCRCA2, &b_SlinkCRCA2);
   fChain->SetBranchAddress("GainA2", GainA2, &b_GainA2);
   fChain->SetBranchAddress("ErrA2", ErrA2, &b_ErrA2);
   fChain->SetBranchAddress("SampleA2", SampleA2, &b_SampleA2);
   fChain->SetBranchAddress("feCRCA2", feCRCA2, &b_feCRCA2);
   fChain->SetBranchAddress("rodCRCA2", rodCRCA2, &b_rodCRCA2);
   fChain->SetBranchAddress("EfitA2", EfitA2, &b_EfitA2);
   fChain->SetBranchAddress("TfitA2", TfitA2, &b_TfitA2);
   fChain->SetBranchAddress("PedfitA2", PedfitA2, &b_PedfitA2);
   fChain->SetBranchAddress("Chi2A2", Chi2A2, &b_Chi2A2);
   fChain->SetBranchAddress("LarSmp", LarSmp, &b_LarSmp);
   fChain->SetBranchAddress("BarSmp", BarSmp, &b_BarSmp);
   fChain->SetBranchAddress("ExtSmp", ExtSmp, &b_ExtSmp);
   fChain->SetBranchAddress("GapSmp", GapSmp, &b_GapSmp);
   Notify();
}

Bool_t TileRec_h1000::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TileRec_h1000::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TileRec_h1000::Cut(Long64_t /*entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.

   return 1;
}




void TileRec_h1000::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L TileRec_h1000.C
//      Root > TileRec_h1000 t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      fChain->GetEntry(jentry);
      // if (Cut(ientry) < 0) continue;
   }
}

int TileRec_h1000::Loop(int evtToFind)
{
   if (fChain == 0) return -1;

   Long64_t nentries = fChain->GetEntriesFast();

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      fChain->GetEntry(jentry);

      if(Evt == evtToFind)
        return int(jentry);
   }
   return -1;
}
