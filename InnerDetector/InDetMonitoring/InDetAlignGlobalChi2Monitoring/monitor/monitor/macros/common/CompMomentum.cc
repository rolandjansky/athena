/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void CompMomentum() {

  if(iBField>0) {

    TH1F *h4Trk_qoverpT;
    TH1F *h4Trk_pT;
    TH1F *h4Trk_pT2;
    TH1F *h4Trk_qoverp;
    TH1F *h4Trk_p;
    TH1F *h4Trk_p2;
    TH1F *h4ProcTrk_qoverpT;
    TH1F *h4ProcTrk_pT;
    TH1F *h4ProcTrk_pT2;
    TH1F *h4ProcTrk_qoverp;
    TH1F *h4ProcTrk_p;
    TH1F *h4ProcTrk_p2;

    cout << " AlignTrk Momentum Comparation..." << endl;

    cCompMomentum = new TCanvas("cAlignTrkCompMomentum","AlignTrk Momentum Comparation",
				CanvasSizeX6[0],CanvasSizeX6[1]);
    cCompMomentum->Divide(3,2);
  
    cCompMomentum->cd(1);
    h4Trk_qoverpT = (TH1F*)file->Get("trkana/Track_Parameters/hRecoQoverPt_0");
    gPad->SetLogy();
    h4Trk_qoverpT->SetTitle("AlignTrk q/pT");
    h4Trk_qoverpT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    if (normalizePlots) h4Trk_qoverpT->Scale(1./(float)iEvents);
    h4Trk_qoverpT->SetLineColor(ColorForAll+2);
    h4Trk_qoverpT->SetLineWidth(1);
    h4Trk_qoverpT->SetFillColor(ColorForAll);
    h4Trk_qoverpT->SetFillStyle(3001);
    h4Trk_qoverpT->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoQoverPt_%d",level);
    h4ProcTrk_qoverpT = (TH1F*)file->Get(name);
    if(normalizePlots) h4ProcTrk_qoverpT->Scale(1./(float)iEvents);
    h4ProcTrk_qoverpT->SetLineColor(ColorForUsed);
    h4ProcTrk_qoverpT->SetLineWidth(1);
    h4ProcTrk_qoverpT->SetFillColor(ColorForUsed-9);
    h4ProcTrk_qoverpT->SetFillStyle(3001);
    h4ProcTrk_qoverpT->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.3);

    if(!AtlasStyle) TLegend *leg0 = new TLegend(0.31,0.89,0.67,1.0);
    else TLegend *leg0 = new TLegend(0.57,0.88,0.94,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(h4Trk_qoverpT,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(h4ProcTrk_qoverpT,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    //
    // -----------------------------------------------------------------
    //

    cCompMomentum->cd(2);
    h4Trk_pT = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPt_0");
    gPad->SetLogy();
    h4Trk_pT->SetTitle("AlignTrk q#upointpT");
    h4Trk_pT->GetXaxis()->SetTitle("q#upointpT (GeV)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    if (normalizePlots) h4Trk_pT->Scale(1./(float)iEvents);
    h4Trk_pT->SetLineColor(ColorForAll+2);
    h4Trk_pT->SetLineWidth(1);
    h4Trk_pT->SetFillColor(ColorForAll);
    h4Trk_pT->SetFillStyle(3001);
    h4Trk_pT->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoPt_%d",level);
    h4ProcTrk_pT = (TH1F*)file->Get(name);
    if (normalizePlots) h4ProcTrk_pT->Scale(1./(float)iEvents);
    h4ProcTrk_pT->SetLineColor(ColorForUsed);
    h4ProcTrk_pT->SetLineWidth(1);
    h4ProcTrk_pT->SetFillColor(ColorForUsed-9);
    h4ProcTrk_pT->SetFillStyle(3001);
    h4ProcTrk_pT->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.3);

    if(!AtlasStyle) TLegend *leg0 = new TLegend(0.31,0.89,0.67,1.0);
    else TLegend *leg0 = new TLegend(0.57,0.88,0.94,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(h4Trk_pT,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(h4ProcTrk_pT,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    //
    // -----------------------------------------------------------------
    //

    cCompMomentum->cd(3);
    h4Trk_pT2 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPtAbs_0");
    gPad->SetLogy();
    h4Trk_pT2->SetTitle("AlignTrk pT");
    h4Trk_pT2->GetXaxis()->SetTitle("pT (GeV)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    if (normalizePlots) h4Trk_pT2->Scale(1./(float)iEvents);
    h4Trk_pT2->SetLineColor(ColorForAll+2);
    h4Trk_pT2->SetLineWidth(1);
    h4Trk_pT2->SetFillColor(ColorForAll);
    h4Trk_pT2->SetFillStyle(3001);
    h4Trk_pT2->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoPtAbs_%d",level);
    h4ProcTrk_pT2 = (TH1F*)file->Get(name);
    if (normalizePlots) h4ProcTrk_pT2->Scale(1./(float)iEvents);
    h4ProcTrk_pT2->SetLineColor(ColorForUsed);
    h4ProcTrk_pT2->SetLineWidth(1);
    h4ProcTrk_pT2->SetFillColor(ColorForUsed-9);
    h4ProcTrk_pT2->SetFillStyle(3001);
    h4ProcTrk_pT2->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.3);

    if(!AtlasStyle) TLegend *leg0 = new TLegend(0.31,0.89,0.67,1.0);
    else TLegend *leg0 = new TLegend(0.57,0.88,0.94,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(h4Trk_pT2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(h4ProcTrk_pT2,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    //
    // -----------------------------------------------------------------
    //

    cCompMomentum->cd(4);
    h4Trk_qoverp = (TH1F*)file->Get("trkana/Track_Parameters/hRecoQoverPt_0");
    gPad->SetLogy();
    h4Trk_qoverp->SetTitle("AlignTrk q/p");
    h4Trk_qoverp->GetXaxis()->SetTitle("q/p (GeV^{-1})");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    if (normalizePlots) h4Trk_qoverp->Scale(1./(float)iEvents);
    h4Trk_qoverp->SetLineColor(ColorForAll+2);
    h4Trk_qoverp->SetLineWidth(1);
    h4Trk_qoverp->SetFillColor(ColorForAll);
    h4Trk_qoverp->SetFillStyle(3001);
    h4Trk_qoverp->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoQoverPt_%d",level);
    h4ProcTrk_qoverp = (TH1F*)file->Get(name);
    gPad->SetLogy();
    if (normalizePlots) h4ProcTrk_qoverp->Scal(1./(float)iEvents);
    h4ProcTrk_qoverp->SetLineColor(ColorForUsed);
    h4ProcTrk_qoverp->SetLineWidth(1);
    h4ProcTrk_qoverp->SetFillColor(ColorForUsed-9);
    h4ProcTrk_qoverp->SetFillStyle(3001);
    h4ProcTrk_qoverp->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.3);
  
    if(!AtlasStyle) TLegend *leg0 = new TLegend(0.31,0.89,0.67,1.0);
    else TLegend *leg0 = new TLegend(0.57,0.88,0.94,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(h4Trk_qoverp,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(h4ProcTrk_qoverp,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  
    //
    // -----------------------------------------------------------------
    //

    cCompMomentum->cd(5);
    h4Trk_p = (TH1F*)file->Get("trkana/Track_Parameters/hRecoP_0");
    gPad->SetLogy();
    h4Trk_p->SetTitle("AlignTrk q#upointp");
    h4Trk_p->GetXaxis()->SetTitle("q#upointp (GeV)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    if (normalizePlots) h4Trk_p->Scale(1./(float)iEvents);
    h4Trk_p->SetLineColor(ColorForAll+2);
    h4Trk_p->SetLineWidth(1);
    h4Trk_p->SetFillColor(ColorForAll);
    h4Trk_p->SetFillStyle(3001);
    h4Trk_p->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoP_%d",level);
    h4ProcTrk_p = (TH1F*)file->Get(name);
    if (normalizePlots) h4ProcTrk_p->Scale(1./(float)iEvents);
    h4ProcTrk_p->SetLineColor(ColorForUsed);
    h4ProcTrk_p->SetLineWidth(1);
    h4ProcTrk_p->SetFillColor(ColorForUsed-9);
    h4ProcTrk_p->SetFillStyle(3001);
    h4ProcTrk_p->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.3);

    if(!AtlasStyle) TLegend *leg0 = new TLegend(0.31,0.89,0.67,1.0);
    else TLegend *leg0 = new TLegend(0.57,0.88,0.94,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(h4Trk_p,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(h4ProcTrk_p,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  
    //
    // -----------------------------------------------------------------
    //

    cCompMomentum->cd(6);
    h4Trk_p2 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPAbs_0");
    gPad->SetLogy();
    h4Trk_p2->SetTitle("AlignTrk p");
    h4Trk_p2->GetXaxis()->SetTitle("p (GeV)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    if (normalizePlots) h4Trk_p2->Scale(1./(float)iEvents);
    h4Trk_p2->SetLineColor(ColorForAll+2);
    h4Trk_p2->SetLineWidth(1);
    h4Trk_p2->SetFillColor(ColorForAll);
    h4Trk_p2->SetFillStyle(3001);
    h4Trk_p2->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoPAbs_%d",level);
    h4ProcTrk_p2 = (TH1F*)file->Get(name);
    if (normalizePlots) h4ProcTrk_p2->Scale(1./(float)iEvents);
    h4ProcTrk_p2->SetLineColor(ColorForUsed);
    h4ProcTrk_p2->SetLineWidth(1);
    h4ProcTrk_p2->SetFillColor(ColorForUsed-9);
    h4ProcTrk_p2->SetFillStyle(3001);
    h4ProcTrk_p2->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.3);

    if(!AtlasStyle) TLegend *leg0 = new TLegend(0.31,0.89,0.67,1.0);
    else TLegend *leg0 = new TLegend(0.57,0.88,0.94,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(h4Trk_p2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(h4ProcTrk_p2,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    cCompMomentum->Update();
  }

  return;
}
