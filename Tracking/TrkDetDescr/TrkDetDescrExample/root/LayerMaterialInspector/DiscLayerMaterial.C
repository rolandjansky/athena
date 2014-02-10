/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define DiscLayerMaterial_cxx

#include "DiscLayerMaterial.h"
#include <TMath.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

void DiscLayerMaterial::Loop()
{

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;


     // ------------------------------------------ LayerMaterial section ------------------------------------------
     TString canvasNameLayer  = "Canvas_LayerMaterial_";
             canvasNameLayer += m_name;

     bool doReference = bool(LayerReferenceBinsTotal); 
     int width = doReference ? 800 : 400;
     int div   = doReference ? 2 : 1;
     int plot  = doReference ? 3 : 2;

     TCanvas* drawCanvasLayer = new TCanvas(canvasNameLayer, "Layer Material Description of this File", 100, 100, width, 700);
     drawCanvasLayer->Divide(div,2); 


      TString discMaterialHistNameX0  = "Hist_x0_";
              discMaterialHistNameX0 += m_name;

      TString discMaterialHistNameZARho  = "Hist_zarho_";
              discMaterialHistNameZARho += m_name;

      // ---------------------------------------------------------------------------------------------------------

      TH2F* discMaterialX0 
       = new TH2F(discMaterialHistNameX0, "Disc Material - t/X0", LayerMaterialBinsR,LayerInnerRadius,LayerOuterRadius,
                                                                 LayerMaterialBinsPhi,-TMath::Pi(), TMath::Pi());

      TH2F* discMaterialZARho 
       = new TH2F(discMaterialHistNameZARho, "Disc Material - Z/A #rho", LayerMaterialBinsR,LayerInnerRadius,LayerOuterRadius,
                                                                 LayerMaterialBinsPhi,-TMath::Pi(), TMath::Pi());

       std::cout << " [h] Histogram created with bins in z : " << LayerMaterialBinsR << std::endl;
       std::cout << "                               in phi: " << LayerMaterialBinsPhi << std::endl;

       // -------------------------------------------------------------------------------------------------------
       Float_t rStep   = (LayerOuterRadius-LayerInnerRadius)/LayerMaterialBinsR;
       std::cout << " [s] z-step determined as   : " << rStep << std::endl;
       Float_t phiStep = 2.*TMath::Pi()/LayerMaterialBinsPhi;
       std::cout << " [s] phi-step determined as : " << phiStep << std::endl;

       // fill this tree 
       for (Int_t ibin = 0; ibin<LayerMaterialBinsTotal; ++ibin)
       {
         // ---------------------------------------------------------
         Int_t zBin           = ibin-int(ibin/LayerMaterialBinsR)*LayerMaterialBinsR;
         Int_t phiBin         = int(ibin/LayerMaterialBinsR);

         // std::cout << "[b] bin determined as [z,phi] = " << zBin << ", " << phiBin << std::endl;

         Float_t rCurr   = LayerInnerRadius  + (zBin+0.5) * rStep;
         Float_t phiCurr = -TMath::Pi() + (phiBin+0.5) * phiStep;
      
         discMaterialX0->Fill(rCurr,phiCurr,LayerMaterialX0[ibin]);
         discMaterialZARho->Fill(rCurr,phiCurr,LayerMaterialZARho[ibin]);
         
       }
      drawCanvasLayer->cd(1);
      discMaterialX0->Draw("lego2");
      drawCanvasLayer->cd(plot);
      discMaterialZARho->Draw("lego2");




     // ------------------------------------------ ReferenceMaterial section ------------------------------------------
     if (LayerReferenceBinsTotal){


        TString discRefMaterialHistNameX0  = "Reference_Hist_x0_";
                discRefMaterialHistNameX0 += m_name;

        TString discRefMaterialHistNameZARho  = "Reference_Hist_zarho_";
                discRefMaterialHistNameZARho += m_name;


        // ---------------------------------------------------------------------------------------------------------
 
        TH2F* referemceMaterialX0 
         = new TH2F(discRefMaterialHistNameX0, "Reference Material - t/X0", LayerReferenceBinsX,-LayerReferenceMaxHalfX,LayerReferenceMaxHalfX,
                                                                 LayerReferenceBinsY,-LayerReferenceHalfY, LayerReferenceHalfY);
 
        TH2F* referenceMaterialZARho 
         = new TH2F(discRefMaterialHistNameZARho, "Referece Material - Z/A #rho", LayerReferenceBinsX,-LayerReferenceMaxHalfX,LayerReferenceMaxHalfX,
                                                                 LayerReferenceBinsY,-LayerReferenceHalfY, LayerReferenceHalfY);

         std::cout << " [h] Histogram (reference material) created with bins in x : " << LayerReferenceBinsX << std::endl;
         std::cout << "                                                     in y : " << LayerReferenceBinsY << std::endl;

         // -------------------------------------------------------------------------------------------------------
         Float_t locXstep   = 2.*LayerReferenceMaxHalfX/LayerReferenceBinsX;
         std::cout << " [s] locX-step determined as   : " << locXstep << std::endl;
         Float_t locYstep   = 2.*LayerReferenceHalfY/LayerReferenceBinsY;
         std::cout << " [s] locY-step determined as   : " << locYstep << std::endl;

         // fill this tree 
         for (Int_t ibin = 0; ibin<LayerReferenceBinsTotal; ++ibin)
         {
           // ---------------------------------------------------------
           Int_t locXbin    = ibin - int(ibin/LayerReferenceBinsX)*LayerReferenceBinsX;
           Int_t locYbin    = int(ibin/LayerReferenceBinsX);

           // std::cout << "[b] bin determined as [locX,locY] = " << locXbin << ", " << locYbin << std::endl;

           Float_t locXcurr   = -LayerReferenceMaxHalfX  + (locXbin+0.5) * locXstep;
           Float_t locYcurr   = -LayerReferenceHalfY  + (locYbin+0.5) * locYstep;
      

           referemceMaterialX0->Fill(locXcurr,locYcurr,LayerReferenceX0[ibin]);
           referenceMaterialZARho->Fill(locXcurr,locYcurr,LayerReferenceZARho[ibin]);
         
         }
        drawCanvasLayer->cd(2);
        referemceMaterialX0->Draw("col");
        drawCanvasLayer->cd(4);
        referenceMaterialZARho->Draw("col");



      }
     // ------------------------------------------ ReferenceMaterial section ------------------------------------------

      TString saveString  = "LayerMaterial_";
              saveString +=m_name;
              saveString +=".gif";

      drawCanvasLayer->SaveAs(saveString);
     


   }
}

DiscLayerMaterial::DiscLayerMaterial(TTree *tree)
{

  std::cout << " [c] Creating DiscMaterial class " << std::endl;

// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("TrackingGeometryTest.root");
      if (!f) {
         f = new TFile("TrackingGeometryTest.root");
         f->cd("TrackingGeometryTest.root:/LayerMaterialInspector");
      }
      tree = (TTree*)gDirectory->Get("DiscLayerMaterial");

   }
   // --------------------
   m_name = tree->GetName();

   std::cout << " [c] Name of the tree set as : " << m_name << std::endl;

   Init(tree);

}

DiscLayerMaterial::~DiscLayerMaterial()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t DiscLayerMaterial::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t DiscLayerMaterial::LoadTree(Long64_t entry)
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

void DiscLayerMaterial::Init(TTree *tree)
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

   fChain->SetBranchAddress("LayerIndex", &LayerIndex, &b_discLayerIndex);
   fChain->SetBranchAddress("LayerInnerRadius", &LayerInnerRadius, &b_discInnerR);
   fChain->SetBranchAddress("LayerOuterRadius", &LayerOuterRadius, &b_discOuterR);
   fChain->SetBranchAddress("LayerPositionZ", &LayerPositionZ, &b_discPositionZ);
   fChain->SetBranchAddress("LayerMaterialBinsR", &LayerMaterialBinsR, &b_discMatBinsR);
   fChain->SetBranchAddress("LayerMaterialBinsPhi", &LayerMaterialBinsPhi, &b_discMatBinsPhi);
   fChain->SetBranchAddress("LayerMaterialBinsTotal", &LayerMaterialBinsTotal, &b_discMatBinsTotal);
   fChain->SetBranchAddress("LayerMaterialX0", LayerMaterialX0, &b_LayerMaterialX0);
   fChain->SetBranchAddress("LayerMaterialZARho", LayerMaterialZARho, &b_LayerMaterialZARho);
   fChain->SetBranchAddress("LayerReferenceMinHalfX", &LayerReferenceMinHalfX, &b_discRefHalfMinX);
   fChain->SetBranchAddress("LayerReferenceMaxHalfX", &LayerReferenceMaxHalfX, &b_discRefHalfMaxX);
   fChain->SetBranchAddress("LayerReferenceHalfY", &LayerReferenceHalfY, &b_discRefHalfY);
   fChain->SetBranchAddress("LayerReferenceBinsX", &LayerReferenceBinsX, &b_discRefBinsX);
   fChain->SetBranchAddress("LayerReferenceBinsY", &LayerReferenceBinsY, &b_discRefBinsY);
   fChain->SetBranchAddress("LayerReferenceBinsTotal", &LayerReferenceBinsTotal, &b_discRefBinsTotal);
   fChain->SetBranchAddress("LayerReferenceX0", LayerReferenceX0, &b_LayerReferenceX0);
   fChain->SetBranchAddress("LayerReferenceZARho", LayerReferenceZARho, &b_LayerReferenceZARho);
   Notify();
}

Bool_t DiscLayerMaterial::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void DiscLayerMaterial::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t DiscLayerMaterial::Cut(Long64_t)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

