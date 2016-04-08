
#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TString.h"
#include "TChain.h"

#include <iostream>

//#include "FastSimulationNTUPToHist.h"
//#include "FastSimulationHistMaker.h"


using namespace std;

void FastSimulationMakePlots(TString inputFileName)
{
  if(inputFileName.EndsWith("af2.NTUP.root")){
    cout << "input file : " << inputFileName << endl;

    /*** first compile and load macros ***/
    cout << "### STARTING COMPILATION ###" << endl;
    gSystem->Load("FastSimulationNTUPToHist_C.so");
    //gROOT->LoadMacro("FastSimulationNTUPToHist.C++");
    //gSystem->Load("FastSimulationHistMaker_C.so");
    gROOT->LoadMacro("FastSimulationHistMaker.C++");
    cout << "### COMPILATION/LOADING DONE ###" << endl;
    
    TChain *chain = new TChain("physics");
    chain->Add(inputFileName);  
    
    FastSimulationHistMaker histMaker(chain);
    histMaker.Loop("outout.root",-1);
  }else{
    cerr << "input file is not NTUP.root file" << endl;
    return;
  }
}
