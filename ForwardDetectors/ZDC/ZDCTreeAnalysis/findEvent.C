#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
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

#include "CountBits.C"

using namespace std;

TChain *chain;

void init_chain()
{
    chain = new TChain("zdcTree");
    chain->Add("test_afterMerge0422_afterdebug0423.root");
}

// unsigned int countBits(int data)
// {
//   bitset<4> bs(data);
//   return bs.count();
// }

void findEvent()
{
    ofstream outFile1, outFile2;
    outFile1.open("eventOutput_2016_himu_aftermerge_0423.txt");
    outFile2.open("eventOutput_2016_himu_aftermerge_0423_array.txt");

    init_chain();

    float zdc_SumAmp[2];
    chain->SetBranchAddress("zdc_SumAmp", &zdc_SumAmp);
    int zdc_ModuleMask;
    chain->SetBranchAddress("zdc_ModuleMask", &zdc_ModuleMask);
    Bool_t L1_ZDC_C;
    chain->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C);

    int index = 1;

    for (int i=0; i<100000; i++)
    {
        chain->GetEntry(i);
        // int count = countBits(zdc_ModuleMask >> 4)
        bitset<4> bs(zdc_ModuleMask >> 4);
        int count = bs.count();
        if (count == 2 || count == 3)
        {
            if (index > 100) return;
            outFile1 << setw(3) << index << setw(6) << i << setw(6) << bs << endl;
            outFile2 << i << ", ";
            index++;
        }
    }

    // Anything come into the single neutron peak
    // for (int i=0; i<100000; i++)
    // {
    //     chain->GetEntry(i);
    //     bitset<4> bs(zdc_ModuleMask >> 4);
    //     if (CountBits(zdc_ModuleMask>>4)>1 && L1_ZDC_C && zdc_SumAmp[1] > 100 && zdc_SumAmp[1] < 300)
    //     {
    //         // if (index > 100) return;
    //         outFile1 << setw(3) << index << setw(6) << i << setw(6) << bs << endl;
    //         outFile2 << i << ", ";
    //         index++;
    //     }
    // }

    // for (int i=0; i<16; i++)
    // {
    //     bitset<4> bs(i);
    //     cout << "bit = " << i <<  "; count = " << countBits(i) << "   " << bs << endl;
    // }
}
