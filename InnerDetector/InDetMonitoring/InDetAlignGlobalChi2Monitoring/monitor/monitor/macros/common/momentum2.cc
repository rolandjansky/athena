/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void momentum2() {

  if(iBField>0) {

    TH1F *h3ProcTrk_qoverpT;
    TH1F *h3ProcTrk_pT;
    TH1F *h3ProcTrk_pT2;
    TH1F *h3ProcTrk_qoverp;
    TH1F *h3ProcTrk_p;
    TH1F *h3ProcTrk_p2;
  
    cout << " AlignTrk Momentum details (processed tracks)..." << endl;

    cMomentum2 = new TCanvas("cMomentum2","Momentum (Processed tracks)",
			     CanvasSizeX6[0],CanvasSizeX6[1]);
    cMomentum2->Divide(3,2);

    cMomentum2->cd(1);
    sprintf(name,"trkana/Track_Parameters/hRecoQoverPt_%d",level);    
    h3ProcTrk_qoverpT = (TH1F*)file->Get(name);
    gPad->SetLogy();
    h3ProcTrk_qoverpT->SetTitle("AlignTrk q over pT (Processed tracks)");
    h3ProcTrk_qoverpT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    h3ProcTrk_qoverpT->SetLineColor(ColorForAll+2);
    h3ProcTrk_qoverpT->SetFillColor(ColorForAll);
    h3ProcTrk_qoverpT->SetFillStyle(3001);
    if (normalizePlots) h3ProcTrk_qoverpT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3ProcTrk_qoverpT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3ProcTrk_qoverpT;

    //
    // -----------------------------------------------------------------
    //

    cMomentum2->cd(2);
    sprintf(name,"trkana/Track_Parameters/hRecoPt_%d",level);    
    h3ProcTrk_pT = (TH1F*)file->Get(name);
    gPad->SetLogy();
    h3ProcTrk_pT->SetTitle("AlignTrk q#upointpT (Processed tracks)");
    h3ProcTrk_pT->GetXaxis()->SetTitle("q#upointpT (GeV)");
    h3ProcTrk_pT->SetLineColor(ColorForAll+2);
    h3ProcTrk_pT->SetFillColor(ColorForAll);
    h3ProcTrk_pT->SetFillStyle(3001);
    if (normalizePlots) h3ProcTrk_pT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3ProcTrk_pT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3ProcTrk_pT;

    //
    // -----------------------------------------------------------------
    //

    cMomentum2->cd(3);
    sprintf(name,"trkana/Track_Parameters/hRecoPtAbs_%d",level);    
    h3ProcTrk_pT2 = (TH1F*)file->Get(name);
    gPad->SetLogy();
    h3ProcTrk_pT2->SetTitle("AlignTrk pT (Processed tracks)");
    h3ProcTrk_pT2->GetXaxis()->SetTitle("pT (GeV)");
    h3ProcTrk_pT2->SetLineColor(ColorForAll+2);
    h3ProcTrk_pT2->SetFillColor(ColorForAll);
    h3ProcTrk_pT2->SetFillStyle(3001);
    if (normalizePlots) h3ProcTrk_pT2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3ProcTrk_pT2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3ProcTrk_pT2;

    //
    // -----------------------------------------------------------------
    //

    cMomentum2->cd(4);
    sprintf(name,"trkana/Track_Parameters/hRecoQoverP_%d",level);    
    h3ProcTrk_qoverp = (TH1F*)file->Get(name);
    gPad->SetLogy();
    h3ProcTrk_qoverp->SetTitle("AlignTrk q over p (Processed tracks)");
    h3ProcTrk_qoverp->GetXaxis()->SetTitle("q/p (GeV^{-1})");
    h3ProcTrk_qoverp->SetLineColor(ColorForAll+2);
    h3ProcTrk_qoverp->SetFillColor(ColorForAll);
    h3ProcTrk_qoverp->SetFillStyle(3001);
    if (normalizePlots) h3ProcTrk_qoverp->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3ProcTrk_qoverp->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3ProcTrk_qoverp;

    //
    // -----------------------------------------------------------------
    //

    cMomentum2->cd(5);
    sprintf(name,"trkana/Track_Parameters/hRecoP_%d",level);    
    h3ProcTrk_p = (TH1F*)file->Get(name);
    gPad->SetLogy();
    h3ProcTrk_p->SetTitle("AlignTrk q#upointp (Processed tracks)");
    h3ProcTrk_p->GetXaxis()->SetTitle("q#upointp (GeV)");
    h3ProcTrk_p->SetLineColor(ColorForAll+2);
    h3ProcTrk_p->SetFillColor(ColorForAll);
    h3ProcTrk_p->SetFillStyle(3001);
    if (normalizePlots) h3ProcTrk_p->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3ProcTrk_p->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3ProcTrk_p;

    //
    // -----------------------------------------------------------------
    //

    cMomentum2->cd(6);
    sprintf(name,"trkana/Track_Parameters/hRecoPAbs_%d",level);    
    h3ProcTrk_p2 = (TH1F*)file->Get(name);
    gPad->SetLogy();
    h3ProcTrk_p2->SetTitle("AlignTrk absolute p (Processed tracks)");
    h3ProcTrk_p2->GetXaxis()->SetTitle("p (GeV)");
    h3ProcTrk_p2->SetLineColor(ColorForAll+2);
    h3ProcTrk_p2->SetFillColor(ColorForAll);
    h3ProcTrk_p2->SetFillStyle(3001);
    if (normalizePlots) h3ProcTrk_p2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h3ProcTrk_p2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete h3ProcTrk_p2;

    cMomentum2->Update();

  }
  return;

}
