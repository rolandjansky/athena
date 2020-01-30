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
#include "TH2.h"
#include "TLegend.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"
#include "TSystem.h"

using namespace std;

void plot_taus()
{
    TChain *chain = new TChain("zdcTree");
    chain->Add("out16_GeneralPulse_doSlew_newT0Cut_6M.root");
    TCanvas* c[2][4];
    TH2D* hist;

    for (int sIndex = 0; sIndex < 2; sIndex++)
    {
        // int threshold = sIndex == 0 ? 100 : 250;
        // int threshold = sIndex == 0 ? 1000 : 2000;
        for (int mIndex = 0; mIndex < 4; mIndex++)
        {
            if (sIndex == 0 && mIndex == 0) continue;
            char cname[100];
            sprintf(cname, "c%d_%d", sIndex, mIndex);
            c[sIndex][mIndex] = new TCanvas(cname, cname, 800, 544);

            // amp vs tau2
            // chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_FitTau2[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>(100, 10, 40, 80, 200, 1000)", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x1F0B)==1" , "colz");
            // chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_FitTau2[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>(100,10,40,100,"+ TString::Itoa(threshold, 10) +",1000)", "zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "] >" + TString::Itoa(threshold, 10) + "&& (zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x109)==1" , "colz");

            // amp vs tau1
            // chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_FitTau1[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>(100,2.51,5.99,80, 200, 1000)", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x1F0B)==1" , "colz");
            // chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_FitTau1[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>(100,2.51,6.01,100,"+ TString::Itoa(threshold, 10) +",1000)", "zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "] >" + TString::Itoa(threshold, 10) + "&& (zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x109)==1" , "colz");

            // amp vs t0
            // chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_FitT0[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>(100,0,100,100,"+ TString::Itoa(threshold, 10) +",1000)", "zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "] >" + TString::Itoa(threshold, 10) + "&& (zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x109)==1" , "colz");
            // chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_FitT0[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>(100,0,100,100,"+ TString::Itoa(threshold, 10) +",10000)", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x109)==9" , "colz");

            // chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_FitT0[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>(100,0,100,80,200,1000)", "zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "] > 200 && (zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x1F0B)==1" , "colz");
            // chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_FitT0[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>(100,0,100,80,2000,10000)", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x1F0B)==0xB" , "colz");

            // amp vs t0corr
            int amphi = 1000;
            char hname[100];
            sprintf(hname, "h%d_%d", sIndex, mIndex);
            hist = new TH2D("hist", hname, 100, 0, amphi, 100, -25, 25);
            chain->Draw("zdc_T0Corr[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>hist", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0xC07)==1", "goff");
            // chain->Draw("zdc_T0Corr[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>hist", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0xF07)==3", "goff");
            hist->GetXaxis()->SetTitle("Amplitude");
            hist->GetYaxis()->SetTitle("T0_{ corrected}");
            hist->Draw("colz");

            TLine *line_max = new TLine(0, 2, amphi, 2);
            line_max->SetLineStyle(2);
            line_max->SetLineColor(kRed);
            line_max->Draw("same");
            TLine *line_min = new TLine(0, -2, amphi, -2);
            line_min->SetLineStyle(2);
            line_min->SetLineColor(kRed);
            line_min->Draw("same");


            gPad->SetLogz(1);
            gPad->SetTicks(1, 1);
            // c[sIndex][mIndex]->Print(Form("2018defaultValues/amp:tau2_%d%d.png", sIndex, mIndex));
            // c[sIndex][mIndex]->Print(Form("2018defaultValues/amp:tau1_%d%d.png", sIndex, mIndex));
            // c[sIndex][mIndex]->Print(Form("2018defaultValues/hg_amp:t0_%d%d.png", sIndex, mIndex));
            // c[sIndex][mIndex]->Print(Form("2018defaultValues/lg_amp:t0_%d%d.png", sIndex, mIndex));

            c[sIndex][mIndex]->Print(Form("update0612/amp:T0Corr_%d%d_HG_cut2_6M_C07.png", sIndex, mIndex));     // amp vs t0corr
            // c[sIndex][mIndex]->Print(Form("update0612/amp:T0Corr_%d%d_LG_cut2_6M.png", sIndex, mIndex));     // amp vs t0corr

            c[sIndex][mIndex]->Close();
            gSystem->ProcessEvents();
            delete c[sIndex][mIndex];
            delete hist;
            c[sIndex][mIndex] = 0;
        }

    }





    return;
}
