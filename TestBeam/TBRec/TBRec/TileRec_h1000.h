/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb  3 16:02:52 2010 by ROOT version 5.25/04
// from TTree h1000/TileBEAM-Ntuple
// found on file: cbnt_RecExTB_Combined_1207_2102095.0.root
//////////////////////////////////////////////////////////

#ifndef TileRec_h1000_h
#define TileRec_h1000_h

#include "CLIDSvc/CLASS_DEF.h"

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TString.h"

class TileRec_h1000 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           EvTime;
   Int_t           Run;
   Int_t           Evt;
   Int_t           Trig;
   Int_t           L1ID[7];
   Int_t           L1Type[7];
   Int_t           EvType[7];
   Int_t           EvBCID[7];
   Int_t           FrBCID[6];
   Float_t         MuBackHit;
   Float_t         MuBackSum;
   Float_t         MuBack[14];
   Float_t         MuCalib[2];
   Float_t         Ecal[8];
   Int_t           LasFlag;
   Float_t         Las0;
   Float_t         Las1;
   Float_t         Las2;
   Float_t         Las3;
   Float_t         LasExtra[4];
   Int_t           Cispar[16];
   Int_t           S1cou;
   Int_t           S2cou;
   Int_t           S3cou;
   Int_t           MuTag;
   Int_t           Cher1;
   Int_t           Cher2;
   Int_t           MuHalo;
   Int_t           MuVeto;
   Int_t           S2extra;
   Int_t           S3extra;
   Int_t           SC1;
   Int_t           SC2;
   Int_t           pu;
   Int_t           tof[8];
   Int_t           btdc1[16];
   Int_t           btdc2[16];
   Float_t         XchN2;
   Float_t         YchN2;
   Float_t         XchN1;
   Float_t         YchN1;
   Float_t         Xcha0;
   Float_t         Ycha0;
   Float_t         Xcha1;
   Float_t         Ycha1;
   Float_t         Xcha2;
   Float_t         Ycha2;
   Float_t         Ximp;
   Float_t         Yimp;
   Int_t           EvtC0;
   Int_t           BCIDC0[16];
   Int_t           rodBCIDC0;
   Int_t           SizeC0;
   Int_t           DMUMaskC0[2];
   Int_t           SlinkCRCC0[2];
   Int_t           GainC0[48];
   Int_t           ErrC0[48];
   Int_t           SampleC0[48][9];
   Int_t           feCRCC0[16];
   Int_t           rodCRCC0[16];
   Float_t         EfitC0[48];
   Float_t         TfitC0[48];
   Float_t         PedfitC0[48];
   Float_t         Chi2C0[48];
   Int_t           EvtC1;
   Int_t           BCIDC1[16];
   Int_t           rodBCIDC1;
   Int_t           SizeC1;
   Int_t           DMUMaskC1[2];
   Int_t           SlinkCRCC1[2];
   Int_t           GainC1[48];
   Int_t           ErrC1[48];
   Int_t           SampleC1[48][9];
   Int_t           feCRCC1[16];
   Int_t           rodCRCC1[16];
   Float_t         EfitC1[48];
   Float_t         TfitC1[48];
   Float_t         PedfitC1[48];
   Float_t         Chi2C1[48];
   Int_t           EvtC2;
   Int_t           BCIDC2[16];
   Int_t           rodBCIDC2;
   Int_t           SizeC2;
   Int_t           DMUMaskC2[2];
   Int_t           SlinkCRCC2[2];
   Int_t           GainC2[48];
   Int_t           ErrC2[48];
   Int_t           SampleC2[48][9];
   Int_t           feCRCC2[16];
   Int_t           rodCRCC2[16];
   Float_t         EfitC2[48];
   Float_t         TfitC2[48];
   Float_t         PedfitC2[48];
   Float_t         Chi2C2[48];
   Int_t           EvtA0;
   Int_t           BCIDA0[16];
   Int_t           rodBCIDA0;
   Int_t           SizeA0;
   Int_t           DMUMaskA0[2];
   Int_t           SlinkCRCA0[2];
   Int_t           GainA0[48];
   Int_t           ErrA0[48];
   Int_t           SampleA0[48][9];
   Int_t           feCRCA0[16];
   Int_t           rodCRCA0[16];
   Float_t         EfitA0[48];
   Float_t         TfitA0[48];
   Float_t         PedfitA0[48];
   Float_t         Chi2A0[48];
   Int_t           EvtA1;
   Int_t           BCIDA1[16];
   Int_t           rodBCIDA1;
   Int_t           SizeA1;
   Int_t           DMUMaskA1[2];
   Int_t           SlinkCRCA1[2];
   Int_t           GainA1[48];
   Int_t           ErrA1[48];
   Int_t           SampleA1[48][9];
   Int_t           feCRCA1[16];
   Int_t           rodCRCA1[16];
   Float_t         EfitA1[48];
   Float_t         TfitA1[48];
   Float_t         PedfitA1[48];
   Float_t         Chi2A1[48];
   Int_t           EvtA2;
   Int_t           BCIDA2[16];
   Int_t           rodBCIDA2;
   Int_t           SizeA2;
   Int_t           DMUMaskA2[2];
   Int_t           SlinkCRCA2[2];
   Int_t           GainA2[48];
   Int_t           ErrA2[48];
   Int_t           SampleA2[48][9];
   Int_t           feCRCA2[16];
   Int_t           rodCRCA2[16];
   Float_t         EfitA2[48];
   Float_t         TfitA2[48];
   Float_t         PedfitA2[48];
   Float_t         Chi2A2[48];
   Float_t         LarSmp[4];
   Float_t         BarSmp[3];
   Float_t         ExtSmp[3];
   Float_t         GapSmp[3];

   // List of branches
   TBranch        *b_EvTime;   //!
   TBranch        *b_Run;   //!
   TBranch        *b_Evt;   //!
   TBranch        *b_Trig;   //!
   TBranch        *b_L1ID;   //!
   TBranch        *b_L1Type;   //!
   TBranch        *b_EvType;   //!
   TBranch        *b_EvBCID;   //!
   TBranch        *b_FrBCID;   //!
   TBranch        *b_MuBackHit;   //!
   TBranch        *b_MuBackSum;   //!
   TBranch        *b_MuBack;   //!
   TBranch        *b_MuCalib;   //!
   TBranch        *b_Ecal;   //!
   TBranch        *b_LasFlag;   //!
   TBranch        *b_Las0;   //!
   TBranch        *b_Las1;   //!
   TBranch        *b_Las2;   //!
   TBranch        *b_Las3;   //!
   TBranch        *b_LasExtra;   //!
   TBranch        *b_Cispar;   //!
   TBranch        *b_S1cou;   //!
   TBranch        *b_S2cou;   //!
   TBranch        *b_S3cou;   //!
   TBranch        *b_MuTag;   //!
   TBranch        *b_Cher1;   //!
   TBranch        *b_Cher2;   //!
   TBranch        *b_MuHalo;   //!
   TBranch        *b_MuVeto;   //!
   TBranch        *b_S2extra;   //!
   TBranch        *b_S3extra;   //!
   TBranch        *b_SC1;   //!
   TBranch        *b_SC2;   //!
   TBranch        *b_pu;   //!
   TBranch        *b_tof;   //!
   TBranch        *b_btdc1;   //!
   TBranch        *b_btdc2;   //!
   TBranch        *b_XchN2;   //!
   TBranch        *b_YchN2;   //!
   TBranch        *b_XchN1;   //!
   TBranch        *b_YchN1;   //!
   TBranch        *b_Xcha0;   //!
   TBranch        *b_Ycha0;   //!
   TBranch        *b_Xcha1;   //!
   TBranch        *b_Ycha1;   //!
   TBranch        *b_Xcha2;   //!
   TBranch        *b_Ycha2;   //!
   TBranch        *b_Ximp;   //!
   TBranch        *b_Yimp;   //!
   TBranch        *b_EvtC0;   //!
   TBranch        *b_BCIDC0;   //!
   TBranch        *b_rodBCIDC0;   //!
   TBranch        *b_SizeC0;   //!
   TBranch        *b_DMUMaskC0;   //!
   TBranch        *b_SlinkCRCC0;   //!
   TBranch        *b_GainC0;   //!
   TBranch        *b_ErrC0;   //!
   TBranch        *b_SampleC0;   //!
   TBranch        *b_feCRCC0;   //!
   TBranch        *b_rodCRCC0;   //!
   TBranch        *b_EfitC0;   //!
   TBranch        *b_TfitC0;   //!
   TBranch        *b_PedfitC0;   //!
   TBranch        *b_Chi2C0;   //!
   TBranch        *b_EvtC1;   //!
   TBranch        *b_BCIDC1;   //!
   TBranch        *b_rodBCIDC1;   //!
   TBranch        *b_SizeC1;   //!
   TBranch        *b_DMUMaskC1;   //!
   TBranch        *b_SlinkCRCC1;   //!
   TBranch        *b_GainC1;   //!
   TBranch        *b_ErrC1;   //!
   TBranch        *b_SampleC1;   //!
   TBranch        *b_feCRCC1;   //!
   TBranch        *b_rodCRCC1;   //!
   TBranch        *b_EfitC1;   //!
   TBranch        *b_TfitC1;   //!
   TBranch        *b_PedfitC1;   //!
   TBranch        *b_Chi2C1;   //!
   TBranch        *b_EvtC2;   //!
   TBranch        *b_BCIDC2;   //!
   TBranch        *b_rodBCIDC2;   //!
   TBranch        *b_SizeC2;   //!
   TBranch        *b_DMUMaskC2;   //!
   TBranch        *b_SlinkCRCC2;   //!
   TBranch        *b_GainC2;   //!
   TBranch        *b_ErrC2;   //!
   TBranch        *b_SampleC2;   //!
   TBranch        *b_feCRCC2;   //!
   TBranch        *b_rodCRCC2;   //!
   TBranch        *b_EfitC2;   //!
   TBranch        *b_TfitC2;   //!
   TBranch        *b_PedfitC2;   //!
   TBranch        *b_Chi2C2;   //!
   TBranch        *b_EvtA0;   //!
   TBranch        *b_BCIDA0;   //!
   TBranch        *b_rodBCIDA0;   //!
   TBranch        *b_SizeA0;   //!
   TBranch        *b_DMUMaskA0;   //!
   TBranch        *b_SlinkCRCA0;   //!
   TBranch        *b_GainA0;   //!
   TBranch        *b_ErrA0;   //!
   TBranch        *b_SampleA0;   //!
   TBranch        *b_feCRCA0;   //!
   TBranch        *b_rodCRCA0;   //!
   TBranch        *b_EfitA0;   //!
   TBranch        *b_TfitA0;   //!
   TBranch        *b_PedfitA0;   //!
   TBranch        *b_Chi2A0;   //!
   TBranch        *b_EvtA1;   //!
   TBranch        *b_BCIDA1;   //!
   TBranch        *b_rodBCIDA1;   //!
   TBranch        *b_SizeA1;   //!
   TBranch        *b_DMUMaskA1;   //!
   TBranch        *b_SlinkCRCA1;   //!
   TBranch        *b_GainA1;   //!
   TBranch        *b_ErrA1;   //!
   TBranch        *b_SampleA1;   //!
   TBranch        *b_feCRCA1;   //!
   TBranch        *b_rodCRCA1;   //!
   TBranch        *b_EfitA1;   //!
   TBranch        *b_TfitA1;   //!
   TBranch        *b_PedfitA1;   //!
   TBranch        *b_Chi2A1;   //!
   TBranch        *b_EvtA2;   //!
   TBranch        *b_BCIDA2;   //!
   TBranch        *b_rodBCIDA2;   //!
   TBranch        *b_SizeA2;   //!
   TBranch        *b_DMUMaskA2;   //!
   TBranch        *b_SlinkCRCA2;   //!
   TBranch        *b_GainA2;   //!
   TBranch        *b_ErrA2;   //!
   TBranch        *b_SampleA2;   //!
   TBranch        *b_feCRCA2;   //!
   TBranch        *b_rodCRCA2;   //!
   TBranch        *b_EfitA2;   //!
   TBranch        *b_TfitA2;   //!
   TBranch        *b_PedfitA2;   //!
   TBranch        *b_Chi2A2;   //!
   TBranch        *b_LarSmp;   //!
   TBranch        *b_BarSmp;   //!
   TBranch        *b_ExtSmp;   //!
   TBranch        *b_GapSmp;   //!

   TileRec_h1000(TTree *tree=0);
   virtual ~TileRec_h1000();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual int      Loop(int evtToFind);	 
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

// generated by:
// $ clid -m "TileRec_h1000"
CLASS_DEF( TileRec_h1000 , 234937385 , 1 )

#endif
