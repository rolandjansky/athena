/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define RiddersTree_cxx
#include "RiddersTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>

TH1F* RiddersTree::Loop(TString& histName, int bins, int low, int high, int entries,
                        bool loc1, bool loc2, bool phi, bool theta, bool qop, bool relAbs)
{
 

   //================ Trying out the style ===================/

   TStyle *plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");
   // Canvas
   plain->SetCanvasBorderMode(0);
   plain->SetCanvasColor(0);
   // Pad
   plain->SetPadBorderMode(0);
   plain->SetPadColor(0);
   // Title
   plain->SetTitleColor(0);
   plain->SetTitleBorderSize(0);
   plain->SetTitleX(0.25);
   plain->SetTitleY(0.98);
   // Stat
   //plain->SetStatColor(1);
   //plain->SetOptStat(0);
   // Labelling
   plain->SetLabelFont(132, "XY");
   plain->SetLabelSize(0.05, "XY");
   // Lines
   //plain->SetLineWidth(0.5);
   gROOT->SetStyle("Plain");

   // =======================================================/
   if (fChain == 0) return 0;

   Long64_t nentries = (!entries) ? fChain->GetEntriesFast() : entries;

   TH1F* relErrorLog = new TH1F(histName, "-log(#Delta C/C_{num})", bins, low,high);


   int draw = relAbs ? 8 : 6;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;


      if (RiddersSteps < 10){
         if (loc1 && Loc1Loc1[draw]>0.)   relErrorLog->Fill(Loc1Loc1[draw]);  
         if (loc1 && Loc1Loc2[draw]>0.)   relErrorLog->Fill(Loc1Loc2[draw]);  
         if (loc1 && Loc1Phi[draw]>0.)    relErrorLog->Fill(Loc1Phi[draw]);   
         if (loc1 && Loc1Theta[draw]>0.)  relErrorLog->Fill(Loc1Theta[draw]); 
         if (loc1 && Loc1qOp[draw]>0.)    relErrorLog->Fill(Loc1qOp[draw]);   
         if (loc2 && Loc2Loc1[draw]>0.)   relErrorLog->Fill(Loc2Loc1[draw]);  
         if (loc2 && Loc2Loc2[draw]>0.)   relErrorLog->Fill(Loc2Loc2[draw]);  
         if (loc2 && Loc2Phi[draw]>0.)    relErrorLog->Fill(Loc2Phi[draw]);   
         if (loc2 && Loc2Theta[draw]>0.)  relErrorLog->Fill(Loc2Theta[draw]); 
         if (loc2 && Loc2qOp[draw]>0.)    relErrorLog->Fill(Loc2qOp[draw]);   
         if (phi && PhiLoc1[draw]>0.)    relErrorLog->Fill(PhiLoc1[draw]);   
         if (phi && PhiLoc2[draw]>0.)    relErrorLog->Fill(PhiLoc2[draw]);   
         if (phi && PhiPhi[draw]>0.)     relErrorLog->Fill(PhiPhi[draw]);   
         if (phi && PhiTheta[draw]>0.)   relErrorLog->Fill(PhiTheta[draw]);  
         if (phi && PhiqOp[draw]>0.)     relErrorLog->Fill(PhiqOp[draw]);   
         if (theta && ThetaLoc1[draw]>0.)  relErrorLog->Fill(ThetaLoc1[draw]); 
         if (theta && ThetaLoc2[draw]>0.)  relErrorLog->Fill(ThetaLoc2[draw]); 
         if (theta && ThetaPhi[draw]>0.)   relErrorLog->Fill(ThetaPhi[draw]);  
         if (theta && ThetaTheta[draw]>0.) relErrorLog->Fill(ThetaTheta[draw]);
         if (theta && ThetaqOp[draw]>0.)   relErrorLog->Fill(ThetaqOp[draw]);  
         if (qop && QopLoc1[draw]>0.)    relErrorLog->Fill(QopLoc1[draw]);   
         if (qop && QopLoc2[draw]>0.)    relErrorLog->Fill(QopLoc2[draw]);   
         if (qop && QopPhi[draw]>0.)     relErrorLog->Fill(QopPhi[draw]);   
         if (qop && QopTheta[draw]>0.)   relErrorLog->Fill(QopTheta[draw]);  
         if (qop && QopqOp[draw]>0.)     relErrorLog->Fill(QopqOp[draw]);   
    }
   }
   return relErrorLog;
}

RiddersTree::RiddersTree(TString& fileName, TString& treeName)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   TFile* file = TFile::Open(fileName);
   file = file ? file : (TFile*)gROOT->GetListOfFiles()->FindObject(fileName);
   TTree* tree = file ? (TTree*)file->Get(treeName) : 0;

   if (tree) {
    Init(tree);
      std::cout << "[m] Tree " << tree->GetName() << " sucessfully initialized!" << std::endl;
    } else 
      std::cout << "[m] Tree could not be loaded. Loop() method not safe!" << std::endl;

}

RiddersTree::~RiddersTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t RiddersTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RiddersTree::LoadTree(Long64_t entry)
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

void RiddersTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normaly not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RiddersSteps", &RiddersSteps, &b_steps);
   fChain->SetBranchAddress("Loc1Loc1", Loc1Loc1, &b_Loc1Loc1);
   fChain->SetBranchAddress("Loc1Loc2", Loc1Loc2, &b_Loc1Loc2);
   fChain->SetBranchAddress("Loc1Phi", Loc1Phi, &b_Loc1Phi);
   fChain->SetBranchAddress("Loc1Theta", Loc1Theta, &b_Loc1Theta);
   fChain->SetBranchAddress("Loc1qOp", Loc1qOp, &b_Loc1qOp);
   fChain->SetBranchAddress("Loc1Steps", Loc1Steps, &b_Loc1Steps);
   fChain->SetBranchAddress("Loc2Loc1", Loc2Loc1, &b_Loc2Loc1);
   fChain->SetBranchAddress("Loc2Loc2", Loc2Loc2, &b_Loc2Loc2);
   fChain->SetBranchAddress("Loc2Phi", Loc2Phi, &b_Loc2Phi);
   fChain->SetBranchAddress("Loc2Theta", Loc2Theta, &b_Loc2Theta);
   fChain->SetBranchAddress("Loc2qOp", Loc2qOp, &b_Loc2qOp);
   fChain->SetBranchAddress("Loc2Steps", Loc2Steps, &b_Loc2Steps);
   fChain->SetBranchAddress("PhiLoc1", PhiLoc1, &b_PhiLoc1);
   fChain->SetBranchAddress("PhiLoc2", PhiLoc2, &b_PhiLoc2);
   fChain->SetBranchAddress("PhiPhi", PhiPhi, &b_PhiPhi);
   fChain->SetBranchAddress("PhiTheta", PhiTheta, &b_PhiTheta);
   fChain->SetBranchAddress("PhiqOp", PhiqOp, &b_PhiqOp);
   fChain->SetBranchAddress("PhiSteps", PhiSteps, &b_PhiSteps);
   fChain->SetBranchAddress("ThetaLoc1", ThetaLoc1, &b_ThetaLoc1);
   fChain->SetBranchAddress("ThetaLoc2", ThetaLoc2, &b_ThetaLoc2);
   fChain->SetBranchAddress("ThetaPhi", ThetaPhi, &b_ThetaPhi);
   fChain->SetBranchAddress("ThetaTheta", ThetaTheta, &b_ThetaTheta);
   fChain->SetBranchAddress("ThetaqOp", ThetaqOp, &b_ThetaqOp);
   fChain->SetBranchAddress("ThetaSteps", ThetaSteps, &b_ThetaSteps);
   fChain->SetBranchAddress("QopLoc1", QopLoc1, &b_QopLoc1);
   fChain->SetBranchAddress("QopLoc2", QopLoc2, &b_QopLoc2);
   fChain->SetBranchAddress("QopPhi", QopPhi, &b_QopPhi);
   fChain->SetBranchAddress("QopTheta", QopTheta, &b_QopTheta);
   fChain->SetBranchAddress("QopqOp", QopqOp, &b_QopqOp);
   fChain->SetBranchAddress("QopSteps", QopSteps, &b_QopSteps);
   Notify();
}

Bool_t RiddersTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void RiddersTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t RiddersTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
