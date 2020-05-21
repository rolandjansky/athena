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
#include "TLegend.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"

using namespace std;

TChain *chain;
TH1F *test;
// TH1F *hist[2][4][2][2];
TH1F *hist[2][4][2];
TCanvas *c[2][4][2];
int colors[10] = {kRed, kBlue, kGreen + 1, kCyan + 1, kYellow + 1, kOrange + 1, kAzure + 1, kMagenta, kGray, kBlack};

void process_File1()
{
    // gStyle->SetOptStat(0);

    UShort_t  zdc_raw[2][4][2][2][7];
    int zdc_Status[2][4];
    chain->SetBranchAddress("zdc_raw", &zdc_raw);
    chain->SetBranchAddress("zdc_Status", &zdc_Status);


    for (int i = 0; i < 1000000; i++)
    {
        chain->GetEntry(i);
        if (i % 100000 == 0) cout << "Processing " << i << " events" << endl;

        for (int m_index = 0; m_index < 4; m_index++)
        {
            for (int g_index = 0; g_index < 2; g_index++)
            {
                if ((zdc_Status[1][m_index] & 0x1F01) == 1) {
                    float diff = ((float)zdc_raw[1][m_index][g_index][1][0] + (float)zdc_raw[1][m_index][g_index][1][1] - (float)zdc_raw[1][m_index][g_index][0][0] - (float)zdc_raw[1][m_index][g_index][0][1]) / 2;
                    hist[1][m_index][g_index]->Fill(diff);
                }
            }
        }
    }
}


void process_distribution()
{
    chain = new TChain("zdcTree");

    chain->Add("out16_5TeV_run312968_FixTau1Tau2_fitTMax140.root");

    char hname[100];

    for (int m_index = 0; m_index < 4; m_index++)
    {
        for (int g_index = 0; g_index < 2; g_index++)
        {

            sprintf(hname, "h_s1_m%d_g%d", m_index, g_index);
            hist[1][m_index][g_index] = new TH1F(hname, hname, 200, -200, 200);
        }
    }

    process_File1();

    // test->Draw();

    char cname[100];
    // for (int m_index = 0; m_index < 4; m_index++)
    // {
    //     for (int g_index = 0; g_index < 2; g_index++)
    //     {
    //         for (int d_index = 0; d_index < 2; d_index++)
    //         {
    //             sprintf(cname, "c1_%d_%d_%d", m_index, g_index, d_index);
    //             c[1][m_index][g_index][d_index] = new TCanvas(cname, cname, 800, 544);
    //             hist[1][m_index][g_index][d_index]->Draw();
    //             c[1][m_index][g_index][d_index]->Print(Form("temp/c_s1_m%d_g%d_d%d.png", m_index, g_index, d_index));
    //         }
    //     }
    // }
    TFile *fout = new TFile("out_diff_avg.root", "RECREATE");

    for (int m_index = 0; m_index < 4; m_index++)
    {
        for (int g_index = 0; g_index < 2; g_index++)
        {
            sprintf(cname, "c1_%d_%d", m_index, g_index);
            c[1][m_index][g_index] = new TCanvas(cname, cname, 800, 544);
            hist[1][m_index][g_index]->Draw();
            gPad->SetLogy(1);
            gPad->SetTicks(1, 1);
            c[1][m_index][g_index]->Print(Form("lowmu_diffDnUD_avgOfFirstTwoSample/c_s1_m%d_g%d_filtered.png", m_index, g_index));

            hist[1][m_index][g_index]->Write();
        }
    }
    fout->Close();


    return;
}
