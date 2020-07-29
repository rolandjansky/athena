#include "Riostream.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"

using namespace std;

TChain *chain;

void init_chain()
{
    chain = new TChain("zdcTree");
    chain->Add("debug.root");
}

void parseDebug() {
    ofstream outFile1;
    outFile1.open("eventNumber_debug.txt");

    init_chain();

    int eventNumber;
    chain->SetBranchAddress("eventNumber", &eventNumber);

    int totEvt = chain->GetEntries();
    outFile1 << "if (";
    for (int i = 0; i < totEvt; i++)
    {
        chain->GetEntry(i);

        // outFile1 << setw(3) << index << setw(6) << i << setw(6) << bs << endl;
        outFile1 << "eventNumber == " << eventNumber << " || ";
    }
}
