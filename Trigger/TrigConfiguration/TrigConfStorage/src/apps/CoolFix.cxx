/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "TrigConfBase/MsgStream.h"

#include "TrigConfStorage/DBLoader.h"
#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

#include "TrigConfCoolWriter.h"

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
  o << "  -c|--cooldb       <cooldb>                    ... cool database and run number (default is COOLONL_TRIGGER/CONDBR2)\n";
  o << "                                                    e.g. COOLONL_TRIGGER/CONDBR2 or testcool.db \n";
  o << "  -r|--run          <run> [<lb>] [<lbend>]      ... run number (mandatory) and optionally lb (range)\n";
  o << "\n";
  o << "     --triggerdb    <triggerdb>                 ... trigger database as source of information (default is TRIGGERDBR2)\n";
  o << "     --smk          <smk>                       ... SMK to load to cool (specify when needed)\n";
  o << "     --bgsk         <bgsk>                      ... Bunchgroupset key to load to cool (specify when needed)\n";
  o << "     --l1psk        <l1psk>                     ... L1PSK to load to cool (specify when needed)\n";
  o << "     --hltpsk       <int>                       ... hltpsk to load to cool (specify when needed)\n";
  o << "     --release      <string>                    ... release, e.g. 20.2.3.1 (specify when configkeys is to be uploaded)\n";
  o << "\n";
  o << "  -f|--fix                                      ... when specified it fixes the database. Only run after a test without it\n";
  o << "  -e|--extended                                 ... allows fixing of any multiversion folder\n";
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

struct JobConfig {
   // input parameters
   string       cooldb{"COOLONL_TRIGGER/CONDBR2"};
   unsigned int run {0};
   unsigned int lb {0};
   unsigned int lbend {0};
   bool openended {true};
   // fix the database
   bool         fix {false};
   bool         extendedSelection {false}; // if true also allows fixing of MV folders
   // trigger db
   string      triggerdb {"TRIGGERDBR2"};
   int         smk {0};
   int         bgsk {0};
   int         l1psk {0};
   int         hltpsk {0};
   string      release {""};
   // other
   bool         help {false};
   int          printlevel {-1};
   MSGTC::Level outputlevel {MSGTC::WARNING};
   // error strings from option parsing
   vector<string> error;
   // resolved cool connection
   string       coolConnection {""};
   int parseProgramOptions(int argc, char* argv[]);
   void PrintSetup();
};




int
JobConfig::parseProgramOptions(int argc, char* argv[]) {
   std::string currentPar("");
   std::string listofUnknownParameters = "";
   uint runargpos(0);
   for(int i=1; i<argc; i++) {
      string currInput(argv[i]);
      int fchar = currInput.find_first_not_of('-');
      string stripped = currInput.substr(fchar);
      bool isParam = (fchar!=0); // string starts with a '-', so it is a parameter name

      if(isParam)
         if( ! (stripped == "c" || stripped == "cooldb" ||
                stripped == "r" || stripped == "run" ||
                stripped == "l" || stripped == "log" ||
                stripped == "p" || stripped == "print" ||
                stripped == "f" || stripped == "fix" ||
                stripped == "e" || stripped == "extended" ||
                stripped == "triggerdb" ||
                stripped == "release" ||
                stripped == "h" || stripped == "help" ||
                stripped == "v" || stripped == "loglevel") ) {
            listofUnknownParameters += " " + currInput;
            continue;
         }

      if(isParam) {
         currentPar = "";
         if(stripped == "h" || stripped == "help" )        { help = true; continue; }
         if(stripped == "f" || stripped == "fix")          { fix = true; continue; }
         if(stripped == "e" || stripped == "extended")     { extendedSelection = true; continue; }
         currentPar = stripped;
      } else {
         if(currentPar == "c" || currentPar == "cooldb")   { cooldb = stripped; currentPar=""; continue; }
         if(currentPar == "r" || currentPar == "run")      {
            unsigned int val = boost::lexical_cast<unsigned int,string>(stripped);
            switch(runargpos) {
            case 0:
               run = val;
               break;
            case 1:
               lb = val;
               break;
            case 2:
               lbend = val;
               openended = false;
               break;
            }
            runargpos++;
            continue;
         }
         if(currentPar == "triggerdb")                     { triggerdb = stripped; continue; }
         if(currentPar == "release")                       { release = stripped; continue; }
         if(currentPar == "p" || currentPar == "print")    { printlevel = boost::lexical_cast<int,string>(stripped); currentPar=""; continue; }
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
            currentPar="";
            continue;
         }
      }
   }
   if(listofUnknownParameters!="")
      error.push_back("Unknown parameter(s): " + listofUnknownParameters);

   if(run==0)
      error.push_back("No run specified, use '--run <run>' option");


   {
      string db = cooldb;
      string dbname = "CONDBR2";
      size_t slashpos = cooldb.find('/');
      if(slashpos != string::npos) {
         db     = cooldb.substr(0,slashpos);
         dbname = cooldb.substr(slashpos+1);
      }
      if(db.find('.') != string::npos) {
         coolConnection = "sqlite://;schema="+db+";dbname=" + dbname;
      } else {
         coolConnection = db + "/" + dbname;
      }
   }

   if(help) {
      printhelp(cout, endl);
      return 0;
   }
   if(error.size()!=0) {
      for(const string& e: error)
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
   cout << "   Run                 : " << run << endl;
   cout << "   LB                  : " << lb << " - " << lbend << endl;
   cout << "   Cool DB             : " << coolConnection << endl;
   cout << "   Trigger database    : " << triggerdb << endl;
   cout << "   Fix flag            : " << (fix ? "yes" : "no") << endl;
   
   if( printlevel>=0)
      cout << "   Print menu detail   : " << printlevel << endl;
   cout << "========================================" << endl;

}

template<typename T>
void
readKeyFromPrompt( const std::string & prompt, T & key) {
   string input("");
   cout << prompt; cin >> input;
   try {
      key = boost::lexical_cast<T, string>(input);
   }
   catch(...) {
      cout << input << " is not a valid input" << endl;
   }
}




int main( int argc, char* argv[] ) {

   /***************************************
    *
    * Getting the program parameters
    *
    ***************************************/
   JobConfig gConfig;
   int retCode = gConfig.parseProgramOptions(argc, argv);
   if(retCode>=0)
      return retCode;
  
   gConfig.PrintSetup();
   
   /*----------------------------
    *
    * Read information from COOL
    * and display it
    *
    *---------------------------*/

   TrigConfCoolWriter * coolReader = new TrigConfCoolWriter( gConfig.coolConnection );

   int displayMode = gConfig.fix ? ( gConfig.extendedSelection ? 2 : 1 ) : 0;

   vector<string> fixableFolders = coolReader->checkPayloadSize( gConfig.run, gConfig.lb, displayMode, gConfig.openended, gConfig.lbend );

   if( ! gConfig.fix ) {
      delete coolReader;
      return 0;
   }

   if(fixableFolders.size()==0) {
      cout << endl << "All folders are properly filled. Exiting ..." << endl;
      return 0; // exit the program
   }

   /*----------------------------
    *
    * Select which folders should
    * be fixed
    *
    *---------------------------*/

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
      cout << endl << "   a  ...  select/deselect all" << endl;
      cout << endl << "   e  ...  " << (gConfig.extendedSelection ? "disallow" : "allow") << " multiversion folder selection (clears selection)" << endl;
      cout << endl << "   f  ...  fix now" << endl;
      cout << endl << endl << "Select/deselect folder to fix or other option : "; cin >> selection;

      try {
         if(selection=="a") {
            // select all
            bool allSelected = (fixableFolders.size() == selectForFixing.size());
            if(allSelected) {
               selectForFixing.clear();
            } else {
               for(unsigned int selInd = 0; selInd<fixableFolders.size();selInd++)
                  selectForFixing.insert(selInd);
            }
         } else if(selection=="e") {
            
            gConfig.extendedSelection = !gConfig.extendedSelection;

            fixableFolders = coolReader->checkPayloadSize( gConfig.run, gConfig.lb, gConfig.extendedSelection ? 2 : 1, gConfig.openended, gConfig.lbend );

            selectForFixing.clear();
            
         } else {
            unsigned int selInd = boost::lexical_cast<unsigned int,string>(selection) - 1;
            if(selectForFixing.count(selInd)) {
               selectForFixing.erase(selInd);
            } else {
               selectForFixing.insert(selInd);
            }
         }
      }
      catch(...){}

   }

   delete coolReader;


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
   bool loadL1(false), loadHLT(false), loadBGSK(false), loadL1PSK(false), loadHLTPSK(false), saveConfigSource(false);
   for(const string & folderToFix : foldersToFix) {

      loadL1 |=
         ( folderToFix == "/TRIGGER/LVL1/Menu") ||
         ( folderToFix == "/TRIGGER/LVL1/ItemDef") ||
         ( folderToFix == "/TRIGGER/LVL1/Thresholds") ||
         ( folderToFix == "/TRIGGER/LVL1/CTPCoreInputMapping") ||
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

      saveConfigSource |=
         ( folderToFix == "/TRIGGER/HLT/HltConfigKeys");
   }


   
   if( (loadL1 || loadHLT) && gConfig.smk==0)
      readKeyFromPrompt("Please specify Supermaster key : ", gConfig.smk);

   if( loadBGSK && gConfig.bgsk==0)
      readKeyFromPrompt("Please specify Bunchgroupset key : ", gConfig.bgsk );

   if( loadL1PSK && gConfig.l1psk==0)
      readKeyFromPrompt("Please specify L1 Prescaleset key : ", gConfig.l1psk );
   
   if( loadHLTPSK && gConfig.hltpsk==0) {
      string prompt = "Please specify HLT Prescaleset key";
      if(gConfig.openended) {
          prompt += " (starting at LB " + lexical_cast<string,int>(gConfig.lb) + "): ";
      } else {
          prompt += " (for LB " + lexical_cast<string,int>(gConfig.lb) + " - " + std::to_string(gConfig.lbend) + "): ";
      }
      readKeyFromPrompt( prompt, gConfig.hltpsk );
   }

   if( saveConfigSource && gConfig.release=="")
      readKeyFromPrompt("Please specify the HLT release : ", gConfig.release );


   CTPConfig * ctpConfig(0);
   BunchGroupSet * bgs(0);
   HLTFrame *  hltFrame(0);
   PrescaleSet * l1pss(0);
   HLTPrescaleSet * hltpss(0);

   {
      unique_ptr<StorageMgr> sm(new StorageMgr(gConfig.triggerdb, "", "", cout));

      ctpConfig = new TrigConf::CTPConfig();
      if(loadL1) {
         ctpConfig->setSMK( gConfig.smk );
         sm->menuLoader().setEnv(IMenuLoader::CTPOnl);
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
         cout << endl << endl << "Retrieving L1 prescales" << endl;
         l1pss = new PrescaleSet();
         l1pss->setId(gConfig.l1psk);
         sm->prescaleSetLoader().load(ctpConfig->ctpVersion(), *l1pss);
         if( gConfig.printlevel>=0)
            l1pss->print("  ",gConfig.printlevel);
      }
   
      if( loadHLTPSK ) {
         cout << endl << endl << "Retrieving HLT prescales" << endl;
         hltpss = new HLTPrescaleSet();
         hltpss->setId(gConfig.hltpsk);
         sm->hltPrescaleSetLoader().load(*hltpss);
         if( gConfig.printlevel>=0)
            hltpss->print("  ",gConfig.printlevel);
      }
   }
   

   /********************************************************************************
    *
    * Writing to COOL
    *
    *******************************************************************************/

   cout << "Writing cool to destination " << gConfig.coolConnection << endl;
   TrigConfCoolWriter * coolWriter = new TrigConfCoolWriter( gConfig.coolConnection );

   ValidityRange vr(gConfig.run);

   if(gConfig.openended) {
       vr = ValidityRange(gConfig.run, gConfig.lb);
   } else {
       cool::ValidityKey since(gConfig.run); since <<= 32; since += gConfig.lb;
       cool::ValidityKey until(gConfig.run); until <<= 32; until += gConfig.lbend+1;
       vr = ValidityRange(since, until);
   }


   /**
    *  BGSK
    */
   {

      bool doWrite(false);
      for(const string & folderToFix : foldersToFix) {
         if( folderToFix == "/TRIGGER/LVL1/BunchGroupKey" ||
             folderToFix == "/TRIGGER/LVL1/BunchGroupContent") {
            coolWriter->addWriteFolder( folderToFix );
            doWrite = true;
         }
      }
      if(loadBGSK && doWrite) {
         coolWriter->writeL1BunchGroupLBPayload( vr, bgs->id(), *bgs);
      }
      coolWriter->clearWriteFolder();

      for(const string & folderToFix : foldersToFix) {
         if( folderToFix == "/TRIGGER/LVL1/BunchGroupDescription") {
            if(ctpConfig && bgs) {
               coolWriter->writeL1BunchGroupRunPayload( vr,
                                                        ctpConfig->menu(),
                                                        *bgs);
            }
         }
      }

   }

   /**
    *  L1PSK
    */
   {

      bool doWrite(false);
      for(const string & folderToFix : foldersToFix) {
         if( folderToFix == "/TRIGGER/LVL1/Lvl1ConfigKey" ||
             folderToFix == "/TRIGGER/LVL1/Prescales") {
            coolWriter->addWriteFolder( folderToFix );
            doWrite = true;
         }
      }
      if(loadL1PSK && doWrite) {
         coolWriter->writeL1PrescalePayload( vr.since(), vr.until(), l1pss->id() , *l1pss);
      }
      coolWriter->clearWriteFolder();


   }



   /**
    *  HLT prescales
    */
   {

      bool doWrite(false);
      for(const string & folderToFix : foldersToFix) {
         if( folderToFix == "/TRIGGER/HLT/PrescaleKey" ||
             folderToFix == "/TRIGGER/HLT/Prescales") {
            coolWriter->addWriteFolder( folderToFix );
            doWrite = true;
         }
      }
      if(hltpss && doWrite) {
         coolWriter->writeHltPrescalePayload( vr.since(), vr.until(), *hltpss);
      }
      coolWriter->clearWriteFolder();
   }

   
   /**
    *  L1
    */
   {
      bool doWrite(false);
      for(const string & folderToFix : foldersToFix) {
         if( folderToFix == "/TRIGGER/LVL1/Menu" ||
             folderToFix == "/TRIGGER/LVL1/ItemDef" ||
             folderToFix == "/TRIGGER/LVL1/Thresholds" ||
             folderToFix == "/TRIGGER/LVL1/CTPCoreInputMapping") {
            coolWriter->addWriteFolder( folderToFix );
            doWrite = true;
         }
      }
      if(ctpConfig && doWrite) {
         coolWriter->writeL1MenuPayload( ValidityRange(gConfig.run), ctpConfig->menu());
      }
      coolWriter->clearWriteFolder();
   }



   /**
    *  HLT
    */
   {
      bool doWrite(false);
      for(const string & folderToFix : foldersToFix) {
         if( folderToFix == "/TRIGGER/HLT/Menu" ||
             folderToFix == "/TRIGGER/HLT/Groups" ||
             folderToFix == "/TRIGGER/HLT/HltConfigKeys" ) {
            coolWriter->addWriteFolder( folderToFix );
            doWrite = true;
         }
      }

      if(hltFrame && doWrite) {
         string configSource = "TRIGGERDBR2," + gConfig.release + ",AtlasP1HLT";

         coolWriter->writeHLTPayload( ValidityRange(gConfig.run), *hltFrame, configSource);
      }
      coolWriter->clearWriteFolder();
   }


   delete ctpConfig;
   delete hltFrame;
   delete bgs;
   delete l1pss;
   delete hltpss;

}
