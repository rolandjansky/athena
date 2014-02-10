/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LayerValidationTree.h"
#include <TH2.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

LayerValidationTree::LayerValidationTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("InDetMaterialG4-ATLAS-DC3-06.root");
      if (!f) {
         f = new TFile("InDetMaterialG4-ATLAS-DC3-06.root");
      }
      tree = (TTree*)gDirectory->Get("LayerValidation_100000500");

   }
   Init(tree);
}

LayerValidationTree::~LayerValidationTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t LayerValidationTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t LayerValidationTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() != TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void LayerValidationTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses of the tree
   // will be set. It is normaly not necessary to make changes to the
   // generated code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running with PROOF.

   // Set branch addresses
   if (tree == 0) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("DensedHitR",&DensedHitR);
   fChain->SetBranchAddress("DensedHitZ",&DensedHitZ);
   fChain->SetBranchAddress("DensedHitPhi",&DensedHitPhi);
   fChain->SetBranchAddress("DensedHitTheta",&DensedHitTheta);
   fChain->SetBranchAddress("CorrFactor",&CorrFactor);
   fChain->SetBranchAddress("PathInX0",&PathInX0);
   fChain->SetBranchAddress("Rho",&Rho);
   fChain->SetBranchAddress("Hits",&Hits);
   fChain->SetBranchAddress("HitPositionX",HitPositionX);
   fChain->SetBranchAddress("HitPositionY",HitPositionY);
   fChain->SetBranchAddress("HitPositionZ",HitPositionZ);
   fChain->SetBranchAddress("HitPositionR",HitPositionR);


   Notify();
}

Bool_t LayerValidationTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. Typically here the branch pointers
   // will be retrieved. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed.

   // Get branch pointers
   b_densedR = fChain->GetBranch("DensedHitR");
   b_densedZ = fChain->GetBranch("DensedHitZ");
   b_densedHitPhi = fChain->GetBranch("DensedHitPhi");
   b_dense = fChain->GetBranch("DensedHitTheta");
   b_corrF = fChain->GetBranch("CorrFactor");
   b_pathInX0 = fChain->GetBranch("PathInX0");
   b_rho = fChain->GetBranch("Rho");
   b_layerhits = fChain->GetBranch("Hits");
   b_HitPositionX = fChain->GetBranch("HitPositionX");
   b_HitPositionY = fChain->GetBranch("HitPositionY");
   b_HitPositionZ = fChain->GetBranch("HitPositionZ");
   b_HitPositionR = fChain->GetBranch("HitPositionR");

   return kTRUE;
}

void LayerValidationTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t LayerValidationTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


void LayerValidationTree::Loop(bool cylinderLayer, TString& ext)
{
   //   In a ROOT session, you can do:
   if (fChain == 0) return;

   // get the name of the Tree
   TString treeName = fChain->GetName();
   TString treeTitle = fChain->GetTitle();

   std::cout << " [m] Processing Tree: " << treeName << " / " << treeTitle << std::endl;

   // canvas name to be constructed
   TString canvasName  = "drawCanvas_";
           canvasName += treeName;
   // canvas title to be constructed		   
   TString canvasTitle  = "Validation Canvas";
           canvasTitle += treeName;
   // create the draw canvas
   TCanvas* drawCanvas = new TCanvas(canvasName,canvasTitle,0,0,900,920);
     drawCanvas->Divide(3,3);
     drawCanvas->cd(1);
   // get the max values in z / r 
   if (!cylinderLayer)
     fChain->Draw("DensedHitR>>hmax");
   else 
     fChain->Draw("DensedHitZ>>hmax");   	 
	 TH1F* maxHisto = (TH1F*)gDirectory->Get("hmax");
	 double xMin = maxHisto ? maxHisto->GetXaxis()->GetXmin() : 0.;
	 double xMax = maxHisto ? maxHisto->GetXaxis()->GetXmax() : 0.;	

	 
   // create the Profiles to be filled
   // (1) the Profile x0 vs (z || r)
   TString tVSrzProfileName  = "thicknessInX0";
   if (cylinderLayer)
           tVSrzProfileName += "_vs_Z_";
	else
           tVSrzProfileName += "_vs_R_";		   
           tVSrzProfileName += treeName;
   TString tVSrzProfileTitle            = "t/X_0 vs. ";
   if (cylinderLayer)
           tVSrzProfileTitle += "z, ";
	else
           tVSrzProfileTitle += "r, ";		   
           tVSrzProfileTitle += treeName;
   TProfile* tVSrzProfile = new TProfile(tVSrzProfileName, tVSrzProfileTitle,100,xMin,xMax);
   // (2) the Profile x0 vs phi           
   TString tVSphiProfileName  = "thicknessInX0";
           tVSphiProfileName += "_vs_Phi_";
	       tVSphiProfileName += treeName;
   TString tVSphiProfileTitle            = "t/X_0 vs. ";
           tVSphiProfileTitle += "#phi, ";
           tVSphiProfileTitle += treeName;
   TProfile* tVSphiProfile = new TProfile(tVSphiProfileName, tVSphiProfileTitle,100,-3.20,3.20);
   // (3) the 2D Profile x0 vs. phi, (z || r) 
   TString tVSrzphiProfileName  = "thicknessInX0";
           tVSrzphiProfileName += "_vs_Phi_vs_";
    if (cylinderLayer)
           tVSrzphiProfileName += "_Z_";		   
	else
           tVSrzphiProfileName += "_R_";
		   tVSrzphiProfileName += treeName;
   TString tVSrzphiProfileTitle            = "t/X_0 vs. ";
    if (cylinderLayer)   
           tVSrzphiProfileTitle += "( #phi, z ), ";
    else
           tVSrzphiProfileTitle += "( #phi, z ), ";
		   tVSrzphiProfileTitle += treeName;
   TProfile2D* tVSrzphiProfile = new TProfile2D(tVSrzphiProfileName, tVSrzphiProfileTitle,100,-3.20,3.20,100,xMin,xMax);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   
   std::cout << " [m] Starting the loop over " <<  nentries << " entries." << std::endl;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
	  
    if (CorrFactor>0.){
	   double correctedPX0 = PathInX0/CorrFactor;
     // if (Cut(ientry) < 0) continue;
	   double rZ = (cylinderLayer) ? DensedHitZ : DensedHitR;
	   // (1) the r/z vs. x0
	   tVSrzProfile->Fill(rZ,correctedPX0);
     // (2) the phi vs. x0
	   tVSphiProfile->Fill(DensedHitPhi, correctedPX0);
	   // (3) the (phi,rz) vs. x0
	   tVSrzphiProfile->Fill(DensedHitPhi,rZ,correctedPX0); 	  
		}
   }
   
   // for memory cleanup
   TH2F* drawHist1 = 0;
   TH2F* drawHist2 = 0;

   std::cout << " [m] Loop finished, starting to draw the histograms." << std::endl;

   // now draw the stuff
   // 1st row vs rz ======================================== 
   drawCanvas->cd(1);
   std::cout << " [m] -> Plot (1) - Hitpositions R" << std::endl;
        fChain->Draw("HitPositionR>>hitposr");
        TH1F* hitposr = (TH1F*)gDirectory->Get("hitposr");
        fChain->Draw("DensedHitR>>layerr", "CorrFactor>0.");
        TH1F* layerr = (TH1F*)gDirectory->Get("layerr");
        if(layerr) {
           layerr->SetLineColor(kRed);
           layerr->Draw();
        }
        if (hitposr) hitposr->Draw("same");
   drawCanvas->cd(2);
   std::cout << " [m] -> Plot (2) - PathInX0/CorrFactor R/Z" << std::endl;
    if (cylinderLayer)
        fChain->Draw("PathInX0/CorrFactor:DensedHitZ>>th1", "CorrFactor>=1.");
    else 
        fChain->Draw("PathInX0/CorrFactor:DensedHitR>>th1", "CorrFactor>=1.");     
   drawHist1 = (TH2F*)gDirectory->Get("th1");
   drawCanvas->cd(3);
   std::cout << " [m] -> Plot (3) - t/X0 vs R/Z" << std::endl;
        tVSrzProfile->Draw();
   // 2nd row vs phi =======================================
   drawCanvas->cd(4);
   std::cout << " [m] -> Plot (4) - Hitpositions R" << std::endl;
        fChain->Draw("HitPositionZ>>hitposz");
        TH1F* hitposz = (TH1F*)gDirectory->Get("hitposz");
        fChain->Draw("DensedHitZ>>layerz", "CorrFactor>0.");
        TH1F* layerz = (TH1F*)gDirectory->Get("layerz");
        if (layerz) {
          layerz->SetLineColor(kRed);
          layerz->Draw();
        }
        if (hitposz) hitposz->Draw("same");
   drawCanvas->cd(5);
   std::cout << " [m] -> Plot (5) - PathInX0/CorrFactor Phi"  << std::endl;
        fChain->Draw("PathInX0/CorrFactor:DensedHitPhi>>th2", "CorrFactor>0.");
   drawHist2 = (TH2F*)gDirectory->Get("th2");
   drawCanvas->cd(6);
   std::cout << " [m] -> Plot (6) - PathInX0/CorrFactor Phi - profile"  << std::endl;
        tVSphiProfile->Draw();
   // 3rd row : a 3dim attempt =============================
   drawCanvas->cd(7);
   std::cout << " [m] -> Plot (7) - Number of hits on the layer"  << std::endl;
        fChain->Draw("Hits>>layerhits", "CorrFactor>=1.");
        TH1F* layerhits = (TH1F*)gDirectory->Get("layerhits");
   drawCanvas->cd(8);
   std::cout << " [m] -> Plot (8) - PathInX0/CorrFactor loc1/loc2 - contour"  << std::endl;
        tVSrzphiProfile->Draw("Surf2");
   drawCanvas->cd(9);
   std::cout << " [m] -> Plot (9) - PathInX0/CorrFactor loc1/loc2 - contour"  << std::endl;
        tVSrzphiProfile->Draw("Cont4");

   std::cout << " [m] Drawing done, prepare the file output." << std::endl;

   // dedicde the draw name, draw it and then kill it
   TString saveName = "Output/";
   if (treeTitle.Contains("Pixel"))
       saveName += "InDet/Pixel/";
   else if (treeTitle.Contains("SCT"))
       saveName += "InDet/SCT/";
   else if (treeTitle.Contains("TRT"))
       saveName += "InDet/TRT/";
   else if (treeTitle.Contains("BeamPipe"))
       saveName += "InDet/BeamPipe/";
   else if (treeTitle.Contains("Solenoid"))
       saveName += "Calo/Solenoid/";
   else if (treeTitle.Contains("LAr"))
       saveName += "Calo/LAr/";
   else if (treeTitle.Contains("Tile"))
       saveName += "Calo/Tile/";


   if (treeTitle.Contains("Barrel"))
       saveName += "Barrel/";
   else if (treeTitle.Contains("PositiveEndcap"))
       saveName += "PosEndcap/";
   else if (treeTitle.Contains("NegativeEndcap"))
       saveName += "NegEndcap/";
   else if (treeTitle.Contains("GapVolumes"))
       saveName += "GapVolumes/";
   else 
      saveName += "Other/";

   saveName += treeName;
   saveName += ".";
   saveName += ext;
   // save the canvas
   drawCanvas->SaveAs(saveName);

    std::cout << " [m] Saving done, starting memory cleanup. " << std::endl;
   // memory cleanup
   delete maxHisto; maxHisto = 0; 
   delete hitposz; hitposz = 0;
   delete hitposr; hitposr = 0;
   delete layerr; layerr = 0;
   delete layerz; layerz = 0;
   delete layerhits; layerhits = 0;
   delete tVSrzProfile; tVSrzProfile = 0;
      // (2) the phi vs. x0
   delete tVSphiProfile; tVSphiProfile = 0;
    // (3) the (phi,rz) vs. x0
   delete tVSrzphiProfile; tVSrzphiProfile = 0;
   // delete the tree draw hists
   delete drawHist1; drawHist1 = 0;
   delete drawHist2; drawHist2 = 0;

   // delete the canvas
   delete drawCanvas; drawCanvas = 0;
   
}
