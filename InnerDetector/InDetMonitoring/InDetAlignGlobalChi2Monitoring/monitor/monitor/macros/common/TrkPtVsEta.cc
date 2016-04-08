/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrkPtVsEta() {

  if (iBField>0) {

    TH2F *hTrkqOverpTeta;
    TH2F *hProcTrkqOverpTeta;
    TH2F *hTrkqpTeta;
    TH2F *hProcTrkqpTeta;
    cout << " Quick Track Parameter Correlations (q/pT and q*p Vs eta)..." << endl;

    cpTvsEta = new TCanvas("cpTvsEta","Track Parameter Correlations 2",
			   CanvasSizeX4[0],CanvasSizeX4[1]);
    cpTvsEta->Divide(2,2);
  
    cpTvsEta->cd(1);
    hTrkqOverpTeta = (TH2F*)file->Get("trkana/Track_Parameters/hRecoQoverPt_RecoEta_0");
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hTrkqOverpTeta->SetTitle("q/pT Vs #eta (All tracks)");
    hTrkqOverpTeta->GetYaxis()->SetTitle("q/pT (GeV^{-1})");
    hTrkqOverpTeta->GetXaxis()->SetTitle("#eta");
    hTrkqOverpTeta->Draw("Cont4Z");
    cpTvsEta->Update();
    if (!AtlasStyle) {
      TPaveStats *st = (TPaveStats*)hTrkqOverpTeta->FindObject("stats"); 
      st->SetX1NDC(0.66); //new x start position
      st->SetX2NDC(0.88); //new x start position
      st->SetY1NDC(0.82); //new y end position
      st->SetOptStat(1110);
      st->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hTrkqOverpTeta->Scale(1./(float)iEvents);
  
    cpTvsEta->cd(2);
    sprintf(name,"trkana/Track_Parameters/hRecoQoverPt_RecoEta_%d",level);
    hProcTrkqOverpTeta = (TH2F*)file->Get(name);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hProcTrkqOverpTeta->SetTitle("q/pT Vs #eta (Processed tracks)");
    hProcTrkqOverpTeta->GetYaxis()->SetTitle("q/pT (GeV^{-1})");
    hProcTrkqOverpTeta->GetXaxis()->SetTitle("#eta");
    hProcTrkqOverpTeta->Draw("Cont4Z");
    cpTvsEta->Update();
    if (!AtlasStyle) {
      TPaveStats *st = (TPaveStats*)hProcTrkqOverpTeta->FindObject("stats"); 
      st->SetX1NDC(0.66); //new x start position
      st->SetX2NDC(0.88); //new x start position
      st->SetY1NDC(0.82); //new y end position
      st->SetOptStat(1110);
      st->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hProcTrkqOverpTeta->Scale(1./(float)iEvents);
  
    cpTvsEta->cd(3);
    hTrkpTeta = (TH2F*)file->Get("trkana/Track_Parameters/hRecoPt_RecoEta_0");
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hTrkpTeta->SetTitle("q#upointpT Vs eta (All tracks)");
    hTrkpTeta->GetYaxis()->SetTitle("q#upointpT (GeV)");
    hTrkpTeta->GetXaxis()->SetTitle("#eta");
    hTrkpTeta->Draw("Cont4Z");
    cpTvsEta->Update();
    if (!AtlasStyle) {
      TPaveStats *st = (TPaveStats*)hTrkpTeta->FindObject("stats"); 
      st->SetX1NDC(0.66); //new x start position
      st->SetX2NDC(0.88); //new x start position
      st->SetY1NDC(0.82); //new y end position
      st->SetOptStat(1110);
      st->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hTrkpTeta->Scale(1./(float)iEvents);

    cpTvsEta->cd(4);
    sprintf(name,"trkana/Track_Parameters/hRecoPt_RecoEta_%d",level);
    hProcTrkpTeta = (TH2F*)file->Get(name);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hProcTrkpTeta->SetTitle("q#upointpT Vs eta (Processed tracks)");
    hProcTrkpTeta->GetYaxis()->SetTitle("q#upointpT (GeV)");
    hProcTrkpTeta->GetXaxis()->SetTitle("#eta");
    hProcTrkpTeta->Draw("Cont4Z");
    cpTvsEta->Update();
    if (!AtlasStyle) {
      TPaveStats *st = (TPaveStats*)hProcTrkpTeta->FindObject("stats"); 
      st->SetX1NDC(0.66); //new x start position
      st->SetX2NDC(0.88); //new x start position
      st->SetY1NDC(0.82); //new y end position
      st->SetOptStat(1110);
      st->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hProcTrkpTeta->Scale(1./(float)iEvents);
  }

  return;
}
