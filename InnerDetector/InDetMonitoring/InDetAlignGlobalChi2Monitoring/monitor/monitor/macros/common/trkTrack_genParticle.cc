/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void trkTrack_genParticle() {

  if (doTrkTrack && doTruth && !iCosmics) {

    cout << " Generated Particle information..." << endl;

    TH1F *hGenParPt;
    TH1F *hGenParEta;
    TH1F *hGenParPhi;
    TH2F *hGenParPhiVsEta;
    TH2F *hGenParPtVsPhi;
    TH2F *hGenParPtVsEta;

    Bool_t showProfiles = false;

    cGenParticle = new TCanvas("cGenParticle","Generated Particle Properties",
			       CanvasSizeX6[0],CanvasSizeX6[1]);
    cGenParticle->Divide(3,2);

    cGenParticle->cd(1);
    hGenParPhi = (TH1F*)file->Get("trkana/TrkTrack/hGenParticlePhi");  
    hGenParPhi->SetTitle("Generated Particle Phi");
    hGenParPhi->GetXaxis()->SetTitle("genParticle #phi (rad)");
    hGenParPhi->GetYaxis()->SetTitle("Entries");
    hGenParPhi->SetLineColor(ColorForGenParticle);
    hGenParPhi->SetFillColor(ColorForGenParticle-2);
    hGenParPhi->SetLineWidth(1);
    hGenParPhi->SetFillStyle(3001);
    hGenParPhi->GetYaxis()->SetRangeUser(hGenParPhi->GetMinimum(),1.15*hGenParPhi->GetMaximum());
    if (normalizePlots) hGenParPhi->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hGenParPhi->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete hGenParPhi;

    //
    // -----------------------------------------------------------------
    //

    cGenParticle->cd(2);
    hGenParEta = (TH1F*)file->Get("trkana/TrkTrack/hGenParticleEta");  
    hGenParEta->SetTitle("Generated Particle Eta");
    hGenParEta->GetXaxis()->SetTitle("genParticle #eta");
    hGenParEta->GetYaxis()->SetTitle("Entries");
    hGenParEta->SetLineColor(ColorForGenParticle);
    hGenParEta->SetFillColor(ColorForGenParticle-2);
    hGenParEta->SetLineWidth(1);
    hGenParEta->SetFillStyle(3001);
    hGenParEta->GetYaxis()->SetRangeUser(hGenParEta->GetMinimum(),1.15*hGenParEta->GetMaximum());
    if (normalizePlots) hGenParEta->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hGenParEta->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete hGenParEta;

    //
    // -----------------------------------------------------------------
    //

    cGenParticle->cd(3);
    hGenParPt = (TH1F*)file->Get("trkana/TrkTrack/hGenParticlePt");  
    hGenParPt->SetTitle("Generated Particle pT");
    hGenParPt->GetXaxis()->SetTitle("genParticle (GeV/c)");
    hGenParPt->SetLineColor(ColorForGenParticle);
    hGenParPt->SetFillColor(ColorForGenParticle-2);
    hGenParPt->SetLineWidth(1);
    hGenParPt->SetFillStyle(3001);
    hGenParPt->GetYaxis()->SetRangeUser(hGenParPt->GetMinimum(),1.15*hGenParPt->GetMaximum());
    if (normalizePlots) hGenParPt->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hGenParPt->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete hGenParPt;


    //
    // -----------------------------------------------------------------
    //

    cGenParticle->cd(4);
    hGenParPhiVsEta = (TH2F*)file->Get("trkana/TrkTrack/hGenParticlePhiVsEta");  
    hGenParPhiVsEta->SetTitle("genParticle phi Vs eta");
    hGenParPhiVsEta->GetXaxis()->SetTitle("genParticle #phi (rad)");
    hGenParPhiVsEta->GetYaxis()->SetTitle("genParticle #eta");
    hGenParPhiVsEta->GetXaxis()->SetNdivisions(8);
    hGenParPhiVsEta->GetYaxis()->SetNdivisions(8);
    hGenParPhiVsEta->Draw("COLZ");
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    cGenParticle->Update();
    moveTPaveStats(hGenParPhiVsEta);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.55);
    if (normalizePlots) hGenParPhiVsEta->Scale(1./(float)iEvents);
    drawCross(hGenParPhiVsEta);

    if (showProfiles) {
      TProfile * hProfRecoPhiVsEta = hGenParPhiVsEta->ProfileX(); 
      hProfRecoPhiVsEta->SetMarkerStyle(21);
      hProfRecoPhiVsEta->SetMarkerSize(0.4);
      hProfRecoPhiVsEta->DrawCopy("SAME");
    }

    //
    // -----------------------------------------------------------------
    //

    cGenParticle->cd(5);
    hGenParPtVsPhi = (TH2F*)file->Get("trkana/TrkTrack/hGenParticlePtVsPhi");  
    hGenParPtVsPhi->SetTitle("genParticle phi Vs eta");
    hGenParPtVsPhi->GetXaxis()->SetTitle("genParticle pT (GeV/c)");
    hGenParPtVsPhi->GetYaxis()->SetTitle("genParticle #eta");
    hGenParPtVsPhi->GetXaxis()->SetNdivisions(8);
    hGenParPtVsPhi->GetYaxis()->SetNdivisions(8);
    hGenParPtVsPhi->Draw("COLZ");
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    cGenParticle->Update();
    moveTPaveStats(hGenParPtVsPhi);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.55);
    if (normalizePlots) hGenParPtVsPhi->Scale(1./(float)iEvents);
    drawCross(hGenParPtVsPhi);

    if (showProfiles) {
      TProfile * hProfRecoPtVsPhi = hGenParPtVsPhi->ProfileX(); 
      hProfRecoPtVsPhi->SetMarkerStyle(21);
      hProfRecoPtVsPhi->SetMarkerSize(0.4);
      hProfRecoPtVsPhi->DrawCopy("SAME");
    }

    //
    // -----------------------------------------------------------------
    //

    cGenParticle->cd(6);
    hGenParPtVsEta = (TH2F*)file->Get("trkana/TrkTrack/hGenParticlePtVsEta");  
    hGenParPtVsEta->SetTitle("genParticle phi Vs eta");
    hGenParPtVsEta->GetXaxis()->SetTitle("genParticle pT (GeV/c)");
    hGenParPtVsEta->GetYaxis()->SetTitle("genParticle #eta");
    hGenParPtVsEta->GetXaxis()->SetNdivisions(8);
    hGenParPtVsEta->GetYaxis()->SetNdivisions(8);
    hGenParPtVsEta->Draw("COLZ");
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    cGenParticle->Update();
    moveTPaveStats(hGenParPtVsEta);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.55);
    if (normalizePlots) hGenParPtVsEta->Scale(1./(float)iEvents);
    drawCross(hGenParPtVsEta);

    if (showProfiles) {
      TProfile * hProfRecoPtVsEta = hGenParPtVsEta->ProfileX(); 
      hProfRecoPtVsEta->SetMarkerStyle(21);
      hProfRecoPtVsEta->SetMarkerSize(0.4);
      hProfRecoPtVsEta->DrawCopy("SAME");
    }

    return;
  }

}
