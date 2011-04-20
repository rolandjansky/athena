/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include "TH1.h"
#include "TFile.h"
#include "TH2.h"
#include "../../MonteCarloReactUtils/MonteCarloReactUtils/BinnedEfficiency.h"
#include "../../MonteCarloReactUtils/MonteCarloReactUtils/EffInfo.h"
#include <vector>
#include <string>

using namespace MonteCarloReact;
using namespace std;

int main(int argc, char** argv) {


  // Process the command line.
  if (argc != 12 && argc != 13) {
    cerr << "Usage: " << argv[0] << " inputFile histoname DataType EffName EffType EffVersion ObjQuality ObjType ObjVersion ObjAlgorithm EffVarNames" << endl;
    return 1;
  }
  char *inROOT = argv[1];

  // Try to open the ROOT tuple: issue an error if this is not possible.
  TFile *finROOT;
  try {
    finROOT = new TFile(inROOT,"READ");
  } catch (...) {
    std::cout << "Can not open the file " << inROOT << std::endl;
    return 1;
  }

  // figure out the histogram name
        // The names looks like hTrigEff_CALONLY_v15_pT_CC
  TString histName(argv[2]);
  cout << "Process histrogram " << histName << endl;
  // make a histogram containing the information
  TH2D * h = (TH2D *)finROOT -> Get(histName);
  // make an efficiency
  if (h) {
    
    BinnedEfficiency * beff = new BinnedEfficiency(h);
    //cout << *beff << endl;
    // create its descriptive data
    cout << "Try to create EffInfo " << endl;
    //EffInfo * info = new EffInfo();
    std::vector< std::string > vars;
    for(int i = 11; i < argc; i++) {
      vars.push_back(string(argv[i]));
    }    
    for(int i = 0; i < vars.size(); i++) cout << vars[i] << " ";
    cout << "DONE" << endl;
    
    cout << "Try to set EffInfo for BinnedEfficiency" << endl;
    cout << argv[3]<< " " <<  argv[5] << " " <<  argv[4] << " " << atoi(argv[6]) << " " << atoi(argv[9]) << " " << 
      argv[8] << " " <<  argv[7] << " " << argv[10] << endl;
    int effver =  atoi(argv[6]);
    int objver =  atoi(argv[9]);
    
    EffInfo *info = new EffInfo(string(argv[3]), string(argv[5]), string(argv[4]), 
                                 vars, effver, objver,
                                 string(argv[8]), string(argv[7]), string(argv[10]));
    //cout << "Created EffInfo " << beff << endl;
    /*
    EffInfo info;
    //cout << info.m_map.size() << endl;
    cout << "Try to set EffInfo Parameters" << endl;
    info.setDataType(argv[3]);
    cout << "Try to set EffName" << endl;
    info.setEffName(argv[4]);
    cout << "Try to set EffType" << endl;
    info.setEffType(argv[5]);
    cout << "Try to set EffVersion" << endl;
    info.setEffVersion(atoi(argv[6]));
    info.setObjQuality(argv[7]);
    info.setObjType(argv[8]);
    info.setObjVersion(atoi(argv[9]));
    info.setObjAlgorithm(argv[10]);
    cout << "Set Variable Names " << endl;
    //std::vector< std::string > vars;
    */
    
    
    // set the info in the efficiency
    //EffInfo * info = new EffInfo();
    //cout << info<< endl;
    //beff->getInfo->setInfo( info.getInfo() );
    beff->setInfo(*info);
    //cout << "Info " <<  *info << endl;
    // now here is how to write the efficiency to a file
    // 1) generate a filename
    cout << "Try to create EffInfo from BinnedEfficiency" << endl;
    
    const EffInfo & spec = beff->getInfo(); // get the info back!
    // Efficiency makes a copy of the info so we can kill the pointer now
    //delete info;
    cout << "Try to create FileName from EffInfo" << endl;
    //std::string fname = "testfile.mcr";
    std::string fname = spec.makeFileName();
    // 2) stream it out to the file
    cout << "Try to stream it" << endl;
    std::ofstream f(fname.c_str());
    cout << "Stream it now" << endl;
    //f << *info;
    f << *beff;
    // done
    return 0;
  } else {
    cerr << "Could not open the histogram." << endl;
    return 1;
  }
}
