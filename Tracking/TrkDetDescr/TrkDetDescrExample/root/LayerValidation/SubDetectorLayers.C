/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LayerValidationTree.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TString.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TKey.h"
#include "TLine.h"
#include "TH1D.h"
#include "TGraph.h"

#include <vector>
#include <iostream>




void SubDetectorLayers(TString& fileName, 
                       TString& subdet, 
                       double zmin, 
                       double zmax,
                       double rmin,
                       double rmax,
                       TString& saveAs,
                       int overallPlotWidth  = 600,
                       int overallPlotHeight = 300,
                       bool singlePlot=false)
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
   plain->SetOptStat(0);
   // Labelling
   plain->SetLabelFont(132, "XY");
   plain->SetLabelSize(0.05, "XY");

   gROOT->SetStyle("Plain");

   // array of 50 to be safe
   TTree* subdetTrees[200];
   TH2F*  barrelRzHists[200];
   int numSubdetTrees = 0;

// =======================================================/



   TFile* oneFile = TFile::Open(fileName);
   if (oneFile){
       std::cout << " [m] File loading successful ! " << std::endl;
        
           TIter next(oneFile->GetListOfKeys());
           TKey* key = 0;
           while (key=(TKey*)next()){
              TString treeName  = key->GetName();
              TString treeTitle = key->GetTitle();
              // only go on if it is a layer validation Tree
              if (treeTitle.Contains(subdet) && treeName.Contains("Layer")){
                  // screen output
                  std::cout << " [m] Got the Tree : " << treeName << " | " << treeTitle << std::endl;
                  // get the TTree
                  TTree* currentTree = (TTree*)oneFile->Get(treeName);
                  if (currentTree){
                      subdetTrees[numSubdetTrees] = currentTree;
                      numSubdetTrees++;
                }
              }
            }
      
      }


   // get two or three rows depending on the number of trees
   int dividerOne = numSubdetTrees/9 + 2;
       dividerOne = singlePlot ? 1 : dividerOne;
   

   int dividedPlusOne = numSubdetTrees/dividerOne+1;
       dividedPlusOne = singlePlot ? 2 : dividedPlusOne;   

   TCanvas* drawCanvas = new TCanvas(subdet, subdet, 100, 100, 250*dividedPlusOne,dividerOne*250+20);
            drawCanvas->Divide(dividedPlusOne, dividerOne);
  
   TCanvas* overallCanvas = new TCanvas("OverallPlot", subdet, 150, 150, overallPlotWidth,overallPlotHeight);
         
   TH2F* drawPlot = new TH2F("emptyDrawPlot", subdet, 500, zmin,zmax, 500, rmin, rmax);
         overallCanvas->cd();
         drawPlot->Draw();

   TString histString = "htmp_";

   int drawn = 0;

   for (int ilayer = 0; ilayer < numSubdetTrees; ++ilayer){
        TTree* currentTree = subdetTrees[ilayer];
        if (currentTree){

            ++drawn;
            int fill = singlePlot ? 1 : drawn;
            drawCanvas->cd(fill);
          
              std::cout << " [m] Drawing: " << currentTree->GetName() << std::endl;

              // ROOT's stupid string manipulations
              TString currentHistString  = histString;
                      currentHistString += ilayer;
              TString drawString = "HitPositionR:HitPositionZ";
//                      drawString += currentHistString;
              
              currentTree->Draw(drawString);
              TGraph* htmp = (TGraph*)gPad->GetPrimitive("Graph");
              htmp->SetMarkerColor(15+ilayer*3);
              htmp->Draw("p");

          overallCanvas->cd();
              htmp->Draw("p, same");      
        } 
   }
   if (saveAs.Contains("gif") || saveAs.Contains("pdf") || saveAs.Contains("eps")) 
     overallCanvas->SaveAs(saveAs);


} 

