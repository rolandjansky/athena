/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ____________________________________________________________________________

  merge root files from several (grid) runs
  dedicated treatment of profiles and efficiencies
  Author: Liza Mijovic
   ____________________________________________________________________________
*/
#define RLN cout << "l" << __LINE__ << ": ";
// 0, 1 or 2 in increasing order of verbosity
#define PRINTDBG 2

#include <iostream>
#include <iterator>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "TFile.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"
#include "TROOT.h"
#include "TKey.h"
#include "TClass.h"
#include "TObject.h"
#include "TObjString.h"

using namespace std;

namespace po = boost::program_options;

bool file_exists(const string name) {
  return (gSystem->AccessPathName(name.c_str(), kFileExists))?
    false : true;    
}

void process_histos(string p_infile, string p_ofile) {
  if (PRINTDBG>0)
    cout << "merging file " << p_infile << "into "<< p_ofile << endl;
  return;
}

int main(int ac, char* av[]) {
  
  vector<string> infiles;
  string ofile="";
  
  try {
    po::options_description desc("Allowed arguments: ");
    desc.add_options()
      ("h", "print help")
      ("o", po::value<string>(), "output root file 'summ.root'")
      ("i", po::value<string>(), "inputs 'file1.root,file2.root,file3.root'")
      ;
    
    po::variables_map vm;        
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    
    
    if (vm.count("h")) {

      cout << "\n" << desc << "\n" << endl;
      const char* ex_compile =  "g++ -O2 -Wall -fPIC -std=c++11"
	" $(root-config --cflags) -o ${NAME} ${NAME}.cxx "
	" $(root-config --libs) -lboost_program_options";
      const char* ex_run = "./postprocessHistos_updt --i \"summ.root\" "
	"--o \"file1.root,file2.root,file3.root\" ";
      cout << "\n------- compile -------" << endl;
      cout << "NAME=postprocessHistos_updt" << endl;
      cout << ex_compile << endl;
      cout << "\n------- run -------" << endl;
      cout << ex_run << endl;
      cout << "\n";
      return 0;
    }
    
    if (vm.count("o")) {
      ofile=vm["o"].as<string>();
      cout << "Output file set to "<< ofile << endl;
    }
    
    if (vm.count("i")) {
      string infiles_cl=vm["i"].as<string>();
      boost::split(infiles , infiles_cl, boost::is_any_of(","));
      cout << "Summing input files ";
      for (auto inf : infiles)
       	cout << inf << " " ;
      cout << endl;
    }

    if ("" == ofile) {
      cout << "Invalid arguments. No output file passed.";
      cout << "\n" << desc <<"\n" << endl;
      return 1;
    }
    
    if (infiles.empty()) {
      cout << "Invalid arguments. No inputs to summ passed.";
      cout << desc <<" \n " << endl;
      return 1;
    }
    
  }
  catch(exception& e) {
    cerr << __FILE__  << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    cerr << __FILE__  << "Exception of unknown type!\n";
    return 1;
  }

  if (file_exists(ofile)) {
    
  }
    
  for (auto infile : infiles) {
    if (infile == infiles[0]) {
      if (!file_exists(infile)) {
      	cout << __FILE__ << "error: input file does not exist: " << infile << endl;
	return 1;	
      }
      cout << "creating initial contents of " << ofile << " from " << infile << endl;
      gSystem->CopyFile(infile.c_str(),ofile.c_str());
    }
    else
      process_histos(ofile,infile);
  }
  return 0;
  
}

