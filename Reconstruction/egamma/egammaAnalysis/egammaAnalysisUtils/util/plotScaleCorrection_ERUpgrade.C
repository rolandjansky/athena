/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// energy : energy (NOT transverse), in GeV
// etamin, etamax : from cluster
// ptype : 0 (electron), 1 (unconverted photon), 2 (converted photon)
// year : "2011" or "2012", Z scales

TGraph* plotScaleCorrection_ERUpgrade(double Energy /* GeV */, double etamin, double etamax, string year, string esmodel) {

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade  ers;
   ers.Init("EnergyRescalerData.root",year,esmodel);
   ers.SetDebugFlag(true);

   double GeV = 1000.;
   double energy = Energy*GeV;

   double etaMin = etamin;
   double etaMax = etamax;
   double etaStep = .01;
   double eta = etaMin;

   TGraph* gAF = new TGraph();
   int i=0;

   while( eta<=etaMax ) {

     double ecorr = ers.applyEnergyCorrection(eta, energy,
					      egRescaler::EnergyRescalerUpgrade::Nominal,  
					      egRescaler::EnergyRescalerUpgrade::Electron);

     gAF->SetPoint( i, eta, energy/ecorr - 1. );

     i++;
     eta += etaStep;

   }
 
   TCanvas * canvas = new TCanvas();

   gAF->SetMaximum(.05);
   gAF->SetMinimum(-.05);

   char grafname[99];
   sprintf(grafname,"Data calibration factor, %s, %s",year.c_str(), esmodel.c_str());
   gAF->SetTitle(grafname);
   
   gAF->SetLineWidth(2);
   gAF->SetLineStyle(2);
   gAF->SetLineColor(4);
   gAF->GetXaxis()->SetTitleOffset(1.25);
   gAF->GetXaxis()->SetTitle("#eta");
   gAF->GetYaxis()->SetTitleOffset(1.25);
   gAF->GetYaxis()->SetTitle("#alpha_{Zee}");
   gAF->GetXaxis()->SetRangeUser(etaMin, etaMax);
   gAF->Draw("ALP");
   
   char plotname[99];
   sprintf(plotname,"scale_%s_%s.png", year.c_str(), esmodel.c_str());
   canvas->SaveAs(plotname);
   
   return gAF;
}
