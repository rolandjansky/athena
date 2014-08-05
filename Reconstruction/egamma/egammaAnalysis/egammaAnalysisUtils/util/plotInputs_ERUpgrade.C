/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void plotInputs_ERUpgrade(string year) {

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111);
  gStyle->SetStatBorderSize(0);
  gStyle->SetStatW(0.15);
  gStyle->SetStatX(0.89);
  gStyle->SetStatY(0.89);
  gStyle->SetFitFormat("4.3f");

  TFile fInputs("../share/EnergyRescalerData.root");
  TCanvas c1; c1.Print("EnergyRescalerData.ps[");

  string showershapes = "ShowerShapes/" + year;
  fInputs.cd( showershapes.c_str() );
  
  E1E2HistC0->Draw("e2");
  E1E2GraphC0->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistC1->Draw("e2");
  E1E2GraphC1->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistC2->Draw("e2");
  E1E2GraphC2->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistC3->Draw("e2");
  E1E2GraphC3->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistC4->Draw("e2");
  E1E2GraphC4->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistC5->Draw("e2");
  E1E2GraphC5->Draw("psame");
  c1.Print("EnergyRescalerData.ps");
  
  b12FitEtaCoarse->Draw("e2");
  b12HistEtaCoarse->Draw("psame");
  c1.Print("EnergyRescalerData.ps");


  E1E2HistN0->Draw("e2");
  E1E2GraphN0->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistN1->Draw("e2");
  E1E2GraphN1->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistN2->Draw("e2");
  E1E2GraphN2->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistN3->Draw("e2");
  E1E2GraphN3->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistN4->Draw("e2");
  E1E2GraphN4->Draw("psame");
  c1.Print("EnergyRescalerData.ps");

  E1E2HistN5->Draw("e2");
  E1E2GraphN5->Draw("psame");
  c1.Print("EnergyRescalerData.ps");
  
  E1E2HistN6->Draw("e2");
  E1E2GraphN6->Draw("psame");
  c1.Print("EnergyRescalerData.ps");
  
  E1E2HistN7->Draw("e2");
  E1E2GraphN7->Draw("psame");
  c1.Print("EnergyRescalerData.ps");
  
  E1E2HistN8->Draw("e2");
  E1E2GraphN8->Draw("psame");
  c1.Print("EnergyRescalerData.ps");
  
  b12FitEtaNarrow->Draw("e2");
  b12HistEtaNarrow->Draw("epsame");
  c1.Print("EnergyRescalerData.ps");

  gStyle->SetOptFit(0);
  slopeHist->Draw("e2");
  slopeGraph->Draw("psame");
  c1.Print("EnergyRescalerData.ps");  


  e12dataHist.Draw("ep");
  e12distHist.Draw("epsame");
  c1.Print("EnergyRescalerData.ps");  

  r12Hist_b12nom_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  r12Hist_b12nom_errB12.Draw("e2p");
  c1.Print("EnergyRescalerData.ps");  
  r12Hist_b12Hist_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  r12Hist_b12Hist_errB12.Draw("e2p");
  c1.Print("EnergyRescalerData.ps");  
  r12Hist_b12Fit_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  r12Hist_b12Fit_errB12.Draw("e2p");
  c1.Print("EnergyRescalerData.ps");  

  matCorr_b12nom_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  matCorr_b12nom_errSlope.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  matCorr_b12nom_errB12.Draw("e2p");
  c1.Print("EnergyRescalerData.ps");  
  matCorr_b12Hist_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  matCorr_b12Hist_errSlope.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  matCorr_b12Hist_errB12.Draw("e2p");
  c1.Print("EnergyRescalerData.ps");  
  matCorr_b12Fit_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  matCorr_b12Fit_errSlope.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  matCorr_b12Fit_errB12.Draw("e2p");
  c1.Print("EnergyRescalerData.ps");  




  e0dataHist.Draw("ep");
  e0distHist.Draw("epsame");
  c1.Print("EnergyRescalerData.ps");  

  e0dataHist.Draw("ep");
  e12dataHist.SetMarkerColor(2);
  e12dataHist.Draw("epsame");
  c1.Print("EnergyRescalerData.ps");  


  string scales = "Scales/" + year;
  fInputs.cd( scales.c_str() );

  alphaPS_b12nom_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12nom_errSlope.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12nom_errMCorr.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12nom_errB12.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  

  alphaPS_b12Hist_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12Hist_errSlope.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12Hist_errMCorr.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12Hist_errB12.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  

  alphaPS_b12Fit_errStat.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12Fit_errSlope.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12Fit_errMCorr.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  
  alphaPS_b12Fit_errB12.Draw("ep");
  c1.Print("EnergyRescalerData.ps");  


  //  alphaPSmod_b12nom_errFit;
  alphaPSmod_b12nom_errTot.GetXaxis()->SetRangeUser(-1.8,1.8);
  alphaPSmod_b12nom_errTot.Draw("e2");
  alphaPSmod_b12nom_errFit.Draw("e2same");
  alphaPS_b12nom_errMCorr.Draw("epsame");
  fInputs.cd(showershapes.c_str());e0dataHist.Draw("esame");fInputs.cd(scales.c_str());
  TLine l1(0.0,0.7,0.0,1.3); l1.SetLineStyle(2); l1.Draw("same");
  TLine l2(0.2,0.7,0.2,1.3); l2.SetLineStyle(2); l2.Draw("same");
  TLine l3(0.4,0.7,0.4,1.3); l3.SetLineStyle(2); l3.Draw("same");
  TLine l4(0.6,0.7,0.6,1.3); l4.SetLineStyle(2); l4.Draw("same");
  TLine l5(0.8,0.7,0.8,1.3); l5.SetLineStyle(2); l5.Draw("same");
  TLine l6(1.0,0.7,1.0,1.3); l6.SetLineStyle(2); l6.Draw("same");
  TLine l7(1.2,0.7,1.2,1.3); l7.SetLineStyle(2); l7.Draw("same");
  TLine l8(1.37,0.7,1.37,1.3); l8.SetLineStyle(2); l8.Draw("same");
  TLine l9(1.55,0.7,1.55,1.3); l9.SetLineStyle(2); l9.Draw("same");
  TLine l12(-0.2,0.7,-0.2,1.3); l12.SetLineStyle(2); l12.Draw("same");
  TLine l13(-0.4,0.7,-0.4,1.3); l13.SetLineStyle(2); l13.Draw("same");
  TLine l14(-0.6,0.7,-0.6,1.3); l14.SetLineStyle(2); l14.Draw("same");
  TLine l15(-0.8,0.7,-0.8,1.3); l15.SetLineStyle(2); l15.Draw("same");
  TLine l16(-1.0,0.7,-1.0,1.3); l16.SetLineStyle(2); l16.Draw("same");
  TLine l17(-1.2,0.7,-1.2,1.3); l17.SetLineStyle(2); l17.Draw("same");
  TLine l18(-1.37,0.7,-1.37,1.3); l18.SetLineStyle(2); l18.Draw("same");
  TLine l19(-1.55,0.7,-1.55,1.3); l19.SetLineStyle(2); l19.Draw("same");
  c1.Print("EnergyRescalerData.ps");  

  //  alphaPSmod_b12Hist_errFit;
  alphaPSmod_b12Hist_errTot.GetXaxis()->SetRangeUser(-1.8,1.8);
  alphaPSmod_b12Hist_errTot.Draw("e2");
  alphaPSmod_b12Hist_errFit.Draw("e2same");
  alphaPS_b12Fit_errMCorr.Draw("epsame");
  fInputs.cd(showershapes.c_str());e0dataHist.Draw("esame");fInputs.cd(scales.c_str());
  TLine l1(0.0,0.7,0.0,1.3); l1.SetLineStyle(2); l1.Draw("same");
  TLine l2(0.2,0.7,0.2,1.3); l2.SetLineStyle(2); l2.Draw("same");
  TLine l3(0.4,0.7,0.4,1.3); l3.SetLineStyle(2); l3.Draw("same");
  TLine l4(0.6,0.7,0.6,1.3); l4.SetLineStyle(2); l4.Draw("same");
  TLine l5(0.8,0.7,0.8,1.3); l5.SetLineStyle(2); l5.Draw("same");
  TLine l6(1.0,0.7,1.0,1.3); l6.SetLineStyle(2); l6.Draw("same");
  TLine l7(1.2,0.7,1.2,1.3); l7.SetLineStyle(2); l7.Draw("same");
  TLine l8(1.37,0.7,1.37,1.3); l8.SetLineStyle(2); l8.Draw("same");
  TLine l9(1.55,0.7,1.55,1.3); l9.SetLineStyle(2); l9.Draw("same");
  TLine l12(-0.2,0.7,-0.2,1.3); l12.SetLineStyle(2); l12.Draw("same");
  TLine l13(-0.4,0.7,-0.4,1.3); l13.SetLineStyle(2); l13.Draw("same");
  TLine l14(-0.6,0.7,-0.6,1.3); l14.SetLineStyle(2); l14.Draw("same");
  TLine l15(-0.8,0.7,-0.8,1.3); l15.SetLineStyle(2); l15.Draw("same");
  TLine l16(-1.0,0.7,-1.0,1.3); l16.SetLineStyle(2); l16.Draw("same");
  TLine l17(-1.2,0.7,-1.2,1.3); l17.SetLineStyle(2); l17.Draw("same");
  TLine l18(-1.37,0.7,-1.37,1.3); l18.SetLineStyle(2); l18.Draw("same");
  TLine l19(-1.55,0.7,-1.55,1.3); l19.SetLineStyle(2); l19.Draw("same");
  c1.Print("EnergyRescalerData.ps");  

  //  alphaPSmod_b12Fit_errFit;
  alphaPSmod_b12Fit_errTot.GetXaxis()->SetRangeUser(-1.8,1.8);
  alphaPSmod_b12Fit_errTot.Draw("e2");
  alphaPSmod_b12Fit_errFit.Draw("e2same");
  alphaPS_b12Fit_errMCorr.Draw("epsame");
  fInputs.cd(showershapes.c_str());e0dataHist.Draw("esame");fInputs.cd(scales.c_str());
  TLine l1(0.0,0.7,0.0,1.3); l1.SetLineStyle(2); l1.Draw("same");
  TLine l2(0.2,0.7,0.2,1.3); l2.SetLineStyle(2); l2.Draw("same");
  TLine l3(0.4,0.7,0.4,1.3); l3.SetLineStyle(2); l3.Draw("same");
  TLine l4(0.6,0.7,0.6,1.3); l4.SetLineStyle(2); l4.Draw("same");
  TLine l5(0.8,0.7,0.8,1.3); l5.SetLineStyle(2); l5.Draw("same");
  TLine l6(1.0,0.7,1.0,1.3); l6.SetLineStyle(2); l6.Draw("same");
  TLine l7(1.2,0.7,1.2,1.3); l7.SetLineStyle(2); l7.Draw("same");
  TLine l8(1.37,0.7,1.37,1.3); l8.SetLineStyle(2); l8.Draw("same");
  TLine l9(1.55,0.7,1.55,1.3); l9.SetLineStyle(2); l9.Draw("same");
  TLine l12(-0.2,0.7,-0.2,1.3); l12.SetLineStyle(2); l12.Draw("same");
  TLine l13(-0.4,0.7,-0.4,1.3); l13.SetLineStyle(2); l13.Draw("same");
  TLine l14(-0.6,0.7,-0.6,1.3); l14.SetLineStyle(2); l14.Draw("same");
  TLine l15(-0.8,0.7,-0.8,1.3); l15.SetLineStyle(2); l15.Draw("same");
  TLine l16(-1.0,0.7,-1.0,1.3); l16.SetLineStyle(2); l16.Draw("same");
  TLine l17(-1.2,0.7,-1.2,1.3); l17.SetLineStyle(2); l17.Draw("same");
  TLine l18(-1.37,0.7,-1.37,1.3); l18.SetLineStyle(2); l18.Draw("same");
  TLine l19(-1.55,0.7,-1.55,1.3); l19.SetLineStyle(2); l19.Draw("same");
  c1.Print("EnergyRescalerData.ps");  




  string material = "Material/" + year;
  fInputs.cd( material.c_str() );

  ZScaleDiff->Draw("ap");
  c1.Print("EnergyRescalerData.ps");  

  UncScaleDiff->Draw("ap");
  c1.Print("EnergyRescalerData.ps");  

  ConvScaleDiff->Draw("ap");
  c1.Print("EnergyRescalerData.ps");  



  TList* le = (TList*) fInputs.Get("PSRecalibration/2011/ElectronGraphs");
  TList* lu  = (TList*) fInputs.Get("PSRecalibration/2011/UnconvertedGraphs");
  TList* lc = (TList*) fInputs.Get("PSRecalibration/2011/ConvertedGraphs");

  for(int i=0; i<le->GetSize(); i++) {
    
    TGraphErrors* gre = (TGraphErrors*) le->At(i);
    TGraphErrors* gru = (TGraphErrors*) lu->At(i);
    TGraphErrors* grc = (TGraphErrors*) lc->At(i);
    
    TH1D* hbins = (TH1D*) fInputs.Get("PSRecalibration/2011/ElectronEtaBins");

    char title[99];
    sprintf(title,"%4.2f < |#eta| < %4.2f", hbins->GetXaxis()->GetBinLowEdge(i+1), hbins->GetXaxis()->GetBinUpEdge(i+1));

    grc->SetNameTitle(title,title);
    grc->GetXaxis()->SetTitle("E [GeV]");
    grc->GetYaxis()->SetTitle("#partial#alpha / #partial#alpha_{PS}");
    grc->GetYaxis()->SetTitleOffset( 1.25*gre->GetYaxis()->GetTitleOffset() );
    grc->SetMinimum(0.);
    grc->GetFunction("f2")->SetLineColor(4);
    grc->GetFunction("f2")->SetLineStyle(2);
    grc->SetMarkerStyle(22);
    grc->Draw("ap");

    gre->SetMarkerStyle(20);
    gre->Draw("p");

    gru->GetFunction("f2")->SetLineColor(4);
    gru->SetMarkerStyle(21);
    gru->Draw("p");

    c1.Print("EnergyRescalerData.ps");  
  }




  fInputs.cd( scales.c_str() );

  hCellCalib.Draw();
  c1.Print("EnergyRescalerData.ps");  

  hCellCalibInv.Draw();
  c1.Print("EnergyRescalerData.ps");  

  hClusterCalib.Draw();
  c1.Print("EnergyRescalerData.ps");  

  hClusterCalibInv.Draw();
  c1.Print("EnergyRescalerData.ps");  

  hAllCalib.Draw();
  c1.Print("EnergyRescalerData.ps");  

  hAllCalibInv.Draw();
  c1.Print("EnergyRescalerData.ps");  

  hCheatCalibInv.Draw();
  c1.Print("EnergyRescalerData.ps");  



  c1.Print("EnergyRescalerData.ps]");
  fInputs.Close();
  
}
