#include <iostream>
#include <iomanip>
#include <fstream>
#include <TFile.h>
#include <math.h>
#include <TProfile.h>
#include <cmath>

#include "TCanvas.h"
#include "TChain.h"
#include "TStyle.h"
#include "TH1.h"
#include "TLegend.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"

#include "CountBits.C"

using namespace std;

int colors[10] = {kRed, kBlue, kGreen + 1, kOrange + 1, kAzure + 1, kPink, kGray, kBlack};

void overlay() {
    gStyle->SetOptStat(0);

    TChain *chain1 = new TChain("zdcTree");
    chain1->Add("out16_GeneralPulse_testNewVaryT0Cut_3sigma_andT05.root");

    TChain *chain2 = new TChain("zdcTree");
    chain2->Add("out16_GeneralPulse_doSlew_T0Cut5_1M.root");

    // TChain *chain3 = new TChain("zdcTree");
    // chain3->Add("out16_8TeV_run313187_fermiExplinear_fixTau1Tau2_Chisqr15_fitTMax140_6M.root");

    // TChain *chain1 = new TChain("zdcTree");;
    // chain1->Add("test_stupied_afterMerge426_1M.root");

    // TChain *chain2 = new TChain("zdcTree");;
    // chain2->Add("test_aftermerge0426_1M.root");

    // TChain *chain1 = new TChain("zdcTree"); chain1->Add("out16_slewtest_1M.root");
    // TChain *chain2 = new TChain("zdcTree"); chain2->Add("out16_GeneralPulse_doSlew_T0Cut2_1M.root");
    // TChain *chain3 = new TChain("zdcTree"); chain3->Add("out16_GeneralPulse_doSlew_T0Cut5_1M.root");
    // TChain *chain4 = new TChain("zdcTree"); chain4->Add("out16_GeneralPulse_doSlew_T0Cut15_1M.root");


    TH1F* h0 = new TH1F("h0", "h0",   100, 0, 1000);
    TH1F* h1 = new TH1F("h1", "h1",   100, 0, 1000);
    // TH1F* h2 = new TH1F("h2", "h2",   100, 0, 1000);
    // TH1F* h3 = new TH1F("h3", "h3",   100, 0, 1000);

    chain1->Draw("zdc_SumAmp[1]>>h0", "L1_ZDC_C && (CountBits(zdc_ModuleMask>>4)>1)");
    chain1->Draw("zdc_SumAmp[1]>>h1", "L1_ZDC_C && (CountBits(zdc_ModuleMask>>4)>0)");
    // chain3->Draw("zdc_SumAmp[1]>>h2", "L1_ZDC_C && (CountBits(zdc_ModuleMask>>4)>1)");
    // chain4->Draw("zdc_SumAmp[1]>>h3", "L1_ZDC_C && (CountBits(zdc_ModuleMask>>4)>1)");

    TCanvas *c1  = new TCanvas("c1", "c1", 800, 600);
    TLegend *leg = new TLegend(.58, .6, .88, .88);
    leg->SetTextSize(.03);
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    // leg->AddEntry("", "ATLAS ZDC Run=312968", "");
    leg->AddEntry("", "ATLAS ZDC 2016 p+Pb", "");
    leg->AddEntry("", "Run=313187", "");
    leg->AddEntry("", "Trigger on C side", "");

    h0->SetLineColor(colors[0]);
    h1->SetLineColor(colors[1]);
    // h2->SetLineColor(colors[2]);
    // h3->SetLineColor(colors[3]);

    h0->Draw();
    h1->Draw("same");
    // h2->Draw("same");
    // h3->Draw("same");

    leg->AddEntry(h0, "count > 1", "L");
    leg->AddEntry(h1, "count > 0", "L");
    // leg->AddEntry(h0, "Old cut [-5,10]", "L");
    // leg->AddEntry(h1, "T0 cut [-2,2]", "L");
    // leg->AddEntry(h2, "T0 cut [-5,5]", "L");
    // leg->AddEntry(h3, "T0 cut [-15,15]", "L");
    leg->Draw();
    gPad->SetTicks(1, 1);
}
