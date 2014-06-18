/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TFile.h>
#include <TTree.h>

#include <iostream>
using namespace std;

int main(int nargc, char *argv[]) {
  TFile *bankfile = TFile::Open(argv[1]);

  TTree *bank0 = dynamic_cast<TTree*>(bankfile->Get("Bank0"));
  if (!bank0) return -1;
  TTree *tsprel = dynamic_cast<TTree*>(bankfile->Get("TSPRelation0"));
  if (!tsprel) return -1;
  TTree *bank1 = dynamic_cast<TTree*>(bankfile->Get("Bank1"));
  if (!bank1) return -1;

  if (bank0->GetEntries()!=tsprel->GetEntries())
    return -1;

  int AMid;
  tsprel->SetBranchAddress("AMID",&AMid);

  TFile *outfile = TFile::Open(argv[2],"recreate");
  TTree *amsize = new TTree("amsize","TSP vs AM");
  Int_t maxAMid(-1);
  Long64_t ipatt;
  amsize->Branch("NTSP",&ipatt,"NTSP/L");
  amsize->Branch("NAM",&maxAMid,"NAM/I");

  Long64_t ipatt_step(50000);

  Long64_t ntsppatt = bank0->GetEntries();
  for (ipatt=0;ipatt!=ntsppatt;++ipatt) { // pattern loop
    tsprel->GetEntry(ipatt);
    
    if (AMid>maxAMid) {
      maxAMid = AMid;
    }

    if (ipatt%ipatt_step==0) {
      //cout << ipatt << '\t' << maxAMid << endl;
      amsize->Fill();
    }
  } // end pattern loop

  outfile->Write();
  outfile->Close();
  return 0;
}
