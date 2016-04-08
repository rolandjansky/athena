/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TChain.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include <string>
#include <iostream>
#include <TApplication.h> 

#include "MuonRecValidationNtuples/MuonInsideOutAnalysis.h"


int main( int argc, char *argv[] ) {
  
  TApplication theApp("App", &argc, argv);
     
  int data = 0;
  if( argc > 1 ) {
    TString str = argv[1];
    data = str.Atoi();
  }

  std::cout << " opening dataset " << data << std::endl;
  TChain* ntupleToRead = new TChain("data") ;
  if( data == 1 ) {
    ntupleToRead->Add("MuonRecoValidation_*.root"); 
  }else{
    ntupleToRead->Add("MuonRecoValidation.root"); 
  }
  TString outName = "MuonRecoAnalysis.root";
  TFile* output = new TFile(outName,"RECREATE");
  //std::cout << " creating analysis " << outName << std::endl;
  Muon::MuonInsideOutAnalysis analysis(output);
  analysis.analyse(*ntupleToRead);
  std::cout << " done, closing file " << std::endl;

  output->Write();
  output->Close();
}
 
