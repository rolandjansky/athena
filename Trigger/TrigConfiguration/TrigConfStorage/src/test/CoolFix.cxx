/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
// NAME:     TrigConfCoolFix.cxx 
// PACKAGE:  TrigConfStorage
//                                                     
// AUTHOR:   J.Stelzer (CERN)	Joerg.Stelzer@cern.ch 
// CREATED:  17 Mar 2013
//                                                     
// PURPOSE: 
//
// This standalone application is designed to read and write the
// trigger configuration (L1+HLT) from DB,XML,COOL and to XML or COOL
// for test purposes
//
//////////////////////////////////////////////////////////////////////

#include "./Helper.h"

#include "TrigConfBase/MsgStream.h"

#include "TrigConfStorage/StorageMgr.h"
//#include "TrigConfStorage/XMLStorageMgr.h"

#include "TrigConfStorage/TrigConfCoolWriter.h"

#include "TrigConfStorage/DBLoader.h"
#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/TrigConfCoolWriter.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"


#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <ctime>
#include <map>
#include <vector>
#include <sys/stat.h> 

using namespace std;
using namespace TrigConf;
using boost::lexical_cast;


void printhelp(std::ostream & o, std::ostream& (*lineend) ( std::ostream& os )) {
  o << "================================================================================\n";
  o << "The program needs to be run with the following specifications:\n" << lineend;
  o << "TrigConfReadWrite <options>\n";
  o << "\n";
  o << "[Global options]\n";
  o << "  -i|--input        <cooldb> <run> [<lb>]       ... cool database and run number (mandatory)\n";
  o << "                                                    e.g. COOLONL_TRIGGER/CONDBR2 242651\n";
  o << "     --triggerdb    <triggerdb>                 ... trigger database as source of information (default is TRIGGERDB)\n";
  o << "     --smk          <smk>                       ... SMK to load to cool (specify when needed)\n";
  o << "     --bgsk         <bgsk>                      ... Bunchgroupset key to load to cool (specify when needed)\n";
  o << "     --l1psk        <l1psk>                     ... L1PSK to load to cool (specify when needed)\n";
  o << "     --hltpsk       <hltpsk>                    ... hltpsk to load to cool (specify when needed)\n";
  o << "\n";
  o << "  -f|--fix                                      ... when specified it fixes the database. Only run after a test without it\n";
  o << "\n";
  o << "  -v|--loglevel     <level>                     ... log level [NIL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS]\n";
  o << "  -p|--print        <detail>                    ... print configuration with detail 0...5 (default 1)\n";
  o << "  -h|--help                                     ... this output\n";
  o << "\n";
  o << "Input database can be specified the following ways: COOLONL_TRIGGER/CONDBR2 or trigconf.db/CONDBR2\n";
  o << "\n";
  o << "Instructions at https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerConfigurationCOOLArchiver\n";
  o << "================================================================================\n";
}

class JobConfig {
public:
   ~JobConfig(){}
   JobConfig() {}

   std::vector<std::string>  inpar;

   // cool input
   string       coolConnection {""};
   unsigned int run {0};
   unsigned int lb {0};

   // fix the database
   bool         fix {false};


   // trigger db
   string      triggerdb {"TRIGGERDB"};
   int         smk {0};
   int         bgsk {0};
   int         l1psk {0};
   int         hltpsk {0};

   // other
   bool         help {false};
   int          printlevel {-1};
   MSGTC::Level outputlevel {MSGTC::WARNING};
   vector<string> error;

   int parseProgramOptions(int argc, char* argv[]);

   void PrintSetup();

} gConfig;


int
JobConfig::parseProgramOptions(int argc, char* argv[]) {
   std::string currentPar("");
   std::string listofUnknownParameters = "";
   for(int i=1; i<argc; i++) {
      string currInput(argv[i]);
      int fchar = currInput.find_first_not_of('-');
      string stripped = currInput.substr(fchar);
      bool isParam = (fchar!=0); // string starts with a '-', so it is a parameter name

      if(isParam)
         if( ! (stripped == "i" || stripped == "input" ||
                stripped == "l" || stripped == "log" ||
                stripped == "p" || stripped == "print" ||
                stripped == "f" || stripped == "fix" ||
                stripped == "h" || stripped == "help" ||
                stripped == "v" || stripped == "loglevel") ) {
            listofUnknownParameters += " " + currInput;
            continue;
         }

      if(isParam) {
         currentPar = "";
         if(stripped == "h" || stripped == "help" )        { help = true; continue; }
         if(stripped == "f" || stripped == "fix")       { fix = true; continue; }
         currentPar = stripped;
      } else {
         if(currentPar == "i" || currentPar == "input")     { inpar.push_back(stripped); continue; }
         if(currentPar == "p" || currentPar == "print")     { printlevel = boost::lexical_cast<int,string>(stripped); currentPar=""; continue; }
         if(currentPar == "v" || currentPar == "loglevel") {
            if("NIL" == stripped ) { outputlevel = MSGTC::NIL; }
            else if("VERBOSE" == stripped ) { outputlevel = MSGTC::VERBOSE; }
            else if("DEBUG" == stripped ) { outputlevel = MSGTC::DEBUG; }
            else if("INFO" == stripped ) { outputlevel = MSGTC::INFO; }
            else if("WARNING" == stripped ) { outputlevel = MSGTC::WARNING; }
            else if("ERROR" == stripped ) { outputlevel = MSGTC::ERROR; }
            else if("FATAL" == stripped ) { outputlevel = MSGTC::FATAL; }
            else if("ALWAYS" == stripped ) { outputlevel = MSGTC::ALWAYS; }
            else { 
               error.push_back("Unknown output level: " + stripped + ". Must be one of NIL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS");
            }
            continue;
         }
      }
   }
   if(listofUnknownParameters!="")
      error.push_back("Unknown parameter(s): " + listofUnknownParameters);

   if(inpar.size()==0)
      error.push_back("No input cool db and run specified, use '-i <db> <run>' option");

   if(inpar.size()==1)
      error.push_back("No input run specified, use '-i <db> <run>' option");

   if( inpar.size()>=2 ) {
      string db = inpar[0];
      string dbname = "CONDBR2";
      size_t slashpos = inpar[0].find('/');
      if(slashpos != string::npos) {
         db     = inpar[0].substr(0,slashpos);
         dbname = inpar[0].substr(slashpos+1);
      }
      if(db.find('.') != string::npos) {
         coolConnection = "sqlite://;schema="+db+";dbname=" + dbname;
      } else {
         coolConnection = db + "/" + dbname;
      }
   }

   if( inpar.size()>=2 )
      run = boost::lexical_cast<unsigned int,string>(inpar[1]);

   if( inpar.size()>=3 ) 
      lb = boost::lexical_cast<unsigned int,string>(inpar[2]);

   if(help) {
      printhelp(cout, endl);
      return 0;
   }
   if(error.size()!=0) {
      for(const string& e: gConfig.error)
         cerr << e << endl;
      printhelp(cout, endl);
      return 1;
   }

   return -1;
}



void
JobConfig::PrintSetup() {
   cout << "========================================" << endl;
   cout << "JOB SETUP: " << endl;
   cout << "----------" << endl;
   cout << "   Cool DB             : " << coolConnection << endl;
   cout << "   Trigger database    : " << triggerdb << endl;
   cout << "   Fix flag            : " << (fix ? "yes" : "no") << endl;
   
   if( printlevel>=0)
      cout << "   Print menu detail   : " << printlevel << endl;
   cout << "========================================" << endl;

}


unsigned int
readKeyFromPrompt( const std::string & prompt ) {
   string input("");
   unsigned int key(0);
   while(key==0) {
      cout << prompt; cin >> input;
      try {
         key = boost::lexical_cast<unsigned int, string>(input);
      }
      catch(...) {
         cout << input << " is not a valid key" << endl;
      }
   }
   return key;
}




int main( int argc, char* argv[] ) {

   /***************************************
    *
    * Getting the program parameters
    *
    ***************************************/
   int retCode = gConfig.parseProgramOptions(argc, argv);
   if(retCode>=0)
      return retCode;
  
   gConfig.PrintSetup();
   

   /*------------------
    * Read information from COOL
    *-----------------*/

   TrigConfCoolWriter * coolWriter = new TrigConfCoolWriter( gConfig.coolConnection );

   vector<string> fixableFolders = coolWriter->checkPayloadSize( gConfig.run, gConfig.lb );

   delete coolWriter;

   if( ! gConfig.fix )
      return 0;


   if(fixableFolders.size()==0) {
      cout << endl << "All folders are properly filled. Exiting ..." << endl;
      return 0; // exit the program
   }


   set<unsigned int> selectForFixing;

   string selection="";
   while(selection != "f" && selection != "0") {
      // print
      cout << endl << "   0  ...  exit" << endl;
      int idx=0;
      for(const string & f : fixableFolders) {
         if(selectForFixing.count(idx)>0) {
            cout << " * " << ++idx << "  ...  " << f << endl;
         } else {
            cout << "   " << ++idx << "  ...  " << f << endl;
         }
      }
      cout << endl << "   f  ...  fix now" << endl;
      cout << endl << endl << "Select/deselect folder to fix or other option : "; cin >> selection;

      try {
         unsigned int selInd = boost::lexical_cast<unsigned int,string>(selection) - 1;
         if(selectForFixing.count(selInd)) {
            selectForFixing.erase(selInd);
         } else {
            selectForFixing.insert(selInd);
         }
      }
      catch(...){}
   }


   if(selection=="0") {
      cout << endl << "Exiting ..." << endl;
      return 0; // exit the program
   }

   if(selectForFixing.empty()) {
      cout << endl << "Nothing selected. Exiting ..." << endl;
      return 0; // exit the program
   }

   vector<string> foldersToFix;
   for(unsigned int idx=0; idx!=fixableFolders.size(); idx++) {
      if(selectForFixing.count(idx)>0)
         foldersToFix.push_back(fixableFolders[idx]);
   }

   // printout which folders were selected
   cout << endl << "Folders for fixing:";
   for(const string & f : foldersToFix)
      cout << " " << f;
   cout << endl;


   // check which information we need from the database
   bool loadL1(false), loadHLT(false), loadBGSK(false), loadL1PSK(false), loadHLTPSK(false);
   for(const string & folderToFix : foldersToFix) {

      loadL1 |=
         ( folderToFix == "/TRIGGER/LVL1/Menu") ||
         ( folderToFix == "/TRIGGER/LVL1/ItemDef") ||
         ( folderToFix == "/TRIGGER/LVL1/Thresholds") ||
         ( folderToFix == "/TRIGGER/LVL1/BunchGroupDescription");

      loadHLT |=
         ( folderToFix == "/TRIGGER/HLT/HltConfigKeys") ||
         ( folderToFix == "/TRIGGER/HLT/Menu") ||
         ( folderToFix == "/TRIGGER/HLT/Groups");

      loadBGSK |=
         ( folderToFix == "/TRIGGER/LVL1/BunchGroupKey") ||
         ( folderToFix == "/TRIGGER/LVL1/BunchGroupContent") ||
         ( folderToFix == "/TRIGGER/LVL1/BunchGroupDescription");
   
      loadL1PSK |=
         ( folderToFix == "/TRIGGER/LVL1/Lvl1ConfigKey") ||
         ( folderToFix == "/TRIGGER/LVL1/Prescales");
   
      loadHLTPSK |=
         ( folderToFix == "/TRIGGER/HLT/PrescaleKey") ||
         ( folderToFix == "/TRIGGER/HLT/Prescales");
   }


   
   if( (loadL1 || loadHLT) && gConfig.smk==0)
      gConfig.smk = readKeyFromPrompt("Please specify Supermaster key : ");

   if( loadBGSK && gConfig.bgsk==0)
      gConfig.bgsk = readKeyFromPrompt("Please specify Bunchgroupset key : ");

   if( loadL1PSK && gConfig.l1psk==0)
      gConfig.l1psk = readKeyFromPrompt("Please specify L1 Prescaleset key : ");
   
   if( loadHLTPSK && gConfig.hltpsk==0)
      gConfig.hltpsk = readKeyFromPrompt("Please specify HLT Prescaleset key : ");






   CTPConfig * ctpConfig(0);
   BunchGroupSet * bgs(0);
   HLTFrame *  hltFrame(0);
   PrescaleSet * l1pss(0);
   HLTPrescaleSet * hltpss(0);

   {
      unique_ptr<StorageMgr> sm(new StorageMgr(gConfig.triggerdb, "", "", cout));

      if(loadL1) {
         ctpConfig = new TrigConf::CTPConfig();
         ctpConfig->setSMK( gConfig.smk );
         cout << endl << endl << "Retrieving Lvl1 CTP configuration" << endl;
         sm->masterTableLoader().setLevel(gConfig.outputlevel);
         sm->masterTableLoader().load(*ctpConfig);
         if( gConfig.printlevel>=0)
            ctpConfig->print("  ",gConfig.printlevel);
      }

      if(loadHLT) {
         cout << endl << endl << "Retrieving HLT menu configuration" << endl;
         hltFrame = new HLTFrame();
         hltFrame->setSMK( gConfig.smk );
         sm->hltFrameLoader().load(*hltFrame);
         if( gConfig.printlevel>=0)
            hltFrame->print("  ",gConfig.printlevel);
      }

      if( loadBGSK ) {
         cout << endl << endl << "Retrieving Lvl1 Bunchgroup set configuration" << endl;
         bgs = new BunchGroupSet();
         bgs->setId(gConfig.bgsk);
         sm->bunchGroupSetLoader().load(*bgs);
         if( gConfig.printlevel>=0)
            bgs->print("  ",gConfig.printlevel);
      }

      if( loadL1PSK ) {
      }
   
      if( loadHLTPSK ) {
      }
   }
   

   /********************************************************************************
    *
    * Writing to COOL
    *
    *******************************************************************************/

   for(const string & folderToFix : foldersToFix) {

      cout << "Writing cool to destination " << gConfig.coolConnection << endl;
      TrigConfCoolWriter * coolWriter = new TrigConfCoolWriter( gConfig.coolConnection );

      /**
       *  L1
       */
      if( folderToFix == "/TRIGGER/LVL1/Menu" ) {
         if(ctpConfig) {
            coolWriter->writeL1MenuPayload( ValidityRange(gConfig.run), ctpConfig->menu());
         }
      }

      if( folderToFix == "/TRIGGER/LVL1/ItemDef")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;

      if( folderToFix == "/TRIGGER/LVL1/Thresholds")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;

      /**
       *  HLT
       */
      if( folderToFix == "/TRIGGER/HLT/HltConfigKeys")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;

      if( folderToFix == "/TRIGGER/HLT/Menu")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;

      if( folderToFix == "/TRIGGER/HLT/Groups")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;
   

      /**
       *  BGSK
       */
      if( folderToFix == "/TRIGGER/LVL1/BunchGroupKey")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;

      if( folderToFix == "/TRIGGER/LVL1/BunchGroupContent")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;

      if( folderToFix == "/TRIGGER/LVL1/BunchGroupDescription") {
         if(ctpConfig && bgs) {
            coolWriter->writeL1BunchGroupRunPayload( ValidityRange(gConfig.run),
                                                     ctpConfig->menu(),
                                                     *bgs);
         }
      }

      /**
       *  L1PSK
       */
      if( folderToFix == "/TRIGGER/LVL1/Lvl1ConfigKey")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;

      if( folderToFix == "/TRIGGER/LVL1/Prescales")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;
   
      /**
       *  HLTPSK
       */
      if( folderToFix == "/TRIGGER/HLT/PrescaleKey")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;
   
      if( folderToFix == "/TRIGGER/HLT/Prescales")
         cout << "Writing of COOL folder " << folderToFix << " not yet implemented" << endl;

   }

   delete ctpConfig;
   delete hltFrame;
   delete bgs;
   delete l1pss;
   delete hltpss;

}
