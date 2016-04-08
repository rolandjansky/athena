/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void MeanResPerLayer() {

  cout << " Mean Residuals per Layer..." << endl;

  cMeanResPerLayer = new TCanvas("AllResidualsPerLayer","All Residuals per Layer",1100,700);
  cMeanResPerLayer->Divide(3,2);

  TCut cut1 = "TrkRes!=0.0";
  TCut cut2 = "Hit_DetType==2 && Hit_BEc==0";
  TCut cutT = "Hit_phi<15"; // Top Sector
  TCut cutB = "Hit_phi>18"; // Bottom Sector

  Float_t jMax = 0.0;
  Float_t sMax = 0.0;
  
  /*
  cMeanResPerLayer->cd(1);
  tree->Draw("TrkRes>>h3SCT_TrkRes(150,-0.2,0.2)",cut1+cut2,"goff");
  h3SCT_TrkRes->SetTitle("SCT Residuals (Barrel)");
  h3SCT_TrkRes->GetXaxis()->SetTitle("SCT Residuals (mm)");
  h3SCT_TrkRes->SetLineColor(13);
  h3SCT_TrkRes->SetFillColor(18);
  h3SCT_TrkRes->SetLineWidth(1.5);
  h3SCT_TrkRes->Draw();
  /*
  cMeanResPerLayer->cd(2);
  tree->Draw("TrkRes>>h3TopSCT_TrkRes(150,-0.2,0.2)",cut1+cut2+cutT,"goff");
  h3TopSCT_TrkRes->SetTitle("SCT Residuals (Top Barrel)");
  h3TopSCT_TrkRes->GetXaxis()->SetTitle("SCT Residuals (mm)");
  h3TopSCT_TrkRes->SetLineColor(13);
  h3TopSCT_TrkRes->SetFillColor(18);
  h3TopSCT_TrkRes->SetLineWidth(1.5);
  h3TopSCT_TrkRes->Draw();

  cMeanResPerLayer->cd(3);
  tree->Draw("TrkRes>>h3BotSCT_TrkRes(150,-0.2,0.2)",cut1+cut2+cutB,"goff");
  h3BotSCT_TrkRes->SetTitle("SCT Residuals (Bottom Barrel)");
  h3BotSCT_TrkRes->GetXaxis()->SetTitle("SCT Residuals (mm)");
  h3BotSCT_TrkRes->SetLineColor(13);
  h3BotSCT_TrkRes->SetFillColor(18);
  h3BotSCT_TrkRes->SetLineWidth(1.5);
  h3BotSCT_TrkRes->Draw();
  */

  // ********************************************
  //
  // Full
  //
  // ********************************************
  tree->Draw("1000*TrkRes:Hit_layer",cut1+cut2,"profsgoff");
  TProfile *h3temp = (TProfile*)gDirectory->FindObject("htemp");
  TCut cut3 = "Hit_side==0";
  tree->Draw("1000*TrkRes:Hit_layer>>htempSCTSide0",cut1+cut2+cut3,"profsgoff");
  TCut cut3 = "Hit_side==1";
  tree->Draw("1000*TrkRes:Hit_layer>>htempSCTSide1",cut1+cut2+cut3,"profsgoff");

  Double_t value[4] = {0.0};
  Double_t valueSide0[4] = {0.0};
  Double_t valueSide1[4] = {0.0};

  Double_t sigma[4] = {0.0};
  Double_t sigmaSide0[4] = {0.0};
  Double_t sigmaSide1[4] = {0.0};

  if (h3temp->GetMean()!=0) {
    jMax = 0.0;
    sMax = 0.0;
    for (int i=1;i<5;i++) {
      value[i-1] = h3temp->GetBinContent(i);
      valueSide0[i-1] = htempSCTSide0->GetBinContent(i);
      valueSide1[i-1] = htempSCTSide1->GetBinContent(i);
      sigma[i-1] = h3temp->GetBinError(i);
      sigmaSide0[i-1] = htempSCTSide0->GetBinError(i);
      sigmaSide1[i-1] = htempSCTSide1->GetBinError(i);
      Float_t ivalue = fabs(value[i-1]);
      Float_t isigma = fabs(sigma[i-1]);
      // cout << "Both sides: " << value[i-1] << endl;
      // cout << " Side0: " << valueSide0[i-1] << endl;
      // cout << " Side1: " << valueSide1[i-1] << endl;
      jMax = max(jMax,fabs(value[i-1]));
      // cout << "Both sides: " << sigma[i-1] << endl;
      // cout << " Side0: " << sigmaSide0[i-1] << endl;
      // cout << " Side1: " << sigmaSide1[i-1] << endl;
      sMax = max(sMax,fabs(sigma[i-1]));
    }
    // cout << int(jMax)+2 << endl;
    delete h3temp;
    if (jMax<100) jMax = int(jMax)+2;
    else jMax = int(jMax)+25;
    if (sMax<100) sMax = int(sMax)+10;
    else sMax = int(sMax)+25;

    TProfile *hprofSCTBarrelResXLy = new TProfile("hprofSCTBarrelResXLy","hprofSCTBarrelResXLy",8,0,4);
    hprofSCTBarrelResXLy->GetYaxis()->SetRangeUser(-jMax,jMax);
    hprofSCTBarrelResXLy->SetTitle("SCT Barrel");
    hprofSCTBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hprofSCTBarrelResXLy->GetYaxis()->SetTitle("SCT Barrel Residual Mean (#mum)");
    TProfile *hprofSCTBarrelResXLySide0 = new TProfile("hprofSCTBarrelResXLySide0","hprofSCTBarrelResXLySide0",8,0,4);
    hprofSCTBarrelResXLySide0->SetLineColor(kBlue);
    TProfile *hprofSCTBarrelResXLySide1 = new TProfile("hprofSCTBarrelResXLySide1","hprofSCTBarrelResXLySide1",8,0,4);
    hprofSCTBarrelResXLySide1->SetLineColor(kRed);

    TProfile *hsprofSCTBarrelResXLy = new TProfile("hsprofSCTBarrelResXLy","hsprofSCTBarrelResXLy",8,0,4);
    hsprofSCTBarrelResXLy->GetYaxis()->SetRangeUser(0.0,sMax);
    hsprofSCTBarrelResXLy->SetTitle("SCT Barrel");
    hsprofSCTBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hsprofSCTBarrelResXLy->GetYaxis()->SetTitle("SCT Barrel Residual sigma (#mum)");
    TProfile *hsprofSCTBarrelResXLySide0 = new TProfile("hsprofSCTBarrelResXLySide0","hsprofSCTBarrelResXLySide0",8,0,4);
    hsprofSCTBarrelResXLySide0->SetLineColor(kBlue);
    TProfile *hsprofSCTBarrelResXLySide1 = new TProfile("hsprofSCTBarrelResXLySide1","hsprofSCTBarrelResXLySide1",8,0,4);
    hsprofSCTBarrelResXLySide1->SetLineColor(kRed);


    for (int i=0;i<4;i++) {
      hprofSCTBarrelResXLy->Fill(i,value[i],1);
      hprofSCTBarrelResXLy->Fill(i+0.5,value[i],1);
      hprofSCTBarrelResXLySide0->Fill(i,valueSide0[i],1);
      hprofSCTBarrelResXLySide1->Fill(i+0.5,valueSide1[i],1);
      hsprofSCTBarrelResXLy->Fill(i,sigma[i],1);
      hsprofSCTBarrelResXLy->Fill(i+0.5,sigma[i],1);
      hsprofSCTBarrelResXLySide0->Fill(i,sigmaSide0[i],1);
      hsprofSCTBarrelResXLySide1->Fill(i+0.5,sigmaSide1[i],1);
      // cout << value[i] << endl;
    }

    cMeanResPerLayer->cd(1);
    gPad->SetGridy();
    gPad->SetGridx();
    hprofSCTBarrelResXLy->DrawCopy();
    hprofSCTBarrelResXLySide0->DrawCopy("SAME");
    hprofSCTBarrelResXLySide1->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.51,0.88,0.75,1.0);
    sprintf(name,"Both Sides");
    leg0->AddEntry(hprofSCTBarrelResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTBarrelResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTBarrelResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    cMeanResPerLayer->cd(4);
    gPad->SetGridy();
    gPad->SetGridx();
    hsprofSCTBarrelResXLy->DrawCopy();
    hsprofSCTBarrelResXLySide0->DrawCopy("SAME");
    hsprofSCTBarrelResXLySide1->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.51,0.88,0.75,1.0);
    sprintf(name,"Both Sides");
    leg0->AddEntry(hprofSCTBarrelResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTBarrelResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTBarrelResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }
  else {
    cMeanResPerLayer->cd(1);
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

    cMeanResPerLayer->cd(4);
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

  // ********************************************
  //
  // Top
  //
  // ********************************************
  tree->Draw("1000*TrkRes:Hit_layer",cut1+cut2+cutT,"profsgoff");
  TProfile *h3Toptemp = (TProfile*)gDirectory->FindObject("htemp");
  TCut cut3 = "Hit_side==0";
  tree->Draw("1000*TrkRes:Hit_layer>>htempTopSCTSide0",cut1+cut2+cut3+cutT,"profsgoff");
  TCut cut3 = "Hit_side==1";
  tree->Draw("1000*TrkRes:Hit_layer>>htempTopSCTSide1",cut1+cut2+cut3+cutT,"profsgoff");

  Double_t valueTop[4] = {0.0};
  Double_t valueTopSide0[4] = {0.0};
  Double_t valueTopSide1[4] = {0.0};

  Double_t sigmaTop[4] = {0.0};
  Double_t sigmaTopSide0[4] = {0.0};
  Double_t sigmaTopSide1[4] = {0.0};

  if (h3Toptemp->GetMean()!=0) {
    jMax = 0.0;
    sMax = 0.0;
    for (int i=1;i<5;i++) {
      valueTop[i-1] = h3Toptemp->GetBinContent(i);
      valueTopSide0[i-1] = htempTopSCTSide0->GetBinContent(i);
      valueTopSide1[i-1] = htempTopSCTSide1->GetBinContent(i);
      sigmaTop[i-1] = h3Toptemp->GetBinError(i);
      sigmaTopSide0[i-1] = htempTopSCTSide0->GetBinError(i);
      sigmaTopSide1[i-1] = htempTopSCTSide1->GetBinError(i);
      Float_t ivalueTop = fabs(valueTop[i-1]);
      Float_t isigmaTop = fabs(sigmaTop[i-1]);
      // cout << "Both sides: " << valueTop[i-1] << endl;
      // cout << " Side0: " << valueTopSide0[i-1] << endl;
      // cout << " Side1: " << valueTopSide1[i-1] << endl;
      jMax = max(jMax,fabs(valueTop[i-1]));
      // cout << "Both sides: " << sigmaTop[i-1] << endl;
      // cout << " Side0: " << sigmaTopSide0[i-1] << endl;
      // cout << " Side1: " << sigmaTopSide1[i-1] << endl;
      sMax = max(sMax,fabs(sigmaTop[i-1]));
    }
    // cout << int(jMax)+2 << endl;
    delete h3Toptemp;
    if (jMax<100) jMax = int(jMax)+2;
    else jMax = int(jMax)+25;
    if (sMax<100) sMax = int(sMax)+10;
    else sMax = int(sMax)+25;

    TProfile *hprofSCTTopBarrelResXLy = new TProfile("hprofSCTTopBarrelResXLy","hprofSCTTopBarrelResXLy",8,0,4);
    hprofSCTTopBarrelResXLy->GetYaxis()->SetRangeUser(-jMax,jMax);
    hprofSCTTopBarrelResXLy->SetTitle("SCT Top Barrel");
    hprofSCTTopBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hprofSCTTopBarrelResXLy->GetYaxis()->SetTitle("SCT Top Barrel Residual Mean (#mum)");
    TProfile *hprofSCTTopBarrelResXLySide0 = new TProfile("hprofSCTTopBarrelResXLySide0","hprofSCTTopBarrelResXLySide0",8,0,4);
    hprofSCTTopBarrelResXLySide0->SetLineColor(kBlue);
    TProfile *hprofSCTTopBarrelResXLySide1 = new TProfile("hprofSCTTopBarrelResXLySide1","hprofSCTTopBarrelResXLySide1",8,0,4);
    hprofSCTTopBarrelResXLySide1->SetLineColor(kRed);

    TProfile *hsprofSCTTopBarrelResXLy = new TProfile("hsprofSCTTopBarrelResXLy","hsprofSCTTopBarrelResXLy",8,0,4);
    hsprofSCTTopBarrelResXLy->GetYaxis()->SetRangeUser(0.0,sMax);
    hsprofSCTTopBarrelResXLy->SetTitle("SCT Top Barrel");
    hsprofSCTTopBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hsprofSCTTopBarrelResXLy->GetYaxis()->SetTitle("SCT Top Barrel Residual sigma (#mum)");
    TProfile *hsprofSCTTopBarrelResXLySide0 = new TProfile("hsprofSCTTopBarrelResXLySide0","hsprofSCTTopBarrelResXLySide0",8,0,4);
    hsprofSCTTopBarrelResXLySide0->SetLineColor(kBlue);
    TProfile *hsprofSCTTopBarrelResXLySide1 = new TProfile("hsprofSCTTopBarrelResXLySide1","hsprofSCTTopBarrelResXLySide1",8,0,4);
    hsprofSCTTopBarrelResXLySide1->SetLineColor(kRed);


    for (int i=0;i<4;i++) {
      hprofSCTTopBarrelResXLy->Fill(i,valueTop[i],1);
      hprofSCTTopBarrelResXLy->Fill(i+0.5,valueTop[i],1);
      hprofSCTTopBarrelResXLySide0->Fill(i,valueTopSide0[i],1);
      hprofSCTTopBarrelResXLySide1->Fill(i+0.5,valueTopSide1[i],1);
      hsprofSCTTopBarrelResXLy->Fill(i,sigmaTop[i],1);
      hsprofSCTTopBarrelResXLy->Fill(i+0.5,sigmaTop[i],1);
      hsprofSCTTopBarrelResXLySide0->Fill(i,sigmaTopSide0[i],1);
      hsprofSCTTopBarrelResXLySide1->Fill(i+0.5,sigmaTopSide1[i],1);
      // cout << valueTop[i] << endl;
    }

    cMeanResPerLayer->cd(2);
    gPad->SetGridy();
    gPad->SetGridx();
    hprofSCTTopBarrelResXLy->DrawCopy();
    hprofSCTTopBarrelResXLySide0->DrawCopy("SAME");
    hprofSCTTopBarrelResXLySide1->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.51,0.88,0.75,1.0);
    sprintf(name,"Both Sides");
    leg0->AddEntry(hprofSCTTopBarrelResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTTopBarrelResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTTopBarrelResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    cMeanResPerLayer->cd(5);
    gPad->SetGridy();
    gPad->SetGridx();
    hsprofSCTTopBarrelResXLy->DrawCopy();
    hsprofSCTTopBarrelResXLySide0->DrawCopy("SAME");
    hsprofSCTTopBarrelResXLySide1->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.51,0.88,0.75,1.0);
    sprintf(name,"Both Sides");
    leg0->AddEntry(hprofSCTTopBarrelResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTTopBarrelResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTTopBarrelResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }
  else {
    cMeanResPerLayer->cd(2);
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

    cMeanResPerLayer->cd(5);
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

  // ********************************************
  //
  // Bottom
  //
  // ********************************************
  tree->Draw("1000*TrkRes:Hit_layer",cut1+cut2+cutB,"profsgoff");
  TProfile *h3Bottemp = (TProfile*)gDirectory->FindObject("htemp");
  TCut cut3 = "Hit_side==0";
  tree->Draw("1000*TrkRes:Hit_layer>>htempBotSCTSide0",cut1+cut2+cut3+cutB,"profsgoff");
  TCut cut3 = "Hit_side==1";
  tree->Draw("1000*TrkRes:Hit_layer>>htempBotSCTSide1",cut1+cut2+cut3+cutB,"profsgoff");

  Double_t valueBot[4] = {0.0};
  Double_t valueBotSide0[4] = {0.0};
  Double_t valueBotSide1[4] = {0.0};

  Double_t sigmaBot[4] = {0.0};
  Double_t sigmaBotSide0[4] = {0.0};
  Double_t sigmaBotSide1[4] = {0.0};

  if (h3Bottemp->GetMean()!=0) {
    jMax = 0.0;
    sMax = 0.0;
    for (int i=1;i<5;i++) {
      valueBot[i-1] = h3Bottemp->GetBinContent(i);
      valueBotSide0[i-1] = htempBotSCTSide0->GetBinContent(i);
      valueBotSide1[i-1] = htempBotSCTSide1->GetBinContent(i);
      sigmaBot[i-1] = h3Bottemp->GetBinError(i);
      sigmaBotSide0[i-1] = htempBotSCTSide0->GetBinError(i);
      sigmaBotSide1[i-1] = htempBotSCTSide1->GetBinError(i);
      Float_t ivalueBot = fabs(valueBot[i-1]);
      Float_t isigmaBot = fabs(sigmaBot[i-1]);
      // cout << "Both sides: " << valueBot[i-1] << endl;
      // cout << " Side0: " << valueBotSide0[i-1] << endl;
      // cout << " Side1: " << valueBotSide1[i-1] << endl;
      jMax = max(jMax,fabs(valueBot[i-1]));
      // cout << "Both sides: " << sigmaBot[i-1] << endl;
      // cout << " Side0: " << sigmaBotSide0[i-1] << endl;
      // cout << " Side1: " << sigmaBotSide1[i-1] << endl;
      sMax = max(sMax,fabs(sigmaBot[i-1]));
    }
    // cout << int(jMax)+2 << endl;
    delete h3Bottemp;
    if (jMax<100) jMax = int(jMax)+2;
    else jMax = int(jMax)+25;
    if (sMax<100) sMax = int(sMax)+10;
    else sMax = int(sMax)+25;

    TProfile *hprofSCTBotBarrelResXLy = new TProfile("hprofSCTBotBarrelResXLy","hprofSCTBotBarrelResXLy",8,0,4);
    hprofSCTBotBarrelResXLy->GetYaxis()->SetRangeUser(-jMax,jMax);
    hprofSCTBotBarrelResXLy->SetTitle("SCT Bottom Barrel");
    hprofSCTBotBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hprofSCTBotBarrelResXLy->GetYaxis()->SetTitle("SCT Bottom Barrel Residual Mean (#mum)");
    TProfile *hprofSCTBotBarrelResXLySide0 = new TProfile("hprofSCTBotBarrelResXLySide0","hprofSCTBotBarrelResXLySide0",8,0,4);
    hprofSCTBotBarrelResXLySide0->SetLineColor(kBlue);
    TProfile *hprofSCTBotBarrelResXLySide1 = new TProfile("hprofSCTBotBarrelResXLySide1","hprofSCTBotBarrelResXLySide1",8,0,4);
    hprofSCTBotBarrelResXLySide1->SetLineColor(kRed);

    TProfile *hsprofSCTBotBarrelResXLy = new TProfile("hsprofSCTBotBarrelResXLy","hsprofSCTBotBarrelResXLy",8,0,4);
    hsprofSCTBotBarrelResXLy->GetYaxis()->SetRangeUser(0.0,sMax);
    hsprofSCTBotBarrelResXLy->SetTitle("SCT Bottom Barrel");
    hsprofSCTBotBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hsprofSCTBotBarrelResXLy->GetYaxis()->SetTitle("SCT Bottom Barrel Residual sigma (#mum)");
    TProfile *hsprofSCTBotBarrelResXLySide0 = new TProfile("hsprofSCTBotBarrelResXLySide0","hsprofSCTBotBarrelResXLySide0",8,0,4);
    hsprofSCTBotBarrelResXLySide0->SetLineColor(kBlue);
    TProfile *hsprofSCTBotBarrelResXLySide1 = new TProfile("hsprofSCTBotBarrelResXLySide1","hsprofSCTBotBarrelResXLySide1",8,0,4);
    hsprofSCTBotBarrelResXLySide1->SetLineColor(kRed);


    for (int i=0;i<4;i++) {
      hprofSCTBotBarrelResXLy->Fill(i,valueBot[i],1);
      hprofSCTBotBarrelResXLy->Fill(i+0.5,valueBot[i],1);
      hprofSCTBotBarrelResXLySide0->Fill(i,valueBotSide0[i],1);
      hprofSCTBotBarrelResXLySide1->Fill(i+0.5,valueBotSide1[i],1);
      hsprofSCTBotBarrelResXLy->Fill(i,sigmaBot[i],1);
      hsprofSCTBotBarrelResXLy->Fill(i+0.5,sigmaBot[i],1);
      hsprofSCTBotBarrelResXLySide0->Fill(i,sigmaBotSide0[i],1);
      hsprofSCTBotBarrelResXLySide1->Fill(i+0.5,sigmaBotSide1[i],1);
      // cout << valueBot[i] << endl;
    }

    cMeanResPerLayer->cd(3);
    gPad->SetGridy();
    gPad->SetGridx();
    hprofSCTBotBarrelResXLy->DrawCopy();
    hprofSCTBotBarrelResXLySide0->DrawCopy("SAME");
    hprofSCTBotBarrelResXLySide1->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.51,0.88,0.75,1.0);
    sprintf(name,"Both Sides");
    leg0->AddEntry(hprofSCTBotBarrelResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTBotBarrelResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTBotBarrelResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

    cMeanResPerLayer->cd(6);
    gPad->SetGridy();
    gPad->SetGridx();
    hsprofSCTBotBarrelResXLy->DrawCopy();
    hsprofSCTBotBarrelResXLySide0->DrawCopy("SAME");
    hsprofSCTBotBarrelResXLySide1->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.51,0.88,0.75,1.0);
    sprintf(name,"Both Sides");
    leg0->AddEntry(hprofSCTBotBarrelResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTBotBarrelResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTBotBarrelResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }
  else {
    cMeanResPerLayer->cd(3);
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

    cMeanResPerLayer->cd(6);
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

}
