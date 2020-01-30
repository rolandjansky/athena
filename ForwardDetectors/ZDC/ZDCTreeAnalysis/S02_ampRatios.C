#include <iostream>
#include <vector>
#include <utility>
#include <TFile.h>
#include <string>
#include <TGraph.h>

#include "TCanvas.h"
#include "TChain.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TString.h"
#include "TF1.h"

using namespace std;

void S02_ampRatios() {
  // gStyle->SetOptStat(0);
  // ---------------------------------------------------------------------------------------
  // Setup tree
  //
  TChain* chain;
  chain = new TChain("zdcTree");
  chain->Add("out16_GP_6M_FixTaus_noSlew_163.root");
  int nEntries = chain->GetEntries();

  Bool_t L1_ZDC_C;            chain->SetBranchAddress("L1_ZDC_C"      , &L1_ZDC_C      );
  int   zdc_ModuleMask;       chain->SetBranchAddress("zdc_ModuleMask", &zdc_ModuleMask);
  float zdc_SumAmp[2];        chain->SetBranchAddress("zdc_SumAmp"    , &zdc_SumAmp    );
  int   zdc_Status[2][4];     chain->SetBranchAddress("zdc_Status"    , &zdc_Status    );
  float zdc_Amp[2][4];        chain->SetBranchAddress("zdc_Amp"       , &zdc_Amp       );
  float zdc_FitT0[2][4];      chain->SetBranchAddress("zdc_FitT0"     , &zdc_FitT0     );
  float zdc_FitPreAmp[2][4];  chain->SetBranchAddress("zdc_FitPreAmp" , &zdc_FitPreAmp );
  float zdc_FitPreT0[2][4];   chain->SetBranchAddress("zdc_FitPreT0"  , &zdc_FitPreT0  );
  float zdc_FitPostAmp[2][4]; chain->SetBranchAddress("zdc_FitPostAmp", &zdc_FitPostAmp);
  float zdc_FitPostT0[2][4];  chain->SetBranchAddress("zdc_FitPostT0" , &zdc_FitPostT0 );
  float zdc_FitExpAmp[2][4];  chain->SetBranchAddress("zdc_FitExpAmp" , &zdc_FitExpAmp );
  float zdc_FitTau1[2][4];    chain->SetBranchAddress("zdc_FitTau1"   , &zdc_FitTau1   );
  float zdc_FitTau2[2][4];    chain->SetBranchAddress("zdc_FitTau2"   , &zdc_FitTau2   );
  float zdc_AmpError[2][4];   chain->SetBranchAddress("zdc_AmpError"  , &zdc_AmpError  );
  // ---------------------------------------------------------------------------------------

  // ---------------------------------------------------------------------------------------
  // initialize histograms and canvas
  //
  char name[100];
  TH2F* r_ampToExpAmp[4];
  TH2F* r_ampToDeltaExp[4];
  TH2F* r_ampToPreAmp[4];
  TH2F* r_ampToPostAmp[4];
  TH2F* r_ampToAmpErr[4];
  TH2F* r_ampToAll[4];
  for (int mIndex : {0, 1, 2, 3}) {
    sprintf(name, "r_ampToExpAmp_s1_m%d"  , mIndex); r_ampToExpAmp[mIndex]   = new TH2F(name, name, 50, 0, 100, 50, 0, 1   );
    sprintf(name, "r_ampToDeltaExp_s1_m%d", mIndex); r_ampToDeltaExp[mIndex] = new TH2F(name, name, 50, 0, 100, 50, 0, 10  );
    sprintf(name, "r_ampToPreAmp_s1_m%d"  , mIndex); r_ampToPreAmp[mIndex]   = new TH2F(name, name, 50, 0, 100, 50, 0, 5   );
    sprintf(name, "r_ampToPostAmp_s1_m%d" , mIndex); r_ampToPostAmp[mIndex]  = new TH2F(name, name, 50, 0, 500, 50, 0, 50  );
    sprintf(name, "r_ampToAmpErr_s1_m%d"  , mIndex); r_ampToAmpErr[mIndex]   = new TH2F(name, name, 50, 0, 500, 50, 0, 1000);
    sprintf(name, "r_ampToAll_s1_m%d"     , mIndex); r_ampToAll[mIndex]      = new TH2F(name, name, 50, 0, 50 , 50, 0, 1   );
  }
  sprintf(name, "h_sumRatio");
  TH2F* h_sumRatio = new TH2F(name, name, 100, 0, 1000, 500, 0, 200);
  // TH2F* h_sumRatio = new TH2F(name, name, 100, 0, 1000, 40, 0,400);
  sprintf(name, "h_sumAmp");
  TH1F* h_sumAmp   = new TH1F(name, name, 100, 0, 1000);
  // ---------------------------------------------------------------------------------------


  // for (int i = 0; i < nEntries; i++) {
  for (int i = 0; i < 100000; i++) {
    chain->GetEntry(i);

    if (i >= 1000) {
      float log10Entry = floor(log ((float) i) / log(10));
      int   modEntry   = i % (int) pow(10, log10Entry);
      if (modEntry == 0) cout << "Processed event " << i << endl;
    }

    bool  isGood = false;
    float sumAmp = 0;
    int   countMod = 0;
    bool  ifIncludeSum = true;
    float sumRatio = 0;
    float maxRatio = 0;
    for (int mIndex : {0, 1, 2, 3}) {
      if (!L1_ZDC_C) continue;
      // if (mIndex != 2) continue;  // test

      // ---------------------------------------------------------------------------------------
      // get status bits
      //
      bool havePulse    = (zdc_Status[1][mIndex] & 0x1   ) == 0x1   ; if (!havePulse   ) continue;
      bool lowGain      = (zdc_Status[1][mIndex] & 0x2   ) == 0x2   ;
      bool prePulse     = (zdc_Status[1][mIndex] & 0x100 ) == 0x100 ; // if (!prePulse    ) continue;
      bool postPulse    = (zdc_Status[1][mIndex] & 0x200 ) == 0x200 ;
      bool fitFail      = (zdc_Status[1][mIndex] & 0x400 ) == 0x400 ; if ( fitFail     ) continue;
      bool badChisq     = (zdc_Status[1][mIndex] & 0x800 ) == 0x800 ; if ( badChisq    ) continue;
      bool badT0        = (zdc_Status[1][mIndex] & 0x1000) == 0x1000; if ( badT0       ) continue;
      bool excluEarlyLB = (zdc_Status[1][mIndex] & 0x2000) == 0x2000; //if ( excluEarlyLB) {ifIncludeSum = false; continue;}
      bool excluLateLB  = (zdc_Status[1][mIndex] & 0x4000) == 0x4000;
      bool preExpTail   = (zdc_Status[1][mIndex] & 0x8000) == 0x8000;
      // ---------------------------------------------------------------------------------------

      // ---------------------------------------------------------------------------------------
      // calculate deltaExp
      //
      float leftExp  = zdc_FitExpAmp[1][mIndex] * exp(-(zdc_FitT0[1][mIndex] - 12.5) / zdc_FitTau2[1][mIndex]);
      float rightExp = zdc_FitExpAmp[1][mIndex] * exp(-(zdc_FitT0[1][mIndex] + 12.5) / zdc_FitTau2[1][mIndex]);
      float deltaExp = fabs(leftExp - rightExp);
      // ---------------------------------------------------------------------------------------

      // ---------------------------------------------------------------------------------------
      // calculate SumAmp_
      //
      // if (!lowGain) {
      //   // if (preExpTail && ) {
      //   //   sumAmp += zdc_Amp[1][mIndex]; countMod++;
      //   // }
      //   if (!preExpTail) {sumAmp += zdc_Amp[1][mIndex]; countMod++;}
      //   else {
      //     if (!excluEarlyLB && zdc_Amp[1][mIndex] / deltaExp > zdc_Amp[1][mIndex] * 3. / 50. + 1) {sumAmp += zdc_Amp[1][mIndex]; countMod++;}
      //   }
      // }
      // if (lowGain) {sumAmp += zdc_Amp[1][mIndex]; countMod++;}

      // -------------------------------------------------------
      // no lower limit and count without M3, require M1 fire
      //
      // if (!lowGain && zdc_Amp[1][mIndex] > 5 ) {
      //   sumAmp += zdc_Amp[1][mIndex];
      //   if (mIndex <  3) countMod++;
      //   if (mIndex == 1) isGood = true;
      // }
      // if ( lowGain && zdc_Amp[1][mIndex] > 50) {
      //   sumAmp += zdc_Amp[1][mIndex];
      //   if (mIndex <  3) countMod++;
      //   if (mIndex == 1) isGood = true;
      // }
      // -------------------------------------------------------

      sumAmp += zdc_Amp[1][mIndex]; countMod++;
      // if (mIndex < 3) countMod++;
      // if (mIndex == 1) isGood = true;
      // ---------------------------------------------------------------------------------------

      // ---------------------------------------------------------------------------------------
      // Fill histograms
      //
      if (!lowGain) {
        if (preExpTail && !excluEarlyLB) {
          r_ampToDeltaExp[mIndex]->Fill(zdc_Amp[1][mIndex], zdc_Amp[1][mIndex] / deltaExp);
          r_ampToExpAmp  [mIndex]->Fill(zdc_Amp[1][mIndex], zdc_Amp[1][mIndex] / zdc_FitExpAmp[1][mIndex]);
          // if (zdc_Amp[1][mIndex] > 400 && zdc_Amp[1][mIndex] < 500 && zdc_Amp[1][mIndex] / deltaExp < 40) cout << i << "    zdc_Amp[1][mIndex] = " << zdc_Amp[1][mIndex] << "    deltaExp = " << deltaExp << endl;
        }
        if (prePulse && !excluEarlyLB) {
          r_ampToPreAmp  [mIndex]->Fill(zdc_Amp[1][mIndex], zdc_Amp[1][mIndex] / zdc_FitPreAmp [1][mIndex]);
          // if (zdc_Amp[1][mIndex] > 400 && zdc_Amp[1][mIndex] < 500 && zdc_Amp[1][mIndex] / zdc_FitPreAmp [1][mIndex] < 5) cout << i << "    zdc_Amp[1][mIndex] = " << zdc_Amp[1][mIndex] << "    zdc_FitPreAmp [1][mIndex] = " << zdc_FitPreAmp [1][mIndex] << endl;
        }
        r_ampToPostAmp [mIndex]->Fill(zdc_Amp[1][mIndex], zdc_Amp[1][mIndex] / zdc_FitPostAmp[1][mIndex]);
        r_ampToAmpErr  [mIndex]->Fill(zdc_Amp[1][mIndex], zdc_Amp[1][mIndex] / zdc_AmpError  [1][mIndex]);

        float sum = zdc_AmpError  [1][mIndex];
        if (preExpTail) sum += deltaExp;
        if (prePulse  && !excluEarlyLB) sum += zdc_FitPreAmp [1][mIndex];
        if (postPulse && zdc_FitPostAmp[1][mIndex] > 0) sum += zdc_FitPostAmp[1][mIndex];

        r_ampToAll     [mIndex]->Fill(zdc_Amp[1][mIndex], zdc_Amp[1][mIndex] / sum);
        sumRatio += zdc_Amp[1][mIndex] / sum;
        if (zdc_Amp[1][mIndex] / sum > maxRatio) maxRatio = zdc_Amp[1][mIndex] / sum;

        // cout << i << "   zdc_Amp[1][mIndex] / sum = " << zdc_Amp[1][mIndex] / sum << endl;
      }
      if (lowGain) {
        float sum = zdc_AmpError  [1][mIndex];
        if (preExpTail) sum += deltaExp;
        if (prePulse  && !excluEarlyLB) sum += zdc_FitPreAmp [1][mIndex];
        if (postPulse && zdc_FitPostAmp[1][mIndex] > 0) sum += zdc_FitPostAmp[1][mIndex];

        r_ampToAll     [mIndex]->Fill(zdc_Amp[1][mIndex], zdc_Amp[1][mIndex] / sum);
        sumRatio += zdc_Amp[1][mIndex] / sum;
        if (zdc_Amp[1][mIndex] / sum > maxRatio) maxRatio = zdc_Amp[1][mIndex] / sum;
      }
      // ---------------------------------------------------------------------------------------
    }
    // if (isGood && countMod > 1) h_sumAmp->Fill(sumAmp);
    if (countMod > 1 && ifIncludeSum) {h_sumAmp->Fill(sumAmp); h_sumRatio->Fill(sumAmp, sumRatio);} //cout << i << "   sumRatio = " << sumRatio << endl;
  }

  // ---------------------------------------------------------------------------------------
  // save plots
  //
  TCanvas* c[5][4];
  for (int mIndex : {0, 1, 2, 3}) {
    // sprintf(name, "c0_m%d", mIndex); c[0][mIndex] = new TCanvas(name, "", 700, 500); r_ampToExpAmp[mIndex]->Draw("colz"); gPad->SetLogz(1); gPad->SetTicks(1, 1);
    // r_ampToExpAmp[mIndex]->GetXaxis()->SetTitle("Amplitude"); r_ampToExpAmp[mIndex]->GetYaxis()->SetTitle("amp / amp_{exp}");
    // sprintf(name, "ampRatioPlots/ampToExpAmp_zoom_1%d.png", mIndex); c[0][mIndex]->Print(name);

    // sprintf(name, "c0_m%d", mIndex); c[0][mIndex] = new TCanvas(name, "", 700, 500); r_ampToDeltaExp[mIndex]->Draw("colz"); gPad->SetLogz(1); gPad->SetTicks(1, 1);
    // r_ampToDeltaExp[mIndex]->GetXaxis()->SetTitle("Amplitude"); r_ampToDeltaExp[mIndex]->GetYaxis()->SetTitle("amp / DExp");
    // sprintf(name, "ampRatioPlots/ampToDeltaExp_onlyExpTail_noExcluEarlyLBCase_zoom_1%d.png", mIndex); c[0][mIndex]->Print(name);

    // sprintf(name, "c1_m%d", mIndex); c[1][mIndex] = new TCanvas(name, "", 700, 500); r_ampToPreAmp[mIndex]  ->Draw("colz"); gPad->SetLogz(1); gPad->SetTicks(1, 1);
    // r_ampToPreAmp[mIndex]  ->GetXaxis()->SetTitle("Amplitude"); r_ampToPreAmp[mIndex]  ->GetYaxis()->SetTitle("amp / Amp_{pre}");
    // sprintf(name, "ampRatioPlots/ampToPreAmp_zoom_1%d.png", mIndex);   c[1][mIndex]->Print(name);

    // sprintf(name, "c2_m%d", mIndex); c[2][mIndex] = new TCanvas(name, "", 700, 500); r_ampToPostAmp[mIndex] ->Draw("colz"); gPad->SetLogz(1); gPad->SetTicks(1, 1);
    // r_ampToPostAmp[mIndex] ->GetXaxis()->SetTitle("Amplitude"); r_ampToPostAmp[mIndex] ->GetYaxis()->SetTitle("amp / Amp_{post}");
    // sprintf(name, "ampRatioPlots/ampToPostAmp_1%d.png", mIndex);  c[2][mIndex]->Print(name);

    // sprintf(name, "c3_m%d", mIndex); c[3][mIndex] = new TCanvas(name, "", 700, 500); r_ampToAmpErr[mIndex]  ->Draw("colz"); gPad->SetLogz(1); gPad->SetTicks(1, 1);
    // r_ampToAmpErr[mIndex]  ->GetXaxis()->SetTitle("Amplitude"); r_ampToAmpErr[mIndex]  ->GetYaxis()->SetTitle("amp / Err_{amp}");
    // sprintf(name, "ampRatioPlots/ampToAmpErr_1%d.png", mIndex);   c[3][mIndex]->Print(name);

    // sprintf(name, "c4_m%d", mIndex); c[4][mIndex] = new TCanvas(name, "", 700, 500); r_ampToAll[mIndex]     ->Draw("colz"); gPad->SetLogz(1); gPad->SetTicks(1, 1);
    // r_ampToAll[mIndex]     ->GetXaxis()->SetTitle("Amplitude"); r_ampToAll[mIndex]     ->GetYaxis()->SetTitle("amp / all");
    // sprintf(name, "ampRatioPlots/ampToAll_zoom_1%d.png", mIndex);      c[4][mIndex]->Print(name);
  }

  // TCanvas* c_sumAmp = new TCanvas("c_sumAmp", "", 700, 500);
  // h_sumAmp->Draw(); gPad->SetLogz(1); gPad->SetTicks(1, 1);
  // sprintf(name, "ampRatioPlots/sumAmp_cutDeltaAmp_1.png"); c_sumAmp->Print(name);

  TCanvas* c_sumRatio = new TCanvas("c_sumRatio", "", 700, 500); h_sumRatio->Draw("colz"); gPad->SetLogz(1); gPad->SetTicks(1, 1);
  h_sumRatio->GetXaxis()->SetTitle("Amplitude"); h_sumRatio->GetYaxis()->SetTitle("sum of amp / all");
  sprintf(name, "ampRatioPlots/sumRatio_test_1.png");      c_sumRatio->Print(name);

  // TFile* outfile = TFile::Open("ampRatioPlots/Plots.root", "recreate");
  // h_sumAmp->Write();
  // outfile->Close();
  // ---------------------------------------------------------------------------------------
}


// ratio: quality sum
// all four  and   single
