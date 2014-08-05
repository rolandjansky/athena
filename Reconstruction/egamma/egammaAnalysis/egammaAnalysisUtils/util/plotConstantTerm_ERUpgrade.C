/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


TGraph* plotConstantTerm_ERUpgrade(double etamin, double etamax, string year, string esmodel) {

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade  ers;
   ers.Init("../share/EnergyRescalerData.root", year, esmodel);

   double etaMin = etamin;
   double etaMax = etamax;
   double etaStep = .01;
   double eta = etaMin;

   TGraph* gAF = new TGraph();
   int i=0;

   while( eta<=etaMax ) {

     gAF->SetPoint( i, eta, ers.dataConstantTerm(eta) );

     i++;
     eta += etaStep;

   }
 
   TCanvas * canvas = new TCanvas();

   gAF->SetMaximum(.05);
   gAF->SetMinimum(0);

   char grafname[99];
   sprintf(grafname,"Smearing correction, %s, %s", year.c_str(), esmodel.c_str());
   gAF->SetTitle(grafname);
   
   gAF->SetLineWidth(2);
   gAF->SetLineStyle(2);
   gAF->SetLineColor(4);
   gAF->GetXaxis()->SetTitleOffset(1.5);
   gAF->GetXaxis()->SetTitle("#eta");
   gAF->Draw("AL");
   
   char plotname[99];
   sprintf(plotname,"smearing_%s_%s.png", year.c_str(), esmodel.c_str());
   canvas->SaveAs(plotname);

   return gAF;

}
