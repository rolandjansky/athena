#include <fstream.h>
#include <sstream>
#include <iomanip.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <TFile.h>
#include <TNtuple.h>
#include <TH1F.h>
using namespace std;

int main(int argc, char *argv[]){

  TFile* ntfile=new TFile(argv[1]);
  TNtuple* hittuple=(TNtuple*)ntfile->Get("ntuple");
  //cout <<  hittuple->GetEntries() << endl;

  TH1F* timeres = new TH1F("test","test",-25,25,100);
  hittuple->Project("test","t-ttrack-t0","t-ttrack-t0>-25 && t-ttrack-t0<25");
  //hittuple->Draw("t-ttrack-t0>>test","t-ttrack-t0>-25 && t-ttrack-t0<25");

  cout << "NTENTRIES " << hittuple->GetEntries() << endl;
  cout << "TRESHITS " << (int)timeres->GetEntries() << endl;


  hittuple->Delete();
  ntfile->Close();
  
}
