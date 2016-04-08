/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void D0bsCorrelations() {

  if(!iCosmics) {

    cout << " DO_bs correlations with track parameters..." << endl;
    TH1F *hRecoD0bs;
    TH2F *hRecoD0_RecoPhi0;
    TH2F *hRecoD0_RecoEta;
    TH2F *hRecoD0_RecoZ0;
    TProfile *HProfiF;
    TProfile *HProfiE;
    TProfile *HProfiZ;

    cout << " DOb correlations with track parameters..." << endl;

    cD0bsCorr = new TCanvas("cD0bsCorr","D0bs correlations",
			    CanvasSizeX4[0],CanvasSizeX4[1]);

    cD0bsCorr->Divide(2,2);
  

    // Reconstructed D0 wrt beam spot
    cD0bsCorr->cd(1);
    sprintf(name,"trkana/Track_Parameters/hRecoD0bs_%d",level);
    hRecoD0bs = (TH1F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    gPad->SetGridx();
    gPad->SetGridy();
    hRecoD0bs->SetLineColor(ColorForAll+2);
    hRecoD0bs->SetFillColor(ColorForAll);
    hRecoD0bs->SetFillStyle(3001);
    hRecoD0bs->SetTitle("d_{0} wrt beam spot");
    hRecoD0bs->GetYaxis()->SetTitle("d_{0} (mm)");
    hRecoD0bs->Draw();
    cD0bsCorr->Update();
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hRecoD0bs->Scale(1./(float)iEvents);
    
    //
    // -----------------------------------------------------------------
    //
    
    // Reco D0 wrt phi0
    cD0bsCorr->cd(2);
    sprintf(name,"trkana/Track_Parameters/hRecoD0bs_RecoPhi0_%d",level);
    hRecoD0_RecoPhi0 = (TH2F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hRecoD0_RecoPhi0->SetTitle("d_{0} wrt beam spot vs #phi_{0}");
    hRecoD0_RecoPhi0->GetYaxis()->SetTitle("d_{0} (mm)");
    hRecoD0_RecoPhi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
    //hRecoD0_RecoPhi0->Draw("Cont4Z");
    hRecoD0_RecoPhi0->Draw("colz");
    cD0bsCorr->Update();
    if (!AtlasStyle) {
      TPaveStats *st2 = (TPaveStats*)hRecoD0_RecoPhi0->FindObject("stats"); 
      st2->SetX1NDC(0.66); //new x start position
      st2->SetX2NDC(0.88); //new x start position
      st2->SetY1NDC(0.82); //new y end position
      st2->SetOptStat(1110);
      st2->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hRecoD0_RecoPhi0->Scale(1./(float)iEvents);
    // Plot the profile on top of the 2D histogra,
    HProfiF = hRecoD0_RecoPhi0->ProfileX("",1,-1,""); 
    HProfiF->DrawCopy("same");

    //
    // -----------------------------------------------------------------
    //
  
    // Reco D0 wrt eta
    cD0bsCorr->cd(3);
    sprintf(name,"trkana/Track_Parameters/hRecoD0bs_RecoEta_%d",level);
    hRecoD0_RecoEta = (TH2F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hRecoD0_RecoEta->SetTitle("d_{0} wrt beam spot vs #eta");
    hRecoD0_RecoEta->GetYaxis()->SetTitle("d_{0} (mm)");
    hRecoD0_RecoEta->GetXaxis()->SetTitle("#eta");
    //hRecoD0_RecoEta->Draw("Cont4Z");
    hRecoD0_RecoEta->Draw("colz");
    cD0bsCorr->Update();
    if (!AtlasStyle) {
      TPaveStats *st2 = (TPaveStats*)hRecoD0_RecoEta->FindObject("stats"); 
      st2->SetX1NDC(0.66); //new x start position
      st2->SetX2NDC(0.88); //new x start position
      st2->SetY1NDC(0.82); //new y end position
      st2->SetOptStat(1110);
      st2->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hRecoD0_RecoEta->Scale(1./(float)iEvents);
    // Plot the profile on top of the 2D histogra,
    HProfiE = hRecoD0_RecoEta->ProfileX("",1,-1,""); 
    HProfiE->DrawCopy("same");

    //
    // -----------------------------------------------------------------
    //

    // Reco D0 wrt Z0
    cD0bsCorr->cd(4);
    sprintf(name,"trkana/Track_Parameters/hRecoD0bs_RecoZ0_%d",level);
    hRecoD0_RecoZ0 = (TH2F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hRecoD0_RecoZ0->SetTitle("d_{0} wrt beam spot vs Z_{0}");
    hRecoD0_RecoZ0->GetYaxis()->SetTitle("d_{0} (mm)");
    hRecoD0_RecoZ0->GetXaxis()->SetTitle("Z_{0} (mm)");
    //hRecoD0_RecoZ0->Draw("Cont4Z");
    hRecoD0_RecoZ0->Draw("colz");
    cD0bsCorr->Update();
    if (!AtlasStyle) {
      TPaveStats *st2 = (TPaveStats*)hRecoD0_RecoZ0->FindObject("stats"); 
      st2->SetX1NDC(0.66); //new x start position
      st2->SetX2NDC(0.88); //new x start position
      st2->SetY1NDC(0.82); //new y end position
      st2->SetOptStat(1110);
      st2->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hRecoD0_RecoZ0->Scale(1./(float)iEvents);
    // Plot the profile on top of the 2D histogra,
    HProfiZ = hRecoD0_RecoZ0->ProfileX(); 
    HProfiZ->DrawCopy("same");
  }
  
  return;
}
