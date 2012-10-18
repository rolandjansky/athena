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
p1->SetLogy(true);
  
layFakeEtaf001_1->SetMinimum(0.0001);
layFakeEtaf001_2->SetMinimum(0.0001);
layFakeEtaf001_3->SetMinimum(0.0001);
layFakeEtaf001_4->SetMinimum(0.0001);
layFakeEtaf001_1->SetStats(0);
layFakeEtaf001_1->SetTitle("#eta Layer Fake with kF*f001");
layFakeEtaf001_1->Draw();
layFakeEtaf001_2->SetLineColor(2);
layFakeEtaf001_2->Draw("same");
layFakeEtaf001_3->SetLineColor(3);
layFakeEtaf001_3->Draw("same");
layFakeEtaf001_4->SetLineColor(4);
layFakeEtaf001_4->Draw("same");
                   
layFakeEtaf001_1->GetXaxis()->SetTitle("kFactor");
layFakeEtaf001_1->GetXaxis()->SetTitleSize(.065);
layFakeEtaf001_1->GetXaxis()->SetTitleOffset(.7);
layFakeEtaf001_1->GetXaxis()->SetLabelSize(.06);
layFakeEtaf001_1->GetYaxis()->SetTitle("#epsilon");
layFakeEtaf001_1->GetYaxis()->SetTitleSize(.07);
layFakeEtaf001_1->GetYaxis()->SetTitleOffset(.7);
layFakeEtaf001_1->GetYaxis()->SetLabelSize(.06);

TLegend *myLg= new TLegend(.65,.85,.85,.55);
myLg->SetTextSize(0.05);
myLg->SetFillColor(0);
myLg->SetBorderSize(0);
myLg->AddEntry(layFakeEtaf001_1,"Layer 1");
myLg->AddEntry(layFakeEtaf001_2,"Layer 2");
myLg->AddEntry(layFakeEtaf001_3,"Layer 3");
myLg->AddEntry(layFakeEtaf001_4,"Layer 4");
myLg->Draw();

c1->cd(2);

p2->SetGridx(true);
p2->SetGridy(true);
p2->SetLogy(true);
layFakePhif001_1->SetStats(0);
layFakePhif001_1->SetMinimum(0.0001);
layFakePhif001_2->SetMinimum(0.0001);
layFakePhif001_3->SetMinimum(0.0001);
layFakePhif001_4->SetMinimum(0.0001);
layFakePhif001_1->SetTitle("#phi Layer Fake with kF*f001");
layFakePhif001_1->Draw();
layFakePhif001_2->SetLineColor(2);
layFakePhif001_2->Draw("same");
layFakePhif001_3->SetLineColor(3);
layFakePhif001_3->Draw("same");
layFakePhif001_4->SetLineColor(4);
layFakePhif001_4->Draw("same");
                   
layFakePhif001_1->GetXaxis()->SetTitle("kFactor");
layFakePhif001_1->GetXaxis()->SetTitleSize(.065);
layFakePhif001_1->GetXaxis()->SetTitleOffset(.7);
layFakePhif001_1->GetXaxis()->SetLabelSize(.06);
layFakePhif001_1->GetYaxis()->SetTitle("#epsilon");
layFakePhif001_1->GetYaxis()->SetTitleSize(.07);
layFakePhif001_1->GetYaxis()->SetTitleOffset(.7);
layFakePhif001_1->GetYaxis()->SetLabelSize(.06);

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
qpFk0->SetStats(0);
p4->SetLogy(1);
qpFk0->Draw();
qpFk1->SetLineColor(2);
qpFk1->Draw("same");
myLg->Draw();

qpFk0->GetXaxis()->SetTitle("electrons");
qpFk0->GetXaxis()->SetTitleSize(.065);
qpFk0->GetXaxis()->SetTitleOffset(.7);
qpFk0->GetXaxis()->SetLabelSize(.06);
qpFk0->GetYaxis()->SetTitle("Entries");
qpFk0->GetYaxis()->SetTitleSize(.07);
qpFk0->GetYaxis()->SetTitleOffset(.7);
qpFk0->GetYaxis()->SetLabelSize(.06);


c1->cd();
c1->Print("f001_FakeRate.gif");
}
