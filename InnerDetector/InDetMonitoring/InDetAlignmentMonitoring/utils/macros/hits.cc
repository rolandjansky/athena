/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void hits(TString trkCol_1, TString trkCol_2)
{
    
    TH1F *hTracks;
    TH1F *hTracks3;
    TH1F *hnPixelhits;
    TH1F *hnPixelhits2;
    TH1F *hnSCThits;
    TH1F *hnSCThits2;
    TH1F *hnhits;
    TH1F *hnhits2;
    TH1F *hntrthits;
    TH1F *hntrthits2;
    TH1F *hnhitsevent;
    TH1F *hnhitsevent2;
    
    float ent1, mean1;
    float ent2, mean2;
    int TracksWithoutPixelHits2, TracksWithoutSCTHits2, TracksWithouttrtHits2;
    TString histname;
    Char_t name[40];
    cout << " <hits> Track and Hit information..." << endl;
    
    cHits = new TCanvas("TracksHits","Tracks and Hits information",CanvasSizeX6[0],CanvasSizeX6[1]);
    cHits->Divide(3,2);
    
    
    cHits->cd(1);
    
    histname = "IDAlignMon/"+trkCol_1+"/GenericTracks/ntracks";
    if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
    hTracks = (TH1F*)file->Get(histname);
    hTracks->SetTitle("Reco Tracks");
    hTracks->GetXaxis()->SetTitle("Tracks per event");
    hTracks->GetYaxis()->SetTitle("Entries");
    if (normalizePlots) hTracks->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTracks->SetLineColor(ColorForFirst+2);
    hTracks->SetLineWidth(1);
    hTracks->SetFillColor(ColorForFirst);
    hTracks->SetFillStyle(3001);
    hTracks->SetStats(kFALSE);
    hTracks->SetTitle("Tracks per event");
    hTracks ->DrawCopy("hist");
    ScalePlots();
    ent1 = hTracks->GetEntries();
    mean1 = hTracks->GetMean();
    
    if (trkCol_2 !=""){
        histname = "IDAlignMon/"+trkCol_2+"/GenericTracks/ntracks";
        if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
        hTracks3 = (TH1F*)file->Get(histname);
        hTracks3->SetLineColor(ColorForSecond);
        hTracks3->SetLineWidth(1);
        hTracks3->SetFillColor(ColorForSecond-9);
        hTracks3->SetFillStyle(3001);
        if (normalizePlots) hTracks3->Scale(1./(float)iEvents);
        hTracks3 ->DrawCopy("same");
        ent2 = hTracks3->GetEntries();
        mean2 = hTracks3->GetMean();
    
    
        if (hTracks->GetMaximum() < hTracks3->GetMaximum()) {
            hTracks->GetYaxis()->SetRangeUser(0,hTracks3->GetMaximum()*1.1);
            hTracks->DrawCopy("hist");
            hTracks3->DrawCopy("same");
        }
    }
    
    setTitleFont(font);
    
    TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
    sprintf(name,"mean: %2.2f",mean1);
    leg0->AddEntry(hTracks,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"mean:% 2.2f",mean2);
        leg0->AddEntry(hTracks3,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.47,0.89,0.98,1.0);
    sprintf(name,"%s (%2.1fk)", trkCol_1.Data(), float(iTracks_1)/1000);
    leg0->AddEntry(hTracks,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"%s (%2.1fk)", trkCol_2.Data(), float(iTracks_2)/1000);
        leg0->AddEntry(hTracks3,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    
    //
    // -----------------------------------------------------------------
    //
    
    cHits->cd(2);
    histname = "IDAlignMon/"+trkCol_1+"/GenericTracks/Nhits_per_track";
    if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
    
    hnhits = (TH1F*)file->Get(histname);
    if (normalizePlots) hnhits->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
        gPad->SetGridy();
        gPad->SetGridx();
    }
    hnhits->SetTitle("Hits on track");
    hnhits->GetXaxis()->SetTitle("Hits on track");
    hnhits->GetYaxis()->SetTitle("Entries");
    hnhits->SetLineColor(ColorForFirst+2);
    hnhits->SetLineWidth(1);
    hnhits->SetFillColor(ColorForFirst);
    hnhits->SetFillStyle(3001);
    hnhits->SetStats(kFALSE);
    hnhits->DrawCopy("hist");
    setTitleFont(font);
    ScalePlots();
    setTitleFont(font);
    ScalePlots();
    ent1 = hnhits->GetEntries();
    mean1 = hnhits->GetMean();
    
    
    
    if (trkCol_2 !=""){
        histname = "IDAlignMon/"+trkCol_2+"/GenericTracks/Nhits_per_track";
        if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
        
        
        hnhits2 = (TH1F*)file->Get(histname);
        hnhits2->SetLineColor(ColorForSecond);
        hnhits2->SetLineWidth(1);
        hnhits2->SetFillColor(ColorForSecond-9);
        hnhits2->SetFillStyle(3001);
        if (normalizePlots) hnhits2->Scale(1./(float)iEvents);
        hnhits2->DrawCopy("same");
        ent2 = hnhits2->GetEntries();
        mean2 = hnhits2->GetMean();
        
        if (hnhits->GetMaximum() < hnhits2->GetMaximum()) {
            hnhits->GetYaxis()->SetRangeUser(0,1.05*hnhits2->GetMaximum());
            hnhits->DrawCopy("hist");
            hnhits2->DrawCopy("same");
        }
    }
    
    leg0 = new TLegend(0.74,0.75,0.98,0.86);
    sprintf(name,"mean: %2.2f",mean1);
    leg0->AddEntry(hnhits,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"mean:% 2.2f",mean2);
        leg0->AddEntry(hnhits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.47,0.89,0.98,1.0);
    sprintf(name,"%s (%2.1fk)", trkCol_1.Data(), float(iTracks_1)/1000);
    leg0->AddEntry(hnhits,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"%s (%2.1fk)", trkCol_2.Data(), float(iTracks_2)/1000);
        leg0->AddEntry(hnhits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    
    //
    // -----------------------------------------------------------------
    //
    
    cHits->cd(3);
    histname = "IDAlignMon/"+trkCol_1+"/GenericTracks/Nhits_per_event";
    if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
    
    hnhitsevent = (TH1F*)file->Get(histname);
    if (normalizePlots) hnhitsevent->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
        gPad->SetGridy();
        gPad->SetGridx();
    }
    hnhitsevent->SetTitle("Hits on event");
    hnhitsevent->GetXaxis()->SetTitle("Hits on event");
    hnhitsevent->GetYaxis()->SetTitle("Entries");
    hnhitsevent->SetLineColor(ColorForFirst+2);
    hnhitsevent->SetLineWidth(1);
    hnhitsevent->SetFillColor(ColorForFirst);
    hnhitsevent->SetFillStyle(3001);
    hnhitsevent->SetStats(kFALSE);
    hnhitsevent->DrawCopy("hist");
    setTitleFont(font);
    ScalePlots();
    setTitleFont(font);
    ScalePlots();
    ent1 = hnhitsevent->GetEntries();
    mean1 = hnhitsevent->GetMean();
    
    
    if (trkCol_2 !=""){
        histname = "IDAlignMon/"+trkCol_2+"/GenericTracks/Nhits_per_event";
        if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
        
        
        hnhitsevent2 = (TH1F*)file->Get(histname);
        hnhitsevent2->SetLineColor(ColorForSecond);
        hnhitsevent2->SetLineWidth(1);
        hnhitsevent2->SetFillColor(ColorForSecond-9);
        hnhitsevent2->SetFillStyle(3001);
        if (normalizePlots) hnhitsevent2->Scale(1./(float)iEvents);
        hnhitsevent2->DrawCopy("same");
        ent2 = hnhitsevent2->GetEntries();
        mean2 = hnhitsevent2->GetMean();
        
        if (hnhitsevent->GetMaximum() < hnhitsevent2->GetMaximum()) {
            hnhitsevent->GetYaxis()->SetRangeUser(0,1.05*hnhitsevent2->GetMaximum());
            hnhitsevent->DrawCopy("hist");
            hnhitsevent2->DrawCopy("same");
        }
    }
    
    leg0 = new TLegend(0.74,0.75,0.98,0.86);
    sprintf(name,"mean: %2.2f",mean1);
    leg0->AddEntry(hnhitsevent,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"mean:% 2.2f",mean2);
        leg0->AddEntry(hnhitsevent2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.47,0.89,0.98,1.0);
    sprintf(name,"%s (%2.1fk)", trkCol_1.Data(), float(iTracks_1)/1000);
    leg0->AddEntry(hnhitsevent,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"%s (%2.1fk)", trkCol_2.Data(), float(iTracks_2)/1000);
        leg0->AddEntry(hnhitsevent2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    //
    // -----------------------------------------------------------------
    //
    
    cHits->cd(4);
    histname = "IDAlignMon/"+trkCol_1+"/GenericTracks/Npixhits_per_track";
    if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
    
    hnPixelhits = (TH1F*)file->Get(histname);
    hnPixelhits->SetTitle("Pixel Hits on track");
    hnPixelhits->GetXaxis()->SetTitle("Pixel Hits on track");
    hnPixelhits->GetYaxis()->SetTitle("Entries");
    if (normalizePlots) hnPixelhits->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hnPixelhits->SetLineColor(ColorForFirst+2);
    hnPixelhits->SetLineWidth(1);
    hnPixelhits->SetFillColor(ColorForFirst);
    hnPixelhits->SetFillStyle(3001);
    int TracksWithoutPixelHits = (int)hnPixelhits->GetBinContent(1);
    hnPixelhits->SetBinContent(1,0.0);
    hnPixelhits->SetStats(kFALSE);
    ForceStyle(hnPixelhits);
    hnPixelhits->DrawCopy("hist");
    ScalePlots();
    ent1 = hnPixelhits->GetEntries();
    mean1 = hnPixelhits->GetMean();
    
    
    if (trkCol_2 !=""){
        histname = "IDAlignMon/"+trkCol_2+"/GenericTracks/Npixhits_per_track";
        if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
        
        
        hnPixelhits2 = (TH1F*)file->Get(histname);
        hnPixelhits2->SetLineColor(ColorForSecond);
        hnPixelhits2->SetLineWidth(1);
        hnPixelhits2->SetFillColor(ColorForSecond-9);
        hnPixelhits2->SetFillStyle(3001);
        TracksWithoutPixelHits2 = (int)hnPixelhits2->GetBinContent(1);
        hnPixelhits2->SetBinContent(1,0.0);
        if (normalizePlots) hnPixelhits2->Scale(1./(float)iEvents);
        hnPixelhits2 ->DrawCopy("same");
        ent2 = hnPixelhits2->GetEntries();
        mean2 = hnPixelhits2->GetMean();
        
        if (hnPixelhits->GetMaximum() < hnPixelhits2->GetMaximum()) {
            hnPixelhits->GetYaxis()->SetRangeUser(0,1.05*hnPixelhits2->GetMaximum());
            hnPixelhits->DrawCopy("hist");
            hnPixelhits2->DrawCopy("same");
        }
    }
    
    setTitleFont(font);
    
    leg0 = new TLegend(0.47,0.89,0.98,1.0);
    sprintf(name,"%s (%2.1fk)", trkCol_1.Data(), float(iTracks_1)/1000);
    leg0->AddEntry(hnPixelhits,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"%s (%2.1fk)", trkCol_2.Data(), float(iTracks_2)/1000);
        leg0->AddEntry(hnPixelhits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.55,0.75,0.98,0.86);
    sprintf(name,"trks w/o hits: %d",TracksWithoutPixelHits);
    leg0->AddEntry(hnPixelhits,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"trks w/o hits: %d",TracksWithoutPixelHits2);
        leg0->AddEntry(hnPixelhits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.74,0.61,0.98,0.72);
    sprintf(name,"mean: %2.2f",mean1);
    leg0->AddEntry(hnPixelhits,name,"L");
    if (trkCol_2 !=""){
        sprintf(name,"mean:% 2.2f",mean2);
        leg0->AddEntry(hnPixelhits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    

    
    //
    // -----------------------------------------------------------------
    //
    
    cHits->cd(5);
    histname = "IDAlignMon/"+trkCol_1+"/GenericTracks/Nscthits_per_track";
    if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
    hnSCThits = (TH1F*)file->Get(histname);
    if (normalizePlots) hnSCThits->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
        gPad->SetGridy();
        gPad->SetGridx();
    }
    hnSCThits->SetTitle("SCT Hits on track");
    hnSCThits->GetXaxis()->SetTitle("SCT Hits on track");
    hnSCThits->GetYaxis()->SetTitle("Entries");
    hnSCThits->SetLineColor(ColorForFirst+2);
    hnSCThits->SetLineWidth(1);
    hnSCThits->SetFillColor(ColorForFirst);
    hnSCThits->SetFillStyle(3001);
    int TracksWithoutSCTHits = (int)hnSCThits->GetBinContent(1);
    hnSCThits->SetBinContent(1,0.0);
    hnSCThits->SetStats(kFALSE);
    ForceStyle(hnSCThits);
    hnSCThits->DrawCopy("hist");
    setTitleFont(font);
    ScalePlots();
    ent1 = hnSCThits->GetEntries();
    mean1 = hnSCThits->GetMean();

    if (trkCol_2 !=""){
        histname = "IDAlignMon/"+trkCol_2+"/GenericTracks/Nscthits_per_track";
        if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
        
        hnSCThits2 = (TH1F*)file->Get(histname);
        hnSCThits2->SetLineColor(ColorForSecond);
        hnSCThits2->SetLineWidth(1);
        hnSCThits2->SetFillColor(ColorForSecond-9);
        hnSCThits2->SetFillStyle(3001);
        TracksWithoutSCTHits2 = (int)hnSCThits2->GetBinContent(1);
        hnSCThits2->SetBinContent(1,0.0);
        if (normalizePlots) hnSCThits2->Scale(1./(float)iEvents);
        hnSCThits2->DrawCopy("same");
        ent2 = hnSCThits2->GetEntries();
        mean2 = hnSCThits2->GetMean();
        
        if (hnSCThits->GetMaximum() < hnSCThits2->GetMaximum()) {
            hnSCThits->GetYaxis()->SetRangeUser(0,1.05*hnSCThits2->GetMaximum());
            hnSCThits->DrawCopy("hist");
            hnSCThits2->DrawCopy("same");
        }
    }
    setTitleFont(font);
    
    
    leg0 = new TLegend(0.47,0.89,0.98,1.0);
    sprintf(name,"%s (%2.1fk)", trkCol_1.Data(), float(iTracks_1)/1000);
    leg0->AddEntry(hnSCThits,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"%s (%2.1fk)", trkCol_2.Data(), float(iTracks_2)/1000);
        leg0->AddEntry(hnSCThits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.60,0.75,0.98,0.86);
    sprintf(name,"trks w/o hits: %d",TracksWithoutSCTHits);
    leg0->AddEntry(hnSCThits,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"trks w/o hits: %d",TracksWithoutSCTHits2);
        leg0->AddEntry(hnSCThits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.74,0.61,0.98,0.72);
    sprintf(name,"mean: %2.2f",mean1);
    leg0->AddEntry(hnSCThits,name,"L");
    if (trkCol_2 !=""){
        sprintf(name,"mean:% 2.2f",mean2);
        leg0->AddEntry(hnSCThits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    //
    // -----------------------------------------------------------------
    //
    
    cHits->cd(6);
    histname = "IDAlignMon/"+trkCol_1+"/GenericTracks/Ntrthits_per_track";
    if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
    
    hntrthits = (TH1F*)file->Get(histname);
    if (normalizePlots) hntrthits->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
        gPad->SetGridy();
        gPad->SetGridx();
    }
    hntrthits->SetTitle("TRT Hits on track");
    hntrthits->GetXaxis()->SetTitle("TRT Hits on track");
    hntrthits->GetYaxis()->SetTitle("Entries");
    int TracksWithouttrtHits = (int)hntrthits->GetBinContent(1);
    hntrthits->SetBinContent(1,0.0);
    hntrthits->SetLineColor(ColorForFirst+2);
    hntrthits->SetLineWidth(1);
    hntrthits->SetFillColor(ColorForFirst);
    hntrthits->SetFillStyle(3001);
    hntrthits->SetStats(kFALSE);
    hntrthits->DrawCopy("hist");
    setTitleFont(font);
    ScalePlots();
    setTitleFont(font);
    ScalePlots();
    ent1 = hntrthits->GetEntries();
    mean1 = hntrthits->GetMean();

    if (trkCol_2 !=""){
        histname = "IDAlignMon/"+trkCol_2+"/GenericTracks/Ntrthits_per_track";
        if(PrintLevel>0) cout << " <hits> Plotting Histogram: " << histname << endl;
        
        hntrthits2 = (TH1F*)file->Get(histname);
        hntrthits2->SetLineColor(ColorForSecond);
        hntrthits2->SetLineWidth(1);
        hntrthits2->SetFillColor(ColorForSecond-9);
        hntrthits2->SetFillStyle(3001);
        TracksWithouttrtHits2 = (int)hntrthits2->GetBinContent(1);
        hntrthits2->SetBinContent(1,0.0);
        if (normalizePlots) hntrthits2->Scale(1./(float)iEvents);
        hntrthits2->DrawCopy("same");
        ent2 = hntrthits2->GetEntries();
        mean2 = hntrthits2->GetMean();
        
        if (hntrthits->GetMaximum() < hntrthits2->GetMaximum()) {
            hntrthits->GetYaxis()->SetRangeUser(0,1.05*hntrthits2->GetMaximum());
            hntrthits->DrawCopy("hist");
            hntrthits2->DrawCopy("same");
        }
    }
    
    leg0 = new TLegend(0.47,0.89,0.98,1.0);
    sprintf(name,"%s (%2.1fk)", trkCol_1.Data(), float(iTracks_1)/1000);
    leg0->AddEntry(hntrthits,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"%s (%2.1fk)", trkCol_2.Data(), float(iTracks_2)/1000);
        leg0->AddEntry(hntrthits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.55,0.75,0.98,0.86);
    sprintf(name,"trks w/o hits: %d",TracksWithouttrtHits);
    if (trkCol_2 != "") {
        leg0->AddEntry(hntrthits,name,"L");
        sprintf(name,"trks w/o hits: %d",TracksWithouttrtHits2);
        leg0->AddEntry(hntrthits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    leg0 = new TLegend(0.74,0.61,0.98,0.72);
    sprintf(name,"mean: %2.2f",mean1);
    leg0->AddEntry(hntrthits,name,"L");
    if (trkCol_2 != "") {
        sprintf(name,"mean:% 2.2f",mean2);
        leg0->AddEntry(hntrthits2,name,"L");
    }
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
    return;
}
