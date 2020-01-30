#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <utility>
#include <TFile.h>
#include <TLine.h>
#include <math.h>
#include <TProfile.h>
#include <cmath>

#include "TCanvas.h"
#include "TChain.h"
#include "TStyle.h"
#include "TH1.h"
#include <TH2.h>
#include "TLegend.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"

#include "CountBits.C"

using namespace std;

TChain* chain[3];
TH1F* hist[3][4];
TString title[4] = {"Amp[0,100)", "Amp[100,300)", "Amp[300,500)", "Amp[500,1000)"};
int colors[10] = {kRed, kBlue, kGreen + 1, kCyan + 1, kYellow + 1, kOrange + 1, kAzure + 1, kMagenta, kGray, kBlack};

void process_Files()
{
    gStyle->SetOptStat(0);

    char hname[100];

    float  zdc_Amp[3][2][4];
    float  zdc_SumAmp[3][2];
    UInt_t zdc_ModuleMask[3] = {0};
    Bool_t L1_ZDC_C[3];

    for (int i = 0; i < 1; i++) {   // 3  1
        for (int j = 0; j < 4; j++) {
            sprintf(hname, "hist_%d_%d", i, j);
            hist[i][j] = new TH1F(hname, title[j], 5, -0.5, 4.5);
        }
        chain[i]->SetBranchAddress("zdc_Amp"       , &zdc_Amp[i]);
        chain[i]->SetBranchAddress("zdc_ModuleMask", &zdc_ModuleMask[i]);
        chain[i]->SetBranchAddress("zdc_SumAmp"    , &zdc_SumAmp[i]);
        chain[i]->SetBranchAddress("L1_ZDC_C"      , &L1_ZDC_C[i]);
    }

    for (int i = 0; i < 1000000; i++)
    {
        if (i > 0) {
            float log10Entry = floor(log10((float) i));
            int   modEntry   = i % ((int) pow(10.0, log10Entry));
            if (modEntry == 0) cout << "Processed event " << i << endl;
        }

        for (int j = 0; j < 1; j++) {   // 3  1
            chain[j]->GetEntry(i);
            int count = CountBits(zdc_ModuleMask[j] >> 4);

            // if (L1_ZDC_C[j]) {
            //     if      (                           zdc_SumAmp[j][1] < 100) hist[j][0]->Fill(count);
            //     else if (zdc_SumAmp[j][1] >= 100 && zdc_SumAmp[j][1] < 300) hist[j][1]->Fill(count);
            //     else if (zdc_SumAmp[j][1] >= 300 && zdc_SumAmp[j][1] < 500) hist[j][2]->Fill(count);
            //     else if (zdc_SumAmp[j][1] >= 500 && zdc_SumAmp[j][1] < 1000) hist[j][3]->Fill(count);
            // }

            // -------------------------------------------
            // for 1 bit only
            int filln = -1;
            if (count == 1) {
                if ((zdc_ModuleMask[j] >> 4) == 1) filln = 0;
                if ((zdc_ModuleMask[j] >> 4) == 2) filln = 1;
                if ((zdc_ModuleMask[j] >> 4) == 4) filln = 2;
                if ((zdc_ModuleMask[j] >> 4) == 8) filln = 3;

                if (L1_ZDC_C[j]) {
                    if      (                           zdc_SumAmp[j][1] < 100)  hist[j][0]->Fill(filln);
                    else if (zdc_SumAmp[j][1] >= 100 && zdc_SumAmp[j][1] < 300)  hist[j][1]->Fill(filln);
                    else if (zdc_SumAmp[j][1] >= 300 && zdc_SumAmp[j][1] < 500)  hist[j][2]->Fill(filln);
                    else if (zdc_SumAmp[j][1] >= 500 && zdc_SumAmp[j][1] < 1000) hist[j][3]->Fill(filln);
                }
            }
            // -------------------------------------------
        }
    }
}

void plot_counts()
{
    // chain[0] = new TChain("zdcTree"); chain[0]->Add("out16_GeneralPulse_doSlew_T0Cut2_1M.root");
    // chain[1] = new TChain("zdcTree"); chain[1]->Add("out16_GeneralPulse_doSlew_T0Cut5_1M.root");
    // chain[2] = new TChain("zdcTree"); chain[2]->Add("out16_GeneralPulse_doSlew_T0Cut15_1M.root");

    chain[0] = new TChain("zdcTree"); chain[0]->Add("out16_GeneralPulse_testNewVaryT0Cut_3sigma_andT05.root");

    process_Files();

    TCanvas* c = new TCanvas("c", "c", 1000, 800);
    c->Divide(2, 2);

    for (int i = 0; i < 4; i++) {
        c->cd(i + 1);
        for (int j = 0; j < 1; j++) {   // 3  1
            if (j == 0) {
                hist[j][i]->GetXaxis()->SetNdivisions(5);
                hist[j][i]->SetLineColor(colors[j]);
                hist[j][i]->SetMaximum(500000);
                hist[j][i]->SetMinimum(0.5);
                hist[j][i]->Draw();
            }
            else {
                hist[j][i]->SetLineColor(colors[j]);
                hist[j][i]->Draw("same");
            }
        }
        // if (i == 0) {
        //     TLegend *leg = new TLegend(.7, .7, .88, .88);
        //     leg->SetTextSize(.03);
        //     leg->SetBorderSize(0);
        //     leg->SetFillColor(0);
        //     leg->AddEntry(hist[0][i], "cut T0 2");
        //     leg->AddEntry(hist[1][i], "cut T0 5");
        //     leg->AddEntry(hist[2][i], "cut T0 15");
        //     leg->Draw("same");
        // }
        gPad->SetRightMargin(0.08);
        gPad->SetLogy(1);
    }
    return;
}
