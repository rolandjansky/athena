/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: Claudia Ciocca
//
// ROOFITS
//
// This macro fits Jpsi in 24 bins of eta 0.2 width from -2.4 to 2.4
// Eta is the one of leading muon
// The fit is a Crystal Ball with Exponential Background
// Muons are Muon Combined (CB) 
// Procedure is repeated for DATA, corrected MC and uncorrected MC
// Mass and Resolution plots are saved in a .gif

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "TTree.h"
#include "TH1D.h"
#include "TRandom.h"
using namespace RooFit ;

void fit_CrysBallBkgExp_pt1_eta01_CB_DATAandMC_24()
{

  int nbin=24;
  int netathin=24;
  float etamedthin[24];
  float etabinthin[25];
  int nbin=24;
  float binw[24];
  float xrebins[25] = {-2.4,-2.2,-2.0,-1.8,-1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0,2.2,2.4};
  float yrebins[151];
  float mass_value_data[24];
  float mass_value_mccorr[24];
  float mass_value_mcnocorr[24];
  float mass_error_data[24];
  float mass_error_mccorr[24];
  float mass_error_mcnocorr[24];
  float sigma_value_data[24];
  float sigma_value_mccorr[24];
  float sigma_value_mcnocorr[24];
  float sigma_error_data[24];
  float sigma_error_mccorr[24];
  float sigma_error_mcnocorr[24];
  float e0_value_data[24];
  float e0_value_mccorr[24];
  float e0_value_mcnocorr[24];
  float chi2_data[24];
  float chi2model_data[24];
  float chi2_mccorr[24];
  float chi2model_mccorr[24];
  float chi2_mcnocorr[24];
  float chi2model_mcnocorr[24];
  float mass_ratio[24];
  float mass_ratio1[24];
  float mass_ratio_error[24];
  float mass_ratio1_error[24];
  float sigma_ratio[24];
  float sigma_ratio1[24];
  float sigma2_ratio[24];
  float sigma_ratio_error[24];
  float sigma_ratio1_error[24];
  float sigma2_ratio[24];
  float mass_data_PDG[24];
  float mass_mccorr_PDG[24];
  float mass_mcnocorr_PDG[24];

// PDG values (GeV)

  float mass_Jpsi=3.096916;
  float mass_psi=3.686109;
  float sigma_Jpsi=0.0000929;
  float sigma_psi=0.000304;
  float mass_shift=0.58919;

  char * hname1 =  new char[32];
  char * hname2 =  new char[32];
  char * hname3 =  new char[32];
  char * hnamefile1 =  new char[32];
  char * hnamefile2 =  new char[32];
  char * hnamefile3 =  new char[32];

  int jj;
  float bl;
  float bw;

  char fileInDATA[100];
  char fileInMCCORR[100];
  char fileInMCNOCORR[100];

// OPEN YOUR ROOT FILES 

  sprintf(fileInDATA,"muonres-data-r7.root");
  TFile * MyFileDATA = TFile::Open(fileInDATA);

  sprintf(fileInMCCORR,"muonres-mccorr-corr1-r7.root");
  TFile * MyFileMCCORR = TFile::Open(fileInMCCORR);

  sprintf(fileInMCNOCORR,"muonres-mc-r7.root");
  TFile * MyFileMCNOCORR = TFile::Open(fileInMCNOCORR);


// DATA ANALYSIS

// Create canvas

  TCanvas* cCBdata = new TCanvas("CrystalBallBkgExp - CB - DATA","CrystalBallBkgExp - CB - DATA",1100,660) ;
  cCBdata->Divide(7,4);

  TCanvas* cCBdataTest = new TCanvas("CrystalBallBkgExp - CB - DATA TEST","CrystalBallBkgExp - CB - DATA TEST",1100,660) ;
  cCBdataTest->Divide(2,2);

  TCanvas* cCBfitdata = new TCanvas("fit CrystalBallBkgExp - CB - DATA","fit CrystalBallBkgExp - CB - DATA",660,660) ;
  cCBfitdata->Divide(2,2);

// Retrive histos

  TDirectory * MyDirDATA = MyFileDATA->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

// Get 1D histo "inclusive Jpsi mass of muon combined"

  TH1 * h1data;
  MyDirDATA->GetObject("Jpsimm_Mmumu_eta_0_CB_",h1data);
  h1data->Print();
  cCBdataTest->cd(1);
  h1data->Draw();

// Get 2D histo "Jpsi mass vs eta of leading muon for muon combined"

  TH2 * h2datao;
  MyDirDATA->GetObject("Jpsimm_h_Zm_1stEta01_eta_0_CB_",h2datao);
  h2datao->Print();
  TH2 *hnew1=(TH2*)h2datao->Clone();
  cCBdataTest->cd(2);
  hnew1->Draw("LEGO");
  h2datao->GetXaxis()->SetLimits(-2.4,2.4);
  h2datao->GetYaxis()->SetLimits(1.5,4.5);
  h2datao->Print();
  TH2 *hnew2=(TH2*)h2datao->Clone();
  cCBdataTest->cd(3);
  hnew2->Draw("LEGO");

// REBIN ETA

// Retrieve original bin limits

//  for (jj=0; jj<50; jj+2){
//    bl = h2datao->GetXaxis()->GetBinLowEdge(jj+1);
//    xrebins[jj] = bl;
//    if (jj==(49)) bl = h2datao->GetXaxis()->GetBinUpEdge(jj+1);
//    xrebins[jj+1] = bl;
//    cout << " eta bl " << bl << endl;
//  }
  for (jj=0; jj<150; jj++){
    bl = h2datao->GetYaxis()->GetBinLowEdge(jj+1);
    yrebins[jj] = bl;
    if (jj==(149)) bl = h2datao->GetYaxis()->GetBinUpEdge(jj+1);
    yrebins[jj+1] = bl;
    cout << " mass bl " << bl << endl;
  }

  TH2D * h2data = new TH2D("h2datarebin",h2datao->GetTitle(),24,xrebins,150,yrebins);
  TAxis * xaxis = h2datao->GetXaxis();
  TAxis * yaxis = h2datao->GetYaxis();
  for (int j=1; j<=yaxis->GetNbins();j++) {
     for (int i=1; i<=xaxis->GetNbins();i++) {
        h2data->Fill(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j),h2datao->GetBinContent(i,j));
     }
  }
  h2data->Print();
  TH2 *hnew3=(TH2*)h2data->Clone();
  cCBdataTest->cd(4);
  hnew3->Draw("LEGO");

// Retrive eta bin width and limits

  for (jj=0; jj<netathin; jj++){
    bl = h2data->GetXaxis()->GetBinLowEdge(jj+1);
    bw = h2data->GetXaxis()->GetBinWidth(jj+1);
    binw[jj] = bw/2;
    etabinthin[jj] = bl;
    if (jj==(netathin-1)) bl = h2data->GetXaxis()->GetBinUpEdge(jj+1);
    etabinthin[jj+1] = bl;
    cout << " eta bl " << bl << endl;
    cout << " eta bw " << bw << endl;
  }

// Do projections to obtain 1D histos of Jpsi mass, one histo for each bin of eta of leading muon

  for (jj=0; jj<netathin; jj++){

    int bin=jj+1;
    sprintf(hname1,"py_data_%d",jj+1);
    TH1 * hpdata = h2data->ProjectionY(hname1, bin, bin);
    hpdata->Print();

// Do Fit - CRYSTAL BALL

    RooRealVar m("m","m",2.5,4.2); // x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.001,1.);
    RooRealVar alpha("alpha","alpha",2.0,0.001,100.0);
    RooRealVar n("n","n",1.0,0.001,1000.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n); // here CB and cb mean Crystal Ball

// Rescale of the mass difference taking into account the energy scale (PDG_mass_Psi/PDG_mass_Jpsi)

    RooRealVar mshift("mshift","mshift",1.19);
    RooRealVar wshift("wshift","wshift",3.08);
//    RooFormulaVar m0_shift("m0_shift","m0*shift",RooArgSet(m0,shift)); // alternative to line below
    RooFormulaVar m0_shift("m0_shift","@0*@1",RooArgSet(m0,mshift));
    RooFormulaVar sigma_shift("sigma_shift","@0*@1",RooArgSet(sigma,wshift));

    RooCBShape cb1("cb1","cb1",m,m0_shift,sigma,alpha,n) ;

    RooRealVar frac("frac","frac",0.95,0.,1.); // Jpsi fraction
    RooAddPdf sig("sig","sig",RooArgList(cb,cb1),frac);

// EXPONENTIAL BKG

    RooRealVar e0("e0","e0", -1.0, -100.,100.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",0.80,0.,1.);
    RooAddPdf model("model","model",RooArgList(sig,bkg),fsig);

    RooDataHist data("data","dataset with m",m,hpdata);
    RooPlot* myframe = m.frame(Title(hname1));
    data.plotOn(myframe, Name("data"));

    model.fitTo(data, Range(2.6,4.1) ); // CHOOSE FIT RANGE 
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));
    model.plotOn(myframe, Name("model"));
    model.paramOn(myframe); // DRAW FIT PARACBTERS
//    data.statOn(myframe); // DRAW STATISTICS

    int nc1=jj+1;
    cCBdata->cd(nc1);
    myframe->Draw();
//    float mychi2 = myframe->chiSquare();
//    TPaveLabel *t1 = new TPaveLabel(0.7,0.3,0.8,0.5 Form("#chi^{2} = %f", mychi2),"brNDC"); 
//    myframe->addObject(t1); 

    if (jj==1){
      cCBfitdata->cd(1);
      myframe->Draw();
    }
    if (jj==4){
      cCBfitdata->cd(2);
      myframe->Draw();
    }
    if (jj==7){
      cCBfitdata->cd(3);
      myframe->Draw();
    }
    if (jj==10){
      cCBfitdata->cd(4);
      myframe->Draw();
    }

    cCBfitdata->Print("fit_data_pt1_eta01_CB_24.gif"); // Save gif

//    RooFitResult* results = cb.fitTo(data, Save() );
//    results->floatParsFinal().Print("s") ;
//    model.Print("t");
    RooArgSet* params = model.getParameters(m);
    params->Print("v");  // Print fit parameters

//    sprintf(hnamefile1,"CB_results_data_%d",jj+1);
//    params->writeToFile(hnamefile1);

// Retrive fit parameters to create the final plots

    m0.Print();
    mass_value_data[jj] = m0.getVal();
    mass_error_data[jj] = m0.getError();
    sigma_value_data[jj] = sigma.getVal();
    sigma_error_data[jj] = sigma.getError();
    e0_value_data[jj] = e0.getVal();
    chi2_data[jj] = myframe->chiSquare();
    chi2model_data[jj] = myframe->chiSquare("model","data",7);

  }

  for (jj=0; jj<netathin; jj++){
    etamedthin[jj]=(etabinthin[jj+1]+etabinthin[jj])/2;
  }

// Jpsi mass vs eta

  TGraphErrors * gr1 = new TGraphErrors(nbin,etamedthin,mass_value_data,binw,mass_error_data);
  cCBdata->cd(27);
  gr1->SetTitle("Jpsi mass DATA");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

// Jpsi resolution vs eta

  TGraphErrors * gr2 = new TGraphErrors(nbin,etamedthin,sigma_value_data,binw,sigma_error_data);
  cCBdata->cd(28);
  gr2->SetTitle("Jpsi width DATA");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");

  cCBdata->Print("fits_data_pt1_eta01_CB_24.gif"); // Save gif

// CORRECTED MC ANALYSIS - REPEAT ALL THE SAME FOR DATA ANALYSIS ABOVE

  TCanvas* cCBmccorr = new TCanvas("CrystalBallBkgExp - CB - MCCORR","CrystalBallBkgExp - CB - MCCORR",1100,660) ;
  cCBmccorr->Divide(7,4);

  TCanvas* cCBfitmccorr = new TCanvas("fit CrystalBallBkgExp - CB - MCCORR","fit CrystalBallBkgExp - CB - MCCORR",660,660) ;
  cCBfitmccorr->Divide(2,2);

  TDirectory * MyDirMCCORR = MyFileMCCORR->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

  TH1 * h1mccorr;
  MyDirMCCORR->GetObject("Jpsimm_Mmumu_eta_0_CB_",h1mccorr);
  h1mccorr->Print();

  TH2 * h2mccorro;
  MyDirMCCORR->GetObject("Jpsimm_h_Zm_1stEta01_eta_0_CB_",h2mccorro);
  h2mccorro->Print();
  h2mccorro->GetXaxis()->SetLimits(-2.4,2.4);
  h2mccorro->GetYaxis()->SetLimits(1.5,4.5);
  h2mccorro->Print();

  TH2D * h2mccorr = new TH2D("h2mccorrrebin",h2mccorro->GetTitle(),24,xrebins,150,yrebins);
  TAxis * xaxis = h2mccorro->GetXaxis();
  TAxis * yaxis = h2mccorro->GetYaxis();
  for (int j=1; j<=yaxis->GetNbins();j++) {
     for (int i=1; i<=xaxis->GetNbins();i++) {
        h2mccorr->Fill(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j),h2mccorro->GetBinContent(i,j));
     }
  }
  h2mccorr->Print();

  for (jj=0; jj<netathin; jj++){

    int bin=jj+1;
    sprintf(hname2,"py_mccorr_%d",jj+1);
    TH1 * hpmccorr = h2mccorr->ProjectionY(hname2, bin, bin);
    hpmccorr->Print();

// Crystal Ball

    RooRealVar m("m","m",2.5,4.2); // x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.001,1.);
    RooRealVar alpha("alpha","alpha",2.0,0.001,100.0);
    RooRealVar n("n","n",5.0,0.001,1000.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n);

// EXPONENTIAL BKG

    RooRealVar e0("e0","e0", -1.0, -100.,100.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",0.99,0.,1.);
    RooAddPdf model("model","model",RooArgList(cb,bkg),fsig);

    RooDataHist data("data","dataset with m",m,hpmccorr);
    RooPlot* myframe = m.frame(Title(hname2));
    data.plotOn(myframe, Name("data"));

    model.fitTo(data, Range(2.6,3.6), SumW2Error(kFALSE) );
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));
    model.plotOn(myframe, Name("model"));
    model.paramOn(myframe);

    int nc1=jj+1;
    cCBmccorr->cd(nc1);
    myframe->Draw();

    if (jj==1){
      cCBfitmccorr->cd(1);
      myframe->Draw();
    }
    if (jj==4){
      cCBfitmccorr->cd(2);
      myframe->Draw();
    }
    if (jj==7){
      cCBfitmccorr->cd(3);
      myframe->Draw();
    }
    if (jj==10){
      cCBfitmccorr->cd(4);
      myframe->Draw();
    }

    cCBfitmccorr->Print("fit_mccorr_pt1_eta01_CB_24.gif"); // Save gif

    RooArgSet* params = model.getParameters(m);
    params->Print("v");

    m0.Print();
    mass_value_mccorr[jj] = m0.getVal();
    mass_error_mccorr[jj] = m0.getError();
    sigma_value_mccorr[jj] = sigma.getVal();
    sigma_error_mccorr[jj] = sigma.getError();
    e0_value_mccorr[jj] = e0.getVal();
    chi2_mccorr[jj] = myframe->chiSquare();
    chi2model_mccorr[jj] = myframe->chiSquare("model","data",7);

  }

  for (jj=0; jj<netathin; jj++){
    etamedthin[jj]=(etabinthin[jj+1]+etabinthin[jj])/2;
  }

  TGraphErrors * gr1 = new TGraphErrors(nbin,etamedthin,mass_value_mccorr,binw,mass_error_mccorr);
  cCBmccorr->cd(27);
  gr1->SetTitle("Jpsi mass");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

  TGraphErrors * gr2 = new TGraphErrors(nbin,etamedthin,sigma_value_mccorr,binw,sigma_error_mccorr);
  cCBmccorr->cd(28);
  gr2->SetTitle("Jpsi width");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");

  cCBmccorr->Print("fits_mccorr_pt1_eta01_CB_24.gif"); // Save gif

// UNCORRECTED MC ANALYSIS - THE SAME

  TCanvas* cCBmcnocorr = new TCanvas("CrystalBallBkgExp - CB - MCNOCORR","CrystalBallBkgExp - CB - MCNOCORR",1100,660) ;
  cCBmcnocorr->Divide(7,4);

  TCanvas* cCBfitmcnocorr = new TCanvas("fit CrystalBallBkgExp - CB - MCNOCORR","fit CrystalBallBkgExp - CB - MCNOCORR",660,660) ;
  cCBfitmcnocorr->Divide(2,2);

  TDirectory * MyDirMCNOCORR = MyFileMCNOCORR->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

  TH1 * h1mcnocorr;
  MyDirMCNOCORR->GetObject("Jpsimm_Mmumu_eta_0_CB_",h1mcnocorr);
  h1mcnocorr->Print();

  TH2 * h2mcnocorro;
  MyDirMCNOCORR->GetObject("Jpsimm_h_Zm_1stEta01_eta_0_CB_",h2mcnocorro);
  h2mcnocorro->Print();
  h2mcnocorro->GetXaxis()->SetLimits(-2.4,2.4);
  h2mcnocorro->GetYaxis()->SetLimits(1.5,4.5);
  h2mcnocorro->Print();

  TH2D * h2mcnocorr = new TH2D("h2mcnocorrrebin",h2mcnocorro->GetTitle(),24,xrebins,150,yrebins);
  TAxis * xaxis = h2mcnocorro->GetXaxis();
  TAxis * yaxis = h2mcnocorro->GetYaxis();
  for (int j=1; j<=yaxis->GetNbins();j++) {
     for (int i=1; i<=xaxis->GetNbins();i++) {
        h2mcnocorr->Fill(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j),h2mcnocorro->GetBinContent(i,j));
     }
  }
  h2mcnocorr->Print();

  for (jj=0; jj<netathin; jj++){

    int bin=jj+1;
    sprintf(hname3,"py_mcnocorr_%d",jj+1);
    TH1 * hpmcnocorr = h2mcnocorr->ProjectionY(hname3, bin, bin);
    hpmcnocorr->Print();

// Crystal Ball

    RooRealVar m("m","m",2.5,4.2); // x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.001,1.);
    RooRealVar alpha("alpha","alpha",2.0,0.001,100.0);
    RooRealVar n("n","n",5.0,0.001,1000.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n);

// EXPONENTIAL BKG

    RooRealVar e0("e0","e0", -1.0, -100.,100.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",0.99,0.,1.);
    RooAddPdf model("model","model",RooArgList(cb,bkg),fsig);

    RooDataHist data("data","dataset with m",m,hpmcnocorr);
    RooPlot* myframe = m.frame(Title(hname3));
    data.plotOn(myframe, Name("data"));

    model.fitTo(data, Range(2.6,3.6), SumW2Error(kFALSE) );
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));
    model.plotOn(myframe, Name("model"));
    model.paramOn(myframe);

    int nc1=jj+1;
    cCBmcnocorr->cd(nc1);
    myframe->Draw();

    if (jj==1){
      cCBfitmcnocorr->cd(1);
      myframe->Draw();
    }
    if (jj==4){
      cCBfitmcnocorr->cd(2);
      myframe->Draw();
    }
    if (jj==7){
      cCBfitmcnocorr->cd(3);
      myframe->Draw();
    }
    if (jj==10){
      cCBfitmcnocorr->cd(4);
      myframe->Draw();
    }

    cCBfitmcnocorr->Print("fit_mcnocorr_pt1_eta01_CB_24.gif"); // Save gif

    RooArgSet* params = model.getParameters(m);
    params->Print("v");

    m0.Print();
    mass_value_mcnocorr[jj] = m0.getVal();
    mass_error_mcnocorr[jj] = m0.getError();
    sigma_value_mcnocorr[jj] = sigma.getVal();
    sigma_error_mcnocorr[jj] = sigma.getError();
    e0_value_mcnocorr[jj] = e0.getVal();
    chi2_mcnocorr[jj] = myframe->chiSquare();
    chi2model_mcnocorr[jj] = myframe->chiSquare("model","data",7);

  }

  for (jj=0; jj<netathin; jj++){
    etamedthin[jj]=(etabinthin[jj+1]+etabinthin[jj])/2;
  }

  TGraphErrors * gr1 = new TGraphErrors(nbin,etamedthin,mass_value_mcnocorr,binw,mass_error_mcnocorr);
  cCBmcnocorr->cd(27);
  gr1->SetTitle("Jpsi mass");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

  TGraphErrors * gr2 = new TGraphErrors(nbin,etamedthin,sigma_value_mcnocorr,binw,sigma_error_mcnocorr);
  cCBmcnocorr->cd(28);
  gr2->SetTitle("Jpsi width");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");

  cCBmcnocorr->Print("fits_mcnocorr_pt1_eta01_CB_24.gif"); // Save gif

// DATA and MC CORRECTED and UNCORRECTED FINAL RESULTS AND PLOTS

// Calculate mass and resolution ratios and errors

  for (jj=0; jj<netathin; jj++){
    mass_ratio[jj]=mass_value_data[jj]/mass_value_mccorr[jj];
    mass_ratio1[jj]=mass_value_data[jj]/mass_value_mcnocorr[jj];
//    mass_ratio_error[jj]=sqrt((mass_error_data[jj]*mass_error_data[jj])/(mass_value_mccorr[jj]*mass_value_mccorr[jj]) + (mass_value_data[jj]*mass_value_data[jj]*mass_error_mccorr[jj]*mass_error_mccorr[jj])/(mass_value_mccorr[jj]*mass_value_mccorr[jj]));  // Claudia
    mass_ratio_error[jj]=sqrt((mass_error_data[jj]*mass_error_data[jj])/mass_value_mccorr[jj] + (mass_error_mccorr[jj]*mass_error_mccorr[jj]*mass_value_data[jj])/(mass_value_mccorr[jj]*mass_value_mccorr[jj])); // Massimo
    mass_ratio1_error[jj]=sqrt((mass_error_data[jj]*mass_error_data[jj])/mass_value_mcnocorr[jj] + (mass_error_mcnocorr[jj]*mass_error_mcnocorr[jj]*mass_value_data[jj])/(mass_value_mcnocorr[jj]*mass_value_mcnocorr[jj]));
    sigma_ratio[jj]=sigma_value_data[jj]/sigma_value_mccorr[jj];
    sigma_ratio1[jj]=sigma_value_data[jj]/sigma_value_mcnocorr[jj];
    sigma_ratio_error[jj]=sqrt((sigma_error_data[jj]*sigma_error_data[jj])/sigma_value_mccorr[jj] + (sigma_error_mccorr[jj]*sigma_error_mccorr[jj]*sigma_value_data[jj])/(sigma_value_mccorr[jj]*sigma_value_mccorr[jj]));
    sigma_ratio1_error[jj]=sqrt((sigma_error_data[jj]*sigma_error_data[jj])/sigma_value_mcnocorr[jj] + (sigma_error_mcnocorr[jj]*sigma_error_mcnocorr[jj]*sigma_value_data[jj])/(sigma_value_mcnocorr[jj]*sigma_value_mcnocorr[jj]));
    mass_data_PDG[jj]=mass_value_data[jj]/mass_Jpsi;
    mass_mccorr_PDG[jj]=mass_value_mccorr[jj]/mass_Jpsi;
    mass_mcnocorr_PDG[jj]=mass_value_mcnocorr[jj]/mass_Jpsi;
  }

// Create Canvas and Graphs, and plots results

  TCanvas* cCBmass = new TCanvas(" CrystalBallBkgExp - CB - DATA and MC mass "," CrystalBallBkgExp - CB - DATA and MC mass ",900,640) ;
  gStyle->SetOptStat(0);
  gStyle->SetLegendBorderSize(0);
//  cCBmass->Divide(1,2,0,0);
  TPad *pad1 = new TPad("pad1", "The pad 65% of the height",0.0,0.35,1.0,1.0);
  TPad *pad2 = new TPad("pad2", "The pad 35% of the height",0.0,0.0,1.0,0.35);
  pad1->Draw();
  pad2->Draw();

  TCanvas* cCBresol = new TCanvas(" CrystalBallBkgExp - CB - DATA and MC resol "," CrystalBallBkgExp - CB - DATA and MC resol ",900,640) ;
  gStyle->SetOptStat(0);
  gStyle->SetLegendBorderSize(0);
//  cCBresol->Divide(1,2,0,0);
  TPad *pad3 = new TPad("pad3", "The pad 65% of the height",0.0,0.35,1.0,1.0);
  TPad *pad4 = new TPad("pad4", "The pad 35% of the height",0.0,0.0,1.0,0.35);
  pad3->Draw();
  pad4->Draw();


  TGraphErrors * gr4 = new TGraphErrors(nbin,etamedthin,mass_ratio,binw,mass_ratio_error);
  TGraphErrors * gr5 = new TGraphErrors(nbin,etamedthin,mass_ratio1,binw,mass_ratio1_error);

  TGraphErrors * gr7 = new TGraphErrors(nbin,etamedthin,sigma_ratio,binw,sigma_ratio_error);
  TGraphErrors * gr8 = new TGraphErrors(nbin,etamedthin,sigma_ratio1,binw,sigma_ratio1_error);

  TGraphErrors * gr10 = new TGraphErrors(nbin,etamedthin,mass_value_data,binw,mass_error_data);
  TGraphErrors * gr11 = new TGraphErrors(nbin,etamedthin,mass_value_mccorr,binw,mass_error_mccorr);
  TGraphErrors * gr12 = new TGraphErrors(nbin,etamedthin,mass_value_mcnocorr,binw,mass_error_mcnocorr);

  TGraphErrors * gr14 = new TGraphErrors(nbin,etamedthin,sigma_value_data,binw,sigma_error_data);
  TGraphErrors * gr15 = new TGraphErrors(nbin,etamedthin,sigma_value_mccorr,binw,sigma_error_mccorr);
  TGraphErrors * gr16 = new TGraphErrors(nbin,etamedthin,sigma_value_mcnocorr,binw,sigma_error_mcnocorr);

  TLine * line1 = new TLine(-2.5,1.,2.5,1.);
  TLine * line1mu = new TLine(-2.5,1.0005,2.5,1.0005); // mass
  TLine * line1md = new TLine(-2.5,0.9995,2.5,0.9995); // mass
  TLine * line1su = new TLine(-2.5,1.01,2.5,1.01); // sigma
  TLine * line1sd = new TLine(-2.5,0.99,2.5,0.99); // sigma
  line1->SetLineWidth(0.5);
  line1->SetLineColor(1);
  line1->SetLineStyle(7);
  line1mu->SetLineWidth(1.5);
  line1mu->SetLineColor(1);
  line1md->SetLineWidth(1.5);
  line1md->SetLineColor(1);
  line1su->SetLineWidth(1.0);
  line1su->SetLineColor(1);
  line1sd->SetLineWidth(1.0);
  line1sd->SetLineColor(1);

  gr4->SetMarkerStyle(20);
  gr4->SetMarkerColor(2);
  gr4->SetMarkerSize(1.0);
  gr4->SetLineColor(2);
  gr4->SetLineWidth(2.0);
  gr5->SetMarkerStyle(24);
  gr5->SetMarkerColor(2);
  gr5->SetMarkerSize(1.0);
  gr5->SetLineColor(2);

  gr7->SetMarkerStyle(20);
  gr7->SetMarkerColor(2);
  gr7->SetMarkerSize(1.0);
  gr7->SetLineColor(2);
  gr7->SetLineWidth(2.0);
  gr8->SetMarkerStyle(24);
  gr8->SetMarkerColor(2);
  gr8->SetMarkerSize(1.0);
  gr8->SetLineColor(2);

  gr10->SetMarkerStyle(20);
  gr10->SetMarkerColor(1);
  gr10->SetMarkerSize(1.0);
  gr10->SetLineColor(1);
  gr10->SetLineWidth(2.0);
  gr11->SetMarkerStyle(20);
  gr11->SetMarkerColor(2);
  gr11->SetMarkerSize(1.0);
  gr11->SetLineColor(2);
  gr11->SetLineWidth(2.0);
  gr12->SetMarkerStyle(24);
  gr12->SetMarkerColor(2);
  gr12->SetMarkerSize(1.0);
  gr12->SetLineColor(2);

  gr14->SetMarkerStyle(20);
  gr14->SetMarkerColor(1);
  gr14->SetMarkerSize(1.0);
  gr14->SetLineColor(1);
  gr14->SetLineWidth(2.0);
  gr15->SetMarkerStyle(20);
  gr15->SetMarkerColor(2);
  gr15->SetMarkerSize(1.0);
  gr15->SetLineColor(2);
  gr15->SetLineWidth(2.0);
  gr16->SetMarkerStyle(24);
  gr16->SetMarkerColor(2);
  gr16->SetMarkerSize(1.0);
  gr16->SetLineColor(2);

  pad1->cd();
  pad1->GetFrame()->SetFillColor(10);
  pad1->SetTicks(1,1);
  pad1->SetLeftMargin(0.14);
  pad1->SetBottomMargin(0.0);
  pad1->SetTopMargin(0.05);
  pad1->SetRightMargin(0.05);
  TMultiGraph * mg3 = new TMultiGraph();
  mg3->Add(gr10);
  mg3->Add(gr11);
  mg3->Add(gr12);
  mg3->Draw("APZ");
  mg3->SetTitle("");
  TAxis *axis = mg3->GetXaxis();           // set axis limits along X
  axis->SetLimits(-2.5,2.5);               // set axis limits along X
  mg3->GetHistogram()->SetMaximum(3.123);   // set axis limits along Y
  mg3->GetHistogram()->SetMinimum(3.082);   // set axis limits along Y
  mg3->GetXaxis()->SetTitleOffset(0.0);
  mg3->GetXaxis()->SetTitleSize(0.0);
  mg3->GetXaxis()->SetLabelSize(0.0);
  mg3->GetXaxis()->SetTitle("");
  mg3->GetYaxis()->SetTitleOffset(0.9);
  mg3->GetYaxis()->SetTitleSize(0.07);
  mg3->GetYaxis()->SetLabelSize(0.07);
  mg3->GetYaxis()->SetTitle("m_{#mu#mu}[GeV]");
  TLegend *legend = new TLegend(0.50,0.60,0.80,0.90);
  legend->AddEntry(gr10,"DATA","P");
  legend->AddEntry(gr11,"MC corrected","P");
  legend->AddEntry(gr12,"MC uncorrected","P");
  legend->SetTextSize(0.05);
  legend->SetHeader("Combined tracks");
  legend->Draw();

  pad2->cd();
  pad2->GetFrame()->SetFillColor(10);
  pad2->SetTicks(1,1);
  pad2->SetLeftMargin(0.14);
  pad2->SetBottomMargin(0.34);
  pad2->SetTopMargin(0.0);
  pad2->SetRightMargin(0.05);
  TMultiGraph * mg1 = new TMultiGraph();
  mg1->Add(gr4);
  mg1->Add(gr5);
  mg1->Draw("APZ");
  mg1->SetTitle("");
  TAxis *axis = mg1->GetXaxis();            // set axis limits along X
  axis->SetLimits(-2.5,2.5);                // set axis limits along X
  mg1->GetHistogram()->SetMaximum(1.004);   // set axis limits along Y
  mg1->GetHistogram()->SetMinimum(0.996);   // set axis limits along Y
  mg1->GetXaxis()->SetTitleOffset(1.0);
  mg1->GetXaxis()->SetTitleSize(0.14);
  mg1->GetXaxis()->SetLabelSize(0.12);
  mg1->GetXaxis()->SetTitle("#eta_{#mu lead}");
  mg1->GetYaxis()->SetTitleOffset(0.5);
  mg1->GetYaxis()->SetTitleSize(0.12);
  mg1->GetYaxis()->SetLabelSize(0.12);
  TAxis *yaxis = mg1->GetYaxis();
  yaxis->SetNdivisions(504,kTRUE);
  mg1->GetYaxis()->SetTitle("m_{#mu#mu}^{DATA}/m_{#mu#mu}^{MC}");
  line1->Draw("P");
  line1mu->Draw("P");
  line1md->Draw("P");

  cCBmass->Print("mass_vs_eta_of_pt1_CB_24.gif");

  pad3->cd();
  pad3->GetFrame()->SetFillColor(10);
  pad3->SetTicks(1,1);
  pad3->SetLeftMargin(0.14);
  pad3->SetBottomMargin(0.0);
  pad3->SetTopMargin(0.05);
  pad3->SetRightMargin(0.05);
  TMultiGraph * mg4 = new TMultiGraph();
  mg4->Add(gr14);
  mg4->Add(gr15);
  mg4->Add(gr16);
  mg4->Draw("APZ");
  TAxis *axis = mg4->GetXaxis();           // set axis limits along X
  axis->SetLimits(-2.5,2.5);               // set axis limits along X
  mg4->SetTitle("");
  mg4->GetHistogram()->SetMaximum(0.082);   // set axis limits along Y
  mg4->GetHistogram()->SetMinimum(0.031);   // set axis limits along Y
  mg4->GetXaxis()->SetTitleOffset(0.0);
  mg4->GetXaxis()->SetTitleSize(0.0);
  mg4->GetXaxis()->SetLabelSize(0.0);
  mg4->GetXaxis()->SetTitle("");
  mg4->GetYaxis()->SetTitleOffset(0.9);
  mg4->GetYaxis()->SetTitleSize(0.07);
  mg4->GetYaxis()->SetLabelSize(0.08);
  mg4->GetYaxis()->SetTitle("#sigma_{#mu#mu}[GeV]");
  TLegend *legend = new TLegend(0.50,0.60,0.80,0.90);
  legend->AddEntry(gr14,"DATA","P");
  legend->AddEntry(gr15,"MC corrected","P");
  legend->AddEntry(gr16,"MC uncorrected","P");
  legend->SetTextSize(0.05);
  legend->SetHeader("Combined tracks");
  legend->Draw();

  pad4->cd();
  pad4->GetFrame()->SetFillColor(10);
  pad4->SetTicks(1,1);
  pad4->SetLeftMargin(0.14);
  pad4->SetBottomMargin(0.34);
  pad4->SetTopMargin(0.0);
  pad4->SetRightMargin(0.05);
  TMultiGraph * mg2 = new TMultiGraph();
  mg2->Add(gr7);
  mg2->Add(gr8);
  mg2->Draw("APZ");
  TAxis *axis = mg2->GetXaxis();          // set axis limits along X
  axis->SetLimits(-2.5,2.5);              // set axis limits along X
  mg2->SetTitle("");
  mg2->GetHistogram()->SetMaximum(1.12);   // set axis limits along Y
  mg2->GetHistogram()->SetMinimum(0.90);   // set axis limits along Y
  mg2->GetXaxis()->SetTitleOffset(1.0);
  mg2->GetXaxis()->SetTitleSize(0.14);
  mg2->GetXaxis()->SetLabelSize(0.12);
  mg2->GetXaxis()->SetTitle("#eta_{#mu lead}");
  mg2->GetYaxis()->SetTitleOffset(0.5);
  mg2->GetYaxis()->SetTitleSize(0.12);
  mg2->GetYaxis()->SetLabelSize(0.12);
  TAxis *yaxis = mg2->GetYaxis();
  yaxis->SetNdivisions(506,kTRUE);
  mg2->GetYaxis()->SetTitle("#sigma_{#mu#mu}^{DATA}/#sigma_{#mu#mu}^{MC}");
  line1->Draw("P");
  line1su->Draw("P");
  line1sd->Draw("P");

  cCBresol->Print("sigma_vs_eta_of_pt1_CB_24.gif");

// PRINT SOME USEFUL VALUE FOR DEBUG

  for (jj=0; jj<netathin; jj++){
  cout << " mass value data " << mass_value_data[jj] << " sigma value data " << sigma_value_data[jj] << " e0 value data " << e0_value_data[jj] << endl;
  }
  for (jj=0; jj<netathin; jj++){
  cout << " mass value mccorr " << mass_value_mccorr[jj] << " sigma value mccorr " << sigma_value_mccorr[jj] << " e0 value mccorr " << e0_value_mccorr[jj] << endl;
  }
  for (jj=0; jj<netathin; jj++){
  cout << " mass value mnoccorr " << mass_value_mcnocorr[jj] << " sigma value mcnocorr " << sigma_value_mcnocorr[jj] << " e0 value mcnocorr " << e0_value_mcnocorr[jj] << endl;
  }
  for (jj=0; jj<netathin; jj++){
  cout << " chi2 value data " << chi2_data[jj] << " chi2 value model data " << chi2model_data[jj] << endl;
  }
  for (jj=0; jj<netathin; jj++){
  cout << " chi2 value mccorr " << chi2_mccorr[jj] << " chi2 value model mccorr " << chi2model_mccorr[jj] << endl;
  }
  for (jj=0; jj<netathin; jj++){
  cout << " chi2 value mcnocorr " << chi2_mcnocorr[jj] << " chi2 value model mcnocorr " << chi2model_mcnocorr[jj] << endl;
  }

}

