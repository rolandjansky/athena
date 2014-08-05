/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// eta : from cluster
// emin, emax : energies (NOT transverse), in GeV
// ptype : 0 (electron), 1 (unconverted photon), 2 (converted photon)
// logx : 0/1

TCanvas* plotLinearityPS_ERUpgrade(double eta, double etmin /* GeV */, double etmax /* GeV */, int ptype, 
				   string year, string esmodel,
				   bool debug=false, bool logx=false, bool subtractAverage=false,
				   TGraph* overlay=0 ) {

  double GeV = 1000;

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade ers;

   ers.Init("EnergyRescalerData.root",year,esmodel);

   ers.SetDebugFlag( debug );

   ers.includeMaterialContribution( false );
   ers.includeLeakageContribution( false );

   double eMin = etmin * cosh(eta) * GeV;
   double eMax = etmax * cosh(eta) * GeV;

   int nStep = 100;
   double eStep;
   if( logx )
     eStep = (log(eMax)-log(eMin))/(double)nStep;
   else
     eStep = (eMax-eMin)/(double)nStep;
   double e = eMin;

   TGraphErrors* gZee = new TGraphErrors();  // Z scale
   TGraph* gNom = new TGraph();        // nominal scale correction
   TGraph* gZeeAllUp = new TGraph();   // Zee scale uncertainty, Up
   TGraph* gZeeAllDown = new TGraph(); //                      , Down
   TGraph* gZeeUp = new TGraph();      // Zee scale uncertainty, Up
   TGraph* gZeeDown = new TGraph();    //                      , Down
   TGraph* gZeeGenUp = new TGraph();   // Zee scale uncertainty, Up
   TGraph* gZeeGenDown = new TGraph(); //                      , Down
   TGraph* gMATUp = new TGraph();      //       MAT uncertainty, Up
   TGraph* gMATDown = new TGraph();    //                      , Down
   TGraph* gPSUp = new TGraph();       //        ps uncertainty, Up
   TGraph* gPSDown = new TGraph();     //                      , Down
   TGraph* gLowUp = new TGraph();      //    low-pt uncertainty, Up
   TGraph* gLowDown = new TGraph();    //                      , Down

   int i=0;

   while( e<=eMax ) {

     double alpha          = ers.getAlphaZee(eta, egRescaler::EnergyRescalerUpgrade::Nominal);
     double alphaUp        = ers.getAlphaZee(eta, egRescaler::EnergyRescalerUpgrade::ZeeStatUp);

     double eNominal       = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::Nominal);

     double eVarZeeAllUp   = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeAllUp);
     double eVarZeeAllDown = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeAllDown);

     double eVarZeeUp      = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeMethodUp);
     double eVarZeeDown    = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeMethodDown);

     double eVarGenUp      = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeGenUp);
     double eVarGenDown    = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::ZeeGenDown);

     double eVarMATUp      = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::R12StatUp);
     double eVarMATDown    = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::R12StatDown);

     double eVarPSUp       = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::B12Up);
     double eVarPSDown     = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::B12Down);

     double eVarLowUp      = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LowPtUp);
     double eVarLowDown    = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LowPtDown);

     if(i==0) {
       gZee->SetPointError(i,0,alphaUp-alpha);
       if( !subtractAverage )
	 gZee->SetPoint(i,40.*cosh(eta),alpha);
       else
	 gZee->SetPoint(i,40.*cosh(eta),0);
     }
     
     if( !subtractAverage ) {
       
       gNom->SetPoint(i, e/GeV, (e-eNominal)/eNominal); 
       
       gZeeAllUp->SetPoint(i, e/GeV, (e-eVarZeeAllUp)/eVarZeeAllUp); 
       gZeeAllDown->SetPoint(i, e/GeV, (e-eVarZeeAllDown)/eVarZeeAllDown); 
       
       gZeeUp->SetPoint(i, e/GeV, (e-eVarZeeUp)/eVarZeeUp); 
       gZeeDown->SetPoint(i, e/GeV, (e-eVarZeeDown)/eVarZeeDown); 
       
       gZeeGenUp->SetPoint(i, e/GeV, (e-eVarGenUp)/eVarGenUp); 
       gZeeGenDown->SetPoint(i, e/GeV, (e-eVarGenDown)/eVarGenDown); 
       
       gMATUp->SetPoint(i, e/GeV, (e-eVarMATUp)/eVarMATUp); 
       gMATDown->SetPoint(i, e/GeV, (e-eVarMATDown)/eVarMATDown); 
       
       gPSUp->SetPoint(i, e/GeV, (e-eVarPSUp)/eVarPSUp); 
       gPSDown->SetPoint(i, e/GeV, (e-eVarPSDown)/eVarPSDown); 
       
       gLowUp->SetPoint(i, e/GeV, (e-eVarLowUp)/eVarLowUp); 
       gLowDown->SetPoint(i, e/GeV, (e-eVarLowDown)/eVarLowDown); 
       
     } else {
       
       gNom->SetPoint(i, e/GeV, (e-eNominal)/eNominal - alpha); 
     
       gZeeAllUp->SetPoint(i, e/GeV, (e-eVarZeeAllUp)/eVarZeeAllUp - alpha); 
       gZeeAllDown->SetPoint(i, e/GeV, (e-eVarZeeAllDown)/eVarZeeAllDown - alpha); 
       
       gZeeUp->SetPoint(i, e/GeV, (e-eVarZeeUp)/eVarZeeUp - alpha); 
       gZeeDown->SetPoint(i, e/GeV, (e-eVarZeeDown)/eVarZeeDown - alpha); 
       
       gZeeGenUp->SetPoint(i, e/GeV, (e-eVarGenUp)/eVarGenUp - alpha); 
       gZeeGenDown->SetPoint(i, e/GeV, (e-eVarGenDown)/eVarGenDown - alpha); 
       
       gMATUp->SetPoint(i, e/GeV, (e-eVarMATUp)/eVarMATUp - alpha); 
       gMATDown->SetPoint(i, e/GeV, (e-eVarMATDown)/eVarMATDown - alpha); 
       
       gPSUp->SetPoint(i, e/GeV, (e-eVarPSUp)/eVarPSUp - alpha); 
       gPSDown->SetPoint(i, e/GeV, (e-eVarPSDown)/eVarPSDown - alpha); 
       
       gLowUp->SetPoint(i, e/GeV, (e-eVarLowUp)/eVarLowUp - alpha); 
       gLowDown->SetPoint(i, e/GeV, (e-eVarLowDown)/eVarLowDown - alpha); 
       
     }

     i++;

     if( logx )
       e *= exp(eStep);
     else
       e += eStep;

   }
 
   TCanvas* canvas = new TCanvas();
   if( logx )
     canvas->SetLogx();

   if( !subtractAverage ) {
     gPSUp->SetMaximum(alpha+.005);
     gPSUp->SetMinimum(alpha+-.007);
   } else {
     gPSUp->SetMaximum(.005);
     gPSUp->SetMinimum(-.007);
   }

   char grafname[99];
   if( ptype==0 )
     sprintf(grafname,"Linearity (%s), #eta = %4.2f, PS Contribution, Electrons", year.c_str(), eta);
   else if( ptype==1 )
     sprintf(grafname,"Linearity (%s), #eta = %4.2f, PS Contribution, Unconverted photons", year.c_str(), eta);
   else if( ptype==2 )
     sprintf(grafname,"Linearity (%s), #eta = %4.2f, PS Contribution, Converted photons", year.c_str(), eta);
   gPSUp->SetTitle(grafname);
   
   gPSUp->GetXaxis()->SetTitleOffset(1.2*gNom->GetXaxis()->GetTitleOffset());
   gPSUp->GetYaxis()->SetTitleOffset(1.2*gNom->GetYaxis()->GetTitleOffset());
   gPSUp->GetXaxis()->SetTitle("E [GeV]");
   gPSUp->GetYaxis()->SetTitle("#alpha(E)");
   gPSUp->SetLineWidth(2);
   gPSUp->SetLineStyle(2);
   gPSUp->SetLineColor(4);
   gPSUp->Draw("AL");

   gPSDown->SetLineWidth(2);
   gPSDown->SetLineStyle(3);
   gPSDown->SetLineColor(4);
   gPSDown->Draw("L");

   gNom->SetLineWidth(2);
   gNom->SetLineColor(4);
   gNom->Draw("L");

   gZee->SetMarkerStyle(20);
   gZee->Draw("P");

   TLegend* leg1 = new TLegend(.7,.15,.89,.3);
   leg1->SetBorderSize(0);
   leg1->SetFillColor(0);
   leg1->AddEntry(gZee,"#alpha, Z peak", "p");
   leg1->AddEntry(gNom,"PS, central", "l");
   leg1->AddEntry(gPSUp,"PS, up", "l");
   leg1->AddEntry(gPSDown,"PS, down", "l");

   leg1->Draw("same");

   // char plotname[99];
   // sprintf(plotname,"linearity_%d_%2.1f.png",ptype,eta);
   //   canvas->Print("overlay.ps");
  
   return canvas;
}


void plotloop(int ptype, string year, string esmodel) {
  
  char psnameFill[99];
  sprintf(psnameFill,"scaleLinearityPS_%d_%s_%s.ps",ptype,year.c_str(),esmodel.c_str());
  
  char psnameOpen[99];
  sprintf(psnameOpen,"scaleLinearityPS_%d_%s_%s.ps[",ptype,year.c_str(),esmodel.c_str());
  
  char psnameClose[99];
  sprintf(psnameClose,"scaleLinearityPS_%d_%s_%s.ps]",ptype,year.c_str(),esmodel.c_str());
  
 TCanvas c1;
  c1.Print(psnameOpen);
  
  for(int i=0; i<48; i++) {

    double eta = (double)i/10. - 2.45 + 0.05;
    TCanvas* cnv = plotLinearityPS_ERUpgrade(eta,10.,100.,ptype, year, esmodel);

    char epsname[99];
    sprintf(epsname,"scaleLinearityPS_%d_%4.2f_%s_%s.eps",ptype,eta,year.c_str(),esmodel.c_str());

    cnv->Print(psnameFill);
    cnv->SaveAs(epsname);

    delete cnv;
  }

  c1.Print(psnameClose);
}
