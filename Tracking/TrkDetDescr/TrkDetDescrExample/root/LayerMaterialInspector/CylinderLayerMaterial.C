/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define CylinderLayerMaterial_cxx
#include "CylinderLayerMaterial.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include "TMath.h"

void CylinderLayerMaterial::Loop()
{


   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   

   Long64_t nbytes = 0, nb = 0;
   // --------------------------------------------------- LOOP over entires : there should be only one -----------
   for (Long64_t jentry=0; jentry<nentries;jentry++) {

      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

     bool doReference = bool(LayerReferenceBinsTotal); 
     int width = doReference ? 800 : 400;
     int div   = doReference ? 2 : 1;
     int plot  = doReference ? 3 : 2;

     // ------------------------------------------ LayerMaterial section ------------------------------------------
     TString canvasNameLayer  = "Canvas_LayerMaterial_";
             canvasNameLayer += m_name;

     TCanvas* drawCanvasLayer = new TCanvas(canvasNameLayer, "Layer Material Description of this File", 100, 100, width, 700);
     drawCanvasLayer->Divide(div,2); 


      TString cylinderMaterialHistNameX0  = "Hist_x0_";
              cylinderMaterialHistNameX0 += m_name;

      TString cylinderMaterialHistNameZARho  = "Hist_zarho_";
              cylinderMaterialHistNameZARho += m_name;


      // ---------------------------------------------------------------------------------------------------------

      TH2F* cylinderMaterialX0 
       = new TH2F(cylinderMaterialHistNameX0, "Cylinder Material - t/X0", LayerMaterialBinsZ,-LayerHalfZ,LayerHalfZ,
                                                                 LayerMaterialBinsPhi,-TMath::Pi(), TMath::Pi());

      TH2F* cylinderMaterialZARho 
       = new TH2F(cylinderMaterialHistNameZARho, "Cylinder Material - Z/A #rho", LayerMaterialBinsZ,-LayerHalfZ,LayerHalfZ,
                                                                 LayerMaterialBinsPhi,-TMath::Pi(), TMath::Pi());

       std::cout << "[h] Histogram created with bins in z : " << LayerMaterialBinsZ << std::endl;
       std::cout << "                               in phi: " << LayerMaterialBinsPhi << std::endl;

       // -------------------------------------------------------------------------------------------------------
       Float_t zStep   = 2.*LayerHalfZ/LayerMaterialBinsZ;
       std::cout << "[s] z-step determined as   : " << zStep << std::endl;
       Float_t phiStep = 2.*TMath::Pi()/LayerMaterialBinsPhi;
       std::cout << "[s] phi-step determined as : " << phiStep << std::endl;

       // fill this tree 
       for (Int_t ibin = 0; ibin<LayerMaterialBinsTotal; ++ibin)
       {
         // ---------------------------------------------------------
         Int_t zBin           = ibin-int(ibin/LayerMaterialBinsZ)*LayerMaterialBinsZ;
         Int_t phiBin         = int(ibin/LayerMaterialBinsZ);

         // std::cout << "[b] bin determined as [z,phi] = " << zBin << ", " << phiBin << std::endl;

         Float_t zCurr   = -LayerHalfZ  + (zBin+0.5) * zStep;
         Float_t phiCurr = -TMath::Pi() + (phiBin+0.5) * phiStep;
      
         cylinderMaterialX0->Fill(zCurr,phiCurr,LayerMaterialX0[ibin]);
         cylinderMaterialZARho->Fill(zCurr,phiCurr,LayerMaterialZARho[ibin]);
         
       }
      drawCanvasLayer->cd(1);
      cylinderMaterialX0->Draw("lego2");
      drawCanvasLayer->cd(plot);
      cylinderMaterialZARho->Draw("lego2");


     // ------------------------------------------ ReferenceMaterial section ------------------------------------------
     if (LayerReferenceBinsTotal){

        TString cylinderRefMaterialHistNameX0  = "Reference_Hist_x0_";
                cylinderRefMaterialHistNameX0 += m_name;

        TString cylinderRefMaterialHistNameZARho  = "Reference_Hist_zarho_";
                cylinderRefMaterialHistNameZARho += m_name;


        // ---------------------------------------------------------------------------------------------------------
 
        TH2F* referemceMaterialX0 
         = new TH2F(cylinderRefMaterialHistNameX0, "Reference Material - t/X0", LayerReferenceBinsX,-LayerReferenceHalfX,LayerReferenceHalfX,
                                                                 LayerReferenceBinsY,-LayerReferenceHalfY, LayerReferenceHalfY);
 
        TH2F* referenceMaterialZARho 
         = new TH2F(cylinderRefMaterialHistNameZARho, "Referece Material - Z/A #rho", LayerReferenceBinsX,-LayerReferenceHalfX,LayerReferenceHalfX,
                                                                 LayerReferenceBinsY,-LayerReferenceHalfY, LayerReferenceHalfY);

         std::cout << "[h] Histogram (reference material) created with bins in x : " << LayerReferenceBinsX << std::endl;
         std::cout << "                                                     in y : " << LayerReferenceBinsY << std::endl;

         // -------------------------------------------------------------------------------------------------------
         Float_t locXstep   = 2.*LayerReferenceHalfX/LayerReferenceBinsX;
         std::cout << "[s] locX-step determined as   : " << locXstep << std::endl;
         Float_t locYstep   = 2.*LayerReferenceHalfY/LayerReferenceBinsY;
         std::cout << "[s] locY-step determined as   : " << locYstep << std::endl;

         // fill this tree 
         for (Int_t ibin = 0; ibin<LayerReferenceBinsTotal; ++ibin)
         {
           // ---------------------------------------------------------
           Int_t locXbin    = ibin - int(ibin/LayerReferenceBinsX)*LayerReferenceBinsX;
           Int_t locYbin    = int(ibin/LayerReferenceBinsX);

           // std::cout << "[b] bin determined as [locX,locY] = " << locXbin << ", " << locYbin << std::endl;

           Float_t locXcurr   = -LayerReferenceHalfX  + (locXbin+0.5) * locXstep;
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

CylinderLayerMaterial::CylinderLayerMaterial(TTree *tree)
{

  std::cout << " [c] Creating CylinderMaterial class " << std::endl;
  // if parameter tree is not specified (or zero), connect the file
  // used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("TrackingGeometryTest.root");
      if (!f) {
         f = new TFile("TrackingGeometryTest.root");
         f->cd("TrackingGeometryTest.root:/LayerMaterialInspector");
      }
      tree = (TTree*)gDirectory->Get("CylinderLayerMaterial");

   }
   // --------------------
   m_name = tree->GetName();

   std::cout << " [c] Name of the tree set as : " << m_name << std::endl;
   Init(tree);
}

CylinderLayerMaterial::~CylinderLayerMaterial()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CylinderLayerMaterial::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CylinderLayerMaterial::LoadTree(Long64_t entry)
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

void CylinderLayerMaterial::Init(TTree *tree)
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

   fChain->SetBranchAddress("LayerIndex", &LayerIndex, &b_cylLayerIndex);
   fChain->SetBranchAddress("LayerRadius", &LayerRadius, &b_cylRadius);
   fChain->SetBranchAddress("LayerHalfZ", &LayerHalfZ, &b_cylHalfZ);
   fChain->SetBranchAddress("LayerMaterialBinsZ", &LayerMaterialBinsZ, &b_cylMatBinsZ);
   fChain->SetBranchAddress("LayerMaterialBinsPhi", &LayerMaterialBinsPhi, &b_cylMatBinsPhi);
   fChain->SetBranchAddress("LayerMaterialBinsTotal", &LayerMaterialBinsTotal, &b_cylMatBinsTotal);
   fChain->SetBranchAddress("LayerMaterialX0", LayerMaterialX0, &b_LayerMaterialX0);
   fChain->SetBranchAddress("LayerMaterialZARho", LayerMaterialZARho, &b_LayerMaterialZARho);
   fChain->SetBranchAddress("LayerReferenceHalfX", &LayerReferenceHalfX, &b_cylRefHalfX);
   fChain->SetBranchAddress("LayerReferenceHalfY", &LayerReferenceHalfY, &b_cylRefHalfY);
   fChain->SetBranchAddress("LayerReferenceBinsX", &LayerReferenceBinsX, &b_cylRefBinsX);
   fChain->SetBranchAddress("LayerReferenceBinsY", &LayerReferenceBinsY, &b_cylRefBinsY);
   fChain->SetBranchAddress("LayerReferenceBinsTotal", &LayerReferenceBinsTotal, &b_cylRefBinsTotal);
   fChain->SetBranchAddress("LayerReferenceX0", &LayerReferenceX0, &b_LayerReferenceX0);
   fChain->SetBranchAddress("LayerReferenceZARho", &LayerReferenceZARho, &b_LayerReferenceZARho);
   Notify();
}

Bool_t CylinderLayerMaterial::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CylinderLayerMaterial::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t CylinderLayerMaterial::Cut(Long64_t)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
