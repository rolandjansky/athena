/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Work/JESUncertaintyR17/JetUncertainties/StandAlone/
//g++ -o TestJESUnc.exe TestJESUnc.C `$ROOTSYS/bin/root-config --cflags --glibs` -I../JetUncertainties -L../StandAlone -lJetUncertainties

#include "JESUncertaintyProvider.h"
#include "TFile.h"
#include "TH1D.h"
#include "TSystem.h"
#include "TGraphErrors.h"
#include <vector>

using namespace std;

int main () {
  
  std::cout << "Will load" << std::endl;
  gSystem->Load("../StandAlone/libJetUncertainties.so");
  std::cout << "did load" << std::endl;
  
  TString JetSizes[2] = {"4", "6"};
  TString JetCalibs[2] = {"EMJES", "LCJES"};
  TString JetCalibNames[2] = {"EM", "LC"};
  TString MCNames[1] = {"MC11b"};
  
  for (unsigned int iMC = 0 ; iMC < 1; iMC++) {
    for (unsigned int iJetSize = 0; iJetSize < 2; iJetSize++)  { 
      for (unsigned int iJetCalib = 0; iJetCalib < 2; iJetCalib++)  { 
	
	cout << "Hey" << endl;
        JESUncertaintyProvider *j = new JESUncertaintyProvider("InsituJES2011_MCCorrelations.config","AntiKt"+JetSizes[iJetSize]+"Topo"+JetCalibNames[iJetCalib], MCNames[iMC]);
	j->useGeV();
	TString calibType= JetCalibs[iJetCalib];
	TString jetSize = JetSizes[iJetSize];

        std::cout << "Uncertainty for jet of 20 GeV at eta = 1.0" << std::endl;
        std::cout << "\t" << j->getRelUncert(20,1.0) << std::endl; // pt, eta, mu, NPV (total uncertainty)
        
        //tests here
      }
    }
  }
}
