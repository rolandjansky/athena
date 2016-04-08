/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 
void VtxCandidatePos() {

  if (!iCosmics && doVtxCandidate) {

    TH1F *hPrimVtxPosX;
    TH1F *hPrimVtxPosY;
    TH1F *hPrimVtxPosZ;
    TH2F *hVtxCandidatePosXY;
    TH2F *hVtxCandidatePosXZ;
    TH2F *hVtxCandidatePosYZ;

    cout << " Vertex Candidate Positions..." << endl;

    cVtxCandidatePos = new TCanvas("VertexCandidatePos","Vertex Candidate Position",
				   CanvasSizeX6[0],CanvasSizeX6[1]);
    cVtxCandidatePos->Divide(3,2);

    cVtxCandidatePos->cd(1);
    if (isNominal && IterNumber==0) hPrimVtxPosX = (TH1F*)file->Get("trkana/Track_Parameters/hVtxX");  
    else hPrimVtxPosX = (TH1F*)file->Get("trkana/Track_Parameters/hVtxZoomX");
    hPrimVtxPosX->SetTitle("Vertex Candidate X Position");
    hPrimVtxPosX->GetXaxis()->SetTitle("Vertex X Position (mm)");
    hPrimVtxPosX->SetLineColor(ColorForUsed+2);
    hPrimVtxPosX->SetLineWidth(1);
    hPrimVtxPosX->SetFillColor(ColorForUsed);
    hPrimVtxPosX->SetFillStyle(3001);
    if (normalizePlots) hPrimVtxPosX->Scale(1./(float)iEvents);
    if (!AtlasStyle) {			    
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hPrimVtxPosX->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.50);
    drawVerticalLine(hPrimVtxPosX);
    delete hPrimVtxPosX;

    //
    // -----------------------------------------------------------------
    //

    cVtxCandidatePos->cd(2);
    if (isNominal && IterNumber==0) hPrimVtxPosY = (TH1F*)file->Get("trkana/Track_Parameters/hVtxY");
    else hPrimVtxPosY = (TH1F*)file->Get("trkana/Track_Parameters/hVtxZoomY");
    hPrimVtxPosY->SetTitle("Vertex Candidate Y Position");
    hPrimVtxPosY->GetXaxis()->SetTitle("Vertex Y Position (mm)");
    hPrimVtxPosY->SetLineColor(ColorForUsed+2);
    hPrimVtxPosY->SetLineWidth(1);
    hPrimVtxPosY->SetFillColor(ColorForUsed);
    hPrimVtxPosY->SetFillStyle(3001);
    if (normalizePlots) hPrimVtxPosY->Scale(1./(float)iEvents);
    if (!AtlasStyle) {	
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hPrimVtxPosY->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.50);
    drawVerticalLine(hPrimVtxPosY);
    delete hPrimVtxPosY;

    //
    // -----------------------------------------------------------------
    //

    cVtxCandidatePos->cd(3);
    hPrimVtxPosZ = (TH1F*)file->Get("trkana/Track_Parameters/hVtxZ"); 
    hPrimVtxPosZ->SetTitle("Vertex Candidate Z Position");
    hPrimVtxPosZ->GetXaxis()->SetTitle("Vertex Z Position (mm)");
    hPrimVtxPosZ->SetLineColor(ColorForUsed+2);
    hPrimVtxPosZ->SetLineWidth(1);
    hPrimVtxPosZ->SetFillColor(ColorForUsed);
    hPrimVtxPosZ->SetFillStyle(3001);
    if (normalizePlots) hPrimVtxPosZ->Scale(1./(float)iEvents);
    if (!AtlasStyle) {	
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hPrimVtxPosZ->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.50);
    drawVerticalLine(hPrimVtxPosZ);
    delete hPrimVtxPosZ;

    //
    // -----------------------------------------------------------------
    //

    cVtxCandidatePos->cd(4);
    if (isNominal && IterNumber==0) hVtxCandidatePosXY = (TH2F*)file->Get("trkana/Track_Parameters/hVtxX_VtxY");
    else hVtxCandidatePosXY = (TH2F*)file->Get("trkana/Track_Parameters/hZoomVtxX_VtxY");
    hVtxCandidatePosXY->SetTitle("Vtx Candidate Pos (X Vs Y)");
    hVtxCandidatePosXY->GetXaxis()->SetTitle("VtxCandidate X (mm)");
    hVtxCandidatePosXY->GetYaxis()->SetTitle("VtxCandidate Y (mm)");
    hVtxCandidatePosXY->Draw("COLZ");
    if (!AtlasStyle) {	
      gPad->SetGridx();
      gPad->SetGridy();
    }
    cVtxCandidatePos->Update();
    moveTPaveStats(hVtxCandidatePosXY);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.50);
    if (normalizePlots) hVtxCandidatePosXY->Scale(1./(float)iEvents);
    drawCross(hVtxCandidatePosXY);

    //
    // -----------------------------------------------------------------
    //

    cVtxCandidatePos->cd(5);
    if (isNominal && IterNumber==0) hVtxCandidatePosXZ = (TH2F*)file->Get("trkana/Track_Parameters/hVtxX_VtxZ");
    else hVtxCandidatePosXZ = (TH2F*)file->Get("trkana/Track_Parameters/hZoomVtxX_VtxZ");
    hVtxCandidatePosXZ->SetTitle("Vtx Candidate Pos (X Vs Z)");
    hVtxCandidatePosXZ->GetXaxis()->SetTitle("VtxCandidate Z (mm)");
    hVtxCandidatePosXZ->GetYaxis()->SetTitle("VtxCandidate X (mm)");
    hVtxCandidatePosXZ->Draw("COLZ");
    if (!AtlasStyle) {	
      gPad->SetGridx();
      gPad->SetGridy();
    }
    cVtxCandidatePos->Update();
    moveTPaveStats(hVtxCandidatePosXZ);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.50);
    if (normalizePlots) hVtxCandidatePosXZ->Scale(1./(float)iEvents);
    drawCross(hVtxCandidatePosXZ);

    //
    // -----------------------------------------------------------------
    //
  
    cVtxCandidatePos->cd(6);
    if (isNominal && IterNumber==0) hVtxCandidatePosYZ = (TH2F*)file->Get("trkana/Track_Parameters/hVtxY_VtxZ");
    else hVtxCandidatePosYZ = (TH2F*)file->Get("trkana/Track_Parameters/hZoomVtxY_VtxZ"); 
    hVtxCandidatePosYZ->SetTitle("Vtx Candidate Pos (Y Vs Z)");
    hVtxCandidatePosYZ->GetXaxis()->SetTitle("VtxCandidate Z (mm)");
    hVtxCandidatePosYZ->GetYaxis()->SetTitle("VtxCandidate Y (mm)");
    hVtxCandidatePosYZ->Draw("COLZ");
    if (!AtlasStyle) {	
      gPad->SetGridx();
      gPad->SetGridy();
    }
    cVtxCandidatePos->Update();
    moveTPaveStats(hVtxCandidatePosYZ);
    setTitleFont(font);
    ScalePlots(1);
    setTitleSize(0.50);
    if (normalizePlots) hVtxCandidatePosYZ->Scale(1./(float)iEvents);
    drawCross(hVtxCandidatePosYZ);
  }

}
