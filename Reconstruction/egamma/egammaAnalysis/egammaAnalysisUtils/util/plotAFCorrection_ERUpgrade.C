/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaAnalysisUtils/EnergyRescaler.h"

#include <iostream>
#include <vector>

#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TROOT.h"

void plotAFCorrection_ERUpgrade(string year) {

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade ers;
   if( year=="2011" )
     ers.Init("EnergyRescalerData.root","2011","es2011a");
   else if( yea )
     ers.Init("EnergyRescalerData.root","2012","es2012");

   double etaMin = -5;
   double etaMax = +5;
   double etaStep = .01;
   double eta = etaMin;

   TGraph* gAF = new TGraph();
   TGraph* gFS = new TGraph();
   int i=0;

   while( eta<=etaMax ) {

     gAF->SetPoint( i, eta, ers.applyAFtoG4(eta) );
     gFS->SetPoint( i, eta, ers.applyFStoG4(eta) );

     i++;
     eta += etaStep;

   }
 
   TCanvas * canvas = new TCanvas();

   gAF->SetMaximum(1.05);
   gAF->SetMinimum(0.95);

   char grafname[99];
   sprintf(grafname,"AF -> G4 Correction factor, %s", year.c_str());
   gAF->SetTitle(grafname);
   
   gAF->SetLineWidth(2);
   gAF->SetLineStyle(2);
   gAF->SetLineColor(4);
   gAF->GetXaxis()->SetTitleOffset(1.25);
   gAF->GetYaxis()->SetTitleOffset(1.25);
   gAF->GetXaxis()->SetTitle("#eta");
   gAF->GetYaxis()->SetTitle("E_{G4}/E_{AF}");
   gAF->Draw("AL");
   
   gFS->SetLineWidth(2);
   gFS->SetLineStyle(2);
   gFS->SetLineColor(2);
   gFS->Draw("L");

   TLegend* leg = new TLegend(.55,.75,.89,.89);
   leg->SetBorderSize(0);
   leg->SetFillColor(0);
   leg->AddEntry(gAF, "AFII (|#eta|<2.5)", "l");
   leg->AddEntry(gFS, "Frozen showers (|#eta|>2.5)", "l");
   leg->Draw();

   char plotname[99];
   sprintf(plotname,"aftog4_%s.png", year.c_str());
   canvas->SaveAs(plotname);
   
}
