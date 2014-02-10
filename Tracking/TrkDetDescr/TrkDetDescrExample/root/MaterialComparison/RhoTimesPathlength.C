/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Draw the density*pathlength seen for EnergyLoss calculation

void RhoTimesPathlength(TString& fileOne, 
                        TString& fileTwo,
                        TString& detector,
                        int bins = 30,
                        double rhoCut=0.00005,
                        TString additionalCut="")
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
   plain->SetTitleColor(1);
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




  // get the files - one
  TFile* oneFile = TFile::Open(fileOne);
  TTree* oneTree = (TTree*)gDirectory->Get("MaterialTree");

  // get the files - two
  TFile* twoFile = TFile::Open(fileTwo);
  TTree* twoTree = (TTree*)gDirectory->Get("MaterialTree");

  TString drawString  = detector;
          drawString += "Rho*";
          drawString += detector;
          drawString += "Path";
  
  TString cutString   = detector;
          cutString  += "Rho>";
          cutString  += rhoCut;
          cutString  += additionalCut;

  TCanvas* drawCanvas = new TCanvas("drawCanvas", detector, 100, 100, 300, 320);

     // string for the range histograms
     TString drawStringRange = drawString;
             drawStringRange += ">>range";        
    

    // get the min max
     oneTree->Draw(drawStringRange,cutString,"", 25000, 0);
     TH1F* rangeHist = (TH1F*)gDirectory->Get("range");
     double minX = rangeHist ? rangeHist->GetXaxis()->GetXmin() : 0.;
     double maxX = rangeHist ? rangeHist->GetXaxis()->GetXmax() : 100.;

     // draw the trees
     TString drawStringOne = drawString;
             drawStringOne += ">>histOne(";
             drawStringOne += bins;
             drawStringOne += ",";
             drawStringOne += minX;
             drawStringOne += ",";
             drawStringOne += maxX;
             drawStringOne += ")";


     TString drawStringTwo = drawString;
             drawStringTwo += ">>histTwo(";
             drawStringTwo += bins;
             drawStringTwo += ",";
             drawStringTwo += minX;
             drawStringTwo += ",";
             drawStringTwo += maxX;
             drawStringTwo += ")";

     // draw the histograms
     oneTree->Draw(drawStringOne,cutString,"", 25000, 0);
     twoTree->Draw(drawStringTwo,cutString,"same", 25000, 0);
     // get the histograms
     TH1F* histOne = (TH1F*)gDirectory->Get("histOne");
     TH1F* histTwo = (TH1F*)gDirectory->Get("histTwo");

     std::cout << " [m] Entries in Histogram One : " << histOne->GetEntries() << std::endl;
     std::cout << " [m] Entries in Histogram Two : " << histTwo->GetEntries() << std::endl;

 



}