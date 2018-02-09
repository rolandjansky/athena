/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Fit Crystal Ball

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
#include "TLine.h"
using namespace RooFit ;

void fit_CrysBallBkgExp_ptavg_eta3eta4_ME_DATA_MC()
{

  int nbin=8;
  int nptavg=8;
  float binw[8];
  float ptbin[9];
  float ptmed[8];
  double xrebins[11] = {0.,7.,8.,9.,10.,12.,15.,20.,30.,50.,100.};
  double yrebins[151] = {1.500,1.520,1.540,1.560,1.580,1.600,1.620,1.640,1.660,1.680,1.700,1.720,1.740,1.760,1.780,1.800,1.820,1.840,1.860,1.880,1.900,1.920,1.940,1.960,1.980,2.000,2.020,2.040,2.060,2.080,2.100,2.120,2.140,2.160,2.180,2.200,2.220,2.240,2.260,2.280,2.300,2.320,2.340,2.360,2.380,2.400,2.420,2.440,2.460,2.480,2.500,2.520,2.540,2.560,2.580,2.600,2.620,2.640,2.660,2.680,2.700,2.720,2.740,2.760,2.780,2.800,2.820,2.840,2.860,2.880,2.900,2.920,2.940,2.960,2.980,3.000,3.020,3.040,3.060,3.080,3.100,3.120,3.140,3.160,3.180,3.200,3.220,3.240,3.260,3.280,3.300,3.320,3.340,3.360,3.380,3.400,3.420,3.440,3.460,3.480,3.500,3.520,3.540,3.560,3.580,3.600,3.620,3.640,3.660,3.680,3.700,3.720,3.740,3.760,3.780,3.800,3.820,3.840,3.860,3.880,3.900,3.920,3.940,3.960,3.980,4.000,4.020,4.040,4.060,4.080,4.100,4.120,4.140,4.160,4.180,4.200,4.220,4.240,4.260,4.280,4.300,4.320,4.340,4.360,4.380,4.400,4.420,4.440,4.460,4.480,4.500};
  float mass_value_data[8];
  float mass_value_mccorr[8];
  float mass_value_mcnocorr[8];
  float mass_error_data[8];
  float mass_error_mccorr[8];
  float mass_error_mcnocorr[8];
  float sigma_value_data[8];
  float sigma_value_mccorr[8];
  float sigma_value_mcnocorr[8];
  float sigma_error_data[8];
  float sigma_error_mccorr[8];
  float sigma_error_mcnocorr[8];
  float e0_value[8];
  float mass_ratio[8];
  float mass_ratio1[8];
  float mass_ratio_error[8];
  float mass_ratio1_error[8];
  float sigma_ratio[8];
  float sigma_ratio1[8];
  float sigma2_ratio[8];
  float sigma_ratio_error[8];
  float sigma_ratio1_error[8];
  float sigma2_ratio[8];
  float mass_data_PDG[8];
  float mass_mccorr_PDG[8];
  float mass_mcnocorr_PDG[8];

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

  char fileInDATA[100];
  char fileInMCCORR[100];
  char fileInMCNOCORR[100];

  sprintf(fileInDATA,"muonresonances-DATA.root");
  TFile * MyFileDATA = TFile::Open(fileInDATA);

  sprintf(fileInMCCORR,"muonresonances-MC-CORR.root");
  TFile * MyFileMCCORR = TFile::Open(fileInMCCORR);

  sprintf(fileInMCNOCORR,"muonresonances-MC-NOCORR.root");
  TFile * MyFileMCNOCORR = TFile::Open(fileInMCNOCORR);

// --------------------------------- DATA

  TCanvas* cMEdata = new TCanvas("CrystalBallBkgExp - ME - DATA","CrystalBallBkgExp - ME - DATA",1100,660) ;
  cMEdata->Divide(5,3) ;

  TDirectory * MyDirDATA = MyFileDATA->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

  TH1 * h1data;
  MyDirDATA->GetObject("Jpsimm_h_Zm_mu_avPt_eta_0_ME_",h1data);
  h1data->Print();
  cMEdata->cd(1);
  h1data->Draw();

  TH2 * h2data;
  TH2 * h2data_2;
  MyDirDATA->GetObject("Jpsimm_h_Zm_mu_avPt_eta_3_ME_",h2data);
  h2data->Print();
  MyDirDATA->GetObject("Jpsimm_h_Zm_mu_avPt_eta_4_ME_",h2data_2);
  h2data_2->Print();
  h2data->Add(h2data_2); 
  h2data->Print();
  cMEdata->cd(2);
  h2data->Draw();
  h2data->GetYaxis()->SetLimits(1.5,4.5);
  h2data->GetXaxis()->SetLimits(0.0,100.0);
  cMEdata->cd(3);
  h2data->Draw();

// REBIN PT AVG

  TH2D * h2datanew = new TH2D("h2datarebin",h2data->GetTitle(),10,xrebins,150,yrebins);
  TAxis * xaxis = h2data->GetXaxis();
  TAxis * yaxis = h2data->GetYaxis();
  for (int j=1; j<=yaxis->GetNbins();j++) {
     for (int i=1; i<=xaxis->GetNbins();i++) {
        h2datanew->Fill(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j),h2data->GetBinContent(i,j));
     }
  }
  h2datanew->Print();
  cMEdata->cd(4);
  h2datanew->Draw();

// Retrive pt bin width and limits

  for (jj=0; jj<nptavg; jj++){
    bl = h2datanew->GetXaxis()->GetBinLowEdge(jj+2);
    bw = h2datanew->GetXaxis()->GetBinWidth(jj+2);
    binw[jj] = bw/2;
    ptbin[jj] = bl;
    if (jj==(nptavg-1)) bl = h2datanew->GetXaxis()->GetBinUpEdge(jj+2);
    ptbin[jj+1] = bl;
  }

// DO PROJECTION

  for (jj=0; jj<nptavg; jj++){

    int bin=jj+2;
    sprintf(hname1,"py_data_%d",jj+2);
    TH1 * hpdata = h2datanew->ProjectionY(hname1, bin, bin);
    hpdata->Print();

// Crystal Ball

    RooRealVar m("m","m",2.0,4.5); //x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.0,1.);
    RooRealVar alpha("alpha","alpha",2.0,0.0,10.0);
    RooRealVar n("n","n",1.5,0.0,20.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n);

// rescale della differenza di massa tenendo conto della scala di energia

    RooRealVar shift("shift","shift",1.19);
    RooFormulaVar m0_shift("m0_shift","m0*shift",RooArgSet(m0,shift));
//    RooFormulaVar m0_shift("m0_shift","@0*@1",RooArgSet(m0,shift));

    RooCBShape cb1("cb1","cb1",m,m0_shift,sigma,alpha,n) ;

    RooRealVar frac("frac","frac",0.95,0.,1.); // frazione della Jpsi
    RooAddPdf sig("sig","sig",RooArgList(cb,cb1),frac);

// Exponential Bkg

    RooRealVar e0("e0","e0", -1.5, -1000.,1000.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",0.65,0.,1.);
    RooAddPdf model("model","model",RooArgList(sig,bkg),fsig);

//    RooDataHist data("data","data",m,Import(*hh)) ;
    RooDataHist data("data","dataset with m",m,hpdata);
    RooPlot* myframe = m.frame(Title(hname1));
    data.plotOn(myframe);

    model.fitTo(data, Range(2.2,4.0) );
    model.plotOn(myframe);
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));

//    model.paramOn(myframe);
    data.statOn(myframe);

    int nc1=jj+5;
    cMEdata->cd(nc1);
    myframe->Draw();
//    myframe->Clear();

//    RooFitResult* results = cb.fitTo(data, Save() );
//    results->floatParsFinal().Print("s") ;
//    model.Print("t");
    RooArgSet* params = model.getParameters(m);
    params->Print("v");

//    sprintf(hnamefile1,"ME_results_data_%d",jj+1);
//    params->writeToFile(hnamefile1);
    m0.Print();
    mass_value_data[jj] = m0.getVal();
    mass_error_data[jj] = m0.getError();
    sigma_value_data[jj] = sigma.getVal();
    sigma_error_data[jj] = sigma.getError();
    e0_value[jj] = e0.getVal();

  }

  for (jj=0; jj<nptavg; jj++){
    ptmed[jj]=(ptbin[jj+1]+ptbin[jj])/2;
  }

//  gStyle->SetEndErrorSize(1)
  
  TGraphErrors * gr1 = new TGraphErrors(nbin,ptmed,mass_value_data,binw,mass_error_data);
  cMEdata->cd(14);
  gr1->SetTitle("Jpsi mass DATA");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

  TGraphErrors * gr2 = new TGraphErrors(nbin,ptmed,sigma_value_data,binw,sigma_error_data);
  cMEdata->cd(15);
  gr2->SetTitle("Jpsi width DATA");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");

// -------------------------------------- MC CORR

  TCanvas* cMEmccorr = new TCanvas("CrystalBallBkgExp - ME - MCCORR","CrystalBallBkgExp - ME - MCCORR",1100,660) ;
  cMEmccorr->Divide(5,3) ;

  TDirectory * MyDirMCCORR = MyFileMCCORR->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

  TH1 * h1mccorr;
  MyDirMCCORR->GetObject("Jpsimm_h_Zm_mu_avPt_eta_0_ME_",h1mccorr);
  h1mccorr->Print();
  cMEmccorr->cd(1);
  h1mccorr->Draw();

  TH2 * h2mccorr;
  TH2 * h2mccorr_2;
  MyDirMCCORR->GetObject("Jpsimm_h_Zm_mu_avPt_eta_3_ME_",h2mccorr);
  MyDirMCCORR->GetObject("Jpsimm_h_Zm_mu_avPt_eta_4_ME_",h2mccorr_2);
  h2mccorr->Add(h2mccorr_2);
  h2mccorr->Print();
  cMEmccorr->cd(2);
  h2mccorr->Draw();
  h2mccorr->GetXaxis()->SetLimits(0.0,100.0);
  h2mccorr->GetYaxis()->SetLimits(1.5,4.5);
  cMEmccorr->cd(3);
  h2mccorr->Draw();

// REBIN PT AVG

  TH2D * h2mccorrnew = new TH2D("h2mccorrrebin",h2mccorr->GetTitle(),10,xrebins,150,yrebins);
  TAxis * xaxis = h2mccorr->GetXaxis();
  TAxis * yaxis = h2mccorr->GetYaxis();
  for (int j=1; j<=yaxis->GetNbins();j++) {
     for (int i=1; i<=xaxis->GetNbins();i++) {
        h2mccorrnew->Fill(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j),h2mccorr->GetBinContent(i,j));
     }
  }
  h2mccorrnew->Print();
  cMEmccorr->cd(4);
  h2mccorrnew->Draw();

// DO PROJECTION

  for (jj=0; jj<nptavg; jj++){

    int bin=jj+2;
    sprintf(hname2,"py_mccorr_%d",jj+2);
    TH1 * hpmccorr = h2mccorrnew->ProjectionY(hname2, bin, bin);
    hpmccorr->Print();

// Crystal Ball

    RooRealVar m("m","m",2.0,4.5); //x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.0,1.);
    RooRealVar alpha("alpha","alpha",2.,0.0,10.0);
    RooRealVar n("n","n",5.0,0.0,50.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n);

// Exponential Bkg

    RooRealVar e0("e0","e0", -0.5, -1000.,1000.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",1.0,0.,1.);
    RooAddPdf model("model","model",RooArgList(cb,bkg),fsig);

    RooDataHist data("data","dataset with m",m,hpmccorr);
    RooPlot* myframe = m.frame(Title(hname2));
    data.plotOn(myframe);

    model.fitTo(data, Range(2.2,3.9), SumW2Error(kFALSE) );
    model.plotOn(myframe);
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));

    model.paramOn(myframe);
    int nc2=jj+5;
    cMEmccorr->cd(nc2);
    myframe->Draw();
//    myframe->Clear();

    RooArgSet* params = model.getParameters(m);
    params->Print("v");

//    sprintf(hnamefile2,"ME_results_mccorr_%d",jj+1);
//    params->writeToFile(hnamefile2);
    m0.Print();
    mass_value_mccorr[jj] = m0.getVal();
    mass_error_mccorr[jj] = m0.getError();
    sigma_value_mccorr[jj] = sigma.getVal();
    sigma_error_mccorr[jj] = sigma.getError();
    e0_value[jj] = e0.getVal();

  }

  for (jj=0; jj<nptavg; jj++){
    ptmed[jj]=(ptbin[jj+1]+ptbin[jj])/2;
  }


  TGraphErrors * gr1 = new TGraphErrors(nbin,ptmed,mass_value_mccorr,binw,mass_error_mccorr);
  cMEmccorr->cd(14);
  gr1->SetTitle("Jpsi mass");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

  TGraphErrors * gr2 = new TGraphErrors(nbin,ptmed,sigma_value_mccorr,binw,sigma_error_mccorr);
  cMEmccorr->cd(15);
  gr2->SetTitle("Jpsi width");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");

// -------------------------------------- MC NO CORR

  TCanvas* cMEmcnocorr = new TCanvas("CrystalBallBkgExp - ME - MCNOCORR","CrystalBallBkgExp - ME - MCNOCORR",1100,660) ;
  cMEmcnocorr->Divide(5,3) ;

  TDirectory * MyDirMCNOCORR = MyFileMCNOCORR->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

  TH1 * h1mcnocorr;
  MyDirMCNOCORR->GetObject("Jpsimm_h_Zm_mu_avPt_eta_0_ME_",h1mcnocorr);
  h1mcnocorr->Print();
  cMEmcnocorr->cd(1);
  h1mcnocorr->Draw();

  TH2 * h2mcnocorr;
  TH2 * h2mcnocorr_2;
  MyDirMCNOCORR->GetObject("Jpsimm_h_Zm_mu_avPt_eta_3_ME_",h2mcnocorr);
  MyDirMCNOCORR->GetObject("Jpsimm_h_Zm_mu_avPt_eta_4_ME_",h2mcnocorr_2);
  h2mcnocorr->Add(h2mcnocorr_2);
  h2mcnocorr->Print();
  cMEmcnocorr->cd(2);
  h2mcnocorr->Draw();
  h2mcnocorr->GetXaxis()->SetLimits(0.0,100.0);
  h2mcnocorr->GetYaxis()->SetLimits(1.5,4.5);
  cMEmcnocorr->cd(3);
  h2mcnocorr->Draw();

// REBIN PT AVG

  TH2D * h2mcnocorrnew = new TH2D("h2mcnocorrbin",h2mcnocorr->GetTitle(),10,xrebins,150,yrebins);
  TAxis * xaxis = h2mcnocorr->GetXaxis();
  TAxis * yaxis = h2mcnocorr->GetYaxis();
  for (int j=1; j<=yaxis->GetNbins();j++) {
     for (int i=1; i<=xaxis->GetNbins();i++) {
        h2mcnocorrnew->Fill(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j),h2mcnocorr->GetBinContent(i,j));
     }
  }
  h2mcnocorrnew->Print();
  cMEmcnocorr->cd(4);
  h2mcnocorrnew->Draw();

  for (jj=0; jj<nptavg; jj++){

    int bin=jj+2;
    sprintf(hname3,"py_mcnocorr_%d",jj+2);
    TH1 * hpmcnocorr = h2mcnocorrnew->ProjectionY(hname3, bin, bin);
    hpmcnocorr->Print();

// Crystal Ball

    RooRealVar m("m","m",2.0,4.5); //x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.0,1.);
    RooRealVar alpha("alpha","alpha",2.,0.0,10.0);
    RooRealVar n("n","n",5.0,0.0,50.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n);

// Exponential Bkg

    RooRealVar e0("e0","e0", -0.5, -1000.,1000.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",1.0,0.,1.);
    RooAddPdf model("model","model",RooArgList(cb,bkg),fsig);

    RooDataHist data("data","dataset with m",m,hpmcnocorr);
    RooPlot* myframe = m.frame(Title(hname3));
    data.plotOn(myframe);

    model.fitTo(data, Range(2.2,3.9), SumW2Error(kFALSE) );
    model.plotOn(myframe);
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));

    model.paramOn(myframe);
    int nc2=jj+5;
    cMEmcnocorr->cd(nc2);
    myframe->Draw();
//    myframe->Clear();

    RooArgSet* params = model.getParameters(m);
    params->Print("v");

//    sprintf(hnamefile3,"ME_results_mcnocorr_%d",jj+1);
//    params->writeToFile(hnamefile3);
    m0.Print();
    mass_value_mcnocorr[jj] = m0.getVal();
    mass_error_mcnocorr[jj] = m0.getError();
    sigma_value_mcnocorr[jj] = sigma.getVal();
    sigma_error_mcnocorr[jj] = sigma.getError();
    e0_value[jj] = e0.getVal();

  }

  for (jj=0; jj<nptavg; jj++){
    ptmed[jj]=(ptbin[jj+1]+ptbin[jj])/2;
  }

  TGraphErrors * gr1 = new TGraphErrors(nbin,ptmed,mass_value_mcnocorr,binw,mass_error_mcnocorr);
  cMEmcnocorr->cd(14);
  gr1->SetTitle("Jpsi mass");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

  TGraphErrors * gr2 = new TGraphErrors(nbin,ptmed,sigma_value_mcnocorr,binw,sigma_error_mcnocorr);
  cMEmcnocorr->cd(15);
  gr2->SetTitle("Jpsi width");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");


// --------------------- DATA and MC ---------------------------------------

  for (jj=0; jj<nptavg; jj++){
    mass_ratio[jj]=mass_value_data[jj]/mass_value_mccorr[jj];
    mass_ratio1[jj]=mass_value_data[jj]/mass_value_mcnocorr[jj];
//    mass_ratio_error[jj]=sqrt((mass_error_data[jj]*mass_error_data[jj])/(mass_value_mccorr[jj]*mass_value_mccorr[jj]) + (mass_value_data[jj]*mass_value_data[jj]*mass_error_mccorr[jj]*mass_error_mccorr[jj])/(mass_value_mccorr[jj]*mass_value_mccorr[jj]));  // Claudia
    mass_ratio_error[jj]=sqrt((mass_error_data[jj]*mass_error_data[jj])/mass_value_mccorr[jj] + (mass_error_mccorr[jj]*mass_error_mccorr[jj]*mass_value_data[jj])/(mass_value_mccorr[jj]*mass_value_mccorr[jj])); // Massimo
    mass_ratio1_error[jj]=sqrt((mass_error_data[jj]*mass_error_data[jj])/mass_value_mcnocorr[jj] + (mass_error_mcnocorr[jj]*mass_error_mcnocorr[jj]*mass_value_data[jj])/(mass_value_mcnocorr[jj]*mass_value_mcnocorr[jj])); // Massimo
    sigma_ratio[jj]=sigma_value_data[jj]/sigma_value_mccorr[jj];
    sigma_ratio1[jj]=sigma_value_data[jj]/sigma_value_mcnocorr[jj];
    sigma_ratio_error[jj]=sqrt((sigma_error_data[jj]*sigma_error_data[jj])/sigma_value_mccorr[jj] + (sigma_error_mccorr[jj]*sigma_error_mccorr[jj]*sigma_value_data[jj])/(sigma_value_mccorr[jj]*sigma_value_mccorr[jj])); // Massimo
    sigma_ratio1_error[jj]=sqrt((sigma_error_data[jj]*sigma_error_data[jj])/sigma_value_mcnocorr[jj] + (sigma_error_mcnocorr[jj]*sigma_error_mcnocorr[jj]*sigma_value_data[jj])/(sigma_value_mcnocorr[jj]*sigma_value_mcnocorr[jj])); // Massimo
    mass_data_PDG[jj]=mass_value_data[jj]/mass_Jpsi;
    mass_mccorr_PDG[jj]=mass_value_mccorr[jj]/mass_Jpsi;
    mass_mcnocorr_PDG[jj]=mass_value_mcnocorr[jj]/mass_Jpsi;
  }


  TCanvas* cMEratio = new TCanvas(" CrystalBallBkgExp - ME - DATA and MC "," CrystalBallBkgExp - ME - DATA and MC",1100,660) ;
  cMEratio->Divide(2,2) ;

  TGraphErrors * gr4 = new TGraphErrors(nbin,ptmed,mass_ratio,binw,mass_ratio_error);
  TGraphErrors * gr5 = new TGraphErrors(nbin,ptmed,mass_ratio1,binw,mass_ratio1_error);

  TGraphErrors * gr7 = new TGraphErrors(nbin,ptmed,sigma_ratio,binw,sigma_ratio_error);
  TGraphErrors * gr8 = new TGraphErrors(nbin,ptmed,sigma_ratio1,binw,sigma_ratio1_error);

  TGraphErrors * gr10 = new TGraphErrors(nbin,ptmed,mass_value_data,binw,mass_error_data);
  TGraphErrors * gr11 = new TGraphErrors(nbin,ptmed,mass_value_mccorr,binw,mass_error_mccorr);
  TGraphErrors * gr12 = new TGraphErrors(nbin,ptmed,mass_value_mcnocorr,binw,mass_error_mcnocorr);

  TGraphErrors * gr14 = new TGraphErrors(nbin,ptmed,sigma_value_data,binw,sigma_error_data);
  TGraphErrors * gr15 = new TGraphErrors(nbin,ptmed,sigma_value_mccorr,binw,sigma_error_mccorr);
  TGraphErrors * gr16 = new TGraphErrors(nbin,ptmed,sigma_value_mcnocorr,binw,sigma_error_mcnocorr);

  TLine * line1 = new TLine(7.0,1.,50.0,1.);
  line1->SetLineWidth(0.5);
  line1->SetLineColor(1);

  gr4->SetMarkerStyle(20);
  gr4->SetMarkerColor(4);
  gr4->SetMarkerSize(1.0);
  gr5->SetMarkerStyle(24);
  gr5->SetMarkerColor(4);
  gr5->SetMarkerSize(1.0);

  gr7->SetMarkerStyle(20);
  gr7->SetMarkerColor(4);
  gr7->SetMarkerSize(1.0);
  gr8->SetMarkerStyle(24);
  gr8->SetMarkerColor(4);
  gr8->SetMarkerSize(1.0);

  gr10->SetMarkerStyle(20);
  gr10->SetMarkerColor(2);
  gr10->SetMarkerSize(1.0);
  gr11->SetMarkerStyle(20);
  gr11->SetMarkerColor(4);
  gr11->SetMarkerSize(1.0);
  gr12->SetMarkerStyle(24);
  gr12->SetMarkerColor(4);
  gr12->SetMarkerSize(1.0);

  gr14->SetMarkerStyle(20);
  gr14->SetMarkerColor(2);
  gr14->SetMarkerSize(1.0);
  gr15->SetMarkerStyle(20);
  gr15->SetMarkerColor(4);
  gr15->SetMarkerSize(1.0);
  gr16->SetMarkerStyle(24);
  gr16->SetMarkerColor(4);
  gr16->SetMarkerSize(1.0);

  cMEratio->cd(1);
  TPad * pad = (TPad*)cMEratio->GetPad(1);
  pad->SetLogx();
  pad->SetLeftMargin(0.14);
  pad->SetBottomMargin(0.14);
  pad->SetTopMargin(0.03);
  pad->SetRightMargin(0.05); 
  TMultiGraph * mg1 = new TMultiGraph();
  mg1->Add(gr4);
  mg1->Add(gr5);
  mg1->Draw("AP");
  TAxis *axis = mg1->GetXaxis();            // set axis limits along X
  axis->SetLimits(6.0,60.0);                // set axis limits along X
  mg1->SetTitle("");
  mg1->GetHistogram()->SetMaximum(1.01);   // set axis limits along Y
  mg1->GetHistogram()->SetMinimum(0.99);   // set axis limits along Y
  mg1->GetXaxis()->SetTitleOffset(0.9);
  mg1->GetXaxis()->SetTitleSize(0.07);
  mg1->GetXaxis()->SetLabelSize(0.05);
  mg1->GetXaxis()->SetTitleFont(42);
  mg1->GetXaxis()->SetTitle("#bar{p}_{T}");
  mg1->GetYaxis()->SetTitleOffset(0.9);
  mg1->GetYaxis()->SetTitleSize(0.07);
  mg1->GetYaxis()->SetLabelSize(0.05);
  mg1->GetYaxis()->SetTitle("m_{#mu#mu}^{DATA}/m_{#mu#mu}^{MC}");
  line1->Draw("P");
  TLegend *legend = new TLegend(0.40,0.83,0.70,0.97);
  legend->AddEntry(gr4,"corrected MC ","P");
  legend->AddEntry(gr5,"uncorrected MC","P");
  legend->SetTextSize(0.05);
  legend->Draw();

  cMEratio->cd(2);
  TPad * pad = (TPad*)cMEratio->GetPad(2);
  pad->SetLogx();
  pad->SetLeftMargin(0.14);
  pad->SetBottomMargin(0.14);
  pad->SetTopMargin(0.03);
  pad->SetRightMargin(0.05);
  TMultiGraph * mg2 = new TMultiGraph();
  mg2->Add(gr7);
  mg2->Add(gr8);
  mg2->Draw("AP");
  TAxis *axis = mg2->GetXaxis();            // set axis limits along X
  axis->SetLimits(6.0,60.0);                // set axis limits along X
  mg2->SetTitle("");
  mg2->GetHistogram()->SetMaximum(1.2);   // set axis limits along Y
  mg2->GetHistogram()->SetMinimum(0.8);   // set axis limits along Y
  mg2->GetXaxis()->SetTitleOffset(0.9);
  mg2->GetXaxis()->SetTitleSize(0.07);
  mg2->GetXaxis()->SetLabelSize(0.05);
  mg2->GetXaxis()->SetTitleFont(62);
  mg2->GetXaxis()->SetTitle("#bar{p}_{T}");
  mg2->GetYaxis()->SetTitleOffset(0.9);
  mg2->GetYaxis()->SetTitleSize(0.07);
  mg2->GetYaxis()->SetLabelSize(0.05);
  mg2->GetYaxis()->SetTitle("#sigma_{#mu#mu}^{DATA}/#sigma_{#mu#mu}^{MC}");
  line1->Draw("P");
  TLegend *legend = new TLegend(0.40,0.83,0.70,0.97);
  legend->AddEntry(gr7,"corrected MC ","P");
  legend->AddEntry(gr8,"uncorrected MC","P");
  legend->SetTextSize(0.05);
  legend->Draw();

  cMEratio->cd(3);
  TPad * pad = (TPad*)cMEratio->GetPad(3);
  pad->SetLogx();
  pad->SetLeftMargin(0.14);
  pad->SetBottomMargin(0.14);
  pad->SetTopMargin(0.03);
  pad->SetRightMargin(0.05);
  TMultiGraph * mg3 = new TMultiGraph();
  mg3->Add(gr10);
  mg3->Add(gr11);
  mg3->Add(gr12);
  mg3->Draw("AP");
  TAxis *axis = mg3->GetXaxis();            // set axis limits along X
  axis->SetLimits(6.0,60.0);                // set axis limits along X
  mg3->SetTitle("");
  mg3->GetHistogram()->SetMaximum(3.15);   // set axis limits along Y
  mg3->GetHistogram()->SetMinimum(3.05);   // set axis limits along Y
  mg3->GetXaxis()->SetTitleOffset(0.9);
  mg3->GetXaxis()->SetTitleSize(0.07);
  mg3->GetXaxis()->SetLabelSize(0.05);
  mg3->GetXaxis()->SetTitleFont(102);
  mg3->GetXaxis()->SetTitle("#bar{p}_{T}");
  mg3->GetYaxis()->SetTitleOffset(0.9);
  mg3->GetYaxis()->SetTitleSize(0.07);
  mg3->GetYaxis()->SetLabelSize(0.05);
  mg3->GetYaxis()->SetTitle("m_{#mu#mu}[GeV]");
  TLegend *legend = new TLegend(0.40,0.77,0.70,0.97);
  legend->AddEntry(gr10,"DATA ","P");
  legend->AddEntry(gr11,"corrected MC ","P");
  legend->AddEntry(gr12,"uncorrected MC","P");
  legend->SetTextSize(0.05);
  legend->Draw();

  cMEratio->cd(4);
  TPad * pad = (TPad*)cMEratio->GetPad(4);
  pad->SetLogx();
  pad->SetLeftMargin(0.14);
  pad->SetBottomMargin(0.14);
  pad->SetTopMargin(0.03);
  pad->SetRightMargin(0.05);
  TMultiGraph * mg4 = new TMultiGraph();
  mg4->Add(gr14);
  mg4->Add(gr15);
  mg4->Add(gr16);
  mg4->Draw("AP");
  TAxis *axis = mg4->GetXaxis();           // set axis limits along X
  axis->SetLimits(6.0,60.0);               // set axis limits along X
  mg4->SetTitle("");
  mg4->GetHistogram()->SetMaximum(0.25);   // set axis limits along Y
  mg4->GetHistogram()->SetMinimum(0.15);   // set axis limits along Y
  mg4->GetXaxis()->SetTitleOffset(0.9);
  mg4->GetXaxis()->SetTitleSize(0.07);
  mg4->GetXaxis()->SetLabelSize(0.05);
  mg4->GetXaxis()->SetTitleFont(132);
  mg4->GetXaxis()->SetTitle("#bar{p}_{T}");
  mg4->GetYaxis()->SetTitleOffset(0.9);
  mg4->GetYaxis()->SetTitleSize(0.07);
  mg4->GetYaxis()->SetLabelSize(0.05);
  mg4->GetYaxis()->SetTitle("#sigma_{#mu#mu}[GeV]");
  TLegend *legend = new TLegend(0.20,0.77,0.50,0.97);
  legend->AddEntry(gr14,"DATA ","P");
  legend->AddEntry(gr15,"corrected MC ","P");
  legend->AddEntry(gr16,"uncorrected MC","P");
  legend->SetTextSize(0.05);
  legend->Draw();

  cMEratio->Print("mass_and_sigma_vs_ptavg_eta3eta4_ME.gif");

//  TGraph * gr6 = new TGraph(20,ptmed,mass_data_PDG);
//  cMEratio->cd(3);
//  gr6->SetTitle("Jpsi mass DATA / mass PDG");
//  gr6->SetMarkerStyle(29);
//  gr6->SetMarkerColor(1);
//  gr6->Draw("AP");

//  TGraph * gr7 = new TGraph(20,ptmed,mass_mccorr_PDG);
//  cMEratio->cd(4);
//  gr7->SetTitle("Jpsi mass MC / mass PDG");
//  gr7->SetMarkerStyle(29);
//  gr7->SetMarkerColor(1);
//  gr7->Draw("AP");


  for (jj=0; jj<nptavg; jj++){
  cout << " mass value data " << mass_value_data[jj] << " sigma value data " << sigma_value_data[jj] << endl;
  }
  for (jj=0; jj<nptavg; jj++){
  cout << " mass value mccorr " << mass_value_mccorr[jj] << " sigma value mccorr " << sigma_value_mccorr[jj] << endl;
  }
  for (jj=0; jj<nptavg; jj++){
  cout << " mass value mnoccorr " << mass_value_mcnocorr[jj] << " sigma value mcnocorr " << sigma_value_mcnocorr[jj] << endl;
  }
  for (jj=0; jj<nptavg; jj++){
  cout << " binw " << binw[jj] << endl;
  }
  for (jj=0; jj<nptavg+1; jj++){
  cout << " ptbin " << ptbin[jj] << endl;
  }

}

