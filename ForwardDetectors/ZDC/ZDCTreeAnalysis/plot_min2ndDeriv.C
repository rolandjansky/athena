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

int nSig = 4;
// string gain = "low";
string gain = "high";
// bool doSlew = true;
bool doSlew = false;

vector<double> corr[2][2][4];    // 0 high gain, 1 low gain

void plot_min2ndDeriv()
{
    gStyle->SetOptStat(0);

    TChain *chain = new TChain("zdcTree");
    chain->Add("out16_GeneralPulse_testNewVaryT0Cut_3sigma_andT05.root");
    TCanvas* c[2][4];
    TH2D* hist = nullptr;
    int  amphi = 0;

    for (int sIndex = 0; sIndex < 2; sIndex++)
    {
        for (int mIndex = 0; mIndex < 4; mIndex++)
        {
            if (sIndex == 0 && mIndex == 0) continue;
            char cname[100];
            sprintf(cname, "c%d_%d", sIndex, mIndex);
            c[sIndex][mIndex] = new TCanvas(cname, cname, 600, 450);
            // c[sIndex][mIndex]->Divide(2);

            char hname[100];
            sprintf(hname, "h%d_%d", sIndex, mIndex);
            // c[sIndex][mIndex]->cd(1);
            if (gain == "high") {
                amphi = 1000;
                hist = new TH2D("hist", hname, 100, -1000, 100, 100, 0, amphi);
                chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_Min2ndDeriv[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>hist", "L1_ZDC_C && (zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x1CF7)==1", "goff");
            }
            if (gain == "low") {
                amphi = 10000;
                hist = new TH2D("hist", hname, 100, -1000, 100, 100, 0, amphi);
                chain->Draw("zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_Min2ndDeriv[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>hist", "L1_ZDC_C && (zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0x1CF7)==3", "goff");
            }
            hist->GetXaxis()->SetTitle("min2ndDeriv");
            hist->GetYaxis()->SetTitle("Amplitude");
            hist->Draw("colz");

            // -------------------------------
            // Draw range
            //
            char PjYfgname[100];
            TH1D* upper = new TH1D("upper", "upper", 100, -1000, 100);
            TH1D* lower = new TH1D("lower", "lower", 100, -1000, 100);
            TH1D* mean  = new TH1D("mean" , "mean" , 100, -1000, 100);
            TH1D* dsig  = new TH1D("dsig" , "dsig", 1000, 0, 1000);
            for (int i = 1; i <= 100; i++) {
                sprintf(PjYfgname, "PjY_bin%.3d", i);
                TH1D* pjy = hist->ProjectionY(PjYfgname, i, i);
                if (pjy->Integral() < 0.001) continue;
                TF1* f1 = new TF1("f1", "gaus", 0, amphi);
                pjy->Fit("f1", "RQN");

                double _mean     = f1->GetParameter(1);
                double _meanErr  = f1->GetParError(1);
                double _sigma    = f1->GetParameter(2);
                double _sigmaErr = f1->GetParError(2);

                upper->SetBinContent(i, _mean + nSig * _sigma);
                lower->SetBinContent(i, _mean - nSig * _sigma);
                upper->SetBinError  (i, nSig * _sigmaErr);
                lower->SetBinError  (i, nSig * _sigmaErr);
                mean ->SetBinContent(i, _mean);
                mean ->SetBinError  (i, _meanErr);
                dsig ->Fill(4*_sigma);

                delete pjy;
                delete f1;
            }

            // upper->SetLineColor(kRed);
            // upper->Draw("same");

            // lower->SetLineColor(kRed);
            // lower->Draw("same");

            mean ->Draw("same");
            gPad->SetLogz(1);
            gPad->SetTicks(1, 1);

            TF1* f2 = nullptr;
            f2 = new TF1("f2", "pol1", -700, 0);
            f2->SetParameter(0, 10);
            f2->SetParameter(1, -1);
            mean->Fit("f2", "RQ");

            TF1* fup = new TF1("fup", "pol1", -700, 0);
            fup->FixParameter(0, f2->GetParameter(0) + 50);
            fup->FixParameter(1, f2->GetParameter(1));
            fup->SetLineColor(kRed);
            fup->Draw("same");

            TF1* flo = new TF1("flo", "pol1", -700, 0);
            flo->FixParameter(0, f2->GetParameter(0) - 50);
            flo->FixParameter(1, f2->GetParameter(1));
            flo->SetLineColor(kRed);
            flo->Draw("same");
            // TLegend *leg = new TLegend(.58, .7, .88, .88);
            // leg->SetTextSize(.03);
            // leg->SetBorderSize(0);
            // leg->SetFillColor(0);
            // leg->AddEntry(f2, Form("p0 = %e", f2->GetParameter(0)));
            // leg->AddEntry(f2, Form("p1 = %e", f2->GetParameter(1)));
            // leg->Draw("same");
            // -------------------------------


            // //--------------------------------
            // // Plot ratio plots
            // //
            // c[sIndex][mIndex]->cd(2);
            // dsig->SetMarkerColor(kBlack);
            // dsig->SetMarkerStyle(20);
            // dsig->SetMarkerSize(0.6);
            // dsig->GetXaxis()->SetTitle("min2ndDeriv");
            // dsig->GetYaxis()->SetTitle("#count");

            // dsig->Draw();
            // gPad->SetLogz(1);
            // gPad->SetTicks(1, 1);
            // TLegend *leg2 = new TLegend(.58, .7, .88, .88);
            // leg2->SetTextSize(.03);
            // leg2->SetBorderSize(0);
            // leg2->SetFillColor(0);
            // leg2->AddEntry("", Form("peak = %d", dsig->GetMaximumBin()));
            // leg2->Draw("same");
            // //--------------------------------



            //--------------------------------
            // Save canvas
            //

            if (gain == "high") c[sIndex][mIndex]->Print(Form("update0621/amp:min2ndDeriv_%d%d_50_HG_6M.png", sIndex, mIndex));
            if (gain == "low")  c[sIndex][mIndex]->Print(Form("update0621/amp:min2ndDeriv_%d%d_50_LG_6M.png", sIndex, mIndex));
            //--------------------------------

            c[sIndex][mIndex]->Close();
            gSystem->ProcessEvents();
            delete c[sIndex][mIndex];
            delete hist;
            delete upper;
            delete lower;
            delete mean;
            delete dsig;
            c[sIndex][mIndex] = 0;
        }
    }

    return;
}
