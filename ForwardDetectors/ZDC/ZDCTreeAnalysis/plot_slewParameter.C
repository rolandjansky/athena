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
#include "TH2.h"
#include "TLegend.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"

#include "CountBits.C"

using namespace std;

void plot_slewParameter() {
    TChain *chain = new TChain("zdcTree");
    chain->Add("out16_GeneralPulse_6M_debugPrepulseT0.root");

    TH2D* hist[2][2][4] = nullptr;
    string status_cut[2] = {"0xF07)==1", "0xF07)==3"};

    char hname[100];
    for (int gain = 0; gain < 2; gain++) {
        // gain = 0  HG;    gain = 1  LG
        for (int side = 0; side < 2; side++) {
            for (int module = 0; module < 4; module++) {
                sprintf(hname, "hist_%d_%d_%d", gain, side, module);
                hist[gain][side][module] = new TH2D(hname,hname, 100, -5, 5, 100, -5, 5);
                chain->Draw("zdc_T0Corr[" + TString::Itoa(side, 10) + "][" + TString::Itoa(module, 10) + "]:(zdc_Amp[" + TString::Itoa(side, 10) + "][" + TString::Itoa(module, 10) + "]-500.)/100.>>" + hname, "(zdc_Status[" + TString::Itoa(side, 10) + "][" + TString::Itoa(module, 10) + "]" + status_cut[gain], "colz");
            }
        }
    }

    return;
}
