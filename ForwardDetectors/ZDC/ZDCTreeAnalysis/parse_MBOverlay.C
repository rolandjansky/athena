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
int colors[7] = {kRed, kBlue, kGreen + 2, kOrange + 1, kMagenta, kCyan + 1, kBlack};

void readInFile() {
    chain = new TChain("zdcTree");

    char name[100];
    for (unsigned int i = 1; i <= 21; i++)
    {
        sprintf(name, "data16_hip8TeV.00313295_MinBiasOverlay/user.steinber.18563677._0000%02d.ANALYSIS.root", i);
        ifstream inFile;
        inFile.open(name);

        //Skip the runs that does not exist
        if (!inFile) {
            cout << "---->File " <<  name << " does not exist!!!!!" << endl;
            continue;
        }
        else {
            chain->Add(TString(name));
            cout << "Adding " << name << " to chain " << endl;
        }

        inFile.close();
    }
}

void processData() {
    float zdc_ZdcEnergy[2];                 chain->SetBranchAddress("zdc_ZdcEnergy"                   , &zdc_ZdcEnergy                   );
    float fcalEtA;                          chain->SetBranchAddress("fcalEtA"                         , &fcalEtA                         );
    bool  HLT_mb_sptrk_L1MBTS_1_OVERLAY;    chain->SetBranchAddress("HLT_mb_sptrk_L1MBTS_1_OVERLAY"   , &HLT_mb_sptrk_L1MBTS_1_OVERLAY   );
    bool  ps_HLT_mb_sptrk_L1MBTS_1_OVERLAY; chain->SetBranchAddress("ps_HLT_mb_sptrk_L1MBTS_1_OVERLAY", &ps_HLT_mb_sptrk_L1MBTS_1_OVERLAY);

    TH2D* hist = new TH2D("hist", "hist", 100, -100, 400, 100, 0, 40000);
    for (int i=0; i<400000; i++) {
        chain->GetEntry(i);
        if (i > 0) {
            float log10Entry = floor(log10((float) i));
            int   modEntry   = i % ((int) pow(10.0, log10Entry));
            if (modEntry == 0) cout << "Processed event " << i << endl;
        }
        if (HLT_mb_sptrk_L1MBTS_1_OVERLAY) {
            hist->Fill(,,ps_HLT_mb_sptrk_L1MBTS_1_OVERLAY);
        }
    }
}

void parse_MBOverlay() {
    readInFile();
    processData();

    return;
}
