/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void chi2() {

  TH1F *histChi2;
  TH1F *histChi2_2;
  TH2F *hRecoChi2RecoPhi0;
  TH2F *hRecoChi2RecoPhi0_2;
  TH2F *hRecoChi2RecoEta;
  TH2F *hRecoChi2RecoEta2;
  TH2F *hRecoChi2RecoPt;
  TH2F *hRecoChi2RecoPt_2;
  
  cout << " Chi2 information..." << endl;
  
  cChi2 = new TCanvas("cChi2","Chi2 information",
		      CanvasSizeX4[0],CanvasSizeX4[1]);
  cChi2->Divide(2,2);
  
  cChi2->cd(1);
  histChi2 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoChi2_0");
  if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
  gPad->SetLogy();
  histChi2->SetTitle("#chi_{2} distribution");
  histChi2->GetYaxis()->SetTitle("Number of tracks");
  histChi2->GetXaxis()->SetTitle("#chi^{2}/ndof");
  histChi2->GetXaxis()->SetRangeUser(0,14);
  histChi2->SetLineColor(ColorForAll+2);
  histChi2->SetLineWidth(1);
  histChi2->SetFillColor(ColorForAll);
  histChi2->SetFillStyle(3001);
  histChi2->DrawCopy();
  setTitleFont(font);
  
  histChi2_2 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoChi2_1");
  if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
  histChi2_2->SetLineColor(ColorForUsed);
  histChi2_2->SetLineWidth(1);
  histChi2_2->SetFillColor(ColorForUsed-9);
  histChi2_2->SetFillStyle(3001);
  histChi2_2->DrawCopy("SAME");
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(histChi2,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(histChi2_2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //
  
  cChi2->cd(2);
  hRecoChi2RecoPhi0 = (TH2F*)file->Get("trkana/Track_Parameters/hRecoChi2_RecoPhi0_0");
  if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
  hRecoChi2RecoPhi0->SetTitle("#chi^{2} Vs #phi0");
  
  TProfile *HProfi = hRecoChi2RecoPhi0->ProfileX(); 
  HProfi->SetLineColor(ColorForAll+2);
  HProfi->SetMarkerColor(ColorForAll+2);
  HProfi->SetMarkerStyle(21);
  HProfi->SetMarkerSize(0.4);
  HProfi->GetXaxis()->SetTitle("#phi0 (rad)");
  HProfi->GetYaxis()->SetTitle("#chi^{2}/ndof");
  HProfi->GetYaxis()->SetRangeUser(0.85*HProfi->GetMinimum(),1.15*HProfi->GetMaximum());
  HProfi->DrawCopy();
  setTitleFont(font);

  hRecoChi2RecoPhi0_2 = (TH2F*)file->Get("trkana/Track_Parameters/hRecoChi2_RecoPhi0_1");
  TProfile *HProfi2 = hRecoChi2RecoPhi0_2->ProfileX(); 
  HProfi2->SetLineColor(ColorForUsed);
  HProfi2->SetMarkerColor(ColorForUsed);
  HProfi2->SetMarkerStyle(21);
  HProfi2->SetMarkerSize(0.4);
  HProfi2->DrawCopy("SAME");

  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(HProfi,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(HProfi2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //
  
  cChi2->cd(3);
  hRecoChi2RecoEta = (TH2F*)file->Get("trkana/Track_Parameters/hRecoChi2_RecoEta_0");
  if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
  hRecoChi2RecoEta->SetTitle("#chi^{2} Vs eta");

  TProfile *HProfi3 = hRecoChi2RecoEta->ProfileX(); 
  HProfi3->SetLineColor(ColorForAll+2);
  HProfi3->SetMarkerColor(ColorForAll+2);
  HProfi3->SetMarkerStyle(21);
  HProfi3->SetMarkerSize(0.4);
  HProfi3->GetXaxis()->SetTitle("#eta");
  HProfi3->GetYaxis()->SetTitle("#chi^{2}/ndof");
  HProfi3->GetYaxis()->SetRangeUser(HProfi3->GetMinimum(),1.3*HProfi3->GetMaximum());
  HProfi3->DrawCopy();
  setTitleFont(font);

  hRecoChi2RecoEta2 = (TH2F*)file->Get("trkana/Track_Parameters/hRecoChi2_RecoEta_1");
  TProfile *HProfi4 = hRecoChi2RecoEta2->ProfileX(); 
  HProfi4->SetLineColor(ColorForUsed);
  HProfi4->SetMarkerColor(ColorForUsed);
  HProfi4->SetMarkerStyle(21);
  HProfi4->SetMarkerSize(0.4);
  HProfi4->DrawCopy("SAME");

  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(HProfi3,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(HProfi4,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
  
  //
  // -----------------------------------------------------------------
  //
  
  cChi2->cd(4);
  hRecoChi2RecoPt = (TH2F*)file->Get("trkana/Track_Parameters/hRecoChi2_RecoPt_0");
  if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
  hRecoChi2RecoPt->SetTitle("#chi^{2} Vs pT");

  TProfile * hProfRecoChi2RecoPt = hRecoChi2RecoPt->ProfileX(); 
  hProfRecoChi2RecoPt->SetLineColor(ColorForAll+2);
  hProfRecoChi2RecoPt->SetMarkerColor(ColorForAll+2);
  hProfRecoChi2RecoPt->SetMarkerStyle(21);
  hProfRecoChi2RecoPt->SetMarkerSize(0.4);
  hProfRecoChi2RecoPt->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hProfRecoChi2RecoPt->GetYaxis()->SetTitle("#chi^{2}/ndof");
  hProfRecoChi2RecoPt->GetYaxis()->SetRangeUser(hProfRecoChi2RecoPt->GetMinimum(),1.4* hProfRecoChi2RecoPt->GetMaximum());
  hProfRecoChi2RecoPt->DrawCopy();
  setTitleFont(font);

  hRecoChi2RecoPt2 = (TH2F*)file->Get("trkana/Track_Parameters/hRecoChi2_RecoPt_1");
  TProfile* hProfRecoChi2RecoPt2 = hRecoChi2RecoPt2->ProfileX(); 
  hProfRecoChi2RecoPt2->SetLineColor(ColorForUsed);
  hProfRecoChi2RecoPt2->SetMarkerColor(ColorForUsed);
  hProfRecoChi2RecoPt2->DrawCopy("SAME");
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(hProfRecoChi2RecoPt,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(hProfRecoChi2RecoPt2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
  cChi2->Update();  
  
  return;
}
