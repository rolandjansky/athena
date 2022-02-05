/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//
// NAME:     TrigConfReadWrite.cxx
// PACKAGE:  TrigConfStorage
//
// AUTHOR:   J.Stelzer (CERN)	Joerg.Stelzer@cern.ch
// CREATED:  17 Mar 2013
//
// PURPOSE:
//
// This standalone application is designed to read and write the
// trigger configuration (L1+HLT) from DB,COOL and to COOL
// for test purposes
//
//////////////////////////////////////////////////////////////////////

#include "TrigConfBase/MsgStream.h"

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/DBLoader.h"
#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/MCKLoader.h"

#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfJobOptData/JobOptionTable.h"

#include "TrigConfCoolWriter.h"
#include "Run2toRun3ConvertersL1.h"
#include "Run2toRun3ConvertersHLT.h"

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"

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

void printhelp(std::ostream & o, std::ostream& (*lineend) ( std::ostream& os )) {
   o << "================================================================================\n";
   o << "The program needs to be run with the following specifications:\n" << lineend;
   o << "TrigConfReadWrite <options>\n";
   o << "\n";
   o << "[Global options]\n";
   o << "  -i|--input        input [input [input]]             ... source of configuration, format see below (mandatory)\n";
   o << "  -2|--comp         input [input [input]]             ... source of a second configuration for comparison\n";
   o << "  -o|--output r3json|cool [output[;cooldb]] [run] ... output format, name (for cool optional run number)\n";
   o << "                                                      ... absolute output file name must contain '/', cooldb can be appended COMP200|OFLP200\n";
   o << "  -v|--loglevel     <string>                          ... log level [NIL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS]\n";
   o << "  -l|--log          <string>                          ... name of a log file\n";
   o << "  --jo                                                ... read and write job options where possible\n";
   o << "  --fw                                                ... read ctp firmware\n";
   o << "  -p|--print        <int>                             ... print configuration with detail 0...5 (default 1)\n";
   o << "  -h|--help                                           ... this output\n";
   o << "  --nomerge                                           ... internally don't merge L2 and EF (by default merge is enabled)\n";
   o << "\n\n";
   o << "Input can be specified the following\n";
   o << "  -i <TRIGDB_ALIAS>|<TRIGDB_connection> smk[,l1psk,hltpsk,bgsk]     ... to read the menu from a trigger db via alias or explicit connection specification (ORACLE or SQlite)\n";
   o << "  -i <COOLDB_ALIAS>|<COOLDB_connection>|cool.db run[,lb]            ... to read the menu from COOL for a certain run and possibly LB [file names must end with '.db']\n";
   o << "\n";
   o << "The cool dbconnection can be specified as one of the following\n";
   o << "     - via alias   : COOLONL_TRIGGER    (use COOLONL_TRIGGER/COMP200 for Run 1 data)";
   o << "     - from sqlite : cool.db            (use cool.db;COMP200 for Run 1 data)";
   o << "\n";
   o << "\n";
   o << "Input for comparison can be specified the same way, using the '-2' or '--comp' option\n";
   o << "\n";
   o << "\n";
   o << "Output formats can be json or cool. In case a second input is specified for comparison, the output will be on screen or an xml file with the differences\n";
   o << "  -o r3json [<test>]                              ... will produce Run 3 config files L1PrescalesSet[_<test>].json, BunchGroupSet[_<test>].json, L1Menu[_<test>].json, HLTPrescalesSet[_<test>].json, and HLTMenu[_<test>].json\n";
   o << "  -o cool                                         ... will produce trig_cool.db with cool db instance CONDBR2 and infinite IOV\n";
   o << "  -o cool 200000                                  ... will produce trig_cool.db with cool db instance CONDBR2 and run number 200000\n";
   o << "  -o cool test [200000]                           ... will produce trig_cool_test.db with cool db instance CONDBR2 [and run number 200000]\n";
   o << "  -o cool ../test.db [200000]                     ... will produce ../test.db with cool db instance CONDBR2 [and run number 200000]\n";
   o << "  -o cool 'test;COMP200' [200000]                 ... will produce Menu_test.db with cool db instance COMP200 [and run number 200000]\n";
   o << "\n";
   o << "================================================================================\n";
}

struct JobConfig {
   enum Format { UNDEF=0x00, DB=0x01, COOL=0x02, XML=0x04, JSON=0x08 };
   enum ETriggerLevel { LVL1 = 0, HLT = 1, NONE = 2 };

   std::vector<std::string>  inpar, inpar2, outpar;

   Format       input {UNDEF};
   Format       input2 {UNDEF};
   unsigned int output {UNDEF};
   string       db{""}, db2{""};
   vector<unsigned int> keys, keys2; // smk[,l1key[,hltkey[,bgkey]]]
   string       outBase {""};

   string       l1JsonOutFile {"L1Menu.json"};
   string       bgkJsonOutFile {"BunchGroupSet.json"};
   string       l1PSJsonOutFile { "L1PrescalesSet.json" };
   string       hltJsonOutFile { "HLTMenu.json" };
   string       hltPSJsonOutFile { "HLTPrescalesSet.json" };

   string       coolInputConnection { "" };
   string       coolOutputConnection { "" };
   unsigned int coolOutputRunNr { 0 };

   // other
   bool         help {false};
   int          printlevel {1};
   MSGTC::Level outputlevel {MSGTC::WARNING};
   bool         jo {false};
   bool         fw {false};
   string       logFileName {""};
   bool         merge = { true };
   vector<string> error;


   string CheckForCompleteSetup();

   void PrintSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ));

   void parseProgramOptions(int argc, char* argv[]);

   unsigned int getKey(unsigned int which) { return keys.size()>which?keys[which]:0; }

   unsigned int getKey2(unsigned int which) { return keys2.size()>which?keys2[which]:0; }

};


namespace {
   bool startswith(const std::string& str, const std::string& sub) {
      if(str.size()<sub.size())
         return false;
      return (str.compare(0,sub.size(),sub) == 0);
   }

   bool isUnsignedInteger(const std::string& str) {
      for(const char c : str)
         if(c<'0' || c>'9') return false;
      return true;
   }
}


void
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
                stripped == "2" || stripped == "comp" ||
                stripped == "o" || stripped == "output" ||
                stripped == "l" || stripped == "log" ||
                stripped == "p" || stripped == "print" ||
                stripped == "jo" ||
                stripped == "fw" ||
                stripped == "nomerge" ||
                stripped == "v" || stripped == "loglevel") ) {
            listofUnknownParameters += " " + currInput;
            continue;
         }

      if(isParam) {
         currentPar = "";
         if(stripped == "h" || stripped == "help" )        { help = true; continue; }
         if(stripped == "jo")                              { jo = true; continue; }
         if(stripped == "fw")                              { fw = true; continue; }
         if(stripped == "nomerge") { merge = false; continue; }
         currentPar = stripped;
      } else {
         if(currentPar == "i" || currentPar == "input")     { inpar.push_back(stripped); continue; }
         if(currentPar == "2" || currentPar == "comp")      { inpar2.push_back(stripped); continue; }
         if(currentPar == "o" || currentPar == "output")    {
            if(outpar.size()==0 && stripped != "r3json" && stripped != "cool") {
               error.push_back("Unknown output type: " + stripped + ". Must be either json or cool, optionally followed by a base string for the output file name");
            } else {
               outpar.push_back(stripped);
            }
            continue; }
         if(currentPar == "l" || currentPar == "log")       { logFileName = stripped; continue; }
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
      error.push_back("No input specified, use '-i' option");


   // parse the input
   if( inpar.size()>=1 && inpar[0].find(".db") != string::npos ) {
      // sqlite file
      input = COOL;
      vector<string> ksv;
      boost::split(ksv,inpar[0],boost::is_any_of(";"));
      if(ksv.size()==1) { // defaults to CONDBR2
         coolInputConnection = "sqlite://;schema="+ksv[0]+";dbname=CONDBR2";
      } else {
         coolInputConnection = "sqlite://;schema="+ksv[0]+";dbname=" + ksv[1];
      }
   } else if( inpar.size()>=1 && startswith(inpar[0],"COOLONL_TRIGGER") ) {
      input = COOL;
      vector<string> ksv;
      boost::split(ksv,inpar[0],boost::is_any_of("/"));
      if(ksv.size()==1) {
         coolInputConnection = ksv[0]+"/CONDBR2";
      } else {
         coolInputConnection = ksv[0]+"/" + ksv[1];
      }

   } else if( inpar.size()==2 ) {
      input = DB;
      db = inpar[0];
      vector<string> ksv;
      boost::split(ksv,inpar[1],boost::is_any_of(","));
      for(const string& ks: ksv) {
         keys.push_back( boost::lexical_cast<unsigned int,string>(ks) );
      };
   }

   // parse the output
   for(const string& o: outpar) {
      if ( o=="r3json" ) {
         output |= JSON;
      } else if ( o=="cool" ) {
         output |= COOL;
      } else if ( isUnsignedInteger(o) ) {
         coolOutputRunNr = boost::lexical_cast<unsigned int,string>(o);
      } else {
         outBase = o;
      }
   }

   if (outBase != "") {
      if( output & JobConfig::COOL ) {
         string dbname = "CONDBR2";
         string outfile = outBase;
         vector<string> ksv;
         boost::split(ksv,outBase,boost::is_any_of(";"));
         if(ksv.size()==2) { // defaults to CONDBR2
            outfile = ksv[0];
            dbname = ksv[1];
         }
         if (outfile.find("/")==string::npos && outfile.find('.')==string::npos) {
            outfile="trig_cool_" + outfile + ".db";
         }
         coolOutputConnection = "sqlite://;schema="+outfile+";dbname="+dbname;
      } else {
         boost::replace_last(l1JsonOutFile, ".json", "_"+outBase+".json");
         boost::replace_last(bgkJsonOutFile, ".json", "_"+outBase+".json");
         boost::replace_last(l1PSJsonOutFile, ".json", "_"+outBase+".json");
         boost::replace_last(hltJsonOutFile, ".json", "_"+outBase+".json");
         boost::replace_last(hltPSJsonOutFile, ".json", "_"+outBase+".json");
      }
   }

}


string JobConfig::CheckForCompleteSetup() {
   if(input==UNDEF)
      return "Use argument '-i' to specify input source and check that the input is specified correctly";
   if( input == DB ) {
      if( db == "" )
         return "No TriggerDB connection string specified";
      if( keys.size()==0 )
         return "No configuration key(s) specified";
   }
   if( input2 == DB ) {
      if( db2 == "" )
         return "No TriggerDB connection string specified for comparison, use option '--db2'";
      if( keys2.size()==0 )
         return "No smk specified for comparison, use option '--dbsmk2'";
   }
   return "";
}


void
JobConfig::PrintSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ) ) {
   log << "========================================" << lineend;
   log << "JOB SETUP: " << (input==DB?"DB":"COOL");
   if(output!=UNDEF)
      log << " --> " << (output==COOL?"COOL":"JSON");
   log << lineend;
   log << "----------" << lineend;
   log << "   Input               : ";
   for(string s: inpar) log << s << ", ";
   log << lineend;
   if( input2 != UNDEF ) {
      log << "   Input for comparison: ";
      for(string s: inpar2) log << s << ", ";
      log << lineend;
   }
   if( output != UNDEF ) {
      log << "   Output              : ";
      if( output&COOL ) {
         log << coolOutputConnection;
         if(coolOutputRunNr==0) { log << ", infinite IOV"; } else { log << ", run nr " << coolOutputRunNr; }
      }
      log << lineend;
   }
   if( printlevel>=0)
      log << "   Print menu detail   : " << printlevel << lineend;
   log << "========================================" << lineend;

}


int main( int argc, char* argv[] ) {

   /***************************************
    *
    * Getting the program parameters
    *
    ***************************************/
   JobConfig gConfig;
   gConfig.parseProgramOptions(argc, argv);
   if(gConfig.help) {
      printhelp(cout, endl);
      return 0;
   }
   if(gConfig.error.size()!=0) {
      for(const string& e: gConfig.error)
         cerr << e << endl;
      printhelp(cout, endl);
      return 1;
   }

   ofstream *outf(0), *errf(0);
   if(gConfig.logFileName != "") {
      string outfn = gConfig.logFileName+".out";
      string errfn = gConfig.logFileName+".err";
      outf = new ofstream(outfn.c_str());
      errf = new ofstream(errfn.c_str());
   }
   ostream& log = (outf==0?cout:*outf);
   ostream& logerr = (errf==0?cerr:*errf);
   ostream& (*lineend) ( ostream& os ) = &endl;

   string check = gConfig.CheckForCompleteSetup();
   if( check != "" ) {
      logerr << lineend << "===> Error in the option specification: " << check << lineend << lineend;
      printhelp(log, lineend);
      if(outf) outf->close();
      if(errf) errf->close();
      return 1;
   }

   gConfig.PrintSetup(log, lineend);


   /***************************************
    *
    * Reading
    *
    ***************************************/
   CTPConfig* ctpc(0);
   BunchGroupSet* bgs(nullptr);
   HLTFrame* hltFrame(0);
   TXC::L1TopoMenu* l1tm = nullptr;
   uint smk(0),l1psk(0),hltpsk(0),bgsk(0), mck{0};
   string release;


   /*------------------
    * from DB
    *-----------------*/
   if (gConfig.input == JobConfig::DB) {
      unique_ptr<StorageMgr> sm(new StorageMgr(gConfig.db, "", "", log));

      // Loading L1 topo
      //log << "Retrieving Lvl1 Topo configuration" << lineend;
      l1tm  = new TXC::L1TopoMenu();
      l1tm->setSMK(gConfig.getKey(0));
      sm->masterTableLoader().setLevel(gConfig.outputlevel);
      sm->masterTableLoader().load(*l1tm);

      //log << "Retrieving Lvl1 CTP configuration" << lineend;
      ctpc = new TrigConf::CTPConfig();
      ctpc->setSMK( gConfig.getKey(0) );
      ctpc->setPrescaleSetId( gConfig.getKey(1) );
      ctpc->setBunchGroupSetId( gConfig.getKey(3) );
      sm->menuLoader().setEnv(IMenuLoader::CTPOnl);
      ctpc->setLoadCtpFiles(gConfig.fw); // load CTP files ?
      sm->masterTableLoader().setLevel( gConfig.outputlevel );
      sm->masterTableLoader().load(*ctpc);
      ctpc->muCTPi().setSMK( gConfig.getKey(0) );
      sm->masterTableLoader().load( ctpc->muCTPi() );

      //log << "Retrieving HLT menu configuration and prescale set from the TriggerDB" << lineend;
      hltFrame = new HLTFrame();
      hltFrame->setSMK( gConfig.getKey(0) );
      if( gConfig.getKey(2)>0 )
         hltFrame->thePrescaleSetCollection().set_prescale_key_to_load( gConfig.getKey(2) );
      sm->hltFrameLoader().setLevel( gConfig.outputlevel );
      sm->hltFrameLoader().load( *hltFrame );

      smk    = gConfig.getKey(0);
      l1psk  = gConfig.getKey(1);
      hltpsk = gConfig.getKey(2);
      bgsk   = gConfig.getKey(3);

      // loading attached MCK
      auto mckloader = new MCKLoader(*sm);
      mckloader->loadMCKlinkedToSMK(smk, mck);
      if ( mck != 0 ) {
         mckloader->loadReleaseLinkedToMCK(mck,release);
        log << "Loaded MCK " << mck << " (active for SMK " << smk << " and release " << release << ")" << endl;
      } else {
         log << "Did not load MCK from DB as MCK is 0 or no MCK is linked";
      }

   }
   /*------------------
    * from COOL
    *-----------------*/
   else if (gConfig.input == JobConfig::COOL) {
      string coolInputConnection = gConfig.coolInputConnection;
      unsigned int runnumber =  gConfig.inpar.size()>1 ? boost::lexical_cast<unsigned int,string>(gConfig.inpar[1]) : 1;
      unsigned int lb =  gConfig.inpar.size()>2 ? boost::lexical_cast<unsigned int,string>(gConfig.inpar[2]) : 0;
      log << "TrigConfReadWrite Reading cool : " << coolInputConnection << lineend;
      log << "                  run number   : " << runnumber << lineend;
      log << "                  lb           : " << lb << lineend;
      TrigConfCoolWriter * coolWriter = new TrigConfCoolWriter( coolInputConnection );
      string configSource("");
      ctpc = new CTPConfig();
      coolWriter->readL1Payload( runnumber, *ctpc);

      PrescaleSet ps;
      coolWriter->readL1PrescalePayload( runnumber, lb, l1psk, ps);
      ctpc->setPrescaleSet( ps );

      //       log << "L1 PSK 0  " << ps.id() << lineend;
      //       log << "L1 PSK 1  " << l1psk << lineend;
      //       log << "L1 PSK 2  " << ctpc->prescaleSet().id() << lineend;
      //       log << "L1 PSK 3  " << ctpc->prescaleSetId() << lineend;   <---- does not work

      int bgKey(0);
      coolWriter->readL1BunchGroupLBPayload( runnumber, lb, bgKey, ctpc->bunchGroupSet() );

      hltFrame = new HLTFrame();
      coolWriter->readHLTPayload(runnumber, *hltFrame);

      if(lb!=0) {
         HLTPrescaleSet * pss = new HLTPrescaleSet();
         coolWriter->readHltPrescalePayload( runnumber, lb, *pss);
         hltpsk = pss->id();
         hltFrame->thePrescaleSetCollection().addPrescaleSet( lb, pss );
      }

      smk    = hltFrame->smk();
      bgsk   = bgKey;

   }

   if(gConfig.printlevel>=0) {
      log << "Loaded this configuration" << lineend;
      log << "    SMK     " << smk << lineend;
      log << "    L1 PSK  " << l1psk << lineend;
      log << "    HLT PSK " << hltpsk << lineend;
      log << "    BGSK    " << bgsk << lineend;
      if(ctpc)     ctpc->print("  ", gConfig.printlevel);
      if(bgs)      bgs->print("  ", gConfig.printlevel);
      if(hltFrame) hltFrame->print("  ", gConfig.printlevel);
   }


   if (gConfig.input2 != JobConfig::UNDEF) {
      CTPConfig* ctpc2(0);
      HLTFrame* hltFrame2(0);

      /*------------------
       * from DB
       *-----------------*/
      if (gConfig.input2 == JobConfig::DB) {
         TrigConf::StorageMgr *sm = new TrigConf::StorageMgr(gConfig.db2, "", "", log);

         log << "Retrieving Lvl1 CTP configuration for comparison" << lineend;
         ctpc2 = new TrigConf::CTPConfig();
         ctpc2->setSMK( gConfig.getKey2(0) );
         ctpc2->setPrescaleSetId( gConfig.getKey2(1) );
         ctpc2->setBunchGroupSetId( gConfig.getKey2(3) );
         sm->menuLoader().setEnv(IMenuLoader::CTP);
         sm->masterTableLoader().setLevel(gConfig.outputlevel);
         sm->masterTableLoader().load(*ctpc2);
         ctpc2->muCTPi().setSMK( gConfig.getKey2(0) );
         sm->masterTableLoader().load(ctpc2->muCTPi());

         log << "Retrieving HLT menu configuration and prescale set from the TriggerDB for comparison" << lineend;
         hltFrame2 = new HLTFrame();
         hltFrame2->setSMK( gConfig.getKey2(0) );
         if( gConfig.getKey2(2)>0)
            hltFrame2->thePrescaleSetCollection().set_prescale_key_to_load( gConfig.getKey2(2) );
         sm->hltFrameLoader().load( *hltFrame2 );
         delete sm;
      } else if (gConfig.input2 == JobConfig::COOL) {
         /*------------------
          * from COOL
          *-----------------*/
      }

      if(ctpc && ctpc2) {
         bool equalMenus = ctpc->equals(ctpc2, "LVL1config_Diff.xml");
         if(equalMenus) {
            log << "LVL1 menus are identical." << lineend;
         } else {
            log << "LVL1 menus differ. Writing LVL1 menu comparison file LVL1config_Diff.xml" << lineend;
         }
      }
      if(hltFrame && hltFrame2) {
         bool equalMenus = hltFrame->equals(hltFrame2, "HLTconfig_Diff.xml");
         if(equalMenus) {
            log << "HLT menus are identical." << lineend;
         } else {
            log << "HLT menus differ. Writing HLT menu comparison file HLTconfig_Diff.xml" << lineend;
         }
      }

   }


   // ========================================
   // Writing
   // ========================================
   if ( (gConfig.output & JobConfig::JSON) != 0 ) {
      /*------------------
       * to JSON
       *-----------------*/
      if(ctpc && l1tm) {
         convertRun2L1MenuToRun3(ctpc, l1tm, gConfig.l1JsonOutFile);
         convertRun2L1PrescalesToRun3(ctpc, gConfig.l1PSJsonOutFile);
         convertRun2BunchGroupsToRun3(ctpc, gConfig.bgkJsonOutFile);
      }
      if(hltFrame) {
         convertRun2HLTMenuToRun3(hltFrame, gConfig.hltJsonOutFile);
         convertRun2HLTPrescalesToRun3(hltFrame, gConfig.hltPSJsonOutFile);
      }
   }

   if ( (gConfig.output & JobConfig::COOL) != 0 ) {
      /*------------------
       * to COOL
       *-----------------*/
      log << "TrigConfReadWrite:                Writing sqlite cool file : " << gConfig.coolOutputConnection << " with ";
      if( gConfig.coolOutputRunNr==0 ) { log << "infinite IOV"; } else { log << " runNr " << gConfig.coolOutputRunNr; }
      log << lineend;
      TrigConfCoolWriter * coolWriter = new TrigConfCoolWriter( gConfig.coolOutputConnection );
      string configSource("");
      string info("");
      unsigned int runNr = gConfig.coolOutputRunNr;
      if(runNr == 0) { runNr = 0x80000000; } // infinite range

      if(ctpc) {
         coolWriter->writeL1Payload(runNr, *ctpc);
      }
      try{
         if(hltFrame) {
   	      coolWriter->writeHLTPayload(runNr, *hltFrame, configSource);
         }
      }
      catch(const cool::StorageTypeStringTooLong& e){
         log << "FATAL: Unable to write data to COOL";
         return 1;
      }
      if(mck) {
         coolWriter->writeMCKPayload(runNr, mck, release, info);
      }
   }

   delete ctpc;
   delete hltFrame;
   if(l1tm!=nullptr)
      delete l1tm;

   if( gConfig.jo ) {

      JobOptionTable jot;
      unique_ptr<IStorageMgr> sm( new StorageMgr(gConfig.db,"","",log) );

      log << "TrigConfReadWrite:                  Retrieving JO from the TriggerDB" << lineend;
      jot.setSMK( gConfig.getKey(0) );
      jot.setTriggerLevel(0); // L2
      sm->jobOptionTableLoader().setLevel(gConfig.outputlevel);
      sm->jobOptionTableLoader().load( jot );
      if(gConfig.printlevel>0) jot.print();
   }

}
