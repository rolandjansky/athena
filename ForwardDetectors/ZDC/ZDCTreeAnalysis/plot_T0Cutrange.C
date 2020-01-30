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

int nSig = 3;
string gain = "low";
// string gain = "high";
bool doSlew = true;
// bool doSlew = false;

float fb_hg[2][2][4] = {
    {{100, 100, 100, 100}, {100, 100, 100, 100}},
    {{600, 600, 600, 600}, {600, 600, 600, 600}}
};
float fb_lg[2][2][4] = {
    {{1000, 1000, 1000, 1000}, {1000, 1000, 1000, 1000}},
    {{2000, 3000, 1800, 1800}, {7000, 7000, 7000, 3000}}
};

vector<double> corr[2][2][4];    // 0 high gain, 1 low gain

void setupParas() {
    /*
    // 4 sigma
    corr[0][0][0] = {0., 0., 0., 0.};
    corr[0][0][1] = {5.523101, 2.735484e-3, -3.045758e-6,  2.398731e-9 };
    corr[0][0][2] = {5.401597, 2.664528e-3, -1.656913e-6,  6.526235e-10};
    corr[0][0][3] = {5.068347, 2.934560e-3, -6.166999e-7, -9.100381e-10};

    corr[0][1][0] = {6.285770, 1.364173e-3, -5.582632e-7,  4.050092e-10};
    corr[0][1][1] = {6.388118, 1.059508e-3, -1.162300e-7,  2.072714e-10};
    corr[0][1][2] = {6.327466, 9.578267e-4, -5.096097e-7,  8.635932e-10};
    corr[0][1][3] = {5.827046, 1.638310e-3,  1.926675e-7, -4.908247e-10};

    corr[1][0][0] = {0., 0., 0., 0.};
    corr[1][0][1] = {6.961994, -1.224803e-4,  3.792120e-7, -8.476456e-11};
    corr[1][0][2] = {0.518704,  1.387013e-2, -9.458857e-6,  2.191948e-9 };
    corr[1][0][3] = {1.909640,  1.250902e-2, -1.041146e-5,  3.010049e-9 };

    corr[1][1][0] = {6.610375, 8.510258e-4, -1.199924e-7,  5.905106e-12};
    corr[1][1][1] = {6.590451, 9.164059e-4, -1.626631e-7,  1.059131e-11};
    corr[1][1][2] = {6.930768, 7.651640e-4, -1.401897e-7,  9.679413e-12};
    corr[1][1][3] = {6.594080, 1.096790e-3, -3.630054e-7,  4.884000e-11};
    */

    // 3 sigma
    corr[0][0][0] = {0., 0., 0., 0.};
    corr[0][0][1] = {5.235419, 2.735484e-3, -3.045758e-6,  2.398731e-9 };
    corr[0][0][2] = {5.113915, 2.664528e-3, -1.656913e-6,  6.526235e-10};
    corr[0][0][3] = {4.780665, 2.934560e-3, -6.166999e-7, -9.100381e-10};

    corr[0][1][0] = {5.998087, 1.364173e-3, -5.582632e-7,  4.050092e-10};
    corr[0][1][1] = {6.100435, 1.059508e-3, -1.162300e-7,  2.072714e-10};
    corr[0][1][2] = {6.039784, 9.578267e-4, -5.096097e-7,  8.635932e-10};
    corr[0][1][3] = {5.539364, 1.638310e-3,  1.926675e-7, -4.908247e-10};

    corr[1][0][0] = {0., 0., 0., 0.};
    corr[1][0][1] = {6.674312, -1.224803e-4,  3.792120e-7, -8.476456e-11};
    corr[1][0][2] = {0.231021,  1.387013e-2, -9.458857e-6,  2.191948e-9 };
    corr[1][0][3] = {1.621958,  1.250902e-2, -1.041146e-5,  3.010049e-9 };

    corr[1][1][0] = {6.322693, 8.510258e-4, -1.199924e-7,  5.905106e-12};
    corr[1][1][1] = {6.302769, 9.164059e-4, -1.626631e-7,  1.059131e-11};
    corr[1][1][2] = {6.643086, 7.651640e-4, -1.401897e-7,  9.679413e-12};
    corr[1][1][3] = {6.306398, 1.096790e-3, -3.630054e-7,  4.884000e-11};

    return;
}

void plot_T0Cutrange()
{
    gStyle->SetOptStat(0);

    if (doSlew) setupParas();

    TChain *chain = new TChain("zdcTree");
    chain->Add("out16_GeneralPulse_doSlew_newT0Cut_6M.root");
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
            c[sIndex][mIndex] = new TCanvas(cname, cname, 1200, 450);
            c[sIndex][mIndex]->Divide(2);

            char hname[100];
            sprintf(hname, "h%d_%d", sIndex, mIndex);
            c[sIndex][mIndex]->cd(1);
            if (gain == "high") {
                amphi = 1000;
                hist = new TH2D("hist", hname, 100, 0, amphi, 100, -20, 20);
                chain->Draw("zdc_T0Corr[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>hist", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0xC07)==1", "goff");
            }
            if (gain == "low") {
                amphi = 10000;
                hist = new TH2D("hist", hname, 100, 0, amphi, 100, -5, 5);
                chain->Draw("zdc_T0Corr[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]>>hist", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0xF07)==3", "goff");
            }
            hist->GetXaxis()->SetTitle("Amplitude");
            hist->GetYaxis()->SetTitle("T0_{ corrected}");
            hist->Draw("colz");

            // -------------------------------
            // Draw range
            //
            char PjYfgname[100];
            TH1D* upper = new TH1D("upper", "upper", 100, 0, amphi);
            TH1D* lower = new TH1D("lower", "lower", 100, 0, amphi);
            TH1D* mean  = new TH1D("mean" , "mean" , 100, 0, amphi);
            TH1D* ratio = new TH1D("ratio", "n#sigma,amplitude", 100, 0, amphi);
            TH1D* cutup = new TH1D("cutup"  , "real cut upper", 100, 0, amphi);
            TH1D* cutlo = new TH1D("cutlo"  , "real cut lower", 100, 0, amphi);
            for (int i = 1; i <= 100; i++) {
                sprintf(PjYfgname, "PjY_bin%.3d", i);
                TH1D* pjy = hist->ProjectionY(PjYfgname, i, i);
                if (pjy->Integral() < 0.001) continue;
                TF1* f1 = new TF1("f1", "gaus", -15, 15);
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

                double t_amp = ratio->GetBinCenter(i);
                double correction = 0;
                if (doSlew) {
                    if (gain == "high") correction = corr[0][sIndex][mIndex][0] + t_amp * corr[0][sIndex][mIndex][1] + t_amp * t_amp * corr[0][sIndex][mIndex][2] + t_amp * t_amp * t_amp * corr[0][sIndex][mIndex][3];
                    if (gain == "low")  correction = corr[1][sIndex][mIndex][0] + t_amp * corr[1][sIndex][mIndex][1] + t_amp * t_amp * corr[1][sIndex][mIndex][2] + t_amp * t_amp * t_amp * corr[1][sIndex][mIndex][3];
                }

                ratio->SetBinContent(i, log(nSig * _sigma * mean->GetBinCenter(i)) - correction);
                ratio->SetBinError  (i, 0.000001);

                delete pjy;
                delete f1;
            }

            if (doSlew) {
                for (int i = 1; i <= 100; i++) {
                    double t_amp = ratio->GetBinCenter(i);
                    double correction = 0;
                    if (gain == "high") correction = corr[0][sIndex][mIndex][0] + t_amp * corr[0][sIndex][mIndex][1] + t_amp * t_amp * corr[0][sIndex][mIndex][2] + t_amp * t_amp * t_amp * corr[0][sIndex][mIndex][3];
                    if (gain == "low")  correction = corr[1][sIndex][mIndex][0] + t_amp * corr[1][sIndex][mIndex][1] + t_amp * t_amp * corr[1][sIndex][mIndex][2] + t_amp * t_amp * t_amp * corr[1][sIndex][mIndex][3];

                    double _cut = exp(correction) / mean->GetBinCenter(i);
                    if (_cut > 5) _cut = 5;
                    cutup->SetBinContent(i, _cut);
                    cutup->SetBinError  (i, 0.000001);
                    cutlo->SetBinContent(i, -_cut);
                    cutlo->SetBinError  (i, 0.000001);
                }
                cutup->SetLineColor(kRed);
                cutup->Draw("same");
                cutlo->SetLineColor(kRed);
                cutlo->Draw("same");
            }
            else {
                upper->SetLineColor(kRed);
                upper->Draw("same");

                lower->SetLineColor(kRed);
                lower->Draw("same");
            }
            mean ->Draw("same");
            gPad->SetLogz(1);
            gPad->SetTicks(1, 1);
            // -------------------------------


            //--------------------------------
            // Plot ratio plots
            //
            c[sIndex][mIndex]->cd(2);
            ratio->SetMarkerColor(kBlack);
            ratio->SetMarkerStyle(20);
            ratio->SetMarkerSize(0.6);
            ratio->GetXaxis()->SetTitle("Amplitude");
            if (!doSlew) {
                ratio->GetYaxis()->SetTitle("log(n#sigma*A)");
                if (gain == "high") {ratio->SetMaximum(9); ratio->SetMinimum(4);}
                if (gain == "low")  {ratio->SetMaximum(12); ratio->SetMinimum(6);}
                ratio->Draw();
                TF1* f2 = nullptr;
                if (gain == "high") f2 = new TF1("f2", "pol3", fb_hg[0][sIndex][mIndex], fb_hg[1][sIndex][mIndex]);
                if (gain == "low")  f2 = new TF1("f2", "pol3", fb_lg[0][sIndex][mIndex], fb_lg[1][sIndex][mIndex]);
                f2->SetParameter(0, 10);
                f2->SetParameter(1, 10);
                ratio->Fit("f2", "RQ");

                TF1* f_long = new TF1("f_long", "pol4", 0, 11000);
                f_long->SetParameters(f2->GetParameter(0), f2->GetParameter(1), f2->GetParameter(2), f2->GetParameter(3));
                // f_long->SetLineStyle(3);
                f_long->SetLineColor(kRed);
                f_long->Draw("l,same");

                TLegend *leg = new TLegend(.58, .7, .88, .88);
                leg->SetTextSize(.03);
                leg->SetBorderSize(0);
                leg->SetFillColor(0);
                leg->AddEntry(f2, Form("p0 = %e", f2->GetParameter(0)));
                leg->AddEntry(f2, Form("p1 = %e", f2->GetParameter(1)));
                leg->AddEntry(f2, Form("p2 = %e", f2->GetParameter(2)));
                leg->AddEntry(f2, Form("p3 = %e", f2->GetParameter(3)));
                leg->Draw("same");
            }
            else {
                ratio->GetYaxis()->SetTitle("log(n#sigma*A) - corr");
                ratio->SetMaximum(2);
                ratio->SetMinimum(-2);
                ratio->Draw();
            }

            // gPad->SetLogy(1);
            gPad->SetTicks(1, 1);
            //--------------------------------



            //--------------------------------
            // Save canvas
            //
            if (doSlew) {
                if (gain == "high") c[sIndex][mIndex]->Print(Form("update0617/amp:T0Corr_%d%d_mult_3sigma_HG_slew_6M.png", sIndex, mIndex));
                if (gain == "low")  c[sIndex][mIndex]->Print(Form("update0617/amp:T0Corr_%d%d_mult_3sigma_LG_slew_6M.png", sIndex, mIndex));
            }
            else {
                if (gain == "high") c[sIndex][mIndex]->Print(Form("update0617/amp:T0Corr_%d%d_mult_3sigma_HG_6M.png", sIndex, mIndex));
                if (gain == "low")  c[sIndex][mIndex]->Print(Form("update0617/amp:T0Corr_%d%d_mult_3sigma_LG_6M.png", sIndex, mIndex));
            }
            //--------------------------------

            c[sIndex][mIndex]->Close();
            gSystem->ProcessEvents();
            delete c[sIndex][mIndex];
            delete hist;
            delete upper;
            delete lower;
            delete mean;
            delete ratio;
            delete cutup;
            delete cutlo;
            c[sIndex][mIndex] = 0;
        }
    }

    return;
}
