//
//   @file    reader.cxx         
//   
//
//   @author M.Sutton
// 
//   Copyright (C) 2012 M.Sutton (sutt@cern.ch)    
//
//   $Id: reader.cxx, v0.0   Mon 30 Jan 2012 18:43:21 CET sutt $



#include <stdlib.h>

#include <iostream>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "TrigInDetAnalysis/TIDAEvent.h"

#include "utils.h"


int usage(std::ostream& s, int argc, char** argv) {
  if ( argc>0 ) { 
    s << "Usage: " << argv[0] << " [OPTIONS] [-r] <input1.root input2.root ...>\n\n";
    s << "Configuration: \n";
    s << "Options: \n";
    s << "    -r,  --release  \t show the release data\n";
    s << "    -ro, --releaseonly \t show the release data and then quit\n";
    s << "    -h, --help     \t this help\n";
    //   s << "\nReport bugs to <" << PACKAGE_BUGREPORT << ">";
    s << std::endl;
  }
  return 0;
}



//extern bool TIDAprinttracks;

int main(int argc, char** argv) { 

  bool show_release = false;
  bool quit_after_release = false;

  std::vector<std::string> files;
  
  if ( argc<2 ) { 
    //    std::cerr << "usage: reader [-v] <filename>" << std::endl;
    usage( std::cerr, argc, argv );
    exit(-1);
  }
  else { 
    for ( int i=1 ; i<argc ; i++ ) { 
      std::string argi = std::string(argv[i]);
      if      ( argi == "-r"  || argi == "--release" ) show_release = true;
      if      ( argi == "-ro" || argi == "--releaseonly" ) show_release = quit_after_release = true;
      else if ( argi == "-h"  || argi == "--help" )    return usage( std::cout, argc, argv );
      else    files.push_back( argi );
    }
  }


  for ( unsigned i=0 ; i<files.size() ; i++ ) {
    
    TFile* _finput = TFile::Open( files[i].c_str() );

    if ( _finput==0 || !_finput->IsOpen() || _finput->IsZombie() ) {
      std::cerr << "Error: could not open output file" << std::endl;
      exit(-1);
    }

    TFile&  finput = *_finput;
  
    if ( show_release || quit_after_release ) { 
      TTree*  dataTree = (TTree*)finput.Get("dataTree");
      TString* releaseData = new TString("");
      
      if ( dataTree ) { 
	dataTree->SetBranchAddress( "ReleaseMetaData", &releaseData);
	
	for (unsigned int i=0; i<dataTree->GetEntries() ; i++ ) {
	  dataTree->GetEntry(i);      
	  std::cout << "releaseData: " << *releaseData << std::endl;
	}
      }
      else { 
	std::cerr << "no release data" << std::endl;
      }
      if ( quit_after_release ) return 0;
    }      


    ///   TChain* data = new TChain("tree");
    /// opening each with a TTree is faster than a chain
    TTree* data = (TTree*)finput.Get("tree");
    
    TIDA::Event* track_ev = new TIDA::Event();
    data->SetBranchAddress("TIDA::Event",&track_ev);
    //    data->AddFile( argv[i] );
    
    for (unsigned int i=0; i<data->GetEntries() ; i++ ) {
      data->GetEntry(i);      
      std::cout << *track_ev << std::endl;
    }
    
    finput.Close();
    
  }
    
  return 0;
}



