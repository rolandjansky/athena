/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// eta : from cluster
// emin, emax : energies (NOT transverse), in GeV
// ptype : 0 (electron), 1 (unconverted photon), 2 (converted photon)
// logx : 0/1

TCanvas* plotLinearity_ERUpgrade(double eta, double etmin /* GeV */, double etmax /* GeV */, int ptype, 
				 string year, string esmodel,
				 bool debug=false, bool logx=false, bool subtractAverage=false,
				 TGraph* overlay=0 ) {

  double GeV = 1000;

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade ers;

   ers.Init("EnergyRescalerData.root",year,esmodel);

   ers.SetDebugFlag( debug );

   // ers.includePresamplerContribution( false );
   // ers.includeMaterialContribution( false );
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
   TGraph* gB12Up = new TGraph();      //       B12 uncertainty, Up
   TGraph* gB12Down = new TGraph();    //                      , Down
   TGraph* gPSUp = new TGraph();       //        ps uncertainty, Up
   TGraph* gPSDown = new TGraph();     //                      , Down
   TGraph* gLowUp = new TGraph();      //    low-pt uncertainty, Up
   TGraph* gLowDown = new TGraph();    //                      , Down
   TGraph* gLeakUp = new TGraph();      //    leakage uncertainty, Up
   TGraph* gLeakDown = new TGraph();    //                      , Down

   int i=0;

   while( e<=eMax ) {

     double eT = e/cosh(eta);

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

     double eVarB12Up      = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::B12Up);
     double eVarB12Down    = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::B12Down);

     double eVarPSUp       = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::PSStatUp);
     double eVarPSDown     = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::PSStatDown);

     double eVarLowUp      = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LowPtUp);
     double eVarLowDown    = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LowPtDown);

     double eVarLeakUp      = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LeakageUp);
     double eVarLeakDown    = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LeakageDown);

     if(i==0) {
       gZee->SetPointError(i,0,alphaUp-alpha);
       if( !subtractAverage )
	 gZee->SetPoint(i,40.,alpha);
       else
	 gZee->SetPoint(i,40.,0);
     }
     
     if( !subtractAverage ) {
       
       gNom->SetPoint(i, eT/GeV, (e-eNominal)/eNominal); 
       
       gZeeAllUp->SetPoint(i, eT/GeV, (e-eVarZeeAllUp)/eVarZeeAllUp); 
       gZeeAllDown->SetPoint(i, eT/GeV, (e-eVarZeeAllDown)/eVarZeeAllDown); 
       
       gZeeUp->SetPoint(i, eT/GeV, (e-eVarZeeUp)/eVarZeeUp); 
       gZeeDown->SetPoint(i, eT/GeV, (e-eVarZeeDown)/eVarZeeDown); 
       
       gZeeGenUp->SetPoint(i, eT/GeV, (e-eVarGenUp)/eVarGenUp); 
       gZeeGenDown->SetPoint(i, eT/GeV, (e-eVarGenDown)/eVarGenDown); 
       
       gMATUp->SetPoint(i, eT/GeV, (e-eVarMATUp)/eVarMATUp); 
       gMATDown->SetPoint(i, eT/GeV, (e-eVarMATDown)/eVarMATDown); 
       
       gB12Up->SetPoint(i, eT/GeV, (e-eVarB12Up)/eVarB12Up); 
       gB12Down->SetPoint(i, eT/GeV, (e-eVarB12Down)/eVarB12Down); 
       
       gPSUp->SetPoint(i, eT/GeV, (e-eVarPSUp)/eVarPSUp); 
       gPSDown->SetPoint(i, eT/GeV, (e-eVarPSDown)/eVarPSDown); 
       
       gLowUp->SetPoint(i, eT/GeV, (e-eVarLowUp)/eVarLowUp); 
       gLowDown->SetPoint(i, eT/GeV, (e-eVarLowDown)/eVarLowDown); 
       
       gLeakUp->SetPoint(i, eT/GeV, .001/*(e-eVarLeakUp)/eVarLowUp*/ + alpha); 
       gLeakDown->SetPoint(i, eT/GeV, -.001/*(e-eVarLeakDown)/eVarLowDown*/ + alpha); 
       
     } else {
       
       gNom->SetPoint(i, eT/GeV, (e-eNominal)/eNominal - alpha); 
     
       gZeeAllUp->SetPoint(i, eT/GeV, (e-eVarZeeAllUp)/eVarZeeAllUp - alpha); 
       gZeeAllDown->SetPoint(i, eT/GeV, (e-eVarZeeAllDown)/eVarZeeAllDown - alpha); 
       
       gZeeUp->SetPoint(i, eT/GeV, (e-eVarZeeUp)/eVarZeeUp - alpha); 
       gZeeDown->SetPoint(i, eT/GeV, (e-eVarZeeDown)/eVarZeeDown - alpha); 
       
       gZeeGenUp->SetPoint(i, eT/GeV, (e-eVarGenUp)/eVarGenUp - alpha); 
       gZeeGenDown->SetPoint(i, eT/GeV, (e-eVarGenDown)/eVarGenDown - alpha); 
       
       gMATUp->SetPoint(i, eT/GeV, (e-eVarMATUp)/eVarMATUp - alpha); 
       gMATDown->SetPoint(i, eT/GeV, (e-eVarMATDown)/eVarMATDown - alpha); 
       
       gB12Up->SetPoint(i, eT/GeV, (e-eVarB12Up)/eVarB12Up - alpha); 
       gB12Down->SetPoint(i, eT/GeV, (e-eVarB12Down)/eVarB12Down - alpha); 
       
       gPSUp->SetPoint(i, eT/GeV, (e-eVarPSUp)/eVarPSUp - alpha); 
       gPSDown->SetPoint(i, eT/GeV, (e-eVarPSDown)/eVarPSDown - alpha); 
       
       gLowUp->SetPoint(i, eT/GeV, (e-eVarLowUp)/eVarLowUp - alpha); 
       gLowDown->SetPoint(i, eT/GeV, (e-eVarLowDown)/eVarLowDown - alpha); 
       
       gLeakUp->SetPoint(i, eT/GeV, .001/*(e-eVarLeakUp)/eVarLowUp*/); 
       gLeakDown->SetPoint(i, eT/GeV, -.001/*(e-eVarLeakDown)/eVarLowDown*/); 
       
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
     gZeeUp->SetMaximum(alpha+.01);
     gZeeUp->SetMinimum(alpha+-.015);
   } else {
     gZeeUp->SetMaximum(.01);
     gZeeUp->SetMinimum(-.015);
   }

   char grafname[99];
   if( ptype==0 )
     sprintf(grafname,"Linearity (%s, %s), #eta = %4.2f, Electrons", year.c_str(), esmodel.c_str(), eta);
   else if( ptype==1 )
     sprintf(grafname,"Linearity (%s, %s), #eta = %4.2f, Unconverted photons", year.c_str(), esmodel.c_str(), eta);
   else if( ptype==2 )
     sprintf(grafname,"Linearity (%s, %s), #eta = %4.2f, Converted photons", year.c_str(), esmodel.c_str(), eta);
   gZeeUp->SetTitle(grafname);
   
   gZeeUp->GetXaxis()->SetTitleOffset(1.2*gNom->GetXaxis()->GetTitleOffset());
   gZeeUp->GetYaxis()->SetTitleOffset(1.2*gNom->GetYaxis()->GetTitleOffset());
   gZeeUp->GetXaxis()->SetTitle("E_{T} [GeV]");
   gZeeUp->GetYaxis()->SetTitle("#alpha(E_{T})");
   gZeeUp->SetLineWidth(2);
   gZeeUp->SetLineStyle(1);
   gZeeUp->SetLineColor(4);
   gZeeUp->Draw("AL");

   gZee->SetMarkerStyle(20);
   gZee->Draw("P");

   gZeeDown->SetLineWidth(2);
   gZeeDown->SetLineStyle(2);
   gZeeDown->SetLineColor(4);
   gZeeDown->Draw("L");

   // gZeeGenUp->SetLineWidth(2);
   // gZeeGenUp->SetLineStyle(1);
   // gZeeGenUp->SetLineColor(8);
   // gZeeGenUp->Draw("L");

   // gZeeGenDown->SetLineWidth(2);
   // gZeeGenDown->SetLineStyle(2);
   // gZeeGenDown->SetLineColor(8);
   // gZeeGenDown->Draw("L");

   gMATUp->SetLineWidth(2);
   gMATUp->SetLineStyle(1);
   gMATUp->SetLineColor(2);
   gMATUp->Draw("L");

   gMATDown->SetLineWidth(2);
   gMATDown->SetLineStyle(2);
   gMATDown->SetLineColor(2);
   gMATDown->Draw("L");

   gPSUp->SetLineWidth(2);
   gPSUp->SetLineStyle(1);
   gPSUp->SetLineColor(6);
   gPSUp->Draw("L");

   gPSDown->SetLineWidth(2);
   gPSDown->SetLineStyle(2);
   gPSDown->SetLineColor(6);
   gPSDown->Draw("L");

   // gB12Up->SetLineWidth(2);
   // gB12Up->SetLineStyle(1);
   // gB12Up->SetLineColor(kOrange);
   // gB12Up->Draw("L");

   // gB12Down->SetLineWidth(2);
   // gB12Down->SetLineStyle(2);
   // gB12Down->SetLineColor(kOrange);
   // gB12Down->Draw("L");

   gLeakUp->SetLineWidth(2);
   gLeakUp->SetLineStyle(1);
   gLeakUp->SetLineColor(1);
   gLeakUp->Draw("L");

   gLeakDown->SetLineWidth(2);
   gLeakDown->SetLineStyle(2);
   gLeakDown->SetLineColor(1);
   gLeakDown->Draw("L");

   gNom->Draw("L");

   gZee->SetMarkerStyle(20);
   gZee->Draw("P");

   TLegend* leg0 = new TLegend(.65,.12,.85,.3);
   leg0->SetBorderSize(0);
   leg0->SetFillColor(0);
   leg0->AddEntry(gZee,"#alpha, Z peak", "p");
   leg0->AddEntry(gZeeUp,"#alpha, Method up", "l");
   leg0->AddEntry(gZeeDown,"#alpha, Method down", "l");
   leg0->AddEntry(gLeakUp,"Leakage, Up", "l");
   leg0->AddEntry(gLeakDown,"Leakage, Down", "l");
   // leg0->AddEntry(gZeeGenUp,"#alpha, Model up", "l");
   // leg0->AddEntry(gZeeGenDown,"#alpha, Model down", "l");

   TLegend* leg1 = new TLegend(.45,.12,.65,.3);
   leg1->SetBorderSize(0);
   leg1->SetFillColor(0);
   leg1->AddEntry(gZee,"", "");
   leg1->AddEntry(gMATUp,"MAT, up", "l");
   leg1->AddEntry(gMATDown,"MAT, down", "l");
   leg1->AddEntry(gPSUp,"PS, up", "l");
   leg1->AddEntry(gPSDown,"PS, down", "l");

   // TLegend* leg2 = new TLegend(.25,.12,.45,.3);
   // leg2->SetName("leg2");
   // leg2->SetBorderSize(0);
   // leg2->SetFillColor(0);
   // leg2->AddEntry(gZee,"", "");
   // leg2->AddEntry(gLeakUp,"Leakage, Up", "l");
   // leg2->AddEntry(gLeakDown,"Leakage, Down", "l");
   // leg2->AddEntry(gB12Up,"B12, Up", "l");
   // leg2->AddEntry(gB12Down,"B12, Down", "l");
   // if( subtractAverage ) {
   //   TText* txt = new TText();
   //   txt->SetNDC();
   //   txt->SetTextAlign(22);
   //   txt->SetTextSize(.03);
   //   txt->SetText(.5,.92,"Average scale subtracted");
   //   txt->Draw();
   // }

   if( overlay ) {
     for(int i=0; i<overlay->GetN(); i++) {
       double x,y;
       overlay->GetPoint(i,x,y);
       overlay->SetPoint(i,x,y);
     }
     overlay->SetMarkerStyle(21);
     overlay->SetMarkerColor(1);
     overlay->SetMarkerSize(gZee->GetMarkerSize());
     overlay->Draw("psame");
     leg2->AddEntry(overlay,"linearity, E/p", "p");
   }

   leg0->Draw("same");
   leg1->Draw("same");
   leg2->Draw("same");


   // char plotname[99];
   // sprintf(plotname,"linearity_%d_%2.1f.png",ptype,eta);
   //   canvas->Print("overlay.ps");
  
   return canvas;
}


void plotloop(int ptype, string year) {
  
  char psnameFill[99];
  sprintf(psnameFill,"scaleLinearity_%d_%s.ps",ptype,year.c_str());
  
  char psnameOpen[99];
  sprintf(psnameOpen,"scaleLinearity_%d_%s.ps[",ptype,year.c_str());
  
  char psnameClose[99];
  sprintf(psnameClose,"scaleLinearity_%d_%s.ps]",ptype,year.c_str());
  
 TCanvas c1;
  c1.Print(psnameOpen);
  
  for(int i=0; i<48; i++) {

    double eta = (double)i/10. - 2.4 + 0.05;
    TCanvas* cnv = plotLinearity_ERUpgrade(eta,10.,100.,ptype, year);

    cnv->Print(psnameFill);

    delete cnv;
  }

  c1.Print(psnameClose);
}
