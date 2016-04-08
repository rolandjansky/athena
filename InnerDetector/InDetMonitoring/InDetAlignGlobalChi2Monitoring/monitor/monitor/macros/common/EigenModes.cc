/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void EigenModes() {

  // check if exists
  TH1F *h1temp = (TH1F*)file->Get("trkana/Eigen/hEigenVal");
  if (h1temp!=NULL) {

    ///////////////////////////////////////
    // 1) Plot eigen modes 
    ///////////////////////////////////////

    cout << " Eigen Modes..." << endl;

    cEigenModes = new TCanvas("eigenmodes","EigenModes",1080,700);
    cEigenModes->Divide(3,2);
    // 
    TH1F *hEigenVal = (TH1F*)file->Get("trkana/Eigen/hEigenVal"); 
    cEigenModes->cd(1);
    hEigenVal->SetXTitle("mode");
    hEigenVal->Draw();
    gPad->SetLogy();
    //
    TH1F *hdbAliCorrRaw = (TH1F*)file->Get("trkana/Eigen/hdbAliCorrRaw"); 
    TH1F *hdbAliCorr    = (TH1F*)file->Get("trkana/Eigen/hdbAliCorr"); 
    cEigenModes->cd(2);
    hdbAliCorrRaw->SetXTitle("mode");
    hdbAliCorrRaw->Draw();
    hdbAliCorr->SetLineColor(kRed);
    hdbAliCorr->SetFillStyle(3001);
    hdbAliCorr->SetFillColor(kRed);
    hdbAliCorr->Draw("same");
    //
    TH1F *hBigV_b = (TH1F*)file->Get("trkana/Eigen/hBigV_b"); 
    cEigenModes->cd(3);
    hBigV_b->SetXTitle("mode");
    hBigV_b->Draw();
    // 
    TH1F *hdbAliCorrErr = (TH1F*)file->Get("trkana/Eigen/hdbAliCorrErr"); 
    cEigenModes->cd(4);
    hdbAliCorrErr->SetXTitle("mode");
    hdbAliCorrErr->Draw();
    gPad->SetLogy();
    // 
    TH1F *hdbPulls = (TH1F*)file->Get("trkana/Eigen/hdbPulls"); 
    cEigenModes->cd(5);
    hdbPulls->SetXTitle("mode");
    hdbPulls->Draw();
    // 
    TH1F *hdbPullsH = (TH1F*)file->Get("trkana/Eigen/hdbPullsH"); 
    cEigenModes->cd(6);
    hdbPullsH->SetXTitle("pull");
    hdbPullsH->Draw(); 


    ///////////////////////////////////////
    // 2) Plot alignment corrections
    ///////////////////////////////////////
    cout << " Alignment corrections..." << endl;
    cAlignCorr = new TCanvas("AlignCorr","Alignment corrections",1080,700);
    cAlignCorr->Divide();
    TH1F *hdaAliCorr = (TH1F*)file->Get("trkana/Eigen/hdaAliCorr"); 
    hdaAliCorr->SetXTitle("dof");
    hdaAliCorr->SetLineColor(kRed);
    hdaAliCorr->SetFillStyle(3001);
    hdaAliCorr->SetFillColor(kRed);
    hdaAliCorr->Draw();
    TH1F *hdaAliCorrRaw = (TH1F*)file->Get("trkana/Eigen/hdaAliCorrRaw"); 
    hdaAliCorrRaw->Draw("same");
    hdaAliCorr->Draw("same");


    // 3 Plot big matrix
    // CanBigMatrix = new TCanvas("big matrix","jacobana: big matrix",700,700);
    // CanBigMatrix->Divide();
  }

  return;
}
