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

int fitMax = 750;

int colors[10] = {kRed, kBlue, kGreen + 1, kCyan + 1, kYellow + 1, kOrange + 1, kAzure + 1, kMagenta, kGray, kBlack};

int n = 5; //n neutrons

double Nnutrons(double *xvec, double *pvec)
{
    double mu    = pvec[0];
    double sigma = pvec[1];

    double N1 = pvec[2] * exp(-0.5 * pow(((xvec[0] - 1 * mu) / (sqrt(1) * sigma)), 2));
    double N2 = pvec[3] * exp(-0.5 * pow(((xvec[0] - 2 * mu) / (sqrt(2) * sigma)), 2));
    double N3 = pvec[4] * exp(-0.5 * pow(((xvec[0] - 3 * mu) / (sqrt(3) * sigma)), 2));
    double N4 = pvec[5] * exp(-0.5 * pow(((xvec[0] - 4 * mu) / (sqrt(4) * sigma)), 2));
    double linear = pvec[6] * xvec[0] + pvec[7];

    return N1 + N2 + N3 + N4 + linear;
}

double Nnutrons2(double *xvec, double *pvec)
{
    double mu    = pvec[0];
    double sigma = pvec[1];

    double N1 = pvec[2] * exp(-0.5 * pow(((xvec[0] - 1 * mu) / (sqrt(1) * sigma)), 2));
    double N2 = pvec[3] * exp(-0.5 * pow(((xvec[0] - 2 * mu) / (sqrt(2) * sigma)), 2));
    double N3 = pvec[4] * exp(-0.5 * pow(((xvec[0] - 3 * mu) / (sqrt(3) * sigma)), 2));
    double N4 = pvec[5] * exp(-0.5 * pow(((xvec[0] - 4 * mu) / (sqrt(4) * sigma)), 2));
    double N5 = pvec[6] * exp(-0.5 * pow(((xvec[0] - 5 * mu) / (sqrt(5) * sigma)), 2));
    double bg = pvec[7] * exp(-(xvec[0] - pvec[8]) / pvec[9]);

    return N1 + N2 + N3 + N4 + N5 + bg;
}

void plot_sumAmp()
{
    gStyle->SetOptStat(0);

    TChain *chain = new TChain("zdcTree");;

    chain->Add("out16_GeneralPulse_testNewVaryT0Cut_3sigma_andT05.root");

    // chain->Add("out16_5TeV_run312968_FixTau1Tau2_fitTMax140.root");

    float zdc_SumAmp[2];
    int   zdc_ModuleMask = 0;
    chain->SetBranchAddress("zdc_SumAmp", &zdc_SumAmp);
    chain->SetBranchAddress("zdc_ModuleMask", &zdc_ModuleMask);
    Bool_t L1_ZDC_C;
    chain->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C);

    TH1F* h0 = new TH1F("h0", "h0",   75, 0, 750);    //8TeV

    chain->Draw("zdc_SumAmp[1]>>h0", "L1_ZDC_C && (CountBits(zdc_ModuleMask>>4)>1)");

    TCanvas *c1  = new TCanvas("c1", "c1", 800, 600);
    TLegend *leg = new TLegend(.58, .7, .88, .88);
    leg->SetTextSize(.03);
    leg->SetBorderSize(0);
    leg->SetFillColor(0);
    // leg->AddEntry("", "ATLAS ZDC Run=312968", "");
    leg->AddEntry("", "ATLAS ZDC 2016 p+Pb", "");
    leg->AddEntry("", "Run=313187", "");
    leg->AddEntry("", "Trigger on C side", "");

    float peakpos = 200;

    TF1* fx_0 = new TF1("total", Nnutrons2, 90, fitMax, 10);
    fx_0->SetParName  (0, "Mean");
    fx_0->SetParameter(0, peakpos);
    fx_0->SetParLimits(0, 0.5 * peakpos, 1.5 * peakpos);

    fx_0->SetParName  (1, "Sigma");
    fx_0->SetParameter(1, 0.2  * peakpos);
    fx_0->SetParLimits(1, 0.05 * peakpos, peakpos);

    fx_0->SetParName  (2, "Constant_1");
    fx_0->SetParameter(2, 2500);
    fx_0->SetParLimits(2, 0, 100000);
    fx_0->SetParName  (3, "Constant_2");
    fx_0->SetParameter(3, 1500);
    fx_0->SetParLimits(3, 0, 100000);
    fx_0->SetParName  (4, "Constant_3");
    fx_0->SetParameter(4, 1000);
    fx_0->SetParLimits(4, 0, 100000);
    fx_0->SetParName  (5, "Constant_4");
    fx_0->SetParameter(5, 800);
    fx_0->SetParLimits(5, 0, 100000);
    fx_0->SetParName  (6, "Constant_5");
    fx_0->SetParameter(6, 800);
    fx_0->SetParLimits(6, 0, 100000);


    fx_0->SetParName  (7, "expF");
    fx_0->SetParameter(7, 500);
    fx_0->SetParName  (8, "expP");
    fx_0->SetParameter(8, 50);
    fx_0->SetParLimits(8, 50, 150);
    fx_0->SetParName  (9, "expW");
    fx_0->SetParameter(9, 1.0);
    fx_0->SetParLimits(9, 1, 500);

    h0->Draw();
    h0->Fit(fx_0, "R");
    fx_0 = (TF1*)h0->GetFunction("total");
    float mean = fx_0->GetParameter(0);
    float sigma = fx_0->GetParameter(1);
    float resolution = sigma / mean;
    h0->SetLineColor(colors[1]);
    h0->SetMinimum(0);
    leg->AddEntry(h0, "Side A", "L");
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

    TF1 *fexp = new TF1("fexp", "[0] * exp(-(x - [1]) / [2])", 0, 1000);
    fexp->SetParameters(fx_0->GetParameter(7), fx_0->GetParameter(8), fx_0->GetParameter(9));
    fexp->SetLineStyle(2);
    fexp->SetLineColor(kBlue);
    fexp->Draw("l,same");

    return;
}


// void plot_sumAmp()
// {
//     gStyle->SetOptStat(0);

//     TChain *chain = new TChain("zdcTree");;

//     chain->Add("out16_8TeV_run313187_afterMerge_0426_6M_nostupid.root");

//     // chain->Add("out16_5TeV_run312968_FixTau1Tau2_fitTMax140.root");

//     float zdc_SumAmp[2];
//     int   zdc_ModuleMask = 0;
//     chain->SetBranchAddress("zdc_SumAmp", &zdc_SumAmp);
//     chain->SetBranchAddress("zdc_ModuleMask", &zdc_ModuleMask);
//     Bool_t L1_ZDC_C;
//     chain->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C);

//     // TH1F* h0 = (TH1F*)fin->Get("h0"); //fire1_triggeroff
//     // TH1F* h0 = (TH1F*)fin->Get("h1"); //fire4_triggeroff
//     // TH1F* h0  = (TH1F*)fin->Get("h2"); //fire1_triggeron
//     // TH1F* h0  = (TH1F*)fin->Get("h3"); //fire4_triggeron

//     TH1F* h0 = new TH1F("h0", "h0",   100, 0, 1000);    //8TeV
//     // TH1F* h0 = new TH1F("h0", "h0",   250, 0, 1500);

//     chain->Draw("zdc_SumAmp[1]>>h0", "L1_ZDC_C && (CountBits(zdc_ModuleMask>>4)>1)");

//     TCanvas *c1  = new TCanvas("c1", "c1", 800, 600);
//     TLegend *leg = new TLegend(.58, .7, .88, .88);
//     leg->SetTextSize(.03);
//     leg->SetBorderSize(0);
//     leg->SetFillColor(0);
//     // leg->AddEntry("", "ATLAS ZDC Run=312968", "");
//     leg->AddEntry("", "ATLAS ZDC 2016 p+Pb", "");
//     leg->AddEntry("", "Run=313187", "");
//     leg->AddEntry("", "Trigger on C side", "");

//     TF1* fx_0 = new TF1("total", Nnutrons, 120, fitMax, n + 4);
//     fx_0->SetParName(0, "Mean");
//     fx_0->SetParName(1, "Sigma");
//     fx_0->SetParName(2, "Constant_1");
//     fx_0->SetParName(3, "Constant_2");
//     fx_0->SetParName(4, "Constant_3");
//     fx_0->SetParName(5, "Constant_4");
//     fx_0->SetParName(6, "linear slope");
//     fx_0->SetParName(7, "linear constant");
//     // fx_0->SetParameters(300, 60, 300, 100, 100, 100);
//     // fx_0->FixParameter(6,0);
//     fx_0->SetParameters(180, 70, 700, 500, 200, 400, 0, 400);   //8TeV

//     h0->Draw();
//     h0->Fit(fx_0, "R");
//     fx_0 = (TF1*)h0->GetFunction("total");
//     float mean = fx_0->GetParameter(0);
//     float sigma = fx_0->GetParameter(1);
//     float resolution = sigma / mean;
//     h0->SetLineColor(colors[1]);
//     h0->SetMinimum(0);
//     leg->AddEntry(h0, "Side A", "L");
//     leg->AddEntry(fx_0, Form("#sigma/#mu = %f", resolution), "L");
//     h0->SetTitle(";SumAmplitude ;Counts;");
//     leg->Draw();
//     gPad->SetTicks(1, 1);

//     // fx_0->SetRange(150, 1050);
//     // fx_0->Draw("same");
//     // fx_0->SetLineColor(kRed);

//     TF1 *f1 = new TF1("f1", "[0] * exp(-0.5 * pow(((x - 1 * [1]) / (sqrt(1) * [2])), 2))", 0, fitMax);
//     f1->SetParameters(fx_0->GetParameter(2), fx_0->GetParameter(0), fx_0->GetParameter(1));
//     f1->SetLineStyle(3);
//     f1->SetLineColor(kBlue);
//     f1->Draw("l,same");

//     TF1 *f2 = new TF1("f2", "[0] * exp(-0.5 * pow(((x - 2 * [1]) / (sqrt(2) * [2])), 2))", 0, fitMax);
//     f2->SetParameters(fx_0->GetParameter(3), fx_0->GetParameter(0), fx_0->GetParameter(1));
//     f2->SetLineStyle(3);
//     f2->SetLineColor(kBlue);
//     f2->Draw("l,same");

//     TF1 *f3 = new TF1("f3", "[0] * exp(-0.5 * pow(((x - 3 * [1]) / (sqrt(3) * [2])), 2))", 0, fitMax);
//     f3->SetParameters(fx_0->GetParameter(4), fx_0->GetParameter(0), fx_0->GetParameter(1));
//     f3->SetLineStyle(3);
//     f3->SetLineColor(kBlue);
//     f3->Draw("l,same");

//     TF1 *f4 = new TF1("f4", "[0] * exp(-0.5 * pow(((x - 4 * [1]) / (sqrt(4) * [2])), 2))", 0, fitMax);
//     f4->SetParameters(fx_0->GetParameter(5), fx_0->GetParameter(0), fx_0->GetParameter(1));
//     f4->SetLineStyle(3);
//     f4->SetLineColor(kBlue);
//     f4->Draw("l,same");

//     TF1 *f5 = new TF1("f5", "[0] * x + [1]", 0, fitMax);
//     f5->SetParameters(fx_0->GetParameter(6), fx_0->GetParameter(7));
//     f5->SetLineStyle(3);
//     f5->SetLineColor(kBlue);
//     f5->Draw("l,same");

//     return;
// }

// void plot_sumAmp()
// {
//     gStyle->SetOptStat(0);

//     TChain *chain = new TChain("zdcTree");;

//     chain->Add("user.steinber..data16_hip5TeV.00312968.calibration_zdcCalib.recon.AOD.c1070_m1730.87z5_ANALYSIS.root/user.steinber.17466118._000001.ANALYSIS.root");
//     chain->Add("user.steinber..data16_hip5TeV.00312968.calibration_zdcCalib.recon.AOD.c1070_m1730.87z5_ANALYSIS.root/user.steinber.17466118._000002.ANALYSIS.root");
//     chain->Add("user.steinber..data16_hip5TeV.00312968.calibration_zdcCalib.recon.AOD.c1070_m1730.87z5_ANALYSIS.root/user.steinber.17466118._000003.ANALYSIS.root");
//     chain->Add("user.steinber..data16_hip5TeV.00312968.calibration_zdcCalib.recon.AOD.c1070_m1730.87z5_ANALYSIS.root/user.steinber.17466118._000004.ANALYSIS.root");
//     chain->Add("user.steinber..data16_hip5TeV.00312968.calibration_zdcCalib.recon.AOD.c1070_m1730.87z5_ANALYSIS.root/user.steinber.17466118._000005.ANALYSIS.root");

//     float  zdc_ZdcAmp[2];
//     UInt_t zdc_ZdcModuleMask = 0;
//     chain->SetBranchAddress("zdc_ZdcAmp", &zdc_ZdcAmp);
//     chain->SetBranchAddress("zdc_ZdcModuleMask", &zdc_ZdcModuleMask);
//     Bool_t L1_ZDC_C;
//     chain->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C);

//     TH1F* h0 = new TH1F("h0", "h0",   300, 0, 1500);    //8TeV

//     chain->Draw("zdc_ZdcAmp[1]>>h0", "L1_ZDC_C && (CountBits(zdc_ZdcModuleMask>>4)>1)");

//     TCanvas *c1  = new TCanvas("c1", "c1", 800, 600);
//     TLegend *leg = new TLegend(.58, .7, .88, .88);
//     leg->SetTextSize(.03);
//     leg->SetBorderSize(0);
//     leg->SetFillColor(0);
//     leg->AddEntry("", "ATLAS ZDC Run=312968", "");
//     // leg->AddEntry("", "ATLAS ZDC 2016 p+Pb", "");
//     // leg->AddEntry("", "Run=313187", "");
//     leg->AddEntry("", "Trigger on C side", "");

//     TF1* fx_0 = new TF1("total", Nnutrons, 150, 1000, n + 2);
//     fx_0->SetParName(0, "Mean");
//     fx_0->SetParName(1, "Sigma");
//     fx_0->SetParName(2, "Constant_1");
//     fx_0->SetParName(3, "Constant_2");
//     fx_0->SetParName(4, "Constant_3");
//     fx_0->SetParName(5, "Constant_4");
//     // fx_0->SetParLimits(0, 170, 200);
//     // fx_0->SetParLimits(1, 0, 80);
//     fx_0->SetParameters(300, 60, 2400, 1200, 800, 800);
//     // fx_0->SetParameters(300, 60, 6000, 5000, 4000, 3500);
//     // fx_0->SetParameters(180, 70, 700, 500, 200, 400);

//     h0->Draw();
//     h0->Fit(fx_0, "R");
//     fx_0 = (TF1*)h0->GetFunction("total");
//     float mean = fx_0->GetParameter(0);
//     float sigma = fx_0->GetParameter(1);
//     float resolution = sigma / mean;
//     h0->SetLineColor(colors[1]);
//     h0->SetMinimum(0);
//     leg->AddEntry(h0, "Side A", "L");
//     leg->AddEntry(fx_0, Form("#sigma/#mu = %f", resolution), "L");
//     h0->SetTitle(";SumAmplitude ;Counts;");
//     leg->Draw();
//     gPad->SetTicks(1, 1);

//     TF1 *f1 = new TF1("f1", "[0] * exp(-0.5 * pow(((x - 1 * [1]) / (sqrt(1) * [2])), 2))", 0, fitMax);
//     f1->SetParameters(fx_0->GetParameter(2), fx_0->GetParameter(0), fx_0->GetParameter(1));
//     f1->SetLineStyle(3);
//     f1->SetLineColor(kBlue);
//     f1->Draw("l,same");

//     TF1 *f2 = new TF1("f2", "[0] * exp(-0.5 * pow(((x - 2 * [1]) / (sqrt(2) * [2])), 2))", 0, fitMax);
//     f2->SetParameters(fx_0->GetParameter(3), fx_0->GetParameter(0), fx_0->GetParameter(1));
//     f2->SetLineStyle(3);
//     f2->SetLineColor(kBlue);
//     f2->Draw("l,same");

//     TF1 *f3 = new TF1("f3", "[0] * exp(-0.5 * pow(((x - 3 * [1]) / (sqrt(3) * [2])), 2))", 0, fitMax);
//     f3->SetParameters(fx_0->GetParameter(4), fx_0->GetParameter(0), fx_0->GetParameter(1));
//     f3->SetLineStyle(3);
//     f3->SetLineColor(kBlue);
//     f3->Draw("l,same");

//     TF1 *f4 = new TF1("f4", "[0] * exp(-0.5 * pow(((x - 4 * [1]) / (sqrt(4) * [2])), 2))", 0, fitMax);
//     f4->SetParameters(fx_0->GetParameter(5), fx_0->GetParameter(0), fx_0->GetParameter(1));
//     f4->SetLineStyle(3);
//     f4->SetLineColor(kBlue);
//     f4->Draw("l,same");

//     return;
// }

