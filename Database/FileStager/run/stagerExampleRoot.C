/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <stdio.h>
#include <iostream>

//#include "Root/TCopyChain.h"
//#include "Root/TCopyFile.h"
//#include "Root/TStageManager.h"

#include <TROOT.h>
#include <TPluginManager.h>
//#include <TChain.h>
#include <TTree.h>
#include <string>
#include <sstream>
#include <fstream>
#include <list>

using namespace std;


void stagerExampleRoot() 
{
}

void
rootinit(void)
{
  // Load the filestager dictionary
  gSystem->Load("../$CMTCONFIG/libFileStagerLib.so");

  // make sure ROOT picks up TCopyFile when filename starts with gridcopy://
  gROOT->GetPluginManager()->AddHandler("TFile", "^gridcopy:", "TCopyFile","TCopyFile_cxx",
                                        "TCopyFile(const char*,Option_t*,const char*,Int_t)");
}


void
Example(const char* sample, bool doStaging=true, const char* treeName="CollectionTree" )
{
  // load the filestager library, configure root
  rootinit();


  /*
  // EXAMPLE: LCG-CP
  if (doStaging) {
    // turn on staging functionality
    TCopyChain::SetOriginalTChain(false);
    TCopyFile::SetOriginalTFile(false);
  
    // stager settings
    TStageManager& manager = TStageManager::instance();
    manager.setInfilePrefix("gridcopy://");
    manager.setCpCommand("wrapper_lcg-cp");
    //by default manager stores in $TMPDIR, or else /tmp ifndef
    //manager.setBaseTmpdir("/tmpdir");
    //manager.setPipeLength(1);

    //// useful for interactive running
    //// this option keeps stages files, and checks for files from previous stage.
    //manager.setInteractive();

    // turn on verbosity
    if (kFALSE) manager.verbose();     // lots of output
    if (kTRUE)  manager.verboseWait(); // useful to see only if your next has not yet finished staging

    // useful for interactive running
    // this option keeps stages files, and checks for files from previous stage.
    //manager.setInteractive();
  }
  */

  //EXAMPLE: RFIO
  if (doStaging) {
    // turn on staging functionality
    TCopyChain::SetOriginalTChain(false);
    TCopyFile::SetOriginalTFile(false);
  
    // stager settings
    TStageManager& manager = TStageManager::instance();
    manager.setInfilePrefix("gridcopy://");
    manager.setCpCommand("rfcp");
    //by default manager stores in $TMPDIR, or else /tmp ifndef
    //manager.setBaseTmpdir("/tmpdir");
    //manager.setPipeLength(1);

    // turn on verbosity
    if (kFALSE) manager.verbose();     // lots of output
    if (kTRUE)  manager.verboseWait(); // useful to see only if your next has not yet finished staging

    // useful for interactive running
    // this option keeps stages files, and checks for files from previous stage.
    //manager.setInteractive();
  }


  // --- Find and open sample definition file ---  
  TString defFileName(sample) ;
  ifstream ifs(defFileName) ;
  if (!ifs) {
    cout << "Error opening sample definition file <" << defFileName << ">" << endl ;
    exit(2);
    return 0;
  }

  TCopyChain *ntupleChain = new TCopyChain(treeName);

  char buf[1024];

  while (!ifs.eof()) {
    ifs.getline(buf,sizeof(buf),'\n');

    if (strncmp(buf,"#",1)==0) continue;
    if (strncmp(buf,"//",2)==0) continue;
    string bufstr(buf);
    if (bufstr.empty()) continue;

    if (bufstr.find("TITLE")!=std::string::npos) continue;
    if (bufstr.find("FLAGS")!=std::string::npos) continue;

    cout << "Adding file <" << bufstr << ">" << endl ;
    ntupleChain->Add(buf) ;
  }

  cout << "Done adding samples.\n" << endl;



  // Loop over chain
  Long64_t nentries = ntupleChain->GetEntriesFast();
  Long64_t jentry=0;  
  Int_t nb=0;  

  while(jentry<nentries) {

     Int_t ientry = ntupleChain->LoadTree(jentry); //in case of a TChain, ientry is the entry number in the current file
     Bool_t newTree = (ientry==0) ;     
     if (ientry < 0) break;

     if (jentry % 1000 == 0) {
       cout << "Now processing event " << jentry << endl ;    
     }

/*   // use this to retrieve events.
     int size = ntupleChain->GetEntry(jentry) ;
     if (size > 0) {
       nb += size;
     } else {
       cout << "Framework: end of tree reached, stopping event processing loop" << endl ;
       break;
     }
*/
     ++jentry;
  }
}
