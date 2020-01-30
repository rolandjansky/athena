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
int colors[10] = {kRed, kBlue, kGreen + 1, kCyan + 1, kYellow + 1, kOrange + 1, kAzure + 1, kMagenta, kGray, kBlack};

void init_chain()
{
    chain = new TChain("zdcTree");
    chain->Add("data16_hip8TeV_00313187_87z4/user.steinber.17047360._000001.ANALYSIS.root");
    chain->Add("data16_hip8TeV_00313187_87z4/user.steinber.17047360._000002.ANALYSIS.root");
}

// void findTrain()
// {
//     ofstream outFile1, outFile2, outFile3;
//     outFile1.open("trainInfo.txt");
//     outFile2.open("fbInfo.txt");
//     outFile3.open("trainNumInfo.txt");

//     init_chain();

//     UInt_t bcid;
//     chain->SetBranchAddress("bcid", &bcid);

//     TH1F *bunch_dhis = new TH1F("bunch_dhis", "bunch_dhis", 3599, 1, 3600);
//     chain->Draw("bcid>>bunch_dhis", "", "goff");

//     int count_zero = 999;
//     int trainNumCount = 0;

//     for (int i = 1; i <= 3600; i++)
//     {
//         int content = bunch_dhis->GetBinContent(i);
//         if (content < 10) {count_zero++;}
//         else if (count_zero > 30) {
//             count_zero = 0;
//             trainNumCount++;

//             if (i != 1 ) outFile1 << endl;
//             outFile1 << i << ", ";
//             outFile2 << i << ", ";
//             if (i != 1 ) outFile3 << trainNumCount << ", ";

//             trainNumCount = 0;
//         }
//         else {
//             count_zero = 0;
//             trainNumCount++;

//             outFile1 << i << ", ";
//         }
//         if (i==3600) {
//             outFile3 << trainNumCount;
//         }
//     }

//     outFile1.close();
//     outFile2.close();
//     outFile3.close();

//     return;
// }

void findTrain()
{
    init_chain();
    std::vector<int> _BCIDPosInTrain;
    _BCIDPosInTrain.assign(3300, 0);

    UInt_t bcid;
    chain->SetBranchAddress("bcid", &bcid);

    TH1F *bunch_dhis = new TH1F("bunch_dhis", "bunch_dhis", 3599, 1, 3600);
    chain->Draw("bcid>>bunch_dhis", "", "goff");

    int count_zero = 999;
    int numBCID = 0;
    int pos     = -1;

    for (int i = 1; i <= 3600; i++)
    {
        int content = bunch_dhis->GetBinContent(i);
        if (content < 10) {count_zero++;}
        else {

            if (count_zero > 30) {
                pos = -1;
            }

            pos++;
            _BCIDPosInTrain[i] = pos;
            count_zero = 0;

            if (i > 3300) {
                cout << "i = " << i << "    _BCIDPosInTrain[i] = " << _BCIDPosInTrain[i] << endl;
            }
        }
    }
    return;
}
