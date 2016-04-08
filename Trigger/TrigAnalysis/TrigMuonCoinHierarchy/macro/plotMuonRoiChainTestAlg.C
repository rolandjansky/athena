/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void plotMuonRoiChainTestAlg() {
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetOptStat(111110);
  gStyle->SetPadTopMargin(0.20);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin(0.20);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.18);
  gStyle->SetStatH(0.45);
  gStyle->SetStatColor(0);
  gStyle->SetStatBorderSize(1); 
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0); 
  gStyle->SetTitleX(0.02); 
  gStyle->SetTitleY(0.99); 
  gStyle->SetTitleH(0.18); 
  gStyle->SetTitleW(0.78); 

  TFile *fin = new TFile("MuonRoiChainTestAlg.root");

  TH2I *m_h_ROI_RDO = (TH2I*)fin->Get("h_ROI_RDO");
  TH2I *m_h_RDO_ROI = (TH2I*)fin->Get("h_RDO_ROI");
  TH2I *m_h_RDO_TgcCoinData = (TH2I*)fin->Get("h_RDO_TgcCoinData");
  TH2I *m_h_TgcCoinData_RDO = (TH2I*)fin->Get("h_TgcCoinData_RDO");

  TCanvas *c = new TCanvas("c", "c", 800, 800);

  c->cd(0);
  TPad *p0 = new TPad("p0", "p0", 0., 0., 0.2, 1.0);
  p0->Draw();
  p0->cd();
  TText *txt = new TText();
  txt->SetTextAlign(22);
  txt->SetTextSize(0.15);
  txt->DrawText(0.5, 0.82, "ROI Builder");
  txt->DrawText(0.5, 0.78, "Muon ROI");
  txt->DrawText(0.5, 0.52, "MuCTPI");
  txt->DrawText(0.5, 0.48, "ROI");
  txt->DrawText(0.5, 0.22, "TGC");
  txt->DrawText(0.5, 0.18, "Sector Logic");
  TArrow *arrow = new TArrow();
  arrow->SetLineWidth(2);

  arrow->SetLineColor(kBlue);
  arrow->SetFillColor(kBlue);
  arrow->DrawArrow(0.4, 0.76, 0.4, 0.54, 0.05);

  arrow->SetLineColor(kRed);
  arrow->SetFillColor(kRed);
  arrow->DrawArrow(0.6, 0.54, 0.6, 0.76, 0.05);

  arrow->SetLineColor(kOrange+1);
  arrow->SetFillColor(kOrange+1);
  arrow->DrawArrow(0.4, 0.46, 0.4, 0.24, 0.05);

  arrow->SetLineColor(kGreen+2);
  arrow->SetFillColor(kGreen+2);
  arrow->DrawArrow(0.6, 0.24, 0.6, 0.46, 0.05);

  c->cd(0);
  TPad *p1 = new TPad("p1", "p1", 0.2, 0., 1.0, 1.0);
  p1->Draw();

  p1->Divide(1, 4);
  
  TLine *line = new TLine();
  line->SetLineWidth(2);
  line->SetLineColor(kMagenta); 

  char percentage[1000];
  txt->SetTextSize(0.08);

  p1->cd(1);
  m_h_ROI_RDO->Draw("text");
  m_h_ROI_RDO->SetMarkerSize(5.);
  m_h_ROI_RDO->SetMarkerColor(kBlue);
  m_h_ROI_RDO->GetXaxis()->SetLabelSize(0.12);
  m_h_ROI_RDO->GetYaxis()->SetLabelSize(0.14);
  line->DrawLine(-0.46, -0.46,  5.46, -0.46); 
  line->DrawLine( 5.46, -0.46,  5.46,  0.46); 
  line->DrawLine( 5.46,  0.46, -0.46,  0.46); 
  line->DrawLine(-0.46,  0.46, -0.46, -0.46); 
  int nAll = m_h_ROI_RDO->GetEntries(); 
  int nGood = 0;
  if(nAll>0) {
    nGood = 
      m_h_ROI_RDO->GetBinContent(1, 2) +
      m_h_ROI_RDO->GetBinContent(2, 2) +
      m_h_ROI_RDO->GetBinContent(3, 2) +
      m_h_ROI_RDO->GetBinContent(4, 2) +
      m_h_ROI_RDO->GetBinContent(5, 2) +
      m_h_ROI_RDO->GetBinContent(6, 2);
    p0->cd();
    sprintf(percentage, "%7.3f%1s", (float)nGood/(float)nAll*100., "%");
    txt->DrawText(0.2, 0.65, percentage); 
  }

  p1->cd(2);
  m_h_RDO_ROI->Draw("text");
  m_h_RDO_ROI->SetMarkerSize(5.);
  m_h_RDO_ROI->SetMarkerColor(kRed);
  m_h_RDO_ROI->GetXaxis()->SetLabelSize(0.12);
  m_h_RDO_ROI->GetYaxis()->SetLabelSize(0.14);
  line->DrawLine(-0.46, -0.46,  5.46, -0.46); 
  line->DrawLine( 5.46, -0.46,  5.46,  0.46); 
  line->DrawLine( 5.46,  0.46, -0.46,  0.46); 
  line->DrawLine(-0.46,  0.46, -0.46, -0.46); 
  nAll = m_h_RDO_ROI->GetEntries(); 
  if(nAll>0) { 
    nGood = 
      m_h_RDO_ROI->GetBinContent(1, 2) +
      m_h_RDO_ROI->GetBinContent(2, 2) +
      m_h_RDO_ROI->GetBinContent(3, 2) +
      m_h_RDO_ROI->GetBinContent(4, 2) +
      m_h_RDO_ROI->GetBinContent(5, 2) +
      m_h_RDO_ROI->GetBinContent(6, 2) +
      m_h_RDO_ROI->GetBinContent(1, 3) +
      m_h_RDO_ROI->GetBinContent(2, 3) +
      m_h_RDO_ROI->GetBinContent(3, 3) +
      m_h_RDO_ROI->GetBinContent(4, 3) +
      m_h_RDO_ROI->GetBinContent(5, 3) +
      m_h_RDO_ROI->GetBinContent(6, 3);
    p0->cd();
    sprintf(percentage, "%7.3f%1s", (float)nGood/(float)nAll*100., "%");
    txt->DrawText(0.8, 0.65, percentage); 
  }

  p1->cd(3);
  m_h_RDO_TgcCoinData->Draw("text");
  m_h_RDO_TgcCoinData->SetMarkerSize(5.);
  m_h_RDO_TgcCoinData->SetMarkerColor(kOrange+1);
  m_h_RDO_TgcCoinData->GetXaxis()->SetLabelSize(0.05);
  m_h_RDO_TgcCoinData->GetYaxis()->SetLabelSize(0.14);
  line->DrawLine(-0.46, -0.46,  0.46, -0.46); 
  line->DrawLine( 0.46, -0.46,  0.46,  0.46); 
  line->DrawLine( 0.46,  0.46, -0.46,  0.46); 
  line->DrawLine(-0.46,  0.46, -0.46, -0.46); 
  line->DrawLine( 1.46, -0.46,  3.46, -0.46); 
  line->DrawLine( 3.46, -0.46,  3.46,  0.46); 
  line->DrawLine( 3.46,  0.46,  1.46,  0.46); 
  line->DrawLine( 1.46,  0.46,  1.46, -0.46); 
  line->DrawLine( 4.46, -0.46,  5.46, -0.46); 
  line->DrawLine( 5.46, -0.46,  5.46,  0.46); 
  line->DrawLine( 5.46,  0.46,  4.46,  0.46); 
  line->DrawLine( 4.46,  0.46,  4.46, -0.46); 
  nAll = m_h_RDO_TgcCoinData->GetEntries(); 
  if(nAll>0) {
    nGood = 
      m_h_RDO_TgcCoinData->GetBinContent(1, 2) +
      m_h_RDO_TgcCoinData->GetBinContent(2, 2) +
      m_h_RDO_TgcCoinData->GetBinContent(3, 2) +
      m_h_RDO_TgcCoinData->GetBinContent(4, 2) +
      m_h_RDO_TgcCoinData->GetBinContent(5, 2) +
      m_h_RDO_TgcCoinData->GetBinContent(6, 2) +
      m_h_RDO_TgcCoinData->GetBinContent(2, 1) +
      m_h_RDO_TgcCoinData->GetBinContent(5, 1);
    p0->cd();
    sprintf(percentage, "%7.3f%1s", (float)nGood/(float)nAll*100., "%");
    txt->DrawText(0.2, 0.35, percentage); 
  }

  p1->cd(4);
  m_h_TgcCoinData_RDO->Draw("text");
  m_h_TgcCoinData_RDO->SetMarkerSize(5.);
  m_h_TgcCoinData_RDO->SetMarkerColor(kGreen+2);
  m_h_TgcCoinData_RDO->GetXaxis()->SetLabelSize(0.16);
  m_h_TgcCoinData_RDO->GetYaxis()->SetLabelSize(0.14);
  line->DrawLine(-0.46, -0.46,  3.46, -0.46); 
  line->DrawLine( 3.46, -0.46,  3.46,  0.46); 
  line->DrawLine( 3.46,  0.46, -0.46,  0.46); 
  line->DrawLine(-0.46,  0.46, -0.46, -0.46); 
  nAll = m_h_TgcCoinData_RDO->GetEntries(); 
  if(nAll>0) {
    nGood = 
      m_h_TgcCoinData_RDO->GetBinContent(1, 2) +
      m_h_TgcCoinData_RDO->GetBinContent(2, 2) +
      m_h_TgcCoinData_RDO->GetBinContent(3, 2) +
      m_h_TgcCoinData_RDO->GetBinContent(4, 2);
    p0->cd();
    sprintf(percentage, "%7.3f%1s", (float)nGood/(float)nAll*100., "%");
    txt->DrawText(0.8, 0.35, percentage); 
  }
}
