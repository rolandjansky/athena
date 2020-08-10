/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/LayerAnalysis.h"

#include "TChain.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include <string>
#include <iostream>
#include <fstream>
#include <TApplication.h> 

int main ATLAS_NOT_THREAD_SAFE(int argc, char* argv[]) {
  
  TApplication theApp("App", &argc, argv);

  //set the input text file
  TString filename = "input";
  if (argc <= 1){std::cout << " Default input!" << std::endl;} 
  else {filename = argv[1];} 
  std::cout<< " The input root file is in " << filename << std::endl;
  std::string line;
  std::vector<std::string> fileList;
  std::ifstream file(filename + ".txt", std::ifstream::in);
  if (file.is_open()) {
    while (!file.eof()) {
        getline(file, line);
        if (line.empty()) continue;
        if (line.find('#')!=std::string::npos) continue;
        fileList.push_back(line);
        std::cout << "  " << line << std::endl;
    }
  }
  else {
    std::cout << " Could not open input file!" << std::endl;
    return 0;
  }
  file.close();

  std::cout << " opening dataset " << filename << std::endl;
  TChain* ntupleToRead = new TChain("data") ;
  ntupleToRead->SetMakeClass(1);
  TString outName = "LayerAnalysis";
  TString postFix = filename;
  for (unsigned int i = 0; i < fileList.size(); i++) {
    ntupleToRead->Add(fileList.at(i).c_str());
  }
  outName += "_" + postFix;
  outName += ".root";
  TFile* output = new TFile(outName,"RECREATE");
  std::cout << " creating analysis " << outName << std::endl;
  MuonHough::LayerAnalysis analysis(*ntupleToRead);
  analysis.analyse();
  std::cout << " done, closing file " << std::endl;

  output->Write();
  output->Close();
}
