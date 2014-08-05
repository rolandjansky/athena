/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// energy : energy (NOT transverse), in GeV
// etamin, etamax : from cluster
// ptype : 0 (electron), 1 (unconverted photon), 2 (converted photon)
// year : "2011" or "2012", Z scales

TGraph* plotScaleUncertainty_ERUpgrade(double Energy /* GeV */, double etamin, double etamax, string year, string esmodel) {

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade  ers;
   ers.Init("EnergyRescalerData.root",year,esmodel);

   double GeV = 1000.;
   double energy = Energy*GeV;

   double etaMin = etamin;
   double etaMax = etamax;
   double etaStep = .01;
   double eta = etaMin;

   TGraph* gAF1 = new TGraph();
   TGraph* gAF2 = new TGraph();
   TGraph* gAF3 = new TGraph();

   int i=0;

   while( eta<=etaMax ) {

     double ecorr = ers.applyEnergyCorrection(eta, energy,  
					      egRescaler::EnergyRescalerUpgrade::Electron,
					      egRescaler::EnergyRescalerUpgrade::Nominal);

     double evar1 = ers.applyEnergyCorrection(eta, energy,  
					      egRescaler::EnergyRescalerUpgrade::Electron,
					      egRescaler::EnergyRescalerUpgrade::ZeeStatUp);

     double evar2 = ers.applyEnergyCorrection(eta, energy,  
					      egRescaler::EnergyRescalerUpgrade::Electron,
					      egRescaler::EnergyRescalerUpgrade::ZeeMethodUp);

     double evar3 = ers.applyEnergyCorrection(eta, energy,  
					      egRescaler::EnergyRescalerUpgrade::Electron,
					      egRescaler::EnergyRescalerUpgrade::ZeeGenUp);

     cout << ecorr << " " << evar1 << " " << evar2 << " " << evar3 << endl;

     gAF1->SetPoint( i, eta, - (energy/ecorr - 1.) + (energy/evar1 - 1.));
     gAF2->SetPoint( i, eta, - (energy/ecorr - 1.) + (energy/evar2 - 1.));
     gAF3->SetPoint( i, eta, - (energy/ecorr - 1.) + (energy/evar3 - 1.));

     i++;
     eta += etaStep;

   }
 
   TCanvas * canvas = new TCanvas();

   gAF1->SetMaximum(.01);
   gAF1->SetMinimum(0);

   char grafname[99];
   sprintf(grafname,"Z #rightarrow ee calibration uncertainty, %s",year.c_str());
   gAF1->SetTitle(grafname);
   
   gAF1->SetLineWidth(2);
   gAF1->SetLineColor(4);
   gAF1->GetXaxis()->SetTitleOffset(1.3);
   gAF1->GetXaxis()->SetTitle("#eta");
   gAF1->GetYaxis()->SetTitleOffset(1.3);
   gAF1->GetYaxis()->SetTitle("#delta#alpha_{Zee}");
   gAF1->GetXaxis()->SetRangeUser(etaMin, etaMax);
   gAF1->Draw("ALP");
   
   gAF2->SetLineWidth(2);
   gAF2->SetLineColor(2);
   gAF2->Draw("LP");
   
   gAF3->SetLineWidth(2);
   gAF3->SetLineColor(3);
   gAF3->Draw("LP");

   TLegend* leg = new TLegend(.4,.75,.65,.89);
   leg->SetBorderSize(0);
   leg->SetFillColor(0);
   leg->AddEntry(gAF1, "Statistics", "l");
   leg->AddEntry(gAF2, "Method", "l");
   leg->AddEntry(gAF3, "Generator", "l");
   leg->Draw();

   char plotname[99];
   sprintf(plotname,"scaleUncertainty_%s_%s.png", year.c_str(), esmodel.c_str());
   canvas->SaveAs(plotname);
   
   return gAF;
}
