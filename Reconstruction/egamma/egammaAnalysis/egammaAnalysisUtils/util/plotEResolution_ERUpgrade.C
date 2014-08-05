/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void plotEResolution_ERUpgrade(double eta, string year, string esmodel, bool testSmearing, bool doEPS){
  
   gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
   egRescaler::EnergyRescalerUpgrade  ers;
   ers.Init("../share/EnergyRescalerData.root", year, esmodel);

   double GeV = 1000.;

   double eMin = 10.;
   double eMax = 1000.;
   int nPts = 25;
   double eStep = (log(eMax)-log(eMin))/nPts;
   double e = eMin;

   TGraph* gResolMC = new TGraph(nPts);
   TGraph* gResolData = new TGraph(nPts);
   TGraph* gResolUp = new TGraph(nPts);
   TGraph* gResolDown = new TGraph(nPts);
   TGraph* gTotUp = new TGraph(nPts);
   TGraph* gTotDown = new TGraph(nPts);
   TGraph* gTotRelUp = new TGraph(nPts);
   TGraph* gTotRelDown = new TGraph(nPts);
   TGraph* gCstUp = new TGraph(nPts);
   TGraph* gCstDown = new TGraph(nPts);

   TGraph* gSmearing = new TGraph(nPts);
   TGraph* gSmearingUp = new TGraph(nPts);
   TGraph* gSmearingDown = new TGraph(nPts);
   
   TGraphErrors* gToy = new TGraphErrors(nPts);
   TGraphErrors* gToyUp = new TGraphErrors(nPts);
   TGraphErrors* gToyDown = new TGraphErrors(nPts);

   TH1D hsm("hsm","hsm",1000,0.75,1.25);
   TH1D hsmUp("hsmUp","hsmUp",1000,0.75,1.25);
   TH1D hsmDown("hsmDown","hsmDown",1000,0.75,1.25);

   int i=0;

   while( i <= nPts ) {

     double erup, erdo;

     ers.resolutionError( e, eta, erup, erdo );
     gTotUp->SetPoint( i, e, erup );
     gTotDown->SetPoint( i, e, erdo );
     gTotRelUp->SetPoint( i, e, erup/ers.resolution( e, eta, true ) );
     gTotRelDown->SetPoint( i, e, erdo/ers.resolution( e, eta, true ) );

     gCstUp->SetPoint( i, e, ers.dataConstantTermError( eta ) );
     gCstDown->SetPoint( i, e, -ers.dataConstantTermError( eta ) );

     gResolMC->SetPoint( i, e, ers.resolution( e, eta, false ) );
     gResolData->SetPoint( i, e, ers.resolution( e, eta, true ) );
     gResolUp->SetPoint( i, e, ers.resolution( e, eta, true ) + erup );
     gResolDown->SetPoint( i, e, ers.resolution( e, eta, true ) + erdo );

     double smearing = pow( ers.resolution( e, eta, true ), 2 ) -  pow( ers.resolution( e, eta, false ), 2 );
     if( smearing>0 )
       smearing = sqrt(smearing);
     else
       smearing = 0;
     gSmearing->SetPoint( i, e, smearing );

     double smearup = pow( ers.resolution( e, eta, true )+erup, 2 ) -  pow( ers.resolution( e, eta, false ), 2 );
     if( smearup>0 )
       smearup = sqrt(smearup);
     else
       smearup = 0;
     gSmearingUp->SetPoint( i, e, smearup );

     double smeardown = pow( ers.resolution( e, eta, true )+erdo, 2 ) -  pow( ers.resolution( e, eta, false ), 2 );
     if( smeardown>0 )
       smeardown = sqrt(smeardown);
     else
       smeardown = 0;
     gSmearingDown->SetPoint( i, e, smeardown );

     // test smearing

     if( testSmearing ) {

       hsm.Reset();
       hsmUp.Reset();
       hsmDown.Reset();

       for( int ievt=0; ievt<1000; ievt++ ) {
	 double esm = ers.getSmearingCorrection(eta, e*GeV, 0, false);
	 hsm.Fill( esm );
	 esm = ers.getSmearingCorrection(eta, e*GeV, 1, false);
	 hsmDown.Fill( esm );
	 esm = ers.getSmearingCorrection(eta, e*GeV, 2, false);
	 hsmUp.Fill( esm );
       }
       
       gToy->SetPoint( i, e, hsm.GetRMS() );
       gToy->SetPointError( i, 0, hsm.GetRMSError() );
       gToyUp->SetPoint( i, e, hsmUp.GetRMS() );
       gToyUp->SetPointError( i, 0, hsmUp.GetRMSError() );
       gToyDown->SetPoint( i, e, hsmDown.GetRMS() );
       gToyDown->SetPointError( i, 0, hsmDown.GetRMSError() );
       
     }

     i++;
     e *= exp(eStep);

   }
 
   TCanvas * canvas = new TCanvas();

   gPad->SetLogx();

   gResolUp->SetMaximum(0.07);
   gResolUp->SetMinimum(0);
   gResolUp->GetXaxis()->SetMoreLogLabels();
   char grafname[99];
   sprintf(grafname,"#eta = %3.1f",eta);
   gResolUp->SetTitle(grafname);
   
   gResolUp->SetLineWidth(2);
   gResolUp->SetLineStyle(2);
   gResolUp->SetLineColor(4);
   gResolUp->GetXaxis()->SetTitleOffset(1.5);
   gResolUp->GetXaxis()->SetTitle("E [GeV]");
   gResolUp->GetYaxis()->SetTitle("#sigma/E");
   gResolUp->Draw("AL");
   
   gResolDown->SetLineWidth(2);
   gResolDown->SetLineStyle(2);
   gResolDown->SetLineColor(4);
   gResolDown->Draw("L");
   
   gResolData->SetLineWidth(2);
   gResolData->SetLineStyle(1);
   gResolData->SetLineColor(4);
   gResolData->Draw("L");
   
   gResolMC->SetLineWidth(2);
   gResolMC->SetLineStyle(1);
   gResolMC->SetLineColor(2);
   gResolMC->Draw("L");

   char plotname[99];
   sprintf(plotname,"res_1_%f_%s_%s.png",eta, year.c_str(), esmodel.c_str());
   canvas->SaveAs(plotname);

   if( doEPS ) {
     char epsname[99];
     sprintf(epsname,"resolutionVsE_%3.1f.eps",eta);
     canvas->SaveAs(epsname);
   }
   
   if( testSmearing ) {
     
     gPad->SetLogx();
     
     gSmearingUp->SetMaximum(0.05);
     gSmearingUp->SetMinimum(0);
     sprintf(grafname,"Smearing correction (relative to Energy)    #eta = %3.1f",eta);
     gSmearingUp->SetTitle(grafname);
     gSmearingUp->SetLineWidth(2);
     gSmearingUp->SetLineStyle(2);
     gSmearingUp->SetLineColor(4);
     gSmearingUp->GetXaxis()->SetTitleOffset(1.5);
     gSmearingUp->GetXaxis()->SetTitle("E [GeV]");
     gSmearingUp->GetYaxis()->SetTitle("#sigma/E");
     gSmearingUp->Draw("AL");
     
     gSmearingDown->SetLineWidth(2);
     gSmearingDown->SetLineStyle(2);
     gSmearingDown->SetLineColor(4);
     gSmearingDown->Draw("L");
     
     gSmearing->SetLineWidth(2);
     gSmearing->SetLineStyle(1);
     gSmearing->SetLineColor(4);
     gSmearing->Draw("L");
     
     gToy->Draw("p");
     gToyUp->Draw("p");
     gToyDown->Draw("p");
     
   }
   
   sprintf(plotname,"res_2_%f_%s_%s.png",eta, year.c_str(), esmodel.c_str());
   canvas->SaveAs(plotname);
   
   
   gPad->SetLogx();

   gTotUp->SetMaximum(0.02);
   gTotUp->SetMinimum(-0.02);
   gTotUp->GetXaxis()->SetMoreLogLabels();
   
   sprintf(grafname,"#eta = %3.1f",eta);
   gTotUp->SetTitle(grafname);
   
   gTotUp->SetLineWidth(2);
   gTotUp->SetLineStyle(1);
   gTotUp->SetLineColor(2);
   gTotUp->GetXaxis()->SetTitleOffset(1.5);
   gTotUp->GetXaxis()->SetTitle("E [GeV]");
   gTotUp->GetYaxis()->SetTitle("#Delta#sigma/E");
   gTotUp->Draw("AL");
   
   gTotDown->SetLineWidth(2);
   gTotDown->SetLineStyle(1);
   gTotDown->SetLineColor(2);
   gTotDown->Draw("L");
   
   gCstUp->SetLineWidth(2);
   gCstUp->SetLineStyle(2);
   gCstUp->SetLineColor(4);
   gCstUp->Draw("L");
   
   gCstDown->SetLineWidth(2);
   gCstDown->SetLineStyle(2);
   gCstDown->SetLineColor(4);
   gCstDown->Draw("L");

   sprintf(plotname,"res_3_%f_%s_%s.png",eta, year.c_str(), esmodel.c_str());
   canvas->SaveAs(plotname);
   
   
   
   gTotRelUp->SetMaximum(0.5);
   gTotRelUp->SetMinimum(-0.5);
   gTotRelUp->GetXaxis()->SetMoreLogLabels();
   
   sprintf(grafname,"#eta = %3.1f",eta);
   gTotRelUp->SetTitle(grafname);
   
   gTotRelUp->SetLineWidth(2);
   gTotRelUp->SetLineStyle(1);
   gTotRelUp->SetLineColor(2);
   gTotRelUp->GetXaxis()->SetTitleOffset(1.5);
   gTotRelUp->GetXaxis()->SetTitle("E [GeV]");
   gTotRelUp->GetYaxis()->SetTitle("#Delta#sigma/#sigma");
   gTotRelUp->Draw("AL");
   
   gTotRelDown->SetLineWidth(2);
   gTotRelDown->SetLineStyle(1);
   gTotRelDown->SetLineColor(2);
   gTotRelDown->Draw("L");
   
   sprintf(plotname,"res_4_%f_%s_%s.png",eta, year.c_str(), esmodel.c_str());
   canvas->SaveAs(plotname);
   
}
