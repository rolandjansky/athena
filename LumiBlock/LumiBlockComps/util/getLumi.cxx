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
/// Note: This code is based on some old code I wrote a while ago, so it is by no means pretty!


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
#include <vector>

#include "GoodRunsLists/TGoodRunsListReader.h"

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"

struct lbx {
   lbx() : nExpected(0),nSeen(0) {}
   long nExpected;
   long nSeen;
   bool fromSuspect = false;
};

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
      std::cout << "Usage: <lumicalcfile.root> <list of xAOD or D3PD>" << std::endl;
      std::cout << "Option: -m     lists run numbers with any missing lumi" << std::endl;
      std::cout << "" << std::endl;
      std::cout << "Further Documentation: see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#How_to_check_the_luminosity_cove " <<std::endl;
      return 0;
   }
   gSystem->Load("libTree.so");

   bool readXML=false;
   Root::TGoodRunsList fromXAOD;Root::TGoodRunsList fromXAODIncomplete;Root::TGoodRunsList fromXAODSuspect;Root::TGoodRunsList fromXAODPossiblySuspect;
   std::map<uint, std::map<uint, lbx> > lbxs;
   Root::TGoodRunsListReader grlReader;

   bool hasMask(false);
   Root::TGoodRunsListReader grlMaskReader;
   //hunt for xml files... we will use these as 'masks' for the included lumi
   for(int i=2+nOptions;i<argc;i++) {
      std::string s(argv[i]);
      if(s.find(".xml")==std::string::npos) continue;
      grlMaskReader.AddXMLFile( s );
      hasMask=true;
   }
   Root::TGoodRunsList grlMask; 
   if(hasMask) {
      grlMaskReader.Interpret();grlMask.AddGRL(grlMaskReader.GetMergedGoodRunsList());
   }

   std::vector<std::string> lumiFiles;

   //open all the d3pd-like files, and hunt for their lumi strings
   for(int i=1+nOptions;i<argc;i++) {
      //require to have ".root" in it
      std::string s(argv[i]);
      if(s.find(".root")==std::string::npos) continue;
      TFile *file = TFile::Open(argv[i]);
      if(file==0) continue;
      std::cout << s;
      
      //check if this file is a LumiMetaData file ... if it is we add to the list for later
      TTree *tmp = (TTree*)file->Get( "LumiMetaData" );
      if(tmp) {
        std::cout << "is lumicalc [ok]" << std::endl;
        file->Close();
        lumiFiles.push_back(s);
        continue;
      }
      
      //try to get lumi

      //if this is xAOD, it will have a MetaData tree ... 
      if(file->Get("MetaData")!=0) {
         TTree* metaTree = static_cast<TTree*>(file->Get("MetaData"));
         //use TEvent if the MetaData tree contains an EventFormat branch and only 1 entry 
         //WB : 21/11/2016 - actually just always avoid using TEvent
         bool useTEvent(false);
         /*if(metaTree->GetEntries()==1 && metaTree->FindBranch("EventFormat")) {
            useTEvent = true;
         } else {*/
            metaTree->SetMakeClass(1);
         //}
         std::vector<unsigned int> startRunNumber;
         std::vector<unsigned int> stopRunNumber;
         std::vector<unsigned int> startLumiBlockNumber;
         std::vector<unsigned int> stopLumiBlockNumber;
         std::vector<unsigned int> eventsExpected;
         std::vector<unsigned int> eventsSeen;

         //use TEvent to read the lumiblocks, if we can
         bool ok(false);
         xAOD::TEvent event;
         const xAOD::LumiBlockRangeContainer* lbrs = 0;
         xAOD::LumiBlockRangeAuxContainer* lbrs_aux = 0;
         if(useTEvent && event.readFrom(file,true,"MetaData").isSuccess()) {
            if(event.containsMeta<xAOD::LumiBlockRangeContainer>("LumiBlocks")) {
               event.retrieveMetaInput<xAOD::LumiBlockRangeContainer>(lbrs, "LumiBlocks").isSuccess();
            }
         } else {
            useTEvent=false;
            if(metaTree->FindBranch("LumiBlocksAux.")) {
               if(metaTree->FindBranch("LumiBlocksAux.")->GetSplitLevel()==0) {
                  std::cout << " Unable to read unsplit LumiBlocksAux" << std::endl;
               }
               lbrs = new xAOD::LumiBlockRangeContainer();
               lbrs_aux = new xAOD::LumiBlockRangeAuxContainer;
               const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->setStore( lbrs_aux );
               metaTree->SetBranchAddress("LumiBlocksAux.startRunNumber",&startRunNumber);
               metaTree->SetBranchAddress("LumiBlocksAux.stopRunNumber",&stopRunNumber);
               metaTree->SetBranchAddress("LumiBlocksAux.startLumiBlockNumber",&startLumiBlockNumber);
               metaTree->SetBranchAddress("LumiBlocksAux.stopLumiBlockNumber",&stopLumiBlockNumber);
               metaTree->SetBranchAddress("LumiBlocksAux.eventsExpected",&eventsExpected);
               metaTree->SetBranchAddress("LumiBlocksAux.eventsSeen",&eventsSeen);
            }
         }

         if(lbrs) {
            std::cout << "...xAOD LumiBlocks";
            for(int ii=0;ii<metaTree->GetEntries();ii++) {
               if(!useTEvent) {
                  const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->clear();
                  metaTree->GetEntry(ii);
                  for(unsigned int j=0;j<startRunNumber.size();j++) {
                     xAOD::LumiBlockRange* r = new xAOD::LumiBlockRange; const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->push_back(r);
                     r->setStartRunNumber(startRunNumber.at(j));r->setStopRunNumber(stopRunNumber.at(j));
                     r->setStartLumiBlockNumber(startLumiBlockNumber.at(j));r->setStopLumiBlockNumber(stopLumiBlockNumber.at(j));
                     r->setEventsExpected(eventsExpected.at(j));r->setEventsSeen(eventsSeen.at(j));
                  }
               }

               //add lb to grl 
               for(auto lbr : *lbrs) {
                  for(uint runNum = lbr->startRunNumber(); runNum <= lbr->stopRunNumber(); runNum++) {
                     if(lbr->startLumiBlockNumber()!=lbr->stopLumiBlockNumber()) {std::cout << " Unexpected behaviour. Please report! " << std::endl; exit(1);}
                     for(uint lb = lbr->startLumiBlockNumber(); lb <= lbr->stopLumiBlockNumber(); lb++) {
                        lbxs[runNum][lb].nSeen += lbr->eventsSeen(); 
                        if(lbxs[runNum][lb].nExpected!=0 && lbxs[runNum][lb].nExpected != lbr->eventsExpected()) {
                          std::cout << "...mismatch on expected events in [run,lb]=[" << runNum << "," << lb << "] got " << lbr->eventsExpected() << " but had " << lbxs[runNum][lb].nExpected << std::endl;
                          std::cout << "...PLEASE REPORT THIS to hn-atlas-PATHelp@cern.ch ... for now I will assume the larger number is correct" << std::endl;
                          if(lbxs[runNum][lb].nExpected < lbr->eventsExpected()) lbxs[runNum][lb].nExpected = lbr->eventsExpected();
                        }
                        else if(lbxs[runNum][lb].nExpected==0) lbxs[runNum][lb].nExpected=lbr->eventsExpected();
                     }
                  }
               }
               ok=true;
            }
            std::cout << "...ok" << std::endl;
            if(!useTEvent) { delete lbrs; delete lbrs_aux; }
         }

         lbrs = 0;lbrs_aux=0;
         if(useTEvent && event.readFrom(file,true,"MetaData").isSuccess()) {
            if(event.containsMeta<xAOD::LumiBlockRangeContainer>("IncompleteLumiBlocks")) {
               event.retrieveMetaInput<xAOD::LumiBlockRangeContainer>(lbrs, "IncompleteLumiBlocks").isSuccess();
            }
         } else {
            useTEvent=false;
            if(metaTree->FindBranch("IncompleteLumiBlocksAux.")) {
               if(metaTree->FindBranch("IncompleteLumiBlocksAux.")->GetSplitLevel()==0) {
                  std::cout << " Unable to read unsplit IncompleteLumiBlocksAux" << std::endl;
               }
               lbrs = new xAOD::LumiBlockRangeContainer();
               lbrs_aux = new xAOD::LumiBlockRangeAuxContainer;
               const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->setStore( lbrs_aux );
               metaTree->ResetBranchAddresses();
               metaTree->SetBranchAddress("IncompleteLumiBlocksAux.startRunNumber",&startRunNumber);
               metaTree->SetBranchAddress("IncompleteLumiBlocksAux.stopRunNumber",&stopRunNumber);
               metaTree->SetBranchAddress("IncompleteLumiBlocksAux.startLumiBlockNumber",&startLumiBlockNumber);
               metaTree->SetBranchAddress("IncompleteLumiBlocksAux.stopLumiBlockNumber",&stopLumiBlockNumber);
               metaTree->SetBranchAddress("IncompleteLumiBlocksAux.eventsExpected",&eventsExpected);
               metaTree->SetBranchAddress("IncompleteLumiBlocksAux.eventsSeen",&eventsSeen);
            }
         }

         if(lbrs) {
            std::cout << "...xAOD IncompleteLumiBlocks";
            for(int ii=0;ii<metaTree->GetEntries();ii++) {
               if(!useTEvent) {
                  const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->clear();
                  metaTree->GetEntry(ii);
                  for(unsigned int j=0;j<startRunNumber.size();j++) {
                     xAOD::LumiBlockRange* r = new xAOD::LumiBlockRange; const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->push_back(r);
                     r->setStartRunNumber(startRunNumber.at(j));r->setStopRunNumber(stopRunNumber.at(j));
                     r->setStartLumiBlockNumber(startLumiBlockNumber.at(j));r->setStopLumiBlockNumber(stopLumiBlockNumber.at(j));
                     r->setEventsExpected(eventsExpected.at(j));r->setEventsSeen(eventsSeen.at(j));
                  }
               }

               //add lb to grl 
               for(auto lbr : *lbrs) {
                  for(uint runNum = lbr->startRunNumber(); runNum <= lbr->stopRunNumber(); runNum++) {
                     if(lbr->startLumiBlockNumber()!=lbr->stopLumiBlockNumber()) {std::cout << " Unexpected behaviour. Please report! " << std::endl; exit(1);}
                     for(uint lb = lbr->startLumiBlockNumber(); lb <= lbr->stopLumiBlockNumber(); lb++) {
                        lbxs[runNum][lb].nSeen += lbr->eventsSeen(); 
                        if(lbxs[runNum][lb].nExpected!=0 && lbxs[runNum][lb].nExpected != lbr->eventsExpected()) {
                          std::cout << "...mismatch on expected events in [run,lb]=[" << runNum << "," << lb << "] got " << lbr->eventsExpected() << " but had " << lbxs[runNum][lb].nExpected << std::endl;
                          std::cout << "...PLEASE REPORT THIS to hn-atlas-PATHelp@cern.ch ... for now I will assume the larger number is correct" << std::endl;
                          if(lbxs[runNum][lb].nExpected < lbr->eventsExpected()) lbxs[runNum][lb].nExpected = lbr->eventsExpected();
                        }
                        else if(lbxs[runNum][lb].nExpected==0) lbxs[runNum][lb].nExpected=lbr->eventsExpected();
                     }
                  }
               }
               ok=true;
            }
            std::cout << "...ok" << std::endl;
            if(!useTEvent) { delete lbrs; lbrs = 0; delete lbrs_aux; lbrs_aux=0; }
         }



         lbrs = 0;lbrs_aux=0;
         if(useTEvent && event.readFrom(file,true,"MetaData").isSuccess()) {
            if(event.containsMeta<xAOD::LumiBlockRangeContainer>("SuspectLumiBlocks")) {
               event.retrieveMetaInput<xAOD::LumiBlockRangeContainer>(lbrs, "SuspectLumiBlocks").isSuccess();
            }
         } else {
            useTEvent=false;
            if(metaTree->FindBranch("SuspectLumiBlocksAux.")) {
               if(metaTree->FindBranch("SuspectLumiBlocksAux.")->GetSplitLevel()==0) {
                  std::cout << " Unable to read unsplit SuspectLumiBlocks" << std::endl;
               }
               lbrs = new xAOD::LumiBlockRangeContainer();
               lbrs_aux = new xAOD::LumiBlockRangeAuxContainer;
               const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->setStore( lbrs_aux );
               metaTree->ResetBranchAddresses();
               metaTree->SetBranchAddress("SuspectLumiBlocksAux.startRunNumber",&startRunNumber);
               metaTree->SetBranchAddress("SuspectLumiBlocksAux.stopRunNumber",&stopRunNumber);
               metaTree->SetBranchAddress("SuspectLumiBlocksAux.startLumiBlockNumber",&startLumiBlockNumber);
               metaTree->SetBranchAddress("SuspectLumiBlocksAux.stopLumiBlockNumber",&stopLumiBlockNumber);
               metaTree->SetBranchAddress("SuspectLumiBlocksAux.eventsExpected",&eventsExpected);
               metaTree->SetBranchAddress("SuspectLumiBlocksAux.eventsSeen",&eventsSeen);
            }
         }

         if(lbrs) {
            std::cout << "...xAOD SuspectLumiBlocksAux";
            for(int ii=0;ii<metaTree->GetEntries();ii++) {
               if(!useTEvent) {
                  const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->clear();
                  metaTree->GetEntry(ii);
                  for(unsigned int j=0;j<startRunNumber.size();j++) {
                     xAOD::LumiBlockRange* r = new xAOD::LumiBlockRange; const_cast<xAOD::LumiBlockRangeContainer*>(lbrs)->push_back(r);
                     r->setStartRunNumber(startRunNumber.at(j));r->setStopRunNumber(stopRunNumber.at(j));
                     r->setStartLumiBlockNumber(startLumiBlockNumber.at(j));r->setStopLumiBlockNumber(stopLumiBlockNumber.at(j));
                     r->setEventsExpected(eventsExpected.at(j));r->setEventsSeen(eventsSeen.at(j));
                  }
               }
               //add lb to grl 
               for(auto lbr : *lbrs) {
                  for(uint runNum = lbr->startRunNumber(); runNum <= lbr->stopRunNumber(); runNum++) {
                     if(lbr->startLumiBlockNumber()!=lbr->stopLumiBlockNumber()) {std::cout << " Unexpected behaviour. Please report! " << std::endl; exit(1);}
                     for(uint lb = lbr->startLumiBlockNumber(); lb <= lbr->stopLumiBlockNumber(); lb++) {
                        lbxs[runNum][lb].nSeen += lbr->eventsSeen(); 
                        lbxs[runNum][lb].fromSuspect = true;
                        if(lbxs[runNum][lb].nExpected!=0 && lbxs[runNum][lb].nExpected != lbr->eventsExpected()) {
                          std::cout << "...mismatch on expected events in [run,lb]=[" << runNum << "," << lb << "] got " << lbr->eventsExpected() << " but had " << lbxs[runNum][lb].nExpected << std::endl;
                          std::cout << "...PLEASE REPORT THIS to hn-atlas-PATHelp@cern.ch ... for now I will assume the larger number is correct" << std::endl;
                          if(lbxs[runNum][lb].nExpected < lbr->eventsExpected()) lbxs[runNum][lb].nExpected = lbr->eventsExpected();
                        }
                        else if(lbxs[runNum][lb].nExpected==0) lbxs[runNum][lb].nExpected=lbr->eventsExpected();

                     }
                  }
               }
               ok=true;
            }
            std::cout << "...ok" << std::endl;
            if(!useTEvent) { delete lbrs; lbrs = 0; delete lbrs_aux; lbrs_aux=0; }
         }

         if(ok) { file->Close(); delete file; continue; } //don't try to read Lumi folder
      }


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
               grlReader.AddXMLString(*in); readXML=true;
            }
            std::cout << "...ok" << std::endl;
         } else {
            TList* keys = gDirectory->GetListOfKeys();
	    //loop over all that are objstrings
	    for( Int_t j = 0; j < keys->GetEntries(); ++j ) {
	      TKey *key = dynamic_cast<TKey*>( keys->At( j ) );
	      TObjString* ostring = (TObjString*) key->ReadObjectAny( TObjString::Class() );
	      if( ! ostring ) { std::cout << "...skipping object: " << key->GetName() << std::endl; continue;}
	      // Check that it has the right format:
	      if( ostring->GetString().BeginsWith( "<?xml version=" ) && ostring->GetString().Contains( "DOCTYPE LumiRangeCollection" ) ) {
		grlReader.AddXMLString( ostring->GetString() ); readXML=true;
		std::cout << "...ok" << std::endl;
	      }
	    }
         }
      }
      file->Close();
      delete file;
   }


   //loop over lbx and build up the complete/incomplete/suspect list:
   std::stringstream definitelySuspect;
   std::stringstream possiblySuspect;
   for(auto& it : lbxs) {
      uint runNum = it.first;
      for(auto& it2 : it.second) {
         uint lbn = it2.first;
         if(it2.second.nSeen > it2.second.nExpected) { fromXAODSuspect.AddRunLumiBlock(runNum,lbn); continue; }
         
         if(it2.second.fromSuspect) { fromXAODPossiblySuspect.AddRunLumiBlock(runNum,lbn);}
         
         if(it2.second.nSeen==it2.second.nExpected) { fromXAOD.AddRunLumiBlock(runNum,lbn); }
         else { fromXAODIncomplete.AddRunLumiBlock(runNum,lbn); }
      }
   }
   
   Root::TGoodRunsList l;Root::TGoodRunsList lIncomplete;Root::TGoodRunsList lSuspect;Root::TGoodRunsList lPossiblySuspect;
   l.AddGRL(fromXAOD); lIncomplete.AddGRL(fromXAODIncomplete); lSuspect.AddGRL(fromXAODSuspect);lPossiblySuspect.AddGRL(fromXAODPossiblySuspect);//have to do this because of weird GRL behaviour
   if(readXML) {
      grlReader.Interpret();
      l.AddGRL(grlReader.GetMergedGoodRunsList());
   }

   double totalLumi = 0; double totalLumiIncomplete = 0; double totalLumiSuspect=0;double allMissing=0; double totalLumiPossiblySuspect=0;
   std::map<UInt_t, float> missingRuns;std::map<UInt_t,bool> allRuns;std::set<UInt_t> incompleteRuns;std::set<UInt_t> suspectRuns;
   std::map<UInt_t, std::string> missingRunLB; //lumiblocks that are missing

   


   

   Root::TGoodRunsList providedL; //keeps track of what came from lumicalc file

   if(lumiFiles.size()==0) {
     std::cout << "FATAL: No lumicalc file provided, please include at least one lumicalc file in your input list" << std::endl;
     return 0;
   }

   std::cout << "Reading LumiMetaData...";

   for(auto lfile : lumiFiles) {
    TFile *lumicalcFile = TFile::Open(lfile.c_str());
    if(lumicalcFile==0) {
        std::cout << "Could not open lumicalc file: " << argv[1+nOptions] << std::endl;
        return 0;
    }
  
    TTree *tmp = (TTree*)lumicalcFile->Get( "LumiMetaData" );
    if(!tmp) {
        std::cout << "Could not find LumiMetaData tree in lumicalc file: " << argv[1+nOptions] << std::endl;
        return 0;
    }
  
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
    
    int startMissingBlock = -1;UInt_t lastRunNumber=0;int lastLb=0;double missingLumi=0; 
  
    for(long i=0;i<nEntries;i++) {
        b_runNbr->GetEntry(i);b_intLumi->GetEntry(i);b_lb->GetEntry(i);
        if(hasMask && !grlMask.HasRunLumiBlock(runNbr,lb)) continue;
        providedL.AddRunLumiBlock(runNbr,lb);
        bool hasLumi = l.HasRunLumiBlock(runNbr,lb);
        if(hasLumi) totalLumi += intLumi;
        else if(lIncomplete.HasRunLumiBlock(runNbr,lb)) {hasLumi=true; totalLumiIncomplete += intLumi; incompleteRuns.insert(runNbr);} //else if ensures we never double count lumi
        else if(lSuspect.HasRunLumiBlock(runNbr,lb)) {hasLumi=true;totalLumiSuspect += intLumi; suspectRuns.insert(runNbr); definitelySuspect << "(" << runNbr << "," << lb << "),";}
  
        if(lPossiblySuspect.HasRunLumiBlock(runNbr,lb)) {totalLumiPossiblySuspect += intLumi; possiblySuspect << "(" << runNbr << "," << lb << "),";  }
  
        if(!hasLumi && intLumi==0.) hasLumi=true; //if there is no lumi, it's as good as having some
        if((lastRunNumber!=runNbr&&startMissingBlock>=0) || (hasLumi && startMissingBlock>=0)) {
          //print now, if startMissingBlock not negative
          missingRunLB[lastRunNumber] += ((startMissingBlock==lastLb) ? TString::Format("%d",startMissingBlock) : TString::Format("%d-%d",startMissingBlock,lastLb));
          missingRunLB[lastRunNumber] += ",";
          if(showMissing) std::cout << "[" << lastRunNumber << "," << startMissingBlock << "-" << lastLb << "," << missingLumi << "]";
          missingRuns[lastRunNumber]+=missingLumi;
          allMissing+=missingLumi; missingLumi=0;
          startMissingBlock=-1;
        }
        if(!hasLumi && startMissingBlock==-1) {startMissingBlock=lb;missingLumi=intLumi/1E6;}
        else if(!hasLumi) { missingLumi+=(intLumi/1E6); }
        lastRunNumber=runNbr;lastLb=lb;
        allRuns[runNbr] = true;
    }
    lumicalcFile->Close();
    allMissing += missingLumi;
    if(missingLumi) {
      missingRuns[lastRunNumber] += missingLumi;
      missingRunLB[lastRunNumber] += ((startMissingBlock==lastLb) ? TString::Format("%d",startMissingBlock) : TString::Format("%d-%d",startMissingBlock,lastLb));
      missingRunLB[lastRunNumber] += ",";
    }
   }
   std::cout << "Done" << std::endl;
   std::cout << "--------------------------------------------" << std::endl;
   std::cout << "***************LUMI REPORT******************" << std::endl << std::endl;


   
   if(possiblySuspect.str().size()) {
     std::cout << "Possibly suspect lumiblocks: " << possiblySuspect.str() << std::endl;
   }
   if(definitelySuspect.str().size()) {
     std::cout << "WARNING ... Definitely suspect lumiblocks (please report this!): " << definitelySuspect.str() << std::endl;
   } else {
     std::cout << std::endl << "No definitely suspect lumiblocks, that's good!";
   }



      std::cout << std::endl << "Runs with incomplete blocks: ";
      for(auto r : incompleteRuns) {
         std::cout << r << ",";
      }
      std::cout << std::endl << "Runs with missing blocks (missing blocks in brackets): " << std::endl << std::endl;
      for(auto it=missingRuns.begin();it!=missingRuns.end();++it) {
	std::cout << "  " << it->first << ": " << it->second << " pb-1 (" << missingRunLB[it->first] << ")" << std::endl;
      }
      std::cout << std::endl << "Complete runs: ";
      for(std::map<UInt_t,bool>::iterator it=allRuns.begin();it!=allRuns.end();++it) {
	if(missingRuns.find(it->first) == missingRuns.end() && incompleteRuns.find(it->first)==incompleteRuns.end() && suspectRuns.find(it->first) == suspectRuns.end()) std::cout << it->first << ",";
      }
      std::cout << std::endl;

   std::cout << std::endl;
   std::cout << "Complete Luminosity = " << totalLumi/1E6 << " pb-1" << std::endl;
   if(totalLumiIncomplete) std::cout << "Incomplete Luminosity = " << totalLumiIncomplete/1E6 << " pb-1   (this is partially processed lumi blocks)" <<  std::endl;
   if(totalLumiSuspect) std::cout << "Definitely Suspect Luminosity = " << totalLumiSuspect/1E6 << " pb-1   (this is problematic luminosity... please report!)" << std::endl;
   std::cout << "Complete+Incomplete";
   if(totalLumiSuspect) std::cout << "+Suspect";
   std::cout << " Luminosity = " << (totalLumi+totalLumiIncomplete+totalLumiSuspect)/1E6 << " pb-1" << std::endl;
   if(totalLumiPossiblySuspect) {
    std::cout << "   (of which is possibly suspect = " << totalLumiPossiblySuspect/1E6 << " pb-1  (caused by jobs that did not finish correctly))" << std::endl;
   }
   if(allMissing) {
      std::cout << "(Missing Lumonisity = " << allMissing << " pb-1)   (this is luminosity in your lumicalc files that you appear not to have run over)"; //already divided by 1E6 in loop above
      //if(!showMissing) std::cout << " rerun with the '-m' option to see runs where this luminosity resides";
      std::cout << std::endl; 
   }

   /*
   Root::TGoodRunsList allL(l); allL.AddGRL(lIncomplete);allL.AddGRL(lSuspect);
   auto uncovered = allL.GetPartNotIn(providedL);
   if(!uncovered.IsEmpty()) {
      std::cout << "Input files contain following lumiblocks not covered by lumicalc file:" << std::endl;
      uncovered.Summary(1);
   }*/

}

