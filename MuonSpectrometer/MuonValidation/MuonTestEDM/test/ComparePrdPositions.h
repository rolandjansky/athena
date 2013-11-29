/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Sep  7 11:59:45 2009 by ROOT version 5.22/00d
// from TTree ComparePrdPositions/Muon RPC Hits output
// found on file: ./WriteESDfromRDO.root
//////////////////////////////////////////////////////////

#ifndef ComparePrdPositions_h
#define ComparePrdPositions_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>
#include <iostream>

class ComparePrdPositions {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   TTree          *fChain2;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           RPC_Id;
   Double_t        RPC_LocalX;
   Double_t        RPC_LocalY;
   Double_t        RPC_HitX;
   Double_t        RPC_HitY;
   Double_t        RPC_HitZ;
   Double_t        RPC_HitR;
   Char_t          RPC_StationName[4];

   // Declaration of leaf types
   Int_t           RPC_Id2;
   Double_t        RPC_LocalX2;
   Double_t        RPC_LocalY2;
   Double_t        RPC_HitX2;
   Double_t        RPC_HitY2;
   Double_t        RPC_HitZ2;
   Double_t        RPC_HitR2;
   Char_t          RPC_StationName2[4];


   // List of branches
   TBranch        *b_RPC_Id;
   TBranch        *b_RPC_LocalX;   //!
   TBranch        *b_RPC_LocalY;   //!
   TBranch        *b_RPC_HitX;   //!
   TBranch        *b_RPC_HitY;   //!
   TBranch        *b_RPC_HitZ;   //!
   TBranch        *b_RPC_HitR;   //!
   TBranch        *b_RPC_StationName;   //!

   // List of branches
   TBranch        *b_RPC_Id2;
   TBranch        *b_RPC_LocalX2;   //!
   TBranch        *b_RPC_LocalY2;   //!
   TBranch        *b_RPC_HitX2;   //!
   TBranch        *b_RPC_HitY2;   //!
   TBranch        *b_RPC_HitZ2;   //!
   TBranch        *b_RPC_HitR2;   //!
   TBranch        *b_RPC_StationName2;   //!

   ComparePrdPositions(/**TTree *tree=0, TTree *tree2=0,*/ std::string dir="RPC", std::string oldFile="./WriteESDfromRDO.root", std::string newFile="./ReadESD.root");
   virtual ~ComparePrdPositions();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree1(Long64_t entry);
   virtual Long64_t LoadTree2(Long64_t entry);
   virtual void     Init(TTree *tree, TTree *tree2);
   virtual bool     Loop( bool checkLocal, bool checkGlobal);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ComparePrdPositions_cxx
ComparePrdPositions::ComparePrdPositions(/**TTree *tree, TTree *tree2,*/ std::string dir, std::string oldFile, std::string newFile)
{
    // for dir, normal options are 'CSC', 'MDT', 'RPC', 'RPCCoin', 'TGC'
    TTree* tree=0;
    TTree* tree2=0;
    
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(oldFile.c_str());
      if (!f) {
         f = new TFile(oldFile.c_str());
         std::string temp = oldFile;
         temp+=dir;
         f->cd(dir.c_str());
      }
      std::string dir2 = dir;
      dir2+="prds";
      
      tree = (TTree*)gDirectory->Get(dir2.c_str());

      TFile *f2 = (TFile*)gROOT->GetListOfFiles()->FindObject(newFile.c_str());
      if (!f2) {
          std::string temp = newFile;
          temp+=dir;
         f2 = new TFile(newFile.c_str());
         f2->cd(dir.c_str());
      }
      tree2 = (TTree*)gDirectory->Get(dir2.c_str());

   }
   Init(tree, tree2);
}

ComparePrdPositions::~ComparePrdPositions()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ComparePrdPositions::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ComparePrdPositions::LoadTree1(Long64_t entry)
{

    // std::cout<<"Load Tree1"<<std::endl;

// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   

   if (centry < 0) return centry;
   // if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   // TChain *chain = (TChain*)fChain;
   // if (chain->GetTreeNumber() != fCurrent) {
   //    fCurrent = chain->GetTreeNumber();
   //    Notify();
   // }
   
   return centry;
}

Long64_t ComparePrdPositions::LoadTree2(Long64_t entry)
{
    // std::cout<<"Load Tree2"<<std::endl;
   if (!fChain2) return -5;
   Long64_t centry = fChain2->LoadTree(entry);
   if (centry < 0) return centry;
   // if (!fChain2->InheritsFrom(TChain::Class()))  return centry;
   // TChain *chain2 = (TChain*)fChain;
   // if (chain2->GetTreeNumber() != fCurrent) {
   //    fCurrent = chain2->GetTreeNumber();
   //    Notify();
   // }
   
   return centry;
}
void ComparePrdPositions::Init(TTree *tree, TTree *tree2)
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

   fChain->SetBranchAddress("Id",   &RPC_Id,   &b_RPC_Id);
   fChain->SetBranchAddress("LocalX", &RPC_LocalX, &b_RPC_LocalX);
   fChain->SetBranchAddress("LocalY", &RPC_LocalY, &b_RPC_LocalY);
   fChain->SetBranchAddress("HitX", &RPC_HitX, &b_RPC_HitX);
   fChain->SetBranchAddress("HitY", &RPC_HitY, &b_RPC_HitY);
   fChain->SetBranchAddress("HitZ", &RPC_HitZ, &b_RPC_HitZ);
   fChain->SetBranchAddress("HitR", &RPC_HitR, &b_RPC_HitR);
   fChain->SetBranchAddress("StationName", RPC_StationName, &b_RPC_StationName);
   
   fChain2 = tree2;
   fChain2->SetMakeClass(1);
         
   fChain2->SetBranchAddress("Id",   &RPC_Id2,   &b_RPC_Id2);
   fChain2->SetBranchAddress("LocalX", &RPC_LocalX2, &b_RPC_LocalX2);
   fChain2->SetBranchAddress("LocalY", &RPC_LocalY2, &b_RPC_LocalY2);
   fChain2->SetBranchAddress("HitX", &RPC_HitX2, &b_RPC_HitX2);
   fChain2->SetBranchAddress("HitY", &RPC_HitY2, &b_RPC_HitY2);
   fChain2->SetBranchAddress("HitZ", &RPC_HitZ2, &b_RPC_HitZ2);
   fChain2->SetBranchAddress("HitR", &RPC_HitR2, &b_RPC_HitR2);
   fChain2->SetBranchAddress("StationName", RPC_StationName2, &b_RPC_StationName2);
   
   Notify();
}

Bool_t ComparePrdPositions::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ComparePrdPositions::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   std::cout<<"1:\n"<<std::endl;
   fChain->Show(entry);
   std::cout<<"2:\n"<<std::endl;
   fChain2->Show(entry);
}
Int_t ComparePrdPositions::Cut(Long64_t /**entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ComparePrdPositions_cxx
