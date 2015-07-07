/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///getLumi - the lumi helper application
///author: Will Buttinger (will@cern.ch)
///Small program to quickly get luminosity out of files with LumiMetaData folder
///Uses a root file generated using lumicalc service (atlas-lumicalc.cern.ch)
///usage:
/// getLumi.exe lumicalc.root /path/to/some/files*.root
///  option: -m   shows which runs from the lumicalc file are not covered by the files you gave it
///               useful option for checking you have run over all data that your GRL covers



#include <iostream>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TApplication.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TSystem.h"

#include <getopt.h>
#include <cstdlib>

#include "GoodRunsLists/TGoodRunsListReader.h"

int main(int argc, char* argv[]) {
   const char* optstring = "m";
   bool showMissing = false;
static struct option long_options[] = 
  {
    /* name    has_arg       flag      val */
    {"showMissing", no_argument ,  NULL,     'm'}
 };

   int longindex;
  int option;
  //int begin =   0;
  //int end   = 999;
  int nOptions = 0;
  while ((option = getopt_long(argc, argv,
          optstring, long_options, &longindex)) != -1) {
      switch (option) {
          case 'm' :
            showMissing = true;nOptions++;
            break;
          case '?' : 
            printf("Please supply a valid option to the program.  Exiting\n");
            exit(1); 
            break;
      }
  }

   //first arg is the lumicalc file
   if(argc<3+nOptions) {
      std::cout << "Usage: <lumicalcfile.root> <list of D3PD>" << std::endl;
      return 0;
   }
   gSystem->Load("libTree.so");

   Root::TGoodRunsListReader grlReader;
   //open all the d3pd-like files, and hunt for their lumi strings
   for(int i=2+nOptions;i<argc;i++) {
      //require to have ".root" in it
      std::string s(argv[i]);
      if(s.find(".root")==std::string::npos) continue;
      TFile *file = TFile::Open(argv[i]);
      if(file==0) continue;
      std::cout << s;
      //try to get lumi
      if(file->GetDirectory("Lumi")==0) { std::cout  << "...no Lumi folder. Skipping" << std::endl; }
      else {
         file->cd("Lumi");
         //look for lumiTree first
         TTree* lumiTree = dynamic_cast<TTree*>(gDirectory->Get("lumiTree"));
         if(lumiTree) {
            std::cout << "...lumiTree";
            //use the lumitree instead
            TString *in = 0;
            lumiTree->SetBranchAddress("lumi",&in);
            for(int j=0;j<lumiTree->GetEntries();j++) {
               lumiTree->GetEntry(j);
               grlReader.AddXMLString(*in);
            }
            std::cout << "...ok" << std::endl;
         } else {
            TList* keys = gDirectory->GetListOfKeys();
	    //loop over all that are objstrings
	    for( Int_t j = 0; j < keys->GetEntries(); ++j ) {
	      TKey *key = dynamic_cast<TKey*>( keys->At( j ) );
              if( ! key ) { std::cout << "..skipping object: " << keys->At(j)->GetName() << std::endl; continue; }
	      TObjString* ostring = (TObjString*) key->ReadObjectAny( TObjString::Class() );
	      if( ! ostring ) { std::cout << "...skipping object: " << key->GetName() << std::endl; continue;}
	      // Check that it has the right format:
	      if( ostring->GetString().BeginsWith( "<?xml version=" ) && ostring->GetString().Contains( "DOCTYPE LumiRangeCollection" ) ) {
		grlReader.AddXMLString( ostring->GetString() );
		std::cout << "...ok" << std::endl;
	      }
	    }
         }
      }
      file->Close();
      delete file;
   }
   grlReader.Interpret();
   Root::TGoodRunsList l = grlReader.GetMergedGoodRunsList();

   TFile *lumicalcFile = TFile::Open(argv[1+nOptions]);
   if(lumicalcFile==0) {
      std::cout << "Could not open lumicalc file: " << argv[1+nOptions] << std::endl;
      return 0;
   }

   TTree *tmp = (TTree*)lumicalcFile->Get( "LumiMetaData" );
   if(!tmp) {
      std::cout << "Could not find LumiMetaData tree in lumicalc file: " << argv[1+nOptions] << std::endl;
      return 0;
   }

   std::cout << "Reading LumiMetaData...";

   //structure expected is as given by iLumiCalc:
   //   RunNbr, AvergeInteractionPerXing, IntLumi
   UInt_t runNbr=0;Float_t intLumi=0;TBranch *b_runNbr;TBranch *b_intLumi;
   UInt_t lb=0.; TBranch *b_lb;
   if(tmp->SetBranchAddress("RunNbr",&runNbr,&b_runNbr)!=0) {
      std::cout << "Could not find RunNbr branch in LumiMetaData TTree" << std::endl; return 0;
   }
   if(tmp->SetBranchAddress("LBStart",&lb,&b_lb)!=0) {
      std::cout << "Could not find LBStart branch in Data TTree" << std::endl; return 0;
   }
   if(tmp->SetBranchAddress("IntLumi",&intLumi,&b_intLumi)!=0) {
      std::cout << "Could not find IntLumi branch in Data TTree" << std::endl; return 0;
   }
   long nEntries = tmp->GetEntries();
   double totalLumi = 0;
   int startMissingBlock = -1;UInt_t lastRunNumber=0;UInt_t lastLb=0;double missingLumi=0;
   std::map<UInt_t, bool> missingRuns;std::map<UInt_t,bool> allRuns;
   for(long i=0;i<nEntries;i++) {
      b_runNbr->GetEntry(i);b_intLumi->GetEntry(i);b_lb->GetEntry(i);
      bool hasLumi = l.HasRunLumiBlock(runNbr,lb);
      if(!hasLumi && intLumi==0.) hasLumi=true; //if there is no lumi, it's as good as having some
      if((lastRunNumber!=runNbr&&startMissingBlock>=0) || (hasLumi && startMissingBlock>=0)) {
         //print now, if startMissingBlock not negative
         if(showMissing) std::cout << "[" << lastRunNumber << "," << startMissingBlock << "-" << lastLb << "," << missingLumi << "]";missingRuns[lastRunNumber]=true;
         startMissingBlock=-1;
      }
      if(hasLumi) {
         totalLumi += intLumi;
      } else if(!hasLumi && startMissingBlock==-1) {startMissingBlock=lb;missingLumi=intLumi/1E6;}
      else if(!hasLumi) { missingLumi+=(intLumi/1E6); }
      lastRunNumber=runNbr;lastLb=lb;
      allRuns[runNbr] = true;
   }
   lumicalcFile->Close();
   if(missingRuns.size()>0 && showMissing) {
      std::cout << std::endl << "Runs with missing blocks: ";
      for(std::map<UInt_t,bool>::iterator it=missingRuns.begin();it!=missingRuns.end();++it) {
         std::cout << it->first << ",";
      }
      std::cout << std::endl << "Complete runs: ";
      for(std::map<UInt_t,bool>::iterator it=allRuns.begin();it!=allRuns.end();++it) {
         if(missingRuns.find(it->first) == missingRuns.end()) std::cout << it->first << ",";
      }
      std::cout << std::endl;
   }
   std::cout << "Done" << std::endl;

   std::cout << "Total Luminosity = " << totalLumi/1E6 << " pb-1" << std::endl;

}
