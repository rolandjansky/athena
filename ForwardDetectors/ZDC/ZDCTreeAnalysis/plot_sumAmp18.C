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

int fitMax = 1000;

int colors[10] = {kRed, kBlue, kGreen + 1, kCyan + 1, kYellow + 1, kOrange + 1, kAzure + 1, kMagenta, kGray, kBlack};

int n = 5; //n neutrons

double Nnutrons2(double *xvec, double *pvec)
{
    double mu    = pvec[0];
    double sigma = pvec[1];

    double N1 = pvec[2] * exp(-0.5 * pow(((xvec[0] - 1 * mu) / (sqrt(1) * sigma)), 2));
    double N2 = pvec[3] * exp(-0.5 * pow(((xvec[0] - 2 * mu) / (sqrt(2) * sigma)), 2));
    double N3 = pvec[4] * exp(-0.5 * pow(((xvec[0] - 3 * mu) / (sqrt(3) * sigma)), 2));
    double N4 = pvec[5] * exp(-0.5 * pow(((xvec[0] - 4 * mu) / (sqrt(4) * sigma)), 2));
    double N5 = pvec[6] * exp(-0.5 * pow(((xvec[0] - 5 * mu) / (sqrt(5) * sigma)), 2));

    return N1 + N2 + N3 + N4 + N5;
}

void plot_sumAmp18()
{
    gStyle->SetOptStat(0);

    TChain *chain = new TChain("zdcTree");;

    chain->Add("out18_run367365_GeneralPulse_test.root");

    float zdc_SumAmp[2];
    int   zdc_ModuleMask = 0;
    chain->SetBranchAddress("zdc_SumAmp", &zdc_SumAmp);
    chain->SetBranchAddress("zdc_ModuleMask", &zdc_ModuleMask);
    Bool_t L1_ZDC_C;
    chain->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C);

    TH1F* h0 = new TH1F("h0", "h0",   100, 0, 1000);

    // chain->Draw("zdc_SumAmp[1]>>h0", "L1_ZDC_C && (CountBits(zdc_ModuleMask>>4)>1) && lumiBlock > 135 && lumiBlock < 185"); // -------------
    // chain->Draw("zdc_SumAmp[0]>>h0", "L1_ZDC_A && (CountBits(zdc_ModuleMask>>4)>1) && lumiBlock > 135 && lumiBlock < 185"); // -------------
    // chain->Draw("zdc_SumAmp[1]>>h0", "L1_ZDC_C && (CountBits(zdc_ModuleMask>>4)>1) && lumiBlock > 380 && lumiBlock < 431"); // -------------
    chain->Draw("zdc_SumAmp[0]>>h0", "L1_ZDC_A && (CountBits(zdc_ModuleMask>>4)>1) && lumiBlock > 380 && lumiBlock < 431"); // -------------

    TCanvas *c1  = new TCanvas("c1", "c1", 800, 600);
    TLegend *leg = new TLegend(.58, .6, .88, .88);
    leg->SetTextSize(.03);
    leg->SetBorderSize(0);
    leg->SetFillColor(0);

    leg->AddEntry("", "ATLAS ZDC 2018 Pb+Pb", "");
    leg->AddEntry("", "Run=367365", "");
    leg->AddEntry("", "Trigger on A side", "");     // -------------
    leg->AddEntry("", "Late LB", "");              // -------------

    float peakpos = 380;

    TF1* fx_0 = new TF1("total", Nnutrons2, 200, fitMax, 7);
    fx_0->SetParName  (0, "Mean");
    fx_0->SetParameter(0, peakpos);
    fx_0->SetParLimits(0, 0.5 * peakpos, 1.5 * peakpos);

    fx_0->SetParName  (1, "Sigma");
    fx_0->SetParameter(1, 0.2  * peakpos);
    fx_0->SetParLimits(1, 0.05 * peakpos, peakpos);

    fx_0->SetParName  (2, "Constant_1");
    fx_0->SetParameter(2, 2000);
    fx_0->SetParLimits(2, 0, 100000);
    fx_0->SetParName  (3, "Constant_2");
    fx_0->SetParameter(3, 400);
    fx_0->SetParLimits(3, 0, 100000);
    fx_0->SetParName  (4, "Constant_3");
    fx_0->SetParameter(4, 400);
    fx_0->SetParLimits(4, 0, 100000);
    fx_0->SetParName  (5, "Constant_4");
    fx_0->SetParameter(5, 400);
    fx_0->SetParLimits(5, 0, 100000);
    fx_0->SetParName  (6, "Constant_5");
    fx_0->SetParameter(6, 400);
    fx_0->SetParLimits(6, 0, 100000);

    h0->Draw();
    h0->Fit(fx_0, "R");
    fx_0 = (TF1*)h0->GetFunction("total");
    float mean = fx_0->GetParameter(0);
    float sigma = fx_0->GetParameter(1);
    float resolution = sigma / mean;
    h0->SetLineColor(colors[1]);
    h0->SetMinimum(0);
    leg->AddEntry(h0, "Side C", "L");           // -------------
    leg->AddEntry(fx_0, Form("#sigma/#mu = %f", resolution), "L");
    h0->SetTitle(";SumAmplitude ;Counts;");
    leg->Draw();
    gPad->SetTicks(1, 1);

    TF1 *f1 = new TF1("f1", "[0] * exp(-0.5 * pow(((x - 1 * [1]) / (sqrt(1) * [2])), 2))", 0, 1000);
    f1->SetParameters(fx_0->GetParameter(2), fx_0->GetParameter(0), fx_0->GetParameter(1));
    f1->SetLineStyle(3);
    f1->SetLineColor(kBlue);
    f1->Draw("l,same");

    TF1 *f2 = new TF1("f2", "[0] * exp(-0.5 * pow(((x - 2 * [1]) / (sqrt(2) * [2])), 2))", 0, 1000);
    f2->SetParameters(fx_0->GetParameter(3), fx_0->GetParameter(0), fx_0->GetParameter(1));
    f2->SetLineStyle(3);
    f2->SetLineColor(kBlue);
    f2->Draw("l,same");

    TF1 *f3 = new TF1("f3", "[0] * exp(-0.5 * pow(((x - 3 * [1]) / (sqrt(3) * [2])), 2))", 0, 1000);
    f3->SetParameters(fx_0->GetParameter(4), fx_0->GetParameter(0), fx_0->GetParameter(1));
    f3->SetLineStyle(3);
    f3->SetLineColor(kBlue);
    f3->Draw("l,same");

    TF1 *f4 = new TF1("f4", "[0] * exp(-0.5 * pow(((x - 4 * [1]) / (sqrt(4) * [2])), 2))", 0, 1000);
    f4->SetParameters(fx_0->GetParameter(5), fx_0->GetParameter(0), fx_0->GetParameter(1));
    f4->SetLineStyle(3);
    f4->SetLineColor(kBlue);
    f4->Draw("l,same");

    TF1 *f5 = new TF1("f5", "[0] * exp(-0.5 * pow(((x - 5 * [1]) / (sqrt(5) * [2])), 2))", 0, 2000);
    f5->SetParameters(fx_0->GetParameter(6), fx_0->GetParameter(0), fx_0->GetParameter(1));
    f5->SetLineStyle(3);
    f5->SetLineColor(kBlue);
    f5->Draw("l,same");

    return;
}
