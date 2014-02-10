/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CylinderLayerMaterial.h"
#include "DiscLayerMaterial.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TString.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TKey.h"
#include "TLine.h"
#include "TH1D.h"

#include <vector>
#include <iostream>




int main(int argc, char **argv)
{
    TApplication theApp("App", &argc, argv);

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
   plain->SetOptStat(0);
   // Labelling
   plain->SetLabelFont(132, "XY");
   plain->SetLabelSize(0.05, "XY");

   gROOT->SetStyle("Plain");

// =======================================================/

//    int bins = 0;

    if (argc<2) {

      std::cout << " [e] Not enough arguments given ! " << std::endl;
      std::cout << " [i] Sample : " << std::endl;
      std::cout << "     (for file1.root pdf) : LayerValidation file1 1" << std::endl;

    } else {

      TString file1       =  argv[1];
              file1      += ".root";

      TString picExtension ="";
      if (argc<3) {
          std::cout << " [w] no saving type specified, using gif." << std::endl;
          picExtension = ".gif";
      } else {
          picExtension = argv[2];  
      }

      TString titleContains ="";

      if (argc>3) {
        titleContains = argv[3];
        std::cout << " [m] only print Layers in : " << titleContains << std::endl; 
      }

      std::cout << " [m] Input file 1:    " << file1 << std::endl;


      TFile* oneFile = TFile::Open(file1);
      TDirectory* oneDir = oneFile ? (TDirectory*)oneFile->Get("LayerMaterialInspector") : 0;

      if (oneDir){
       std::cout << " [m] File loading successful ! " << std::endl;
        

           TIter next(oneDir->GetListOfKeys());
           TKey* key = 0;
           while ((key=(TKey*)next())){
              TString treeName  = key->GetName();
              TString treeTitle = key->GetTitle();
              std::cout << " [m] Processing Tree : " << treeName << " (" << treeTitle << ")" << std::endl;
              // only go on if it is a layer validation Tree
              if (treeName.Contains("Layer_") && treeTitle.Contains(titleContains)){
                  // get the TTree
                  TTree* currentTree = (TTree*)oneDir->Get(treeName);
                  if (currentTree){
                    // now decide whether its a cylinder tree or not
                    if (treeName.Contains("Cylinder")){
                     // construct the LayerValidationTree
                      CylinderLayerMaterial* cylinderMaterial = new CylinderLayerMaterial(currentTree);
                      cylinderMaterial->Loop();
                    } else {
                     // construct the LayerValidationTree
                      DiscLayerMaterial* discMaterial = new DiscLayerMaterial(currentTree);
                      discMaterial->Loop();
                   }
                }
              }
            }
      
      }




    } // not enough arguments given

   theApp.Run();
   return(0);
}
