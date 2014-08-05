/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// eta : from cluster
// emin, emax : energies (NOT transverse), in GeV
// ptype : 0 (electron), 1 (unconverted photon), 2 (converted photon)
// logx : 0/1

TCanvas* plotLinearityPS_AllTypes_ERUpgrade(double eta, double etmin /* GeV */, double etmax /* GeV */, 
					    string year, string esmodel,
					    bool debug=false, bool logx=false, bool subtractAverage=false,
					    TGraph* overlay=0 ) {

  double GeV = 1000;

   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade ers;

   ers.Init("../share/EnergyRescalerData.root",year,esmodel);

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
   TGraph* gPSUp = new TGraph();       //        ps uncertainty, Up
   TGraph* gPSDown = new TGraph();     //                      , Down

   TGraph* gNom_UC = new TGraph();    
   TGraph* gPSUp_UC = new TGraph();   
   TGraph* gPSDown_UC = new TGraph(); 

   TGraph* gNom_CV = new TGraph();    
   TGraph* gPSUp_CV = new TGraph();   
   TGraph* gPSDown_CV = new TGraph(); 

   int i=0;

   while( e<=eMax ) {
     
     double eT = e/cosh(eta);

     double alpha          = ers.getAlphaZee(eta, egRescaler::EnergyRescalerUpgrade::Nominal);
     double alphaUp        = ers.getAlphaZee(eta, egRescaler::EnergyRescalerUpgrade::ZeeStatUp);

     double eNominal       = ers.applyEnergyCorrection(eta, e, 0, egRescaler::EnergyRescalerUpgrade::Nominal);

     double eVarPSUp       = ers.applyEnergyCorrection(eta, e, 0, egRescaler::EnergyRescalerUpgrade::B12Up);
     double eVarPSDown     = ers.applyEnergyCorrection(eta, e, 0, egRescaler::EnergyRescalerUpgrade::B12Down);

     double eNominal_UC       = ers.applyEnergyCorrection(eta, e, 1, egRescaler::EnergyRescalerUpgrade::Nominal);

     double eVarPSUp_UC       = ers.applyEnergyCorrection(eta, e, 1, egRescaler::EnergyRescalerUpgrade::B12Up);
     double eVarPSDown_UC     = ers.applyEnergyCorrection(eta, e, 1, egRescaler::EnergyRescalerUpgrade::B12Down);

     double eNominal_CV       = ers.applyEnergyCorrection(eta, e, 2, egRescaler::EnergyRescalerUpgrade::Nominal);

     double eVarPSUp_CV       = ers.applyEnergyCorrection(eta, e, 2, egRescaler::EnergyRescalerUpgrade::B12Up);
     double eVarPSDown_CV     = ers.applyEnergyCorrection(eta, e, 2, egRescaler::EnergyRescalerUpgrade::B12Down);

     if(i==0) {
       gZee->SetPointError(i,0,alphaUp-alpha);
       if( !subtractAverage )
	 gZee->SetPoint(i,40.,alpha);
       else
	 gZee->SetPoint(i,40.,0);
     }
     
     if( !subtractAverage ) {
       
       gNom->SetPoint(i, eT/GeV, (e-eNominal)/eNominal); 
       
       gPSUp->SetPoint(i, eT/GeV, (e-eVarPSUp)/eVarPSUp); 
       gPSDown->SetPoint(i, eT/GeV, (e-eVarPSDown)/eVarPSDown); 
       
       gNom_UC->SetPoint(i, eT/GeV, (e-eNominal_UC)/eNominal_UC); 
       
       gPSUp_UC->SetPoint(i, eT/GeV, (e-eVarPSUp_UC)/eVarPSUp_UC); 
       gPSDown_UC->SetPoint(i, eT/GeV, (e-eVarPSDown_UC)/eVarPSDown_UC); 
       
       gNom_CV->SetPoint(i, eT/GeV, (e-eNominal_CV)/eNominal_CV); 
       
       gPSUp_CV->SetPoint(i, eT/GeV, (e-eVarPSUp_CV)/eVarPSUp_CV); 
       gPSDown_CV->SetPoint(i, eT/GeV, (e-eVarPSDown_CV)/eVarPSDown_CV); 
       
     } else {
       
       gNom->SetPoint(i, eT/GeV, (e-eNominal)/eNominal - alpha); 
     
       gPSUp->SetPoint(i, eT/GeV, (e-eVarPSUp)/eVarPSUp - alpha); 
       gPSDown->SetPoint(i, eT/GeV, (e-eVarPSDown)/eVarPSDown - alpha); 
       
       gNom_UC->SetPoint(i, eT/GeV, (e-eNominal_UC)/eNominal_UC - alpha); 
       
       gPSUp_UC->SetPoint(i, eT/GeV, (e-eVarPSUp_UC)/eVarPSUp_UC - alpha); 
       gPSDown_UC->SetPoint(i, eT/GeV, (e-eVarPSDown_UC)/eVarPSDown_UC - alpha); 
       
       gNom_CV->SetPoint(i, eT/GeV, (e-eNominal_CV)/eNominal_CV - alpha); 
       
       gPSUp_CV->SetPoint(i, eT/GeV, (e-eVarPSUp_CV)/eVarPSUp_CV - alpha); 
       gPSDown_CV->SetPoint(i, eT/GeV, (e-eVarPSDown_CV)/eVarPSDown_CV - alpha); 
       
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
   sprintf(grafname,"Linearity (%s), PS contribution, #eta = %4.2f", year.c_str(), eta);
   gPSUp->SetTitle(grafname);

   // Electrons
   
   gPSUp->GetXaxis()->SetTitleOffset(1.2*gNom->GetXaxis()->GetTitleOffset());
   gPSUp->GetYaxis()->SetTitleOffset(1.2*gNom->GetYaxis()->GetTitleOffset());
   gPSUp->GetXaxis()->SetTitle("E_{T} [GeV]");
   gPSUp->GetYaxis()->SetTitle("#alpha(E_{T})");
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

   // Unconverted

   gPSUp_UC->SetLineWidth(2);
   gPSUp_UC->SetLineStyle(2);
   gPSUp_UC->SetLineColor(2);
   gPSUp_UC->Draw("L");

   gPSDown_UC->SetLineWidth(2);
   gPSDown_UC->SetLineStyle(3);
   gPSDown_UC->SetLineColor(2);
   gPSDown_UC->Draw("L");

   gNom_UC->SetLineWidth(2);
   gNom_UC->SetLineColor(2);
   gNom_UC->Draw("L");

   // Converted 

   gPSUp_CV->SetLineWidth(2);
   gPSUp_CV->SetLineStyle(2);
   gPSUp_CV->SetLineColor(8);
   gPSUp_CV->Draw("L");

   gPSDown_CV->SetLineWidth(2);
   gPSDown_CV->SetLineStyle(3);
   gPSDown_CV->SetLineColor(8);
   gPSDown_CV->Draw("L");

   gNom_CV->SetLineWidth(2);
   gNom_CV->SetLineColor(8);
   gNom_CV->Draw("L");

   gZee->SetMarkerStyle(20);
   gZee->Draw("P");

   TLegend* leg1 = new TLegend(.65,.15,.9,.3);
   leg1->SetBorderSize(0);
   leg1->SetFillColor(0);
    leg1->AddEntry(gZee,"#alpha, Z peak", "p");
   leg1->AddEntry(gNom,"PS, e, nom.", "l");
   leg1->AddEntry(gPSUp,"PS, e, up", "l");
   leg1->AddEntry(gPSDown,"PS, e, down", "l");

   leg1->Draw("same");

   TLegend* leg2 = new TLegend(.4,.15,.65,.3);
   leg2->SetBorderSize(0);
   leg2->SetFillColor(0);
   leg2->AddEntry(gZee," ", "");
   leg2->AddEntry(gNom_UC,"PS, #gamma (unc.), nom.", "l");
   leg2->AddEntry(gPSUp_UC,"PS, #gamma (unc.), up", "l");
   leg2->AddEntry(gPSDown_UC,"PS, #gamma (unc.), down", "l");

   leg2->Draw("same");

   TLegend* leg3 = new TLegend(.15,.15,.4,.3);
   leg3->SetBorderSize(0);
   leg3->SetFillColor(0);
   leg3->AddEntry(gZee," ", "");
   leg3->AddEntry(gNom_CV,"PS, #gamma (cnv.), nom.", "l");
   leg3->AddEntry(gPSUp_CV,"PS, #gamma (cnv.), up", "l");
   leg3->AddEntry(gPSDown_CV,"PS, #gamma (cnv.), down", "l");

   leg3->Draw("same");

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

    cnv->Print(psnameFill);

    delete cnv;
  }

  c1.Print(psnameClose);
}
