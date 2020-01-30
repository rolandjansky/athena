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

using namespace std;

TChain *chain;
TH1 *h0;
TH1 *h1;
TH1 *h2;
TH1 *h3;
int colors[10] = {kRed, kBlue, kGreen + 1, kCyan + 1, kYellow + 1, kOrange + 1, kAzure + 1, kMagenta, kGray, kBlack};

TH2 *hist;

int n = 4; //n neutrons

double Nnutrons(double *xvec, double *pvec)
{
    double mu    = pvec[0];
    double sigma = pvec[1];

    double function = 0.;

    for (int i = 0; i < n; i++)
    {
        function += pvec[i + 2] * exp(-0.5 * pow(((xvec[0] - (i + 1) * mu) / (sqrt(i + 1) * sigma)), 2));
    }

    return function;
}

void process_File1()
{
    gStyle->SetOptStat(0);

    h0 = new TH1F("h0", "h0",   130, 50, 1350);
    h1 = new TH1F("h1", "h1",   130, 50, 1350);
    h2 = new TH1F("h2", "h2",   130, 50, 1350);
    h3 = new TH1F("h3", "h3",   130, 50, 1350);

    float  zdc_ZdcModuleAmp[2][4];
    UInt_t zdc_ZdcModuleMask = 0;
    chain->SetBranchAddress("zdc_ZdcModuleAmp", &zdc_ZdcModuleAmp);
    chain->SetBranchAddress("zdc_ZdcModuleMask", &zdc_ZdcModuleMask);

    Bool_t L1_ZDC_A;
    Bool_t L1_ZDC_C;
    chain->SetBranchAddress("L1_ZDC_A", &L1_ZDC_A);
    chain->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C);

    // fChain->GetEntry(1);
    // cout << zdc_ModuleMask << endl;
    // cout << zdc_Amp[1][0] << endl;
    // cout << zdc_ZdcModuleMask << endl;
    // cout << zdc_ZdcModuleAmp[1][0] << endl;

    for (int i = 0; i < 5000000; i++)
    {
        chain->GetEntry(i);
        if (i % 1000000 == 0) cout << "Processing " << i << " events" << endl;

        float SumAmp_0 = 0;
        float SumAmp_1 = 0;
        float SumAmp_2 = 0;
        float SumAmp_3 = 0;

        // if ((zdc_ZdcModuleMask & 0xF0) != 0                    ) SumAmp_0 = zdc_ZdcModuleAmp[1][0] + zdc_ZdcModuleAmp[1][1] + zdc_ZdcModuleAmp[1][2] + zdc_ZdcModuleAmp[1][3];
        // if ((zdc_ZdcModuleMask & 0xF0) == 0xF0                 ) SumAmp_1 = zdc_ZdcModuleAmp[1][0] + zdc_ZdcModuleAmp[1][1] + zdc_ZdcModuleAmp[1][2] + zdc_ZdcModuleAmp[1][3];
        // if ((zdc_ZdcModuleMask & 0xF0) != 0    && L1_ZDC_C == 1) SumAmp_2 = zdc_ZdcModuleAmp[1][0] + zdc_ZdcModuleAmp[1][1] + zdc_ZdcModuleAmp[1][2] + zdc_ZdcModuleAmp[1][3];
        // if ((zdc_ZdcModuleMask & 0xF0) == 0xF0 && L1_ZDC_C == 1) SumAmp_3 = zdc_ZdcModuleAmp[1][0] + zdc_ZdcModuleAmp[1][1] + zdc_ZdcModuleAmp[1][2] + zdc_ZdcModuleAmp[1][3];

        if ((zdc_ZdcModuleMask & 0xF) != 0                   ) SumAmp_0 = zdc_ZdcModuleAmp[0][0] + zdc_ZdcModuleAmp[0][1] + zdc_ZdcModuleAmp[0][2] + zdc_ZdcModuleAmp[0][3];
        if ((zdc_ZdcModuleMask & 0xF) == 0xF                 ) SumAmp_1 = zdc_ZdcModuleAmp[0][0] + zdc_ZdcModuleAmp[0][1] + zdc_ZdcModuleAmp[0][2] + zdc_ZdcModuleAmp[0][3];
        if ((zdc_ZdcModuleMask & 0xF) != 0   && L1_ZDC_A == 1) SumAmp_2 = zdc_ZdcModuleAmp[0][0] + zdc_ZdcModuleAmp[0][1] + zdc_ZdcModuleAmp[0][2] + zdc_ZdcModuleAmp[0][3];
        if ((zdc_ZdcModuleMask & 0xF) == 0xF && L1_ZDC_A == 1) SumAmp_3 = zdc_ZdcModuleAmp[0][0] + zdc_ZdcModuleAmp[0][1] + zdc_ZdcModuleAmp[0][2] + zdc_ZdcModuleAmp[0][3];

        h0->Fill(SumAmp_0);
        h1->Fill(SumAmp_1);
        h2->Fill(SumAmp_2);
        h3->Fill(SumAmp_3);
    }
}

void process_File2()
{
    gStyle->SetOptStat(0);

    h0 = new TH1F("h0", "h0",   130, 50, 1350);
    // h1 = new TH1F("h1", "h1",   130, 50, 1350);
    // h2 = new TH1F("h2", "h2",   130, 50, 1350);
    // h3 = new TH1F("h3", "h3",   130, 50, 1350);

    float zdc_Amp[2][4];
    int   zdc_ModuleMask = 0;
    chain->SetBranchAddress("zdc_Amp", &zdc_Amp);
    chain->SetBranchAddress("zdc_ModuleMask", &zdc_ModuleMask);
    int zdc_Status[2][4];
    chain->SetBranchAddress("zdc_Status", &zdc_Status);

    Bool_t L1_ZDC_A;
    Bool_t L1_ZDC_C;
    chain->SetBranchAddress("L1_ZDC_A", &L1_ZDC_A);
    chain->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C);

    // fChain->GetEntry(1);
    // cout << zdc_ModuleMask << endl;
    // cout << zdc_Amp[1][0] << endl;
    // cout << zdc_ZdcModuleMask << endl;
    // cout << zdc_ZdcModuleAmp[1][0] << endl;

    for (int i = 0; i < 1000000; i++)
    {
        chain->GetEntry(i);
        if (i % 100000 == 0) cout << "Processing " << i << " events" << endl;

        float SumAmp_0 = 0;
        float SumAmp_1 = 0;
        float SumAmp_2 = 0;
        float SumAmp_3 = 0;

        // if (zdc_Amp[1][0] < 20) zdc_Amp[1][0] = 0;
        // if (zdc_Amp[1][1] < 20) zdc_Amp[1][1] = 0;
        // if (zdc_Amp[1][2] < 20) zdc_Amp[1][2] = 0;
        // if (zdc_Amp[1][3] < 20) zdc_Amp[1][3] = 0;

        // if ((zdc_ModuleMask & 0xF0) != 0                    ) SumAmp_0 = zdc_Amp[1][0] + zdc_Amp[1][1] + zdc_Amp[1][2] + zdc_Amp[1][3];
        // if ((zdc_ModuleMask & 0xF0) == 0xF0                 ) SumAmp_1 = zdc_Amp[1][0] + zdc_Amp[1][1] + zdc_Amp[1][2] + zdc_Amp[1][3];
        if ((zdc_ModuleMask & 0xF0) != 0    && L1_ZDC_C == 1) SumAmp_2 = zdc_Amp[1][0] + zdc_Amp[1][1] + zdc_Amp[1][2] + zdc_Amp[1][3];
        if ((zdc_ModuleMask & 0xF0) == 0xF0 && L1_ZDC_C == 1) SumAmp_3 = zdc_Amp[1][0] + zdc_Amp[1][1] + zdc_Amp[1][2] + zdc_Amp[1][3];
        if (((zdc_Status[1][0] & 0x705) == 1 && (zdc_Status[1][1] & 0x705) == 1 && (zdc_Status[1][2] & 0x705) == 1 && (zdc_Status[1][3] & 0x705) == 1   ) && L1_ZDC_C == 1) SumAmp_0 = zdc_Amp[1][0] + zdc_Amp[1][1] + zdc_Amp[1][2] + zdc_Amp[1][3];
        if (((zdc_Status[1][0] & 1) == 1 && (zdc_Status[1][1] & 1) == 1 && (zdc_Status[1][2] & 1) == 1 && (zdc_Status[1][3] & 1) == 1   ) && L1_ZDC_C == 1) SumAmp_1 = zdc_Amp[1][0] + zdc_Amp[1][1] + zdc_Amp[1][2] + zdc_Amp[1][3];

        // if ((zdc_Status[1][0] & 0x605) == 1 && (zdc_Status[1][1] & 0x605) == 1 && (zdc_Status[1][2] & 0x605) == 1 && (zdc_Status[1][3] & 0x605) == 1) SumAmp_0 = zdc_Amp[1][0] + zdc_Amp[1][1] + zdc_Amp[1][2] + zdc_Amp[1][3];
        // if ((zdc_Status[1][0] & 0x705) == 1 && (zdc_Status[1][1] & 0x705) == 1 && (zdc_Status[1][2] & 0x705) == 1 && (zdc_Status[1][3] & 0x705) == 1) SumAmp_1 = zdc_Amp[1][0] + zdc_Amp[1][1] + zdc_Amp[1][2] + zdc_Amp[1][3];

        h0->Fill(SumAmp_0);
        // h1->Fill(SumAmp_1);
        // h2->Fill(SumAmp_2);
        // h3->Fill(SumAmp_3);
    }
}

void process_File3()
{
    // gStyle->SetOptStat(0);

    h0 = new TH1F("h0", "h0",   100, -50, 50);

    float zdc_Amp[2][4];
    chain->SetBranchAddress("zdc_Amp", &zdc_Amp);
    int zdc_Status[2][4];
    chain->SetBranchAddress("zdc_Status", &zdc_Status);
    float zdc_Min2ndDeriv[2][4];
    chain->SetBranchAddress("zdc_Min2ndDeriv", &zdc_Min2ndDeriv);
    vector<vector<float>> *zdc_samplesDeriv2nd = 0;
    chain->SetBranchAddress("zdc_samplesDeriv2nd", &zdc_samplesDeriv2nd);
    vector<vector<float>> *zdc_samplesDeriv = 0;
    chain->SetBranchAddress("zdc_samplesDeriv", &zdc_samplesDeriv);

    TCanvas *c = new TCanvas("c", "c", 800, 544);

    char name[100];
    char cname[100];
    char status[100];

    // for (int j = 7; j < 11; j++) {
    //     for (int i = 4; i < 8; i++) {
    //         // sprintf(name, "(zdc_samplesDeriv2nd[%d][%d]-0.05*zdc_Min2ndDeriv[1][%d])>>h0", i, j, i-4);
    //         // sprintf(name, "(zdc_samplesDeriv[%d][%d])>>(100,-50,50)", i, j);
    //         // sprintf(status, "(zdc_Status[1][%d]&0xC09)==9", i-4);
    //         // sprintf(cname, "update0323/samplesDeriv_%d_%d_LG.eps", i, j);

    //         // plot 2nd deriv and 1st deriv vs amp
    //         //
    //         // sprintf(name, "zdc_samplesDeriv2nd[%d][%d]:zdc_Amp[1][%d]>>(100,0,800,100,-150,200)", i, j, i-4);   // Deriv2nd vs amp
    //         // sprintf(name, "zdc_samplesDeriv[%d][%d]:zdc_Amp[1][%d]>>(100,0,800,100,-100,100)", i, j, i-4);   // Deriv vs amp
    //         // sprintf(status, "(zdc_Status[1][%d]&0x1C0D)==1", i-4);
    //         // sprintf(cname, "update0408/samplesDeriv_Amp_1_%d_%d_HG_slope.eps", i-4, j);
    //         // chain->Draw(name, status,"colz");
    //         // gPad->SetLogz(1);
    //         // c->SaveAs(cname);

    //         sprintf(name, "zdc_samplesDeriv[%d][%d]:zdc_samplesDeriv[%d][%d]>>(100,-100,100,100,-100,100)", i, j, i, j+1);   // Deriv vs amp
    //         sprintf(status, "(zdc_Status[1][%d]&0x1C0D)==1", i-4);
    //         sprintf(cname, "update0408/samplesDeriv_Amp_1_%d_%d_HG_slope.eps", i-4, j);
    //         chain->Draw(name, status,"colz");
    //         gPad->SetLogz(1);
    //         c->SaveAs(cname);
    //     }
    // }


    for (int i = 4; i < 8; i++) {
        sprintf(name, "zdc_samplesDeriv2nd[1][%d][7]:zdc_samplesDeriv2nd[1][%d][8]", i-4, i-4);
        sprintf(status, "(zdc_Status[1][%d]&0x1C0D)==1", i - 4);
        sprintf(cname, "update0408/samplesDeriv2nd_1_%d_7vs8_HG_slope.eps", i - 4);
        hist = new TH2F("hist", name,   100,-150,200,100,-150,200);
        sprintf(name, "zdc_samplesDeriv2nd[%d][7]:zdc_samplesDeriv2nd[%d][8]>>hist", i, i);
        chain->Draw(name, status, "colz");
        gPad->SetLogz(1);
        hist->GetXaxis()->SetTitle("sample 8");
        hist->GetYaxis()->SetTitle("sample 7");
        c->SaveAs(cname);
        delete hist;

        sprintf(name, "zdc_samplesDeriv2nd[1][%d][8]:zdc_samplesDeriv2nd[1][%d][9]", i-4, i-4);
        sprintf(cname, "update0408/samplesDeriv2nd_1_%d_8vs9_HG_slope.eps", i - 4);
        hist = new TH2F("hist", name,   100,-150,200,100,-150,200);
        sprintf(name, "zdc_samplesDeriv2nd[%d][8]:zdc_samplesDeriv2nd[%d][9]>>hist", i, i);
        chain->Draw(name, status, "colz");
        gPad->SetLogz(1);
        hist->GetXaxis()->SetTitle("sample 9");
        hist->GetYaxis()->SetTitle("sample 8");
        c->SaveAs(cname);
        delete hist;

        sprintf(name, "zdc_samplesDeriv2nd[1][%d][7]:zdc_samplesDeriv2nd[1][%d][9]", i-4, i-4);
        sprintf(cname, "update0408/samplesDeriv2nd_1_%d_7vs9_HG_slope.eps", i - 4);
        hist = new TH2F("hist", name,   100,-150,200,100,-150,200);
        sprintf(name, "zdc_samplesDeriv2nd[%d][7]:zdc_samplesDeriv2nd[%d][9]>>hist", i, i);
        chain->Draw(name, status, "colz");
        gPad->SetLogz(1);
        hist->GetXaxis()->SetTitle("sample 9");
        hist->GetYaxis()->SetTitle("sample 7");
        c->SaveAs(cname);
        delete hist;
    }
}



void process_data()
{
    chain = new TChain("zdcTree");

    chain->Add("out16_8TeV_run313187_fermiExplinear_fixTau1Tau2_Chisqr15_fitTMax140_LGtoHG_partLOG_1M.root.root");
    // chain->Add("out16_8TeV_run313187_fermiExplinear_fixTau1Tau2_Chisqr15_fitTMax140_6M.root");


    // chain->Add("data15_hi.00287931.calibration_zdcCalib.merge.AOD.c932_m1533.85/user.steinber.16744698._000001.ANALYSIS.root");
    // chain->Add("data15_hi.00287931.calibration_zdcCalib.merge.AOD.c932_m1533.85/user.steinber.16744698._000002.ANALYSIS.root");
    // chain->Add("data15_hi.00287931.calibration_zdcCalib.merge.AOD.c932_m1533.85/user.steinber.16744698._000003.ANALYSIS.root");

    // process_File1();
    process_File3();

    // TFile *fout = new TFile("out_sumAmp.root","RECREATE");
    // TFile *fout = new TFile("out_8TeV_check.root","RECREATE");
    // h0->Write();
    // h1->Write();
    // h2->Write();
    // h3->Write();
    // fout->Close();

    return;
}
