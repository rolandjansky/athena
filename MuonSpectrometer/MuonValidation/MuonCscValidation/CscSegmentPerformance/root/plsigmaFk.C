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


TCanvas* c1 = new TCanvas("c1", "SIGMA Efficiency Study", 0, 0, 800, 600);
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
  
layFakeEtasigma_1->SetMinimum(0.0001);
layFakeEtasigma_2->SetMinimum(0.0001);
layFakeEtasigma_3->SetMinimum(0.0001);
layFakeEtasigma_4->SetMinimum(0.0001);
layFakeEtasigma_1->SetStats(0);
layFakeEtasigma_1->SetTitle("#eta Layer Fake with kF*sigma");
layFakeEtasigma_1->Draw();
layFakeEtasigma_2->SetLineColor(2);
layFakeEtasigma_2->Draw("same");
layFakeEtasigma_3->SetLineColor(3);
layFakeEtasigma_3->Draw("same");
layFakeEtasigma_4->SetLineColor(4);
layFakeEtasigma_4->Draw("same");
                   
layFakeEtasigma_1->GetXaxis()->SetTitle("kFactor");
layFakeEtasigma_1->GetXaxis()->SetTitleSize(.065);
layFakeEtasigma_1->GetXaxis()->SetTitleOffset(.7);
layFakeEtasigma_1->GetXaxis()->SetLabelSize(.06);
layFakeEtasigma_1->GetYaxis()->SetTitle("#epsilon");
layFakeEtasigma_1->GetYaxis()->SetTitleSize(.07);
layFakeEtasigma_1->GetYaxis()->SetTitleOffset(.7);
layFakeEtasigma_1->GetYaxis()->SetLabelSize(.06);

TLegend *myLg= new TLegend(.65,.85,.85,.55);
myLg->SetTextSize(0.05);
myLg->SetFillColor(0);
myLg->SetBorderSize(0);
myLg->AddEntry(layFakeEtasigma_1,"Layer 1");
myLg->AddEntry(layFakeEtasigma_2,"Layer 2");
myLg->AddEntry(layFakeEtasigma_3,"Layer 3");
myLg->AddEntry(layFakeEtasigma_4,"Layer 4");
myLg->Draw();

c1->cd(2);

p2->SetGridx(true);
p2->SetGridy(true);
p2->SetLogy(true);
layFakePhisigma_1->SetStats(0);
layFakePhisigma_1->SetMinimum(0.0001);
layFakePhisigma_2->SetMinimum(0.0001);
layFakePhisigma_3->SetMinimum(0.0001);
layFakePhisigma_4->SetMinimum(0.0001);
layFakePhisigma_1->SetTitle("#phi Layer Fake with kF*sigma");
layFakePhisigma_1->Draw();
layFakePhisigma_2->SetLineColor(2);
layFakePhisigma_2->Draw("same");
layFakePhisigma_3->SetLineColor(3);
layFakePhisigma_3->Draw("same");
layFakePhisigma_4->SetLineColor(4);
layFakePhisigma_4->Draw("same");
                   
layFakePhisigma_1->GetXaxis()->SetTitle("kFactor");
layFakePhisigma_1->GetXaxis()->SetTitleSize(.065);
layFakePhisigma_1->GetXaxis()->SetTitleOffset(.7);
layFakePhisigma_1->GetXaxis()->SetLabelSize(.06);
layFakePhisigma_1->GetYaxis()->SetTitle("#epsilon");
layFakePhisigma_1->GetYaxis()->SetTitleSize(.07);
layFakePhisigma_1->GetYaxis()->SetTitleOffset(.7);
layFakePhisigma_1->GetYaxis()->SetLabelSize(.06);

myLg->Draw();


c1->cd(3);
p3->SetLogy(true);
sigma_0->SetStats(0);
sigma_0->GetXaxis()->SetNdivisions(505);
sigma_0->Draw();
sigma_1->SetLineColor(2);
sigma_1->Draw("same");

sigma_0->GetXaxis()->SetTitle("electrons");
sigma_0->GetXaxis()->SetTitleSize(.065);
sigma_0->GetXaxis()->SetTitleOffset(.7);
sigma_0->GetXaxis()->SetLabelSize(.06);
sigma_0->GetYaxis()->SetTitle("Entries");
sigma_0->GetYaxis()->SetTitleSize(.07);
sigma_0->GetYaxis()->SetTitleOffset(.7);
sigma_0->GetYaxis()->SetLabelSize(.06);


TLegend *myLg= new TLegend(.62,.85,.82,.66);
myLg->SetTextSize(0.06);
myLg->SetFillColor(0);
myLg->SetBorderSize(0);
myLg->AddEntry(sigma_0,"#eta");
myLg->AddEntry(sigma_1,"#phi");
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
c1->Print("sigma_FakeRate.gif");
}
