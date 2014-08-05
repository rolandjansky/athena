/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// eta : from cluster
// year : 2011, 2012. MC correction should return !=1 only for crack electrons in 2011.

void plotCrackCorrection_ERUpgrade(double eta, string year) {

  double GeV = 1000;
  gROOT->ProcessLine(".L EnergyRescalerUpgrade.cxx");
  
  egRescaler::EnergyRescalerUpgrade ers;
  if( year=="2011" )
    ers.Init("../share/EnergyRescalerData.root","2011","es2011a");
  else if( year=="2012" )
    ers.Init("../share/EnergyRescalerData.root","2012","es2012");

  double eMin = 0.;
  double eMax = 100.;
  double eStep = 1;
  double e = eMin;

  TGraph* gCrack = new TGraph();
  int i=0;

  while( e<=eMax ) {

    double corr = ers.applyMCCalibration(eta, e*GeV, 0);
    gCrack->SetPoint( i, e, corr );

    i++;
    e += eStep;

  }
 
  TCanvas * canvas = new TCanvas();

  gCrack->SetMaximum(1.2);
  gCrack->SetMinimum(0.7);

  char grafname[99];
  sprintf(grafname,"ET Correction factor, #eta = %4.2f",eta);
  gCrack->SetTitle(grafname);
   
  gCrack->SetLineWidth(2);
  gCrack->SetLineStyle(2);
  gCrack->SetLineColor(4);
  gCrack->GetXaxis()->SetTitleOffset(1.5);
  gCrack->GetXaxis()->SetTitle("E_{T} [GeV]");
  gCrack->Draw("AL");
   
  char plotname[99];
  sprintf(plotname,"crack_%f.png",eta);
  canvas->SaveAs(plotname);
   
}
