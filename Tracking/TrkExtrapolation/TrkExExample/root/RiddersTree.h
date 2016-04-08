/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May 31 13:59:15 2007 by ROOT version 5.15/06
// from TTree RiddersTree/Output of the RiddersAlgorithm
// found on file: RiddersAlgorithm_Sl.root
//////////////////////////////////////////////////////////

#ifndef RiddersTree_h
#define RiddersTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>

class RiddersTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           RiddersSteps;
   Float_t         Loc1Loc1[9];   //[steps]
   Float_t         Loc1Loc2[9];   //[steps]
   Float_t         Loc1Phi[9];   //[steps]
   Float_t         Loc1Theta[9];   //[steps]
   Float_t         Loc1qOp[9];   //[steps]
   Float_t         Loc1Steps[9];   //[steps]
   Float_t         Loc2Loc1[9];   //[steps]
   Float_t         Loc2Loc2[9];   //[steps]
   Float_t         Loc2Phi[9];   //[steps]
   Float_t         Loc2Theta[9];   //[steps]
   Float_t         Loc2qOp[9];   //[steps]
   Float_t         Loc2Steps[9];   //[steps]
   Float_t         PhiLoc1[9];   //[steps]
   Float_t         PhiLoc2[9];   //[steps]
   Float_t         PhiPhi[9];   //[steps]
   Float_t         PhiTheta[9];   //[steps]
   Float_t         PhiqOp[9];   //[steps]
   Float_t         PhiSteps[9];   //[steps]
   Float_t         ThetaLoc1[9];   //[steps]
   Float_t         ThetaLoc2[9];   //[steps]
   Float_t         ThetaPhi[9];   //[steps]
   Float_t         ThetaTheta[9];   //[steps]
   Float_t         ThetaqOp[9];   //[steps]
   Float_t         ThetaSteps[9];   //[steps]
   Float_t         QopLoc1[9];   //[steps]
   Float_t         QopLoc2[9];   //[steps]
   Float_t         QopPhi[9];   //[steps]
   Float_t         QopTheta[9];   //[steps]
   Float_t         QopqOp[9];   //[steps]
   Float_t         QopSteps[9];   //[steps]

   // List of branches
   TBranch        *b_steps;   //!
   TBranch        *b_Loc1Loc1;   //!
   TBranch        *b_Loc1Loc2;   //!
   TBranch        *b_Loc1Phi;   //!
   TBranch        *b_Loc1Theta;   //!
   TBranch        *b_Loc1qOp;   //!
   TBranch        *b_Loc1Steps;   //!
   TBranch        *b_Loc2Loc1;   //!
   TBranch        *b_Loc2Loc2;   //!
   TBranch        *b_Loc2Phi;   //!
   TBranch        *b_Loc2Theta;   //!
   TBranch        *b_Loc2qOp;   //!
   TBranch        *b_Loc2Steps;   //!
   TBranch        *b_PhiLoc1;   //!
   TBranch        *b_PhiLoc2;   //!
   TBranch        *b_PhiPhi;   //!
   TBranch        *b_PhiTheta;   //!
   TBranch        *b_PhiqOp;   //!
   TBranch        *b_PhiSteps;   //!
   TBranch        *b_ThetaLoc1;   //!
   TBranch        *b_ThetaLoc2;   //!
   TBranch        *b_ThetaPhi;   //!
   TBranch        *b_ThetaTheta;   //!
   TBranch        *b_ThetaqOp;   //!
   TBranch        *b_ThetaSteps;   //!
   TBranch        *b_QopLoc1;   //!
   TBranch        *b_QopLoc2;   //!
   TBranch        *b_QopPhi;   //!
   TBranch        *b_QopTheta;   //!
   TBranch        *b_QopqOp;   //!
   TBranch        *b_QopSteps;   //!

   RiddersTree(TString& fileName, TString& treeName);
   virtual ~RiddersTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual TH1F*    Loop(TString& histName, int bins, int low, int high, int entries=0,
                         bool loc1=true, bool loc2=true, bool phi=true, bool theta=true, bool qop=true, bool relAbs=true);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

