/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define SurfaceValidationTree_cxx
#include "SurfaceValidationTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TProfile.h>
#include <TProfile2D.h>






SurfaceValidationTree::SurfaceValidationTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("InDetMaterialG4-ATLAS-DC3-06.root");
      if (!f) {
         f = new TFile("InDetMaterialG4-ATLAS-DC3-06.root");
      }
      tree = (TTree*)gDirectory->Get("SurfaceValidation");

   }
   Init(tree);
}

SurfaceValidationTree::~SurfaceValidationTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SurfaceValidationTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SurfaceValidationTree::LoadTree(Long64_t entry)
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

void SurfaceValidationTree::Init(TTree *tree)
{

   // Set branch addresses
   if (tree == 0) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Local1",&Local1);
   fChain->SetBranchAddress("Local2",&Local2);
   fChain->SetBranchAddress("CorrFactor",&CorrFactor);
   fChain->SetBranchAddress("PathInX0",&PathInX0);
   fChain->SetBranchAddress("Rho",&Rho);
   Notify();
}

Bool_t SurfaceValidationTree::Notify()
{

   // Get branch pointers
   b_local1 = fChain->GetBranch("Local1");
   b_local2 = fChain->GetBranch("Local2");
   b_corrF = fChain->GetBranch("CorrFactor");
   b_pathInX0 = fChain->GetBranch("PathInX0");
   b_rho = fChain->GetBranch("Rho");

   return kTRUE;
}

void SurfaceValidationTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SurfaceValidationTree::Cut(Long64_t entry)
{
   return 1;
}




void SurfaceValidationTree::Loop(int bins, TString& ext)
{

   if (fChain == 0) return;

   // get the name of the Tree
   TString treeName = fChain->GetName();
   TString treeTitle = fChain->GetTitle();

   // canvas name to be constructed
   TString canvasName  = "drawCanvas_";
           canvasName += treeName;
   // canvas title to be constructed       
   TString canvasTitle  = "Validation Canvas";
           canvasTitle += treeName;

   double minLoc1 = 0.;
   double maxLoc1 = 0.;
   double minLoc2 = 0.;
   double maxLoc2 = 0.;

   // create the draw canvas
   TCanvas* drawCanvas = new TCanvas(canvasName,canvasTitle,0,0,600,920);
     drawCanvas->Divide(2,3);
     drawCanvas->cd(1);
        // get the min/max values for histogram booking: loc1
        fChain->Draw("Local1>>hl1", "CorrFactor>0.");
        TH1F* hl1 = (TH1F*)gDirectory->Get("hl1");
        minLoc1 = hl1->GetXaxis()->GetXmin();
        maxLoc1 = hl1->GetXaxis()->GetXmax();
        delete hl1; hl1 = 0;
        // get the min/max vlues for histogram bookin: loc2
        fChain->Draw("Local2>>hl2", "CorrFactor>0.");
        TH1F* hl2 = (TH1F*)gDirectory->Get("hl2");
        minLoc2 = hl2->GetXaxis()->GetXmin();
        maxLoc2 = hl2->GetXaxis()->GetXmax();
        delete hl2; hl2 = 0;

   // Book the Profile histograms
     // (1) the Profile x0 vs loc1
   TString tVSl1ProfileName  = "thicknessInX0";
           tVSl1ProfileName += "_vs_loc1_";
           tVSl1ProfileName += treeName;
   TString tVSl1ProfileTitle            = "t/X_0 vs. ";
           tVSl1ProfileTitle += "loc1 ";
   TProfile* tVSl1Profile = new TProfile(tVSl1ProfileName, tVSl1ProfileTitle,bins,minLoc1,maxLoc1);

   // (1) the Profile x0 vs loc2
   TString tVSl2ProfileName  = "thicknessInX0";
           tVSl2ProfileName += "_vs_loc2_";
           tVSl2ProfileName += treeName;
   TString tVSl2ProfileTitle            = "t/X_0 vs. ";
           tVSl2ProfileTitle += "loc2 ";
   TProfile* tVSl2Profile = new TProfile(tVSl2ProfileName, tVSl2ProfileTitle,bins,minLoc2,maxLoc2);

   // (3) the 2D Profile x0 vs. phi, (z || r) 
   TString tVSl1l2ProfileName  = "thicknessInX0";
           tVSl1l2ProfileName += "_vs_loc1_vs_loc2";
           tVSl1l2ProfileName += treeName;
   TString tVSl1l2ProfileTitle            = "t/X_0 vs. ";
           tVSl1l2ProfileTitle += "( loc1, loc2 ), ";
           tVSl1l2ProfileTitle += treeName;
   TProfile2D* tVSl1l2Profile = new TProfile2D(tVSl1l2ProfileName, 
                                               tVSl1l2ProfileTitle,
                                               bins,minLoc1,maxLoc1,
                                               bins,minLoc2,maxLoc2);

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      if (CorrFactor<=0.) continue;
      tVSl1Profile->Fill(Local1,PathInX0/CorrFactor);
      tVSl2Profile->Fill(Local2,PathInX0/CorrFactor);
      tVSl1l2Profile->Fill(Local1,Local2,PathInX0/CorrFactor);
   }


   // for memory cleanup
   TH2F* drawHist1 = 0;
   TH2F* drawHist2 = 0;

   // now draw the historgrams
   // now draw the stuff
   // 1st row vs loc1 
   drawCanvas->cd(1);
        fChain->Draw("PathInX0/CorrFactor:Local1>>th1", "CorrFactor>0.");
   drawCanvas->cd(2);
        tVSl1Profile->Draw();
   // 2nd row vs loc2
   drawCanvas->cd(3);
        fChain->Draw("PathInX0/CorrFactor:Local2>>th2", "CorrFactor>0.");
   drawCanvas->cd(4);
        tVSl2Profile->Draw();
   // 3rd row vs (loc1,loc2)
   drawCanvas->cd(5);
        tVSl1l2Profile->Draw("Surf2");
   drawCanvas->cd(6);
        tVSl1l2Profile->Draw("Cont4");
 
   // dedicde the draw name, draw it and then kill it
   TString saveName = "Output/";
   if (treeTitle.Contains("Pixel"))
       saveName += "Pixel/";
   else if (treeTitle.Contains("SCT"))
       saveName += "SCT/";
   else if (treeTitle.Contains("TRT"))
       saveName += "TRT/";
   else if (treeTitle.Contains("BeamPipe"))
       saveName += "BeamPipe/";

   if (treeTitle.Contains("Barrel"))
       saveName += "Barrel/";
   else if (treeTitle.Contains("PositiveEndcap"))
       saveName += "PosEndcap/";
   else if (treeTitle.Contains("NegativeEndcap"))
       saveName += "NegEndcap/";
   else if (treeTitle.Contains("GapVolumes"))
       saveName += "GapVolumes/";

   saveName += treeName;
   saveName += ext;
   // save the canvas
   drawCanvas->SaveAs(saveName);

   // memory cleanup
   delete tVSl1Profile; tVSl1Profile = 0;
   delete tVSl2Profile; tVSl2Profile = 0;
   delete tVSl1l2Profile; tVSl1l2Profile = 0;

   drawHist1 = (TH2F*)gDirectory->Get("th1");
   drawHist2 = (TH2F*)gDirectory->Get("th2");
   delete drawHist1; drawHist1 = 0;
   delete drawHist2; drawHist2 = 0;


   // delete the canvas
   delete drawCanvas; drawCanvas = 0;

}
