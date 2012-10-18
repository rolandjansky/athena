/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
gStyle->SetPalette(1);
gStyle->SetPadTickX(1);
gStyle->SetPadTickY(1);
//gStyle->SetOptStat(1111111);
//gStyle->SetOptStat(0);
//gStyle->SetOptFit(1111);
gStyle->SetTitleFontSize(0.1);
gStyle->SetLineWidth(1); // postscript dashes
//gStyle->SetMarkerSize(0.8);
// use smaller markers in a histogram with many bins
//gStyle->SetMarkerStyle(20);
// use smaller markers in a histogram with many bins
gStyle->SetTitleOffset(0.65,"y");  // bring y axis label closer to narrow values
//..Get rid of X error bars
gStyle->SetErrorX(0.001);
gStyle->SetPadColor(0);
gStyle->SetCanvasColor(0);
gStyle->SetStatColor(0);
gStyle->SetTitleFillColor(0);
gStyle->SetTitleFontSize(0.07);


TCanvas* c1 = new TCanvas("c1", "F001 Efficiency Study", 0, 0, 800, 600);
c1->Divide(2,2);
TPad* p1 = (TPad*)c1->FindObject("c1_1");
TPad* p2 = (TPad*)c1->FindObject("c1_2");
TPad* p3 = (TPad*)c1->FindObject("c1_3");
TPad* p4 = (TPad*)c1->FindObject("c1_4");

TFile f("csc_layerEff.root");

c1->cd(1);

p1->SetGridx(true);
p1->SetGridy(true);
  
layEffEtaf001_1->SetStats(0);
layEffEtaf001_1->SetTitle("#eta Layer Eff with kF*f001");
layEffEtaf001_1->Draw();
layEffEtaf001_2->SetLineColor(2);
layEffEtaf001_2->Draw("same");
layEffEtaf001_3->SetLineColor(3);
layEffEtaf001_3->Draw("same");
layEffEtaf001_4->SetLineColor(4);
layEffEtaf001_4->Draw("same");
                   
layEffEtaf001_1->GetXaxis()->SetTitle("kFactor");
layEffEtaf001_1->GetXaxis()->SetTitleSize(.065);
layEffEtaf001_1->GetXaxis()->SetTitleOffset(.7);
layEffEtaf001_1->GetXaxis()->SetLabelSize(.06);
layEffEtaf001_1->GetYaxis()->SetTitle("#epsilon");
layEffEtaf001_1->GetYaxis()->SetTitleSize(.07);
layEffEtaf001_1->GetYaxis()->SetTitleOffset(.7);
layEffEtaf001_1->GetYaxis()->SetLabelSize(.06);

TLegend *myLg= new TLegend(.15,.45,.35,.15);
myLg->SetTextSize(0.05);
myLg->SetFillColor(0);
myLg->SetBorderSize(0);
myLg->AddEntry(layEffEtaf001_1,"Layer 1");
myLg->AddEntry(layEffEtaf001_2,"Layer 2");
myLg->AddEntry(layEffEtaf001_3,"Layer 3");
myLg->AddEntry(layEffEtaf001_4,"Layer 4");
myLg->Draw();

c1->cd(2);

p2->SetGridx(true);
p2->SetGridy(true);
layEffPhif001_1->SetStats(0);
layEffPhif001_1->SetTitle("#phi Layer Efficiency with kF*f001");
layEffPhif001_1->Draw();
layEffPhif001_2->SetLineColor(2);
layEffPhif001_2->Draw("same");
layEffPhif001_3->SetLineColor(3);
layEffPhif001_3->Draw("same");
layEffPhif001_4->SetLineColor(4);
layEffPhif001_4->Draw("same");
                   
layEffPhif001_1->GetXaxis()->SetTitle("kFactor");
layEffPhif001_1->GetXaxis()->SetTitleSize(.065);
layEffPhif001_1->GetXaxis()->SetTitleOffset(.7);
layEffPhif001_1->GetXaxis()->SetLabelSize(.06);
layEffPhif001_1->GetYaxis()->SetTitle("#epsilon");
layEffPhif001_1->GetYaxis()->SetTitleSize(.07);
layEffPhif001_1->GetYaxis()->SetTitleOffset(.7);
layEffPhif001_1->GetYaxis()->SetLabelSize(.06);

myLg->Draw();


c1->cd(3);
p3->SetLogy(true);
f001_0->SetStats(0);
f001_0->GetXaxis()->SetNdivisions(505);
f001_0->Draw();
f001_1->SetLineColor(2);
f001_1->Draw("same");

f001_0->GetXaxis()->SetTitle("electrons");
f001_0->GetXaxis()->SetTitleSize(.065);
f001_0->GetXaxis()->SetTitleOffset(.7);
f001_0->GetXaxis()->SetLabelSize(.06);
f001_0->GetYaxis()->SetTitle("Entries");
f001_0->GetYaxis()->SetTitleSize(.07);
f001_0->GetYaxis()->SetTitleOffset(.7);
f001_0->GetYaxis()->SetLabelSize(.06);


TLegend *myLg= new TLegend(.62,.85,.82,.66);
myLg->SetTextSize(0.06);
myLg->SetFillColor(0);
myLg->SetBorderSize(0);
myLg->AddEntry(f001_0,"#eta");
myLg->AddEntry(f001_1,"#phi");
myLg->Draw();


c1->cd(4);
qp0->SetStats(0);
qp0->Draw();
qp1->SetLineColor(2);
qp1->Draw("same");
myLg->Draw();

qp0->GetXaxis()->SetTitle("electrons");
qp0->GetXaxis()->SetTitleSize(.065);
qp0->GetXaxis()->SetTitleOffset(.7);
qp0->GetXaxis()->SetLabelSize(.06);
qp0->GetYaxis()->SetTitle("Entries");
qp0->GetYaxis()->SetTitleSize(.07);
qp0->GetYaxis()->SetTitleOffset(.7);
qp0->GetYaxis()->SetLabelSize(.06);


c1->cd();

c1->Print("f001_Efficiency.gif");
}
