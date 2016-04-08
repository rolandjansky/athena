/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrkParCorrProf2NoTruth() {

  if (iBField>0) {

    TH2F *hTrkqOverpTphi0;
    TH2F *hProcTrkqOverpTphi0;
    TH2F *hTrkpTphi0;
    TH2F *hProcTrkpTphi0;
    cout << " Quick Track Parameter Correlations (q/pT and q*p Vs phi0)..." << endl;

    cParCorr2 = new TCanvas("cParCorr2","Track Parameter Correlations 3",
			    CanvasSizeX4[0],CanvasSizeX4[1]);
    cParCorr2->Divide(2,2);
  
    cParCorr2->cd(1);
    hTrkqOverpTphi0 = (TH2F*)file->Get("trkana/Track_Parameters/hRecoQoverPt_RecoPhi0_0");
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hTrkqOverpTphi0->SetTitle("q/pT Vs #phi_{0} (All tracks)");
    hTrkqOverpTphi0->GetYaxis()->SetTitle("q/pT (GeV^{-1})");
    hTrkqOverpTphi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
    hTrkqOverpTphi0->Draw("Cont4Z");
    cParCorr2->Update();
    if (!AtlasStyle) {
      TPaveStats *st = (TPaveStats*)hTrkqOverpTphi0->FindObject("stats"); 
      st->SetX1NDC(0.66); //new x start position
      st->SetX2NDC(0.88); //new x start position
      st->SetY1NDC(0.82); //new y end position
      st->SetOptStat(1110);
      st->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hTrkqOverpTphi0->Scale(1./(float)iEvents);
  
    cParCorr2->cd(2);
    sprintf(name,"trkana/Track_Parameters/hRecoQoverPt_RecoPhi0_%d",level);
    hProcTrkqOverpTphi0 = (TH2F*)file->Get(name);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hProcTrkqOverpTphi0->SetTitle("q/pT Vs #phi_{0} (Processed tracks)");
    hProcTrkqOverpTphi0->GetYaxis()->SetTitle("q/pT (GeV^{-1})");
    hProcTrkqOverpTphi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
    hProcTrkqOverpTphi0->Draw("Cont4Z");
    cParCorr2->Update();
    if (!AtlasStyle) { 
      TPaveStats *st = (TPaveStats*)hProcTrkqOverpTphi0->FindObject("stats"); 
      st->SetX1NDC(0.66); //new x start position
      st->SetX2NDC(0.88); //new x start position
      st->SetY1NDC(0.82); //new y end position
      st->SetOptStat(1110);
      st->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hProcTrkqOverpTphi0->Scale(1./(float)iEvents);
  
    cParCorr2->cd(3);
    hTrkpTphi0 = (TH2F*)file->Get("trkana/Track_Parameters/hRecoPt_RecoPhi0_0");
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hTrkpTphi0->SetTitle("q#upointpT Vs #phi_{0} (All tracks)");
    hTrkpTphi0->GetYaxis()->SetTitle("q#upointpT (GeV)");
    hTrkpTphi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
    hTrkpTphi0->Draw("Cont4Z");
    cParCorr2->Update();
    if (!AtlasStyle) {
      TPaveStats *st = (TPaveStats*)hTrkpTphi0->FindObject("stats"); 
      st->SetX1NDC(0.66); //new x start position
      st->SetX2NDC(0.88); //new x start position
      st->SetY1NDC(0.82); //new y end position
      st->SetOptStat(1110);
      st->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hTrkpTphi0->Scale(1./(float)iEvents);

    cParCorr2->cd(4);
    sprintf(name,"trkana/Track_Parameters/hRecoPt_RecoPhi0_%d",level);
    hProcTrkpTphi0 = (TH2F*)file->Get(name);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hProcTrkpTphi0->SetTitle("q#upointpT Vs #phi_{0} (Processed tracks)");
    hProcTrkpTphi0->GetYaxis()->SetTitle("q#upointpT (GeV)");
    hProcTrkpTphi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
    hProcTrkpTphi0->Draw("Cont4Z");
    cParCorr2->Update();
    if (!AtlasStyle) {
      TPaveStats *st = (TPaveStats*)hProcTrkpTphi0->FindObject("stats"); 
      st->SetX1NDC(0.66); //new x start position
      st->SetX2NDC(0.88); //new x start position
      st->SetY1NDC(0.82); //new y end position
      st->SetOptStat(1110);
      st->Draw();
    }
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hProcTrkpTphi0->Scale(1./(float)iEvents);
  }

  return;
}
