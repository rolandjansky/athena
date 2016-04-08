/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ntracks(int level=1) {

  if (doTrkTrack) {

    TH1F *hNTracks;
    TH1F *hNTracks3;
    TH1F *hnPixelhits;
    TH1F *hnPixelhits2;
    TH1F *hnSCThits;
    TH1F *hnSCThits2;
    TH1F *hhits;
    TH1F *hhits2;
    TH1F *hHoles;
    TH1F *hHoles2;
    TH1F *hShared;
    TH1F *hShared2;

    bool showProcessed = true;

    Color_t icolor = kRed;
    
    cout << " AlignTrk and Trk::Track information..." << endl;
 
    cNTracks = new TCanvas("cNtrkTracks","AlignTrk and Trk::Track information",
			   CanvasSizeX2[0],CanvasSizeX2[1],
			   CanvasSizeX2[2],CanvasSizeX2[3]);
    cNTracks->Divide(2,1);
    
    cNTracks->cd(1);
    hNTracks = (TH1F*)file->Get("trkana/HitsTracks/hNTrk_0");
    hNTracks->SetStats(kFALSE);
    hNTracks->GetXaxis()->SetTitle("Number of tracks");
    hNTracks->GetYaxis()->SetTitle("Events with tracks");
    if (normalizePlots) hNTracks->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hNTracks->SetBinContent(1,0.0);
    hNTracks->SetLineColor(ColorForAll+2);
    hNTracks->SetLineWidth(1);
    hNTracks->SetFillColor(ColorForAll);
    hNTracks->SetFillStyle(3001);
    if (showProcessed) hNTracks->SetStats(kFALSE);
    else {
      hNTracks->SetEntries(hNTracks->GetEntries()-1);
      hNTracks->SetStats(kTRUE);
    }
    hNTracks->SetTitle("AlignTrk: Tracks per event");
    hNTracks ->DrawCopy();
    int ent1 = hNTracks->GetEntries();
    float mean1 = hNTracks->GetMean();
    float iNoTracks = hNTracks->GetBinContent(1);
    setTitleFont(font);
    setTitleSize(0.46);
    ScalePlots();
    
    if (showProcessed) {
      sprintf(name,"trkana/HitsTracks/hNTrk_%d",level);
      hNTracks3 = (TH1F*)file->Get(name);
      hNTracks3->SetBinContent(1,0.0);
      hNTracks3->SetLineColor(ColorForUsed);
      hNTracks3->SetLineWidth(1);
      hNTracks3->SetFillColor(ColorForUsed-9);
      hNTracks3->SetFillStyle(3001);
      if (normalizePlots) hNTracks3->Scale(1./(float)iEvents);
      hNTracks3 ->DrawCopy("same");
      int ent2 = hNTracks3->GetEntries();
      float mean2 = hNTracks3->GetMean();
      setTitleFont(font);
      setTitleSize(0.46);
      ScalePlots();   

      float iNoTracks3 = hNTracks3->GetBinContent(1);

      if (hNTracks->GetMaximum() < hNTracks3->GetMaximum()) {  
	hNTracks ->DrawCopy();
	hNTracks3 ->DrawCopy("same");
      }
      setTitleFont(font);
      setTitleSize(0.46);
      ScalePlots();
  
      TLegend *leg0 = new TLegend(0.74,0.63,0.98,0.74);
      sprintf(name,"mean: %2.2f",mean1);
      leg0->AddEntry(hNTracks,name,"L");
      sprintf(name,"mean:% 2.2f",mean2);
      leg0->AddEntry(hNTracks3,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw(); 

      TLegend *leg0 = new TLegend(0.47,0.89,0.98,1.0);
      sprintf(name,"All AlignTrks (%2.1fk)",float(iTracks)/1000);
      leg0->AddEntry(hNTracks,name,"L");
      sprintf(name,"Processed AlignTrks (%2.1fk)",float(iUsedTracks)/1000);
      leg0->AddEntry(hNTracks3,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();

      TLegend *leg0 = new TLegend(0.44,0.75,0.98,0.86);
      sprintf(name,"Zero-tracks events: %2.1f %%",100*iNoTracks/iEvents);
      leg0->AddEntry(hNTracks,name,"L");
      sprintf(name,"Zero-tracks events: %2.1f %%",100*iNoTracks3/iEvents);
      leg0->AddEntry(hNTracks3,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }
   
    //
    // -----------------------------------------------------------------
    //

    cNTracks->cd(2);
    hNTrkTracks = (TH1F*)file->Get("trkana/TrkTrack/hNTrkTrack_0");
    hNTrkTracks->SetStats(kFALSE);
    hNTrkTracks->GetXaxis()->SetTitle("Number of tracks");
    hNTrkTracks->GetYaxis()->SetTitle("Events with tracks");
    if (normalizePlots) hNTrkTracks->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hNTrkTracks->SetBinContent(1,0.0);
    hNTrkTracks->SetLineColor(ColorForAll+2);
    hNTrkTracks->SetLineWidth(1);
    hNTrkTracks->SetFillColor(ColorForAll);
    hNTrkTracks->SetFillStyle(3001);
    hNTrkTracks->SetStats(kFALSE);
    hNTrkTracks->SetTitle("Trk::Track: Tracks per event");
    hNTrkTracks ->DrawCopy();
    int Trk_ent1 = hNTrkTracks->GetEntries();
    float Trk_mean1 = hNTrkTracks->GetMean();

    float iNoTrkTracks = hNTrkTracks->GetBinContent(1);

    setTitleFont(font);
    setTitleSize(0.46);
    ScalePlots();

    TLegend *leg0 = new TLegend(0.74,0.63,0.98,0.74);
    sprintf(name,"mean: %2.2f",Trk_mean1);
    leg0->AddEntry(hNTrkTracks,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw(); 

    TLegend *leg0 = new TLegend(0.47,0.89,0.98,1.0);
    sprintf(name,"All Trk:Tracks (%2.1fk)",float(iTrkTracks)/1000);
    leg0->AddEntry(hNTrkTracks,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    TLegend *leg0 = new TLegend(0.44,0.75,0.98,0.86);
    sprintf(name,"Zero-tracks events: %2.1f %%",100*iNoTrkTracks/iEvents);
    leg0->AddEntry(hNTrkTracks,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

     
    //
    // -----------------------------------------------------------------
    //

//     pad3->cd();
//     h1NTrkVsEvt = (TH1F*)file->Get("trkana/HitsTracks/hNTrk_Event_0");
//     h1NTrkVsEvt->GetXaxis()->SetRange(0,iEvents);
//     if (!AtlasStyle) {
//       gPad->SetGridx();
//       gPad->SetGridy();
//     }
//     h1NTrkVsEvt->SetTitle("AlignTrks Vs Events");
//     h1NTrkVsEvt->GetXaxis()->SetTitle("event");
//     h1NTrkVsEvt->GetYaxis()->SetTitle("Number of tracks");
//     h1NTrkVsEvt->SetLineColor(icolor);
//     h1NTrkVsEvt->SetMarkerColor(icolor);
//     h1NTrkVsEvt->SetMarkerStyle(20);
//     h1NTrkVsEvt->SetMarkerSize(4);
//     h1NTrkVsEvt->SetMarkerSize(0.4);
//     for (int i=0; i<h1NTrkVsEvt->GetNbinsX(); i++) {
//       if (h1NTrkVsEvt->GetBinContent(i)>0) h1NTrkVsEvt->SetBinError(i,0.001);
//     }
//     h1NTrkVsEvt->DrawCopy("E");
//     setTitleFont(font);
//     delete h1NTrkVsEvt;

  }

}
