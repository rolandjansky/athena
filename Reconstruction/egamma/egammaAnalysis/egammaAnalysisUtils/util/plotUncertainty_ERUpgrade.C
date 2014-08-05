/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// eta : from cluster
// emin, emax : energies (NOT transverse), in GeV
// ptype : 0 (electron), 1 (unconverted photon), 2 (converted photon)
// logx : 0/1

TCanvas* plotUncertainty_ERUpgrade(double eta, double etmin /* GeV */, double etmax /* GeV */, int ptype, string year, 
				   bool logx=false) {

  double GeV = 1000;

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade ers;
   if( year=="2011" )
     ers.Init("../share/EnergyRescalerData.root","2011","es2011a");
   else if( year=="2012" )
     ers.Init("../share/EnergyRescalerData.root","2012","es2012");

   double eMin = etmin * cosh(eta) * GeV;
   double eMax = etmax * cosh(eta) * GeV;

   int nStep = 100;
   double eStep;
   if( logx )
     eStep = (log(eMax)-log(eMin))/(double)nStep;
   else
     eStep = (eMax-eMin)/(double)nStep;
   double e = eMin;

   TGraph* gZeeAllUp = new TGraph();   // Zee scale uncertainty
   TGraph* gZeeAllDown = new TGraph();   // Zee scale uncertainty
   TGraph* gZeeUp = new TGraph();      // Zee scale uncertainty
   TGraph* gZeeDown = new TGraph();      // Zee scale uncertainty
   TGraph* gZeeGenUp = new TGraph();   // Zee scale uncertainty
   TGraph* gZeeGenDown = new TGraph();   // Zee scale uncertainty
   TGraph* gMATUp = new TGraph();      //       MAT uncertainty
   TGraph* gMATDown = new TGraph();      //       MAT uncertainty
   TGraph* gPSUp = new TGraph();       //        ps uncertainty
   TGraph* gPSDown = new TGraph();       //        ps uncertainty
   TGraph* gLowUp = new TGraph();      //       all uncertainty
   TGraph* gLowDown = new TGraph();      //       all uncertainty
   TGraph* gAllUp = new TGraph();      //       all uncertainty
   TGraph* gAllDown = new TGraph();      //       all uncertainty

   int i=0;

   while( e<=eMax ) {

     double eVarZeeAllUp   = ers.getCorrectionUncertainty(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeAllUp);

     double eVarZeeUp      = ers.getCorrectionUncertainty(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeMethodUp);

     double eVarGenUp      = ers.getCorrectionUncertainty(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeGenUp);

     double eVarMATUp      = ers.getCorrectionUncertainty(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::R12StatUp);

     double eVarPSUp       = ers.getCorrectionUncertainty(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::PSStatUp);

     double eVarPSUp       = ers.getCorrectionUncertainty(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::PSStatUp);

     double eVarLowUp      = ers.getCorrectionUncertainty(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LowPtUp);

     double eVarAllUp      = ers.getCorrectionUncertainty(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::AllUp);

       
     gZeeAllUp->SetPoint(i, e/GeV, eVarZeeAllUp); 
     gZeeAllDown->SetPoint(i, e/GeV, -eVarZeeAllUp); 
     
     gZeeUp->SetPoint(i, e/GeV, eVarZeeUp); 
     gZeeDown->SetPoint(i, e/GeV, -eVarZeeUp); 
     
     gZeeGenUp->SetPoint(i, e/GeV, eVarGenUp); 
     gZeeGenDown->SetPoint(i, e/GeV, -eVarGenUp); 
     
     gMATUp->SetPoint(i, e/GeV, eVarMATUp); 
     gMATDown->SetPoint(i, e/GeV, -eVarMATUp); 
     
     gPSUp->SetPoint(i, e/GeV, eVarPSUp); 
     gPSDown->SetPoint(i, e/GeV, -eVarPSUp); 
     
     gLowUp->SetPoint(i, e/GeV, eVarLowUp); 
     gLowDown->SetPoint(i, e/GeV, -eVarLowUp); 
       
     gAllUp->SetPoint(i, e/GeV, eVarAllUp); 
     gAllDown->SetPoint(i, e/GeV, -eVarAllUp); 
       
     i++;

     if( logx )
       e *= exp(eStep);
     else
       e += eStep;

   }
 
   TCanvas* canvas = new TCanvas();
   if( logx )
     canvas->SetLogx();

   char grafname[99];
   if( ptype==0 )
     sprintf(grafname,"ES Uncertainty (%s), #eta = %4.2f, Electrons", year.c_str(), eta);
   else if( ptype==1 )
     sprintf(grafname,"ES Uncertainty (%s), #eta = %4.2f, Unconverted photons", year.c_str(), eta);
   else if( ptype==2 )
     sprintf(grafname,"ES Uncertainty (%s), #eta = %4.2f, Converted photons", year.c_str(), eta);
   gAllUp->SetTitle(grafname);
   gAllUp->GetYaxis()->SetRangeUser(-.01,.01);
   gAllUp->SetLineWidth(2);
   gAllUp->SetLineColor(1);
   gAllUp->Draw("AL");

   gAllDown->SetLineWidth(2);
   gAllDown->SetLineColor(1);
   gAllDown->Draw("L");

   gLowUp->SetLineWidth(2);
   gLowUp->SetLineColor(7);
   gLowUp->Draw("L");

   gLowDown->SetLineWidth(2);
   gLowDown->SetLineColor(7);
   gLowDown->Draw("L");

   gZeeUp->SetLineWidth(2);
   gZeeUp->SetLineColor(4);
   gZeeUp->Draw("L");

   gZeeDown->SetLineWidth(2);
   gZeeDown->SetLineColor(4);
   gZeeDown->Draw("L");

   gZeeGenUp->SetLineWidth(2);
   gZeeGenUp->SetLineColor(8);
   gZeeGenUp->Draw("L");

   gZeeGenDown->SetLineWidth(2);
   gZeeGenDown->SetLineColor(8);
   gZeeGenDown->Draw("L");

   gZeeAllUp->SetLineWidth(2);
   gZeeAllUp->SetLineColor(1);
   gZeeAllUp->SetLineStyle(2);
   gZeeAllUp->Draw("L");

   gZeeAllDown->SetLineWidth(2);
   gZeeAllDown->SetLineColor(1);
   gZeeAllDown->SetLineStyle(2);
   gZeeAllDown->Draw("L");

   gMATUp->SetLineWidth(2);
   gMATUp->SetLineColor(2);
   gMATUp->Draw("L");

   gMATDown->SetLineWidth(2);
   gMATDown->SetLineColor(2);
   gMATDown->Draw("L");

   gPSUp->SetLineWidth(2);
   gPSUp->SetLineColor(6);
   gPSUp->Draw("L");

   gPSDown->SetLineWidth(2);
   gPSDown->SetLineColor(6);
   gPSDown->Draw("L");



   TLegend* leg0 = new TLegend(.45,.15,.65,.3);
   leg0->SetBorderSize(0);
   leg0->SetFillColor(0);
   leg0->AddEntry(gZeeUp,"#alpha, Method", "l");
   leg0->AddEntry(gZeeGenUp,"#alpha, Model", "l");
   leg0->AddEntry(gZeeAllUp,"#alpha, Total", "l");
   leg0->AddEntry(gAllUp,"Total Uncertainty", "l");

   TLegend* leg1 = new TLegend(.65,.15,.85,.3);
   leg1->SetBorderSize(0);
   leg1->SetFillColor(0);
   leg1->AddEntry(gMATUp,"MAT", "l");
   leg1->AddEntry(gPSUp,"PS", "l");
   leg1->AddEntry(gLowUp,"Low-pt", "l");
   leg1->AddEntry(gLowUp,"", "");

   leg0->Draw("same");
   leg1->Draw("same");

   return canvas;
}


void plotloop_2011a() {

  TCanvas c1;
  c1.Print("scaleUncertainty.ps[");
  
  for(int i=0; i<48; i++) {

    double eta = (double)i/10. - 2.4 + 0.05;
    TCanvas* cnv = plotLinearity_ERUpgrade_2011a(eta,15.,70.,0);

    cnv->Print("scaleUncertainty.ps");

  }

  c1.Print("scaleUncertainty.ps]");
}
