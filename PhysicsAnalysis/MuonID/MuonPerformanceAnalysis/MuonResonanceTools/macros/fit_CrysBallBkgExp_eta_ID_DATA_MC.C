/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This macro fits Jpsi in 20 bin of eta, from -25 to 2.5 as defined in MuonJpsi... , etabin=0.25 
// The fit is a Crystal Ball with Exponential Background
// Muon combined 
// DATA and MC with and without momentum corrections

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

void fit_CrysBallBkgExp_eta_ID_DATA_MC()
{

  int nbin=20;
  int neta=20;
  float binw[20];
  float etabin[21];
  float etamed[20];
  float mass_value_data[20];
  float mass_value_mccorr[20];
  float mass_value_mcnocorr[20];
  float mass_error_data[20];
  float mass_error_mccorr[20];
  float mass_error_mcnocorr[20];
  float sigma_value_data[20];
  float sigma_value_mccorr[20];
  float sigma_value_mcnocorr[20];
  float sigma_error_data[20];
  float sigma_error_mccorr[20];
  float sigma_error_mcnocorr[20];
  float e0_value[20];
  float mass_ratio[20];
  float mass_ratio1[20];
  float mass_ratio_error[20];
  float mass_ratio1_error[20];
  float sigma_ratio[20];
  float sigma_ratio1[20];
  float sigma2_ratio[20];
  float sigma_ratio_error[20];
  float sigma_ratio1_error[20];
  float sigma2_ratio[20];
  float mass_data_PDG[20];
  float mass_mccorr_PDG[20];
  float mass_mcnocorr_PDG[20];

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

// Open your root files

  sprintf(fileInDATA,"muonresonances-DATA.root");
  TFile * MyFileDATA = TFile::Open(fileInDATA);

  sprintf(fileInMCCORR,"muonresonances-MC-CORR.root");
  TFile * MyFileMCCORR = TFile::Open(fileInMCCORR);

  sprintf(fileInMCNOCORR,"muonresonances-MC-NOCORR.root");
  TFile * MyFileMCNOCORR = TFile::Open(fileInMCNOCORR);

// --------------------------------- DATA

  TCanvas* cIDdata = new TCanvas("CrystalBallBkgExp - ID - DATA","CrystalBallBkgExp - ID - DATA",1100,660) ;
  cIDdata->Divide(6,4) ;

  TCanvas* cIDfitdata = new TCanvas("fit CrystalBallBkgExp - ID - DATA","fit CrystalBallBkgExp - ID - DATA",660,660) ;

  TDirectory * MyDirDATA = MyFileDATA->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

  TH1 * h1data;
  MyDirDATA->GetObject("Jpsimm_Mmumu_eta_0_ID_",h1data);
  h1data->Print();
  cIDdata->cd(1);
  h1data->Draw();

// GET 2D HISTO Jpsi mass vs eta muon combined

  TH2 * h2data;
  MyDirDATA->GetObject("Jpsimm_h_Zm_Eta_eta_0_ID_",h2data);
  h2data->Print();
  cIDdata->cd(2);
  h2data->Draw();

// Retrive eta bin width and limits

  for (jj=0; jj<neta; jj++){
    bl = h2data->GetXaxis()->GetBinLowEdge(jj+1);
    bw = h2data->GetXaxis()->GetBinWidth(jj+1);
    binw[jj] = bw/2;
    etabin[jj] = bl;
    if (jj==(neta-1)) bl = h2data->GetXaxis()->GetBinUpEdge(jj+1);
    etabin[jj+1] = bl;
  }

// DO PROJECTIONS -> RESULTS: 1D histos of Jpsi mass 

  for (jj=0; jj<neta; jj++){

    int bin=jj+1;
    sprintf(hname1,"py_data_%d",jj+1);
    TH1 * hpdata = h2data->ProjectionY(hname1, bin, bin);
    hpdata->Print();
    hpdata->GetXaxis()->SetLimits(1.5,4.5);

// DO FIT - CRYSTAL BALL

    RooRealVar m("m","m",2.0,4.5); //x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.0,1.);
    RooRealVar alpha("alpha","alpha",2.0,0.0,10.0);
    RooRealVar n("n","n",1.5,0.0,20.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n);

// Rescale of the mass ratioerence taking into account the energy scale

    RooRealVar shift("shift","shift",1.19);
//    RooFormulaVar m0_shift("m0_shift","m0*shift",RooArgSet(m0,shift));
    RooFormulaVar m0_shift("m0_shift","@0*@1",RooArgSet(m0,shift));

    RooCBShape cb1("cb1","cb1",m,m0_shift,sigma,alpha,n) ;

    RooRealVar frac("frac","frac",0.95,0.,1.); // frazione della Jpsi
    RooAddPdf sig("sig","sig",RooArgList(cb,cb1),frac);

// Exponential Bkg

    RooRealVar e0("e0","e0", -1.5, -1000.,1000.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",0.65,0.,1.);
    RooAddPdf model("model","model",RooArgList(sig,bkg),fsig);

// RooDataHist data("data","data",m,Import(*hh)) ;
    RooDataHist data("data","dataset with m",m,hpdata);
    RooPlot* myframe = m.frame(Title(hname1));
    data.plotOn(myframe);

    model.fitTo(data, Range(2.6,4.0) );
    model.plotOn(myframe);
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));
//    data->plotOn(myframe,DataError(RooAbsData::SumW2)) 

    data.statOn(myframe);   // DRAW 

    int nc1=jj+3;
    cIDdata->cd(nc1);
    myframe->Draw();
    if (jj==6){
//      float chi2 = myframe->chiSquare();
//      cout << " chi2 " << chi2 << endl;
      cIDfitdata->cd();
      model.paramOn(myframe);  // DRAW FIT PARAMETERS
      myframe->Draw();
    }

//    RooFitResult* results = cb.fitTo(data, Save() );
//    results->floatParsFinal().Print("s") ;
//    model.Print("t");
    RooArgSet* params = model.getParameters(m);
    params->Print("v");

//    sprintf(hnamefile1,"ID_results_data_%d",jj+1);
//    params->writeToFile(hnamefile1);
    m0.Print();
    mass_value_data[jj] = m0.getVal();
    mass_error_data[jj] = m0.getError();
    sigma_value_data[jj] = sigma.getVal();
    sigma_error_data[jj] = sigma.getError();
    e0_value[jj] = e0.getVal();

  }

  for (jj=0; jj<neta; jj++){
    etamed[jj]=(etabin[jj+1]+etabin[jj])/2;
  }

//  gStyle->SetEndErrorSize(1)
  
  TGraphErrors * gr1 = new TGraphErrors(nbin,etamed,mass_value_data,binw,mass_error_data);
  cIDdata->cd(23);
  gr1->SetTitle("Jpsi mass DATA");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

  TGraphErrors * gr2 = new TGraphErrors(nbin,etamed,sigma_value_data,binw,sigma_error_data);
  cIDdata->cd(24);
  gr2->SetTitle("Jpsi width DATA");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");

// -------------------------------------- MC CORR

  TCanvas* cIDmccorr = new TCanvas("CrystalBallBkgExp - ID - MCCORR","CrystalBallBkgExp - ID - MCCORR",1100,660) ;
  cIDmccorr->Divide(6,4) ;

  TDirectory * MyDirMCCORR = MyFileMCCORR->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

  TH1 * h1mccorr;
  MyDirMCCORR->GetObject("Jpsimm_Mmumu_eta_0_ID_",h1mccorr);
  h1mccorr->Print();
  cIDmccorr->cd(1);
  h1mccorr->Draw();

  TH2 * h2mccorr;
  MyDirMCCORR->GetObject("Jpsimm_h_Zm_Eta_eta_0_ID_",h2mccorr);
  h2mccorr->Print();
  cIDmccorr->cd(2);
  h2mccorr->Draw();

  for (jj=0; jj<neta; jj++){

    int bin=jj+1;
    sprintf(hname2,"py_mccorr_%d",jj+1);
    TH1 * hpmccorr = h2mccorr->ProjectionY(hname2, bin, bin);
    hpmccorr->Print();
    hpmccorr->GetXaxis()->SetLimits(1.5,4.5);

// Crystal Ball

    RooRealVar m("m","m",2.0,4.5); //x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.0,1.);
    RooRealVar alpha("alpha","alpha",2.,0.0,10.0);
    RooRealVar n("n","n",5.0,0.0,50.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n);

// rescale della ratioerenza di massa tenendo conto della scala di energia
    RooRealVar shift("shift","shift",1.19);
//    RooFormulaVar m0_shift("m0_shift","m0*shift",RooArgSet(m0,shift));
    RooFormulaVar m0_shift("m0_shift","@0*@1",RooArgSet(m0,shift));


    RooCBShape cb1("cb1","cb1",m,m0_shift,sigma,alpha,n) ;

    RooRealVar frac("frac","frac",1.0,0.,1.); // frazione della Jpsi
    RooAddPdf sig("sig","sig",RooArgList(cb,cb1),frac);

// Exponential Bkg

    RooRealVar e0("e0","e0", -0.5, -1000.,1000.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",1.0,0.,1.);
    RooAddPdf model("model","model",RooArgList(sig,bkg),fsig);

    RooDataHist data("data","dataset with m",m,hpmccorr);
    RooPlot* myframe = m.frame(Title(hname2));
    data.plotOn(myframe);

    model.fitTo(data, Range(2.6,3.7), SumW2Error(kFALSE) );
    model.plotOn(myframe);
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));

    model.paramOn(myframe);
    int nc2=jj+3;
    cIDmccorr->cd(nc2);
    myframe->Draw();
//    myframe->Clear();

    RooArgSet* params = model.getParameters(m);
    params->Print("v");

//    sprintf(hnamefile2,"ID_results_mccorr_%d",jj+1);
//    params->writeToFile(hnamefile2);
    m0.Print();
    mass_value_mccorr[jj] = m0.getVal();
    mass_error_mccorr[jj] = m0.getError();
    sigma_value_mccorr[jj] = sigma.getVal();
    sigma_error_mccorr[jj] = sigma.getError();
    e0_value[jj] = e0.getVal();

  }

  for (jj=0; jj<neta; jj++){
    etamed[jj]=(etabin[jj+1]+etabin[jj])/2;
  }


  TGraphErrors * gr1 = new TGraphErrors(nbin,etamed,mass_value_mccorr,binw,mass_error_mccorr);
  cIDmccorr->cd(23);
  gr1->SetTitle("Jpsi mass");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

  TGraphErrors * gr2 = new TGraphErrors(nbin,etamed,sigma_value_mccorr,binw,sigma_error_mccorr);
  cIDmccorr->cd(24);
  gr2->SetTitle("Jpsi width");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");

// -------------------------------------- MC NO CORR

  TCanvas* cIDmcnocorr = new TCanvas("CrystalBallBkgExp - ID - MCNOCORR","CrystalBallBkgExp - ID - MCNOCORR",1100,660) ;
  cIDmcnocorr->Divide(6,4) ;

  TDirectory * MyDirMCNOCORR = MyFileMCNOCORR->GetDirectory("ToolSvc.MuonResonanceJpsiTool");

  TH1 * h1mcnocorr;
  MyDirMCNOCORR->GetObject("Jpsimm_Mmumu_eta_0_ID_",h1mcnocorr);
  h1mcnocorr->Print();
  cIDmcnocorr->cd(1);
  h1mcnocorr->Draw();

  TH2 * h2mcnocorr;
  MyDirMCNOCORR->GetObject("Jpsimm_h_Zm_Eta_eta_0_ID_",h2mcnocorr);
  h2mcnocorr->Print();
  cIDmcnocorr->cd(2);
  h2mcnocorr->Draw();

  for (jj=0; jj<neta; jj++){

    int bin=jj+1;
    sprintf(hname3,"py_mcnocorr_%d",jj+1);
    TH1 * hpmcnocorr = h2mcnocorr->ProjectionY(hname3, bin, bin);
    hpmcnocorr->Print();
    hpmcnocorr->GetXaxis()->SetLimits(1.5,4.5);

// Crystal Ball

    RooRealVar m("m","m",2.0,4.5); //x
    RooRealVar m0("m0","m0",3.1,3.0,3.2); // mean
    RooRealVar sigma("sigma","sigma",0.04,0.0,1.);
    RooRealVar alpha("alpha","alpha",2.,0.0,10.0);
    RooRealVar n("n","n",5.0,0.0,50.0);
    RooCBShape cb("cb","cb",m,m0,sigma,alpha,n);
// rescale della ratioerenza di massa tenendo conto della scala di energia
    RooRealVar shift("shift","shift",1.19);
//    RooFormulaVar m0_shift("m0_shift","m0*shift",RooArgSet(m0,shift));
    RooFormulaVar m0_shift("m0_shift","@0*@1",RooArgSet(m0,shift));


    RooCBShape cb1("cb1","cb1",m,m0_shift,sigma,alpha,n) ;

    RooRealVar frac("frac","frac",1.0,0.,1.); // frazione della Jpsi
    RooAddPdf sig("sig","sig",RooArgList(cb,cb1),frac);

// Exponential Bkg

    RooRealVar e0("e0","e0", -0.5, -1000.,1000.);
    RooExponential bkg("bkg","background",m,e0);

// model(x) = fsig*sig(x) + (1-fsig)*bkg(x)
    RooRealVar fsig("fsig","signal fraction",1.0,0.,1.);
    RooAddPdf model("model","model",RooArgList(sig,bkg),fsig);

    RooDataHist data("data","dataset with m",m,hpmcnocorr);
    RooPlot* myframe = m.frame(Title(hname3));
    data.plotOn(myframe);

    model.fitTo(data, Range(2.6,3.7), SumW2Error(kFALSE) );
    model.plotOn(myframe);
    model.plotOn(myframe,Components("bkg"),LineStyle(kDashed));

    model.paramOn(myframe);
    int nc2=jj+3;
    cIDmcnocorr->cd(nc2);
    myframe->Draw();
//    myframe->Clear();

    RooArgSet* params = model.getParameters(m);
    params->Print("v");

//    sprintf(hnamefile3,"ID_results_mcnocorr_%d",jj+1);
//    params->writeToFile(hnamefile3);
    m0.Print();
    mass_value_mcnocorr[jj] = m0.getVal();
    mass_error_mcnocorr[jj] = m0.getError();
    sigma_value_mcnocorr[jj] = sigma.getVal();
    sigma_error_mcnocorr[jj] = sigma.getError();
    e0_value[jj] = e0.getVal();

  }

  for (jj=0; jj<neta; jj++){
    etamed[jj]=(etabin[jj+1]+etabin[jj])/2;
  }


  TGraphErrors * gr1 = new TGraphErrors(nbin,etamed,mass_value_mcnocorr,binw,mass_error_mcnocorr);
  cIDmcnocorr->cd(23);
  gr1->SetTitle("Jpsi mass");
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(1);
  gr1->Draw("AP");

  TGraphErrors * gr2 = new TGraphErrors(nbin,etamed,sigma_value_mcnocorr,binw,sigma_error_mcnocorr);
  cIDmcnocorr->cd(24);
  gr2->SetTitle("Jpsi width");
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(1);
  gr2->Draw("AP");


// --------------------- DATA and MC ---------------------------------------

  for (jj=0; jj<neta; jj++){
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


  TCanvas* cIDratio = new TCanvas(" CrystalBallBkgExp - ID - DATA and MC "," CrystalBallBkgExp - ID - DATA and MC",1100,660) ;
  cIDratio->Divide(2,2) ;

  TGraphErrors * gr4 = new TGraphErrors(nbin,etamed,mass_ratio,binw,mass_ratio_error);
  TGraphErrors * gr5 = new TGraphErrors(nbin,etamed,mass_ratio1,binw,mass_ratio1_error);

  TGraphErrors * gr7 = new TGraphErrors(nbin,etamed,sigma_ratio,binw,sigma_ratio_error);
  TGraphErrors * gr8 = new TGraphErrors(nbin,etamed,sigma_ratio1,binw,sigma_ratio1_error);

  TGraphErrors * gr10 = new TGraphErrors(nbin,etamed,mass_value_data,binw,mass_error_data);
  TGraphErrors * gr11 = new TGraphErrors(nbin,etamed,mass_value_mccorr,binw,mass_error_mccorr);
  TGraphErrors * gr12 = new TGraphErrors(nbin,etamed,mass_value_mcnocorr,binw,mass_error_mcnocorr);

  TGraphErrors * gr14 = new TGraphErrors(nbin,etamed,sigma_value_data,binw,sigma_error_data);
  TGraphErrors * gr15 = new TGraphErrors(nbin,etamed,sigma_value_mccorr,binw,sigma_error_mccorr);
  TGraphErrors * gr16 = new TGraphErrors(nbin,etamed,sigma_value_mcnocorr,binw,sigma_error_mcnocorr);

  TLine * line1 = new TLine(-2.5,1.,2.5,1.);
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

  cIDratio->cd(1);
  TPad * pad = (TPad*)cIDratio->GetPad(1);
  pad->SetLeftMargin(0.14);
  pad->SetBottomMargin(0.14);
  pad->SetTopMargin(0.03);
  pad->SetRightMargin(0.05); 
  TMultiGraph * mg1 = new TMultiGraph();
  mg1->Add(gr4);
  mg1->Add(gr5);
  mg1->Draw("AP");
//  TAxis *axis = gr3->GetXaxis();            // set axis limits along X
//  axis->SetLimits(-2.5,2.5);                // set axis limits along X
  mg1->SetTitle("");
  mg1->GetHistogram()->SetMaximum(1.003);   // set axis limits along Y
  mg1->GetHistogram()->SetMinimum(0.997);   // set axis limits along Y
  mg1->GetXaxis()->SetTitleOffset(0.9);
  mg1->GetXaxis()->SetTitleSize(0.07);
  mg1->GetXaxis()->SetLabelSize(0.05);
  mg1->GetXaxis()->SetTitle("#eta");
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

  cIDratio->cd(2);
  TPad * pad = (TPad*)cIDratio->GetPad(2);
  pad->SetLeftMargin(0.14);
  pad->SetBottomMargin(0.14);
  pad->SetTopMargin(0.03);
  pad->SetRightMargin(0.05);
  TMultiGraph * mg2 = new TMultiGraph();
  mg2->Add(gr7);
  mg2->Add(gr8);
  mg2->Draw("AP");
//  TAxis *axis = gr3->GetXaxis();          // set axis limits along X
//  axis->SetLimits(-2.5,2.5);              // set axis limits along X
  mg2->SetTitle("");
  mg2->GetHistogram()->SetMaximum(1.2);   // set axis limits along Y
  mg2->GetHistogram()->SetMinimum(0.8);   // set axis limits along Y
  mg2->GetXaxis()->SetTitleOffset(0.9);
  mg2->GetXaxis()->SetTitleSize(0.07);
  mg2->GetXaxis()->SetLabelSize(0.05);
  mg2->GetXaxis()->SetTitle("#eta");
  mg2->GetYaxis()->SetTitleOffset(0.9);
  mg2->GetYaxis()->SetTitleSize(0.07);
  mg2->GetYaxis()->SetLabelSize(0.05);
  mg2->GetYaxis()->SetTitle("#sigma_{#mu#mu}^{DATA}/#sigma_{#mu#mu}^{MC}");
  line1->Draw("P");
  TLegend *legend = new TLegend(0.20,0.83,0.50,0.97);
  legend->AddEntry(gr7,"corrected MC ","P");
  legend->AddEntry(gr8,"uncorrected MC","P");
  legend->SetTextSize(0.05);
  legend->Draw();

  cIDratio->cd(3);
  TPad * pad = (TPad*)cIDratio->GetPad(3);
  pad->SetLeftMargin(0.14);
  pad->SetBottomMargin(0.14);
  pad->SetTopMargin(0.03);
  pad->SetRightMargin(0.05);
  TMultiGraph * mg3 = new TMultiGraph();
  mg3->Add(gr10);
  mg3->Add(gr11);
  mg3->Add(gr12);
  mg3->Draw("AP");
//  TAxis *axis = gr3->GetXaxis();           // set axis limits along X
//  axis->SetLimits(-2.5,2.5);               // set axis limits along X
  mg3->SetTitle("");
  mg3->GetHistogram()->SetMaximum(3.13);   // set axis limits along Y
  mg3->GetHistogram()->SetMinimum(3.07);   // set axis limits along Y
  mg3->GetXaxis()->SetTitleOffset(0.9);
  mg3->GetXaxis()->SetTitleSize(0.07);
  mg3->GetXaxis()->SetLabelSize(0.05);
  mg3->GetXaxis()->SetTitle("#eta");
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

  cIDratio->cd(4);
  TPad * pad = (TPad*)cIDratio->GetPad(4);
  pad->SetLeftMargin(0.14);
  pad->SetBottomMargin(0.14);
  pad->SetTopMargin(0.03);
  pad->SetRightMargin(0.05);
  TMultiGraph * mg4 = new TMultiGraph();
  mg4->Add(gr14);
  mg4->Add(gr15);
  mg4->Add(gr16);
  mg4->Draw("AP");
//  TAxis *axis = gr3->GetXaxis();           // set axis limits along X
//  axis->SetLimits(-2.5,2.5);               // set axis limits along X
  mg4->SetTitle("");
  mg4->GetHistogram()->SetMaximum(0.13);   // set axis limits along Y
  mg4->GetHistogram()->SetMinimum(0.03);   // set axis limits along Y
  mg4->GetXaxis()->SetTitleOffset(0.9);
  mg4->GetXaxis()->SetTitleSize(0.07);
  mg4->GetXaxis()->SetLabelSize(0.05);
  mg4->GetXaxis()->SetTitle("#eta");
  mg4->GetYaxis()->SetTitleOffset(0.9);
  mg4->GetYaxis()->SetTitleSize(0.07);
  mg4->GetYaxis()->SetLabelSize(0.05);
  mg4->GetYaxis()->SetTitle("#sigma_{#mu#mu}[GeV]");
  TLegend *legend = new TLegend(0.40,0.77,0.70,0.97);
  legend->AddEntry(gr14,"DATA ","P");
  legend->AddEntry(gr15,"corrected MC ","P");
  legend->AddEntry(gr16,"uncorrected MC","P");
  legend->SetTextSize(0.05);
  legend->Draw();

  cIDratio->Print("mass_and_sigma_vs_eta_ID.gif");

//  TGraph * gr6 = new TGraph(20,etamed,mass_data_PDG);
//  cIDratio->cd(3);
//  gr6->SetTitle("Jpsi mass DATA / mass PDG");
//  gr6->SetMarkerStyle(29);
//  gr6->SetMarkerColor(1);
//  gr6->Draw("AP");

//  TGraph * gr7 = new TGraph(20,etamed,mass_mccorr_PDG);
//  cIDratio->cd(4);
//  gr7->SetTitle("Jpsi mass MC / mass PDG");
//  gr7->SetMarkerStyle(29);
//  gr7->SetMarkerColor(1);
//  gr7->Draw("AP");


  for (jj=0; jj<neta; jj++){
  cout << " mass value data " << mass_value_data[jj] << " sigma value data " << sigma_value_data[jj] << endl;
  }
  for (jj=0; jj<neta; jj++){
  cout << " mass value mccorr " << mass_value_mccorr[jj] << " sigma value mccorr " << sigma_value_mccorr[jj] << endl;
  }
  for (jj=0; jj<neta; jj++){
  cout << " mass value mnoccorr " << mass_value_mcnocorr[jj] << " sigma value mcnocorr " << sigma_value_mcnocorr[jj] << endl;
  }
  for (jj=0; jj<neta; jj++){
  cout << " binw " << binw[jj] << endl;
  }
  for (jj=0; jj<neta+1; jj++){
  cout << " etabin " << etabin[jj] << endl;
  }

}

