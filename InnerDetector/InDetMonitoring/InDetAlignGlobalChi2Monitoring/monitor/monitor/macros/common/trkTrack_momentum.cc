/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void trkTrack_momentum() {

  if (doTrkTrack && iBField>0) {

    TH1F *h3TrkTrack_qoverpT;
    TH1F *h3TrkTrack_pT;
    TH1F *h3TrkTrack_pT2;
    TH1F *h3TrkTrack_qoverp;
    TH1F *h3TrkTrack_p;
    TH1F *h3TrkTrack_p2;

    cout << " Trk::Track Momentum details..." << endl;

    cTrkMomentum = new TCanvas("cTrkMomentum","TrkMomentum",
			       CanvasSizeX6[0],CanvasSizeX6[1]);
    cTrkMomentum->Divide(3,2);

    cTrkMomentum->cd(1);
    h3TrkTrack_qoverpT = (TH1F*)file->Get("trkana/TrkTrack/hTRecoQoverPt_0");
    gPad->SetLogy();
    h3TrkTrack_qoverpT->SetTitle("Trk::Track charge over transverse momentum");
    h3TrkTrack_qoverpT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    h3TrkTrack_qoverpT->SetLineColor(ColorForAll+2);
    h3TrkTrack_qoverpT->SetFillColor(ColorForAll);
    h3TrkTrack_qoverpT->SetFillStyle(3001);
    if (normalizePlots) h3TrkTrack_qoverpT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3TrkTrack_qoverpT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3TrkTrack_qoverpT;

    //
    // -----------------------------------------------------------------
    //

    cTrkMomentum->cd(2);
    h3TrkTrack_pT = (TH1F*)file->Get("trkana/TrkTrack/hTRecoPt_0");
    gPad->SetLogy();
    h3TrkTrack_pT->SetTitle("Trk::Track charge #upoint transverse momentum");
    h3TrkTrack_pT->GetXaxis()->SetTitle("q#upointpT (GeV)");
    h3TrkTrack_pT->SetLineColor(ColorForAll+2);
    h3TrkTrack_pT->SetFillColor(ColorForAll);
    h3TrkTrack_pT->SetFillStyle(3001);
    if (normalizePlots) h3TrkTrack_pT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3TrkTrack_pT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3TrkTrack_pT;

    //
    // -----------------------------------------------------------------
    //

    cTrkMomentum->cd(3);
    h3TrkTrack_pT2 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoPtAbs_0");
    gPad->SetLogy();
    h3TrkTrack_pT2->SetTitle("Trk::Track transverse momentum");
    h3TrkTrack_pT2->GetXaxis()->SetTitle("pT (GeV)");
    h3TrkTrack_pT2->SetLineColor(ColorForAll+2);
    h3TrkTrack_pT2->SetFillColor(ColorForAll);
    h3TrkTrack_pT2->SetFillStyle(3001);
    if (normalizePlots) h3TrkTrack_pT2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3TrkTrack_pT2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3TrkTrack_pT2;

    //
    // -----------------------------------------------------------------
    //

    cTrkMomentum->cd(4);
    h3TrkTrack_qoverp = (TH1F*)file->Get("trkana/TrkTrack/hTRecoQoverPt_0");
    gPad->SetLogy();
    h3TrkTrack_qoverp->SetTitle("Trk::Track charge over momentum");
    h3TrkTrack_qoverp->GetXaxis()->SetTitle("q/p (GeV^{-1})");
    h3TrkTrack_qoverp->SetLineColor(ColorForAll+2);
    h3TrkTrack_qoverp->SetFillColor(ColorForAll);
    h3TrkTrack_qoverp->SetFillStyle(3001);
    if (normalizePlots) h3TrkTrack_qoverp->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3TrkTrack_qoverp->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3TrkTrack_qoverp;

    //
    // -----------------------------------------------------------------
    //

    cTrkMomentum->cd(5);
    h3TrkTrack_p = (TH1F*)file->Get("trkana/TrkTrack/hTRecoP_0");
    gPad->SetLogy();
    h3TrkTrack_p->SetTitle("Trk::Track charge #upoint momentum");
    h3TrkTrack_p->GetXaxis()->SetTitle("q#upointp (GeV)");
    h3TrkTrack_p->SetLineColor(ColorForAll+2);
    h3TrkTrack_p->SetFillColor(ColorForAll);
    h3TrkTrack_p->SetFillStyle(3001);
    if (normalizePlots) h3TrkTrack_p->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3TrkTrack_p->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3TrkTrack_p;

    //
    // -----------------------------------------------------------------
    //

    cTrkMomentum->cd(6);
    h3TrkTrack_p2 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoPAbs_0");
    gPad->SetLogy();
    h3TrkTrack_p2->SetTitle("Trk::Track momentum");
    h3TrkTrack_p2->GetXaxis()->SetTitle("p (GeV)");
    h3TrkTrack_p2->SetLineColor(ColorForAll+2);
    h3TrkTrack_p2->SetFillColor(ColorForAll);
    h3TrkTrack_p2->SetFillStyle(3001);
    if (normalizePlots) h3TrkTrack_p2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3TrkTrack_p2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3TrkTrack_p2;

    cTrkMomentum->Update();

  }

}
