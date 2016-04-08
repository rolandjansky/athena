/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void AllProcessedPulls() {
  TH1F *hPixelTrkPhiPulls;
  TH1F *hPixelTrkEtaPulls;
  TH1F *h1BarrelSCT_TrkPulls;
  TH1F *hPixel_EndCapTrkPhiPulls;
  TH1F *hPixel_EndCapTrkEtaPulls;
  TH1F *h1EndCapSCT_TrkPulls;
  
  cout << " Processed Pulls..." << endl;

  cAllProcPulls = new TCanvas("AllProcPulls","All Processed Pulls",900,700);
  cAllProcPulls->Divide(3,2);
  
  cAllProcPulls->cd(1);
  TH1F *hPixel_TrkPhiPulls = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiPulBa_1");
  hPixel_TrkPhiPulls->SetTitle("Pixel #phi Processed Pulls (Barrel)");
  hPixel_TrkPhiPulls->GetXaxis()->SetTitle("Pixel Barrel #phi Processed Pulls");
  hPixel_TrkPhiPulls->SetFillColor(192);
  hPixel_TrkPhiPulls->SetLineWidth(1.5);
  if (hPixel_TrkPhiPulls->GetMean()!=0) hPixel_TrkPhiPulls->DrawCopy();
  else {
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel Barrel");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  cAllProcPulls->cd(2);
  TH1F *hPixel_TrkEtaPulls = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaPulBa_1");
  hPixel_TrkEtaPulls->SetTitle("Pixel #eta Processed Pulls (Barrel)");
  hPixel_TrkEtaPulls->GetXaxis()->SetTitle("Pixel Barrel #eta Processed Pulls");
  hPixel_TrkEtaPulls->SetFillColor(192);
  hPixel_TrkEtaPulls->SetLineWidth(1.5);
  if (hPixel_TrkEtaPulls->GetMean()!=0) hPixel_TrkEtaPulls->DrawCopy();
  else {
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel Barrel");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  cAllProcPulls->cd(3);
  TH1F *h1BarrelSCT_TrkPulls = (TH1F*)file->Get("trkana/SCTRes/hSCTPulBa_1");
  h1BarrelSCT_TrkPulls->SetTitle("SCT Processed Pulls (Barrel)");
  h1BarrelSCT_TrkPulls->GetXaxis()->SetTitle("SCT Barrel Processed Pulls");
  h1BarrelSCT_TrkPulls->SetFillColor(192);
  h1BarrelSCT_TrkPulls->SetLineWidth(1.5);
  if (h1BarrelSCT_TrkPulls->GetMean()!=0) h1BarrelSCT_TrkPulls->DrawCopy();
  else {
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"SCT Barrel");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);
    
    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  cAllProcPulls->cd(4);
  TH1F *hPixel_EndCapTrkPhiPulls = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiPulEndcap_1");
  hPixel_EndCapTrkPhiPulls->SetTitle("Pixel #phi Processed Pulls (EndCap)");
  hPixel_EndCapTrkPhiPulls->GetXaxis()->SetTitle("Pixel EndCap #phi Processed Pulls");
  hPixel_EndCapTrkPhiPulls->SetFillColor(192);
  hPixel_EndCapTrkPhiPulls->SetLineWidth(1.5);
  if (hPixel_EndCapTrkPhiPulls->GetMean()!=0) hPixel_EndCapTrkPhiPulls->DrawCopy();
  else {
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel EndCap");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  cAllProcPulls->cd(5);
  TH1F *hPixel_EndCapTrkEtaPulls = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaPulEndcap_1");
  hPixel_EndCapTrkEtaPulls->SetTitle("Pixel #eta Processed Pulls (EndCap)");
  hPixel_EndCapTrkEtaPulls->GetXaxis()->SetTitle("Pixel EndCap #eta Processed Pulls");
  hPixel_EndCapTrkEtaPulls->SetLineColor(13);
  hPixel_EndCapTrkEtaPulls->SetFillColor(192);
  hPixel_EndCapTrkEtaPulls->SetLineWidth(1.5);
  if (hPixel_EndCapTrkEtaPulls->GetMean()!=0) hPixel_EndCapTrkEtaPulls->DrawCopy();
  else {
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel EndCap");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  cAllProcPulls->cd(6);
  TH1F *h1EndCapSCT_TrkPulls = (TH1F*)file->Get("trkana/SCTRes/hSCTPulEndcap_1");
  h1EndCapSCT_TrkPulls->SetTitle("SCT Processed Pulls (EndCap)");
  h1EndCapSCT_TrkPulls->GetXaxis()->SetTitle("SCT EndCap Processed Pulls");
  h1EndCapSCT_TrkPulls->SetFillColor(192);
  h1EndCapSCT_TrkPulls->SetLineWidth(1.5);
  if (h1EndCapSCT_TrkPulls->GetMean()!=0) h1EndCapSCT_TrkPulls->DrawCopy();
  else {
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"SCT EndCap");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  return;

}
