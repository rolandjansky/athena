/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// eta : from cluster
// emin, emax : energies (NOT transverse), in GeV
// ptype : 0 (electron), 1 (unconverted photon), 2 (converted photon)
// logx : 0/1

TCanvas* plotLinearityLKG_ERUpgrade(double eta, double etmin /* GeV */, double etmax /* GeV */, int ptype, 
				    string year, string esmodel,
				    bool debug=false, bool logx=false, bool subtractAverage=false,
				    TGraph* overlay=0 ) {

  double GeV = 1000;

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade ers;

   ers.Init("EnergyRescalerData.root",year,esmodel);

   ers.SetDebugFlag( debug );

   ers.includePresamplerContribution( false );
   ers.includeMaterialContribution( false );
   ers.includeLeakageContribution( true );

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
   TGraph* gNom = new TGraph();              // nominal scale correction
   TGraph* gLeakageUp = new TGraph();
   TGraph* gLeakageDown = new TGraph();

   int i=0;

   while( e<=eMax ) {

     double alpha          = ers.getAlphaZee(eta, egRescaler::EnergyRescalerUpgrade::Nominal);
     double alphaUp        = ers.getAlphaZee(eta, egRescaler::EnergyRescalerUpgrade::ZeeStatUp);

     double eNominal       = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::Nominal);

     double eVarLeakageUp      = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LeakageUp);
     double eVarLeakageDown    = ers.applyEnergyCorrection(eta, e, ptype, egRescaler::EnergyRescalerUpgrade::LeakageDown);

     if(i==0) {
       gZee->SetPointError(i,0,alphaUp-alpha);
       if( !subtractAverage )
	 gZee->SetPoint(i,40.*cosh(eta),alpha);
       else
	 gZee->SetPoint(i,40.*cosh(eta),0);
     }
     
     if( !subtractAverage ) {
       
       gNom->SetPoint(i, e/GeV, (e-eNominal)/eNominal); 
       
       gLeakageUp->SetPoint(i, e/GeV, (e-eVarLeakageUp)/eVarLeakageUp); 
       gLeakageDown->SetPoint(i, e/GeV, (e-eVarLeakageDown)/eVarLeakageDown); 
       
     } else {
       
       gNom->SetPoint(i, e/GeV, (e-eNominal)/eNominal - alpha); 
       
       gLeakageUp->SetPoint(i, e/GeV, (e-eVarLeakageUp)/eVarLeakageUp - alpha); 
       gLeakageDown->SetPoint(i, e/GeV, (e-eVarLeakageDown)/eVarLeakageDown - alpha); 
       
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
     gLeakageUp->SetMaximum(alpha+.005);
     gLeakageUp->SetMinimum(alpha-.007);
   } else {
     gLeakageUp->SetMaximum(.005);
     gLeakageUp->SetMinimum(-.007);
   }

   char grafname[99];
   sprintf(grafname,"Linearity (%s), #eta = %4.2f, Leakage Contribution, Electrons", year.c_str(), eta);
   gLeakageUp->SetTitle(grafname);
   
   gLeakageUp->GetXaxis()->SetRangeUser(eMin/GeV, eMax/GeV);
   gLeakageUp->GetXaxis()->SetTitleOffset(1.2*gNom->GetXaxis()->GetTitleOffset());
   gLeakageUp->GetYaxis()->SetTitleOffset(1.2*gNom->GetYaxis()->GetTitleOffset());
   gLeakageUp->GetXaxis()->SetTitle("E [GeV]");
   gLeakageUp->GetYaxis()->SetTitle("#alpha(E)");
   gLeakageUp->SetLineWidth(2);
   gLeakageUp->SetLineStyle(2);
   gLeakageUp->SetLineColor(4);
   gLeakageUp->Draw("AL");

   gLeakageDown->SetLineWidth(2);
   gLeakageDown->SetLineStyle(3);
   gLeakageDown->SetLineColor(4);
   gLeakageDown->Draw("L");

   gNom->SetLineWidth(2);
   gNom->SetLineColor(4);
   gNom->Draw("L");

   gZee->SetMarkerStyle(20);
   gZee->Draw("P");

   TLegend* leg1 = new TLegend(.7,.15,.89,.3);
   leg1->SetBorderSize(0);
   leg1->SetFillColor(0);
   leg1->AddEntry(gZee,"#alpha, Z peak", "p");
   leg1->AddEntry(gNom,"Leakage, nom.", "l");
   leg1->AddEntry(gLeakageUp,"Leakage, up", "l");
   leg1->AddEntry(gLeakageDown,"Leakage, down", "l");

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

    double eta = (double)i/10. - 2.4 + 0.05;
    TCanvas* cnv = plotLinearityPS_ERUpgrade(eta,10.,100.,ptype, year, esmodel);

    char epsname[99];
    sprintf(epsname,"scaleLinearityPS_%d_%4.2f_%s_%s.eps",ptype,eta,year.c_str(),esmodel.c_str());

    cnv->Print(psnameFill);
    cnv->SaveAs(epsname);

    delete cnv;
  }

  c1.Print(psnameClose);
}
