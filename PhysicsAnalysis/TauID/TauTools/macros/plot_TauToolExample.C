/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  gROOT->SetStyle("Plain");                       // plain histogram style
  gStyle->SetOptStat(0);                          // expanded stats box
  gStyle->SetPadTickX(1);                         // tic marks on all axes
  gStyle->SetPadTickY(1);                         //
  gStyle->SetOptFit(1111);                        // the results of the fits
  //gStyle->SetPadGridX(kTRUE);                     // draw horizontal and vertical grids
  //gStyle->SetPadGridY(kTRUE);
  gStyle->SetPalette(1,0);                        // pretty and useful palette
  gStyle->SetHistLineWidth(4);                    // a thicker histogram line
  gStyle->SetFrameFillColor(10);                  // a different frame colour   
  gStyle->SetTitleFillColor(10);                 // title colour to highlight it
  gStyle->SetTitleW(.77);                         // Title Width
  gStyle->SetTitleH(.07);                        // Title height

  TFile *f1 = new TFile("TauToolProngExample.root");
  f1->cd();

  m_eff_reco_1P_TauRec->SetMarkerStyle(21);
  m_eff_reco_1P_TauRec->SetMarkerColor(4);
  m_eff_reco_1P_TauRec->SetMarkerSize(2);
  m_eff_reco_1Peta_TauRec->SetMarkerStyle(21);
  m_eff_reco_1Peta_TauRec->SetMarkerColor(4);
  m_eff_reco_1Peta_TauRec->SetMarkerSize(2);
  m_eff_reco_1Pphi_TauRec->SetMarkerStyle(21);
  m_eff_reco_1Pphi_TauRec->SetMarkerColor(4);
  m_eff_reco_1Pphi_TauRec->SetMarkerSize(2);

  m_eff_id_1P_TauRec->SetMarkerStyle(21);
  m_eff_id_1P_TauRec->SetMarkerColor(4);
  m_eff_id_1P_TauRec->SetMarkerSize(2);
  m_eff_id_1Peta_TauRec->SetMarkerStyle(21);
  m_eff_id_1Peta_TauRec->SetMarkerColor(4);
  m_eff_id_1Peta_TauRec->SetMarkerSize(2);
  m_eff_id_1Pphi_TauRec->SetMarkerStyle(21);
  m_eff_id_1Pphi_TauRec->SetMarkerColor(4);
  m_eff_id_1Pphi_TauRec->SetMarkerSize(2);

  m_eff_reco_3P_TauRec->SetMarkerStyle(22);
  m_eff_reco_3P_TauRec->SetMarkerColor(6);
  m_eff_reco_3P_TauRec->SetMarkerSize(2);
  m_eff_reco_3Peta_TauRec->SetMarkerStyle(22);
  m_eff_reco_3Peta_TauRec->SetMarkerColor(6);
  m_eff_reco_3Peta_TauRec->SetMarkerSize(2);
  m_eff_reco_3Pphi_TauRec->SetMarkerStyle(22);
  m_eff_reco_3Pphi_TauRec->SetMarkerColor(6);
  m_eff_reco_3Pphi_TauRec->SetMarkerSize(2);

  m_eff_id_3P_TauRec->SetMarkerStyle(22);
  m_eff_id_3P_TauRec->SetMarkerColor(6);
  m_eff_id_3P_TauRec->SetMarkerSize(2);
  m_eff_id_3Peta_TauRec->SetMarkerStyle(22);
  m_eff_id_3Peta_TauRec->SetMarkerColor(6);
  m_eff_id_3Peta_TauRec->SetMarkerSize(2);
  m_eff_id_3Pphi_TauRec->SetMarkerStyle(22);
  m_eff_id_3Pphi_TauRec->SetMarkerColor(6);
  m_eff_id_3Pphi_TauRec->SetMarkerSize(2);

  m_eff_reco_13P_TauRec->SetMarkerStyle(24);
  m_eff_reco_13P_TauRec->SetMarkerColor(2);
  m_eff_reco_13P_TauRec->SetMarkerSize(2);
  m_eff_reco_13Peta_TauRec->SetMarkerStyle(24);
  m_eff_reco_13Peta_TauRec->SetMarkerColor(2);
  m_eff_reco_13Peta_TauRec->SetMarkerSize(2);
  m_eff_reco_13Pphi_TauRec->SetMarkerStyle(24);
  m_eff_reco_13Pphi_TauRec->SetMarkerColor(2);
  m_eff_reco_13Pphi_TauRec->SetMarkerSize(2);

  m_eff_id_13P_TauRec->SetMarkerStyle(24);
  m_eff_id_13P_TauRec->SetMarkerColor(2);
  m_eff_id_13P_TauRec->SetMarkerSize(2);
  m_eff_id_13Peta_TauRec->SetMarkerStyle(24);
  m_eff_id_13Peta_TauRec->SetMarkerColor(2);
  m_eff_id_13Peta_TauRec->SetMarkerSize(2);
  m_eff_id_13Pphi_TauRec->SetMarkerStyle(24);
  m_eff_id_13Pphi_TauRec->SetMarkerColor(2);
  m_eff_id_13Pphi_TauRec->SetMarkerSize(2);

  TLegend *aLegend = new TLegend(0.35,0.2,0.65,0.4);
  aLegend->SetFillColor(10);
  aLegend->SetTextAlign(23);
  aLegend->SetTextSize(0.05);
  //aLegend->SetTextColor(2);
  aLegend->SetBorderSize(0);

  TCanvas *c1 = new TCanvas("c1","c1",500,500);
  c1->cd();
  m_eff_reco_13P_TauRec->GetXaxis()->SetTitle("visible p_{T} (GeV)");
  m_eff_reco_13P_TauRec->GetYaxis()->SetTitle("tauRec RECO Efficiency");
  m_eff_reco_13P_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_eff_reco_13P_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_eff_reco_13P_TauRec->SetTitle("RECO Efficiency vs visible  p_{T}");
  m_eff_reco_13P_TauRec->Draw();
  m_eff_reco_1P_TauRec->Draw("same");
  m_eff_reco_3P_TauRec->Draw("same");
  aLegend->AddEntry(m_eff_reco_13P_TauRec,"All prong","p");
  aLegend->AddEntry(m_eff_reco_1P_TauRec,"1 prong","p");
  aLegend->AddEntry(m_eff_reco_3P_TauRec,"3 prong","p");
  aLegend->Draw(); 
  pt = new TPaveText(0.2,0.25,0.3,0.35,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(a)");
  pt->SetTextSize(0.05);
  pt->Draw();
  c1->Print("recoeff_visPt.png");
  c1->Print("recoeff_visPt.eps");
  aLegend->Clear();
  //
  c1->cd();
  m_eff_reco_13Peta_TauRec->GetXaxis()->SetTitle("visible #eta");
  m_eff_reco_13Peta_TauRec->GetYaxis()->SetTitle("tauRec RECO Efficiency");
  m_eff_reco_13Peta_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_eff_reco_13Peta_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_eff_reco_13Peta_TauRec->SetTitle("RECO Efficiency vs visible   #eta");
  m_eff_reco_13Peta_TauRec->Draw();
  m_eff_reco_1Peta_TauRec->Draw("same");
  m_eff_reco_3Peta_TauRec->Draw("same");
  aLegend->AddEntry(m_eff_reco_13Peta_TauRec,"All prong","p");
  aLegend->AddEntry(m_eff_reco_1Peta_TauRec,"1 prong","p");
  aLegend->AddEntry(m_eff_reco_3Peta_TauRec,"3 prong","p");
  aLegend->Draw();  
  pt = new TPaveText(0.2,0.25,0.3,0.35,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(b)");
  pt->SetTextSize(0.05);
  pt->Draw();
  c1->Print("recoeff_viseta.png");
  c1->Print("recoeff_viseta.eps");
  aLegend->Clear();
  //
  c1->cd();
  m_eff_reco_13Pphi_TauRec->GetXaxis()->SetTitle("visible #phi");
  m_eff_reco_13Pphi_TauRec->GetYaxis()->SetTitle("tauRec RECO Efficiency");
  m_eff_reco_13Pphi_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_eff_reco_13Pphi_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_eff_reco_13Pphi_TauRec->SetTitle("RECO Efficiency vs visible   #phi");
  m_eff_reco_13Pphi_TauRec->Draw();
  m_eff_reco_1Pphi_TauRec->Draw("same");
  m_eff_reco_3Pphi_TauRec->Draw("same");
  aLegend->AddEntry(m_eff_reco_13Pphi_TauRec,"All prong","p");
  aLegend->AddEntry(m_eff_reco_1Pphi_TauRec,"1 prong","p");
  aLegend->AddEntry(m_eff_reco_3Pphi_TauRec,"3 prong","p");
  aLegend->Draw();  
  pt = new TPaveText(0.2,0.25,0.3,0.35,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(c)");
  pt->SetTextSize(0.05);
  pt->Draw();
  c1->Print("recoeff_visphi.png");
  c1->Print("recoeff_visphi.eps");
  aLegend->Clear();
  //
  //Reco+ID efficiency
  //
  c1->cd();
  m_eff_id_13P_TauRec->GetXaxis()->SetTitle("visible p_{T} (GeV)");
  m_eff_id_13P_TauRec->GetYaxis()->SetTitle("tauRec RECO+ID Efficiency");
  m_eff_id_13P_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_eff_id_13P_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_eff_id_13P_TauRec->SetTitle("RECO+ID Efficiency vs visible   p_{T}");
  m_eff_id_13P_TauRec->Draw();
  m_eff_id_1P_TauRec->Draw("same");
  m_eff_id_3P_TauRec->Draw("same");
  aLegend->AddEntry(m_eff_id_13P_TauRec,"All prong","p");
  aLegend->AddEntry(m_eff_id_1P_TauRec,"1 prong","p");
  aLegend->AddEntry(m_eff_id_3P_TauRec,"3 prong","p");
  aLegend->Draw(); 
  pt = new TPaveText(0.2,0.25,0.3,0.35,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(a)");
  pt->SetTextSize(0.05);
  pt->Draw();
  c1->Print("recoIDeff_visPt.png");
  c1->Print("recoIDeff_visPt.eps");
  aLegend->Clear();
  //
  c1->cd();
  m_eff_id_13Peta_TauRec->GetXaxis()->SetTitle("visible #eta");
  m_eff_id_13Peta_TauRec->GetYaxis()->SetTitle("tauRec RECO+ID Efficiency");
  m_eff_id_13Peta_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_eff_id_13Peta_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_eff_id_13Peta_TauRec->SetTitle("RECO+ID Efficiency vs visible   #eta");
  m_eff_id_13Peta_TauRec->Draw();
  m_eff_id_1Peta_TauRec->Draw("same");
  m_eff_id_3Peta_TauRec->Draw("same");
  aLegend->AddEntry(m_eff_id_13Peta_TauRec,"All prong","p");
  aLegend->AddEntry(m_eff_id_1Peta_TauRec,"1 prong","p");
  aLegend->AddEntry(m_eff_id_3Peta_TauRec,"3 prong","p");
  aLegend->Draw(); 
  pt = new TPaveText(0.2,0.25,0.3,0.35,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(b)");
  pt->SetTextSize(0.05);
  pt->Draw(); 
  c1->Print("recoIDeff_viseta.png");
  c1->Print("recoIDeff_viseta.eps");
  aLegend->Clear();
  //
  c1->cd();
  m_eff_id_13Pphi_TauRec->GetXaxis()->SetTitle("visible #phi");
  m_eff_id_13Pphi_TauRec->GetYaxis()->SetTitle("tauRec RECO+ID Efficiency");
  m_eff_id_13Pphi_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_eff_id_13Pphi_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_eff_id_13Pphi_TauRec->SetTitle("RECO+ID Efficiency vs visible   #phi");
  m_eff_id_13Pphi_TauRec->Draw();
  m_eff_id_1Pphi_TauRec->Draw("same");
  m_eff_id_3Pphi_TauRec->Draw("same");
  aLegend->AddEntry(m_eff_id_13Pphi_TauRec,"All prong","p");
  aLegend->AddEntry(m_eff_id_1Pphi_TauRec,"1 prong","p");
  aLegend->AddEntry(m_eff_id_3Pphi_TauRec,"3 prong","p");
  aLegend->Draw();  
  pt = new TPaveText(0.2,0.25,0.3,0.35,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(c)");
  pt->SetTextSize(0.05);
  pt->Draw();
  c1->Print("recoIDeff_visphi.png");
  c1->Print("recoIDeff_visphi.eps");
  aLegend->Clear();
  //
  //
  //
  TFile *f2 = new TFile("TauToolResolutionExample.root");
  f2->cd();

  m_etRes_reco_TauRec->SetLineColor(2);
  m_etaRes_reco_TauRec->SetLineColor(2);
  m_phiRes_reco_TauRec->SetLineColor(2);

  m_etRes_id_TauRec->SetLineColor(4);
  m_etaRes_id_TauRec->SetLineColor(4);
  m_phiRes_id_TauRec->SetLineColor(4);

  m_etRes_reco_TauRec->SetLineWidth(3);
  m_etaRes_reco_TauRec->SetLineWidth(3);
  m_phiRes_reco_TauRec->SetLineWidth(3);

  m_etRes_id_TauRec->SetLineWidth(3);
  m_etaRes_id_TauRec->SetLineWidth(3);
  m_phiRes_id_TauRec->SetLineWidth(3);

  TLegend *aLeg = new TLegend(0.65,0.65,0.8,0.85);
  aLeg->SetFillColor(10);
  //aLeg->SetTextAlign(23);
  aLeg->SetTextSize(0.05);
  //aLeg->SetTextColor(2);
  aLeg->SetBorderSize(0);

  //
  c1->cd();
  m_etRes_reco_TauRec->GetXaxis()->SetTitle("(p_{T}^{visible} - p_{T})/p_{T}^{visible} ");
  //  m_etRes_reco_TauRec->GetYaxis()->SetTitle("tauRec RECO Efficiency");
  m_etRes_reco_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_etRes_reco_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_etRes_reco_TauRec->SetTitle("p_{T} Resolution");
  m_etRes_reco_TauRec->Draw();
  m_etRes_id_TauRec->Draw("same");
  aLeg->AddEntry(m_etRes_reco_TauRec,"RECO","l");
  aLeg->AddEntry(m_etRes_id_TauRec,"RECO+ID","l");
  aLeg->Draw();  
  pt = new TPaveText(0.2,0.7,0.3,0.8,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(a)");
  pt->SetTextSize(0.05);
  pt->Draw();
  c1->Print("etResolution.png");
  c1->Print("etResolution.eps");
  aLeg->Clear();
  //
  c1->cd();
  m_etaRes_reco_TauRec->GetXaxis()->SetTitle("#eta^{visible} - #eta");
  //  m_etaRes_reco_TauRec->GetYaxis()->SetTitle("tauRec RECO Efficiency");
  m_etaRes_reco_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_etaRes_reco_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_etaRes_reco_TauRec->SetTitle("#eta  Resolution");
  m_etaRes_reco_TauRec->Draw();
  m_etaRes_id_TauRec->Draw("same");
  aLeg->AddEntry(m_etaRes_reco_TauRec,"RECO","l");
  aLeg->AddEntry(m_etaRes_id_TauRec,"RECO+ID","l");
  aLeg->Draw();  
  pt = new TPaveText(0.2,0.7,0.3,0.8,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(b)");
  pt->SetTextSize(0.05);
  pt->Draw();
  c1->Print("etaResolution.png");
  c1->Print("etaResolution.eps");
  aLeg->Clear();
  //
  c1->cd();
  m_phiRes_reco_TauRec->GetXaxis()->SetTitle("#phi^{visible} - #phi");
  //  m_phiRes_reco_TauRec->GetYaxis()->SetTitle("tauRec RECO Efficiency");
  m_phiRes_reco_TauRec->GetXaxis()->SetTitleOffset(1.2);
  m_phiRes_reco_TauRec->GetYaxis()->SetTitleOffset(1.2);
  m_phiRes_reco_TauRec->SetTitle("#phi  Resolution");
  m_phiRes_reco_TauRec->Draw();
  m_phiRes_id_TauRec->Draw("same");
  aLeg->AddEntry(m_phiRes_reco_TauRec,"RECO","l");
  aLeg->AddEntry(m_phiRes_id_TauRec,"RECO+ID","l");
  aLeg->Draw(); 
  pt = new TPaveText(0.2,0.7,0.3,0.8,"brNDC");
  pt->SetLineColor(0);
  pt->SetBorderSize(1);
  pt->SetFillColor(10);
  text = pt->AddText("(c)");
  pt->SetTextSize(0.05);
  pt->Draw(); 
  c1->Print("phiResolution.png");
  c1->Print("phiResolution.eps");
  aLeg->Clear();
  //
}
