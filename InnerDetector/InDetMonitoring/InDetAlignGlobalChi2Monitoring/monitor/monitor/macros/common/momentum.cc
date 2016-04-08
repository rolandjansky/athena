/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void momentum() {

  if(iBField>0) {

    TH1F *h3Trk_qoverpT;
    TH1F *h3Trk_pT;
    TH1F *h3Trk_pT2;
    TH1F *h3Trk_qoverp;
    TH1F *h3Trk_p;
    TH1F *h3Trk_p2;

    cout << " Momentum details..." << endl;

    cMomentum = new TCanvas("cMomentum","Momentum",
			    CanvasSizeX6[0],CanvasSizeX6[1]);
    cMomentum->Divide(3,2);

    cMomentum->cd(1);
    h3Trk_qoverpT = (TH1F*)file->Get("trkana/Track_Parameters/hRecoQoverPt_0");
    gPad->SetLogy();
    h3Trk_qoverpT->SetTitle("AlignTrk charge over transverse momentum");
    h3Trk_qoverpT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    h3Trk_qoverpT->SetLineColor(ColorForAll+2);
    h3Trk_qoverpT->SetFillColor(ColorForAll);
    h3Trk_qoverpT->SetFillStyle(3001);
    if (normalizePlots) h3Trk_qoverpT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3Trk_qoverpT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3Trk_qoverpT;

    //
    // -----------------------------------------------------------------
    //

    cMomentum->cd(2);
    h3Trk_pT = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPt_0");
    gPad->SetLogy();
    h3Trk_pT->SetTitle("AlignTrk charge #upoint transverse momentum");
    h3Trk_pT->GetXaxis()->SetTitle("q#upointpT (GeV)");
    h3Trk_pT->SetLineColor(ColorForAll+2);
    h3Trk_pT->SetFillColor(ColorForAll);
    h3Trk_pT->SetFillStyle(3001);
    if (normalizePlots) h3Trk_pT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3Trk_pT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3Trk_pT;

    //
    // -----------------------------------------------------------------
    //

    cMomentum->cd(3);
    h3Trk_pT2 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPtAbs_0");
    gPad->SetLogy();
    h3Trk_pT2->SetTitle("AlignTrk transverse momentum");
    h3Trk_pT2->GetXaxis()->SetTitle("pT (GeV)");
    h3Trk_pT2->SetLineColor(ColorForAll+2);
    h3Trk_pT2->SetFillColor(ColorForAll);
    h3Trk_pT2->SetFillStyle(3001);
    if (normalizePlots) h3Trk_pT2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3Trk_pT2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3Trk_pT2;

    //
    // -----------------------------------------------------------------
    //

    cMomentum->cd(4);
    h3Trk_qoverp = (TH1F*)file->Get("trkana/Track_Parameters/hRecoQoverPt_0");
    gPad->SetLogy();
    h3Trk_qoverp->SetTitle("AlignTrk charge over momentum");
    h3Trk_qoverp->GetXaxis()->SetTitle("q/p (GeV^{-1})");
    h3Trk_qoverp->SetLineColor(ColorForAll+2);
    h3Trk_qoverp->SetFillColor(ColorForAll);
    h3Trk_qoverp->SetFillStyle(3001);
    if (normalizePlots) h3Trk_qoverp->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3Trk_qoverp->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3Trk_qoverp;

    //
    // -----------------------------------------------------------------
    //

    cMomentum->cd(5);
    h3Trk_p = (TH1F*)file->Get("trkana/Track_Parameters/hRecoP_0");
    gPad->SetLogy();
    h3Trk_p->SetTitle("AlignTrk charge #upoint momentum");
    h3Trk_p->GetXaxis()->SetTitle("q#upointp (GeV)");
    h3Trk_p->SetLineColor(ColorForAll+2);
    h3Trk_p->SetFillColor(ColorForAll);
    h3Trk_p->SetFillStyle(3001);
    if (normalizePlots) h3Trk_p->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3Trk_p->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3Trk_p;

    //
    // -----------------------------------------------------------------
    //

    cMomentum->cd(6);
    h3Trk_p2 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPAbs_0");
    gPad->SetLogy();
    h3Trk_p2->SetTitle("AlignTrk momentum");
    h3Trk_p2->GetXaxis()->SetTitle("p (GeV)");
    h3Trk_p2->SetLineColor(ColorForAll+2);
    h3Trk_p2->SetFillColor(ColorForAll);
    h3Trk_p2->SetFillStyle(3001);
    if (normalizePlots) h3Trk_p2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3Trk_p2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3Trk_p2;

    cMomentum->Update();

  }

  return;
}
