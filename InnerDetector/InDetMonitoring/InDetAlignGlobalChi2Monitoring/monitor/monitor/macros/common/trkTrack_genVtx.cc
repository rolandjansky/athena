/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void trkTrack_genVtx() {

  if (doTrkTrack && doTruth && !iCosmics) {

    cout << " Generated Particle Vertex Position..." << endl;

    TH1F *hTrkVtxPosX;
    TH1F *hTrkVtxPosY;
    TH1F *hTrkVtxPosZ;
    TH2F *hTrkVtxXVtxY;
    TH2F *hTrkVtxXVtxZ;
    TH2F *hTrkVtxYVtxZ;

    cTruthVtx = new TCanvas("ProductionVertex","Production Vertex Position",
			    CanvasSizeX6[0],CanvasSizeX6[1]);
    cTruthVtx->Divide(3,2);

    cTruthVtx->cd(1);
    hTrkVtxPosX = (TH1F*)file->Get("trkana/TrkTrack/hTrkVtxX");  
    hTrkVtxPosX->SetTitle("Generated Particle Vertex X Position");
    hTrkVtxPosX->GetXaxis()->SetTitle("genParticle Vertex X Position (mm)");
    hTrkVtxPosX->GetYaxis()->SetTitle("Entries");
    hTrkVtxPosX->SetLineColor(ColorForGenParticle);
    hTrkVtxPosX->SetFillColor(ColorForGenParticle-2);
    hTrkVtxPosX->SetLineWidth(1);
    hTrkVtxPosX->SetFillStyle(3001);
    if (normalizePlots) hTrkVtxPosX->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hTrkVtxPosX->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    drawVerticalLine(hTrkVtxPosX);
    delete hTrkVtxPosX;

    //
    // -----------------------------------------------------------------
    //

    cTruthVtx->cd(2);
    hTrkVtxPosY = (TH1F*)file->Get("trkana/TrkTrack/hTrkVtxY");  
    hTrkVtxPosY->SetTitle("Generated Particle Vertex Y Position");
    hTrkVtxPosY->GetXaxis()->SetTitle("genParticle Vertex Y Position (mm)");
    hTrkVtxPosY->GetYaxis()->SetTitle("Entries");
    hTrkVtxPosY->SetLineColor(ColorForGenParticle);
    hTrkVtxPosY->SetFillColor(ColorForGenParticle-2);
    hTrkVtxPosY->SetLineWidth(1);
    hTrkVtxPosY->SetFillStyle(3001);
    if (normalizePlots) hTrkVtxPosY->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hTrkVtxPosY->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    drawVerticalLine(hTrkVtxPosY);
    delete hTrkVtxPosY;

    //
    // -----------------------------------------------------------------
    //

    cTruthVtx->cd(3);
    hTrkVtxPosZ = (TH1F*)file->Get("trkana/TrkTrack/hTrkVtxZ");  
    hTrkVtxPosZ->SetTitle("Generated Particle Vertex Z Position");
    hTrkVtxPosZ->GetXaxis()->SetTitle("genParticle Vertex Z Position (mm)");
    hTrkVtxPosZ->GetYaxis()->SetTitle("Entries");
    hTrkVtxPosZ->SetLineColor(ColorForGenParticle);
    hTrkVtxPosZ->SetFillColor(ColorForGenParticle-2);
    hTrkVtxPosZ->SetLineWidth(1);
    hTrkVtxPosZ->SetFillStyle(3001);
    if (normalizePlots) hTrkVtxPosZ->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hTrkVtxPosZ->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    drawVerticalLine(hTrkVtxPosZ);
    delete hTrkVtxPosZ;

    //
    // -----------------------------------------------------------------
    //

    cTruthVtx->cd(4);
    hTrkVtxXVtxY = (TH2F*)file->Get("trkana/TrkTrack/hTrkVtxXVtxY");  
    hTrkVtxXVtxY->SetTitle("genParticle Vertex X Vs Y Pos");
    hTrkVtxXVtxY->GetXaxis()->SetTitle("genParticle Vertex X Position (mm)");
    hTrkVtxXVtxY->GetYaxis()->SetTitle("genParticle Vertex Y Position (mm)");
    hTrkVtxXVtxY->GetXaxis()->SetNdivisions(8);
    hTrkVtxXVtxY->GetYaxis()->SetNdivisions(8);
    hTrkVtxXVtxY->Draw("COLZ");
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    cTruthVtx->Update();
    moveTPaveStats(hTrkVtxXVtxY);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.55);
    if (normalizePlots) hTrkVtxXVtxY->Scale(1./(float)iEvents);
    drawCross(hTrkVtxXVtxY);

    //
    // -----------------------------------------------------------------
    //

    cTruthVtx->cd(5);
    hTrkVtxXVtxZ = (TH2F*)file->Get("trkana/TrkTrack/hTrkVtxXVtxZ");  
    hTrkVtxXVtxZ->SetTitle("genParticle Vertex X Vs Z Pos");
    hTrkVtxXVtxZ->GetXaxis()->SetTitle("genParticle Vertex Z Position (mm)");
    hTrkVtxXVtxZ->GetYaxis()->SetTitle("genParticle Vertex X Position (mm)");
    hTrkVtxXVtxZ->GetYaxis()->SetNdivisions(8);
    hTrkVtxXVtxZ->GetXaxis()->SetNdivisions(8);
    hTrkVtxXVtxZ->Draw("COLZ");
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    cTruthVtx->Update();
    moveTPaveStats(hTrkVtxXVtxZ);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.55);
    if (normalizePlots) hTrkVtxXVtxZ->Scale(1./(float)iEvents);
    drawCross(hTrkVtxXVtxZ);

    //
    // -----------------------------------------------------------------
    //

    cTruthVtx->cd(6);
    hTrkVtxYVtxZ = (TH2F*)file->Get("trkana/TrkTrack/hTrkVtxYVtxZ");  
    hTrkVtxYVtxZ->SetTitle("genParticle Vertex Y Vs Z Pos");
    hTrkVtxYVtxZ->GetXaxis()->SetTitle("genParticle Vertex Z Position (mm)");
    hTrkVtxYVtxZ->GetYaxis()->SetTitle("genParticle Vertex X Position (mm)");
    hTrkVtxYVtxZ->GetYaxis()->SetNdivisions(8);
    hTrkVtxYVtxZ->GetXaxis()->SetNdivisions(8);
    hTrkVtxYVtxZ->Draw("COLZ");
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    cTruthVtx->Update();
    moveTPaveStats(hTrkVtxYVtxZ);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.55);
    if (normalizePlots) hTrkVtxYVtxZ->Scale(1./(float)iEvents);
    drawCross(hTrkVtxYVtxZ);

    return;
  }

}
