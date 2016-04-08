/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void BeamSpotHistos() {

  if(!iCosmics) {

    TH1F *hBeamSpotX;
    TH1F *hBeamSpotY;
    TH1F *hBeamSpotZ;
    TH1F *hBeamSpotTiltX;
    TH1F *hBeamSpotTiltY;
    TH1F *hLumiBlockID;

    cout << " Beam spot histograms..." << endl;

    cBeamSpot = new TCanvas("cBeamSpot","Beam Spot histograms",
			    CanvasSizeX6[0],CanvasSizeX6[1]);

    cBeamSpot->Divide(3,2);
  
    //
    // -----------------------------------------------------------------
    //

    // beam spot X
    cBeamSpot->cd(1);
    sprintf(name,"trkana/BeamSpot/hBeamSpotX_%d",level);
    hBeamSpotX = (TH1F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hBeamSpotX->SetTitle("Beam spot: X_{BS}");
    hBeamSpotX->GetXaxis()->SetTitle("X_{BS} (mm)");
    hBeamSpotX->GetYaxis()->SetTitle("tracks");
    hBeamSpotX->Draw();
    cBeamSpot->Update();
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hBeamSpotX->Scale(1./(float)iEvents);

    //
    // -----------------------------------------------------------------
    //

    // beam spot Y
    cBeamSpot->cd(2);
    sprintf(name,"trkana/BeamSpot/hBeamSpotY_%d",level);
    hBeamSpotY = (TH1F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hBeamSpotY->SetTitle("Beam spot: Y_{BS}");
    hBeamSpotY->GetXaxis()->SetTitle("Y_{BS} (mm)");
    hBeamSpotY->GetYaxis()->SetTitle("tracks");
    hBeamSpotY->Draw();
    cBeamSpot->Update();
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hBeamSpotY->Scale(1./(float)iEvents);

    //
    // -----------------------------------------------------------------
    //

    // beam spot Z
    cBeamSpot->cd(3);
    sprintf(name,"trkana/BeamSpot/hBeamSpotZ_%d",level);
    hBeamSpotZ = (TH1F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hBeamSpotZ->SetTitle("Beam spot: Z_{BS}");
    hBeamSpotZ->GetXaxis()->SetTitle("Z_{BS} (mm)");
    hBeamSpotZ->GetYaxis()->SetTitle("tracks");
    hBeamSpotZ->Draw();
    cBeamSpot->Update();
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hBeamSpotZ->Scale(1./(float)iEvents);

    //
    // -----------------------------------------------------------------
    //

    // beam spot TiltX
    cBeamSpot->cd(4);
    sprintf(name,"trkana/BeamSpot/hBeamSpotTiltX_%d",level);
    hBeamSpotTiltX = (TH1F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hBeamSpotTiltX->SetTitle("Beam spot: TiltX_{BS}");
    hBeamSpotTiltX->GetXaxis()->SetTitle("TiltX_{BS} (mrad)");
    hBeamSpotTiltX->GetYaxis()->SetTitle("tracks");
    hBeamSpotTiltX->Draw();
    cBeamSpot->Update();
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hBeamSpotTiltX->Scale(1./(float)iEvents);

    //
    // -----------------------------------------------------------------
    //

    // beam spot TiltY
    cBeamSpot->cd(5);
    sprintf(name,"trkana/BeamSpot/hBeamSpotTiltY_%d",level);
    hBeamSpotTiltY = (TH1F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hBeamSpotTiltY->SetTitle("Beam spot: TiltY_{BS}");
    hBeamSpotTiltY->GetXaxis()->SetTitle("TiltY_{BS} (mrad)");
    hBeamSpotTiltY->GetYaxis()->SetTitle("tracks");
    hBeamSpotTiltY->Draw();
    cBeamSpot->Update();
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hBeamSpotTiltY->Scale(1./(float)iEvents);

    //
    // -----------------------------------------------------------------
    //

    // Lumi Block
    cBeamSpot->cd(6);
    sprintf(name,"trkana/BeamSpot/hLumiBlockID_%d",level);
    hLumiBlockID = (TH1F*)file->Get(name);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hLumiBlockID->SetTitle("Beam spot: Lumi Block ID");
    hLumiBlockID->GetXaxis()->SetTitle("Lumi Block");
    hLumiBlockID->GetYaxis()->SetTitle("tracks");
    hLumiBlockID->Draw();
    cBeamSpot->Update();
    setTitleFont(font);
    ScalePlots(1);
    if (normalizePlots) hLumiBlockID->Scale(1./(float)iEvents);

  }

  return;
}
