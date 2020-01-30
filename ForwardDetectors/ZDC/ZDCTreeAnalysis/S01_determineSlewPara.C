// -------------------------------
// Make T0Corr vs Amp plots
// Macro will print out parameters
//   in terminal
// -------------------------------

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

void S01_determineSlewPara() {
  gStyle->SetOptStat(0);

  TChain *chain = new TChain("zdcTree");
  chain->Add("out18_FE_fixTaus_Slew.root");
  TCanvas* c[2][2][4];
  TH2D* hist = nullptr;
  char name[100];
  int  scale[2] = {1, 10};
  int  state[2] = {1,  3};  // determine if it's high gain or low gain after &

  for (int gIndex : {0, 1}) {   // 0: high gain, 1: low gain
    for (int sIndex : {0, 1}) {
      for (int mIndex : {0, 1, 2, 3}) {
        // if (sIndex == 0 && mIndex == 0) continue;

        sprintf(name, "c_g%d_s%d_m%d", gIndex, sIndex, mIndex);
        c[gIndex][sIndex][mIndex] = new TCanvas(name, name, 800, 600);

        sprintf(name, "h_g%d_s%d_m%d", gIndex, sIndex, mIndex);
        hist = new TH2D("hist", name, 120, -5, 7, 120, -6, 6);
        chain->Draw("zdc_T0Corr[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]:(zdc_Amp[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "] / " + TString::Itoa(scale[gIndex], 10) + " - 500)/100.>>hist", "(zdc_Status[" + TString::Itoa(sIndex, 10) + "][" + TString::Itoa(mIndex, 10) + "]&0xF07)==" + TString::Itoa(state[gIndex], 10), "goff");

        hist->GetXaxis()->SetTitle("Amplitude");
        hist->GetYaxis()->SetTitle("T0_{ corrected}");
        hist->Draw("colz");

        //--------------------------------
        // projection
        //
        sprintf(name, "PjX_g%d_s%d_m%d", gIndex, sIndex, mIndex);
        TProfile* profx = hist->ProfileX(name);
        profx->Draw("same");
        TF1* f1 = new TF1("f1", "pol3", -5, 5);
        profx->Fit("f1", "RQ");   // R: Use the range specified in the function range, Q: Quiet mode (minimum printing)
        //--------------------------------

        TLegend *leg = new TLegend(.6, .7, .88, .88);
        leg->SetTextSize(.03);
        leg->SetBorderSize(0);
        leg->SetFillColor(0);
        leg->AddEntry("", Form("p0 = %e", f1->GetParameter(0)), "");
        leg->AddEntry("", Form("p1 = %e", f1->GetParameter(1)), "");
        leg->AddEntry("", Form("p2 = %e", f1->GetParameter(2)), "");
        leg->AddEntry("", Form("p3 = %e", f1->GetParameter(3)), "");
        leg->Draw("same");

        sprintf(name, "%e", f1->GetParameter(0));
        cout << name << ", ";
        sprintf(name, "%e", f1->GetParameter(1));
        cout << name << ", ";
        sprintf(name, "%e", f1->GetParameter(2));
        cout << name << ", ";
        sprintf(name, "%e", f1->GetParameter(3));
        cout << name << endl;

        gPad->SetLogz(1);
        gPad->SetTicks(1, 1);

        //--------------------------------
        // Save canvas
        //
        sprintf(name, "T0CorrToAmp/T0Corr:amp_g%d_s%d_m%d.png", gIndex, sIndex, mIndex);
        c[gIndex][sIndex][mIndex]->Print(name);
        //--------------------------------

        c[gIndex][sIndex][mIndex]->Close();
        gSystem->ProcessEvents();
        delete c[gIndex][sIndex][mIndex];
        delete hist;
        delete f1;
        delete profx;
        c[gIndex][sIndex][mIndex] = 0;
      }
    }
  }
}
