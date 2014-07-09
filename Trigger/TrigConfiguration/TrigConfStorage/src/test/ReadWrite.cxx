/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
// trigger configuration (L1+HLT) from DB,XML,COOL and to XML or COOL
// for test purposes
//
//////////////////////////////////////////////////////////////////////

#include "./Helper.h"

#include "TrigConfBase/MsgStream.h"

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/XMLStorageMgr.h"

#include "TrigConfStorage/TrigConfCoolWriter.h"

#include "TrigConfStorage/DBLoader.h"
#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/TrigConfCoolWriter.h"

#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfJobOptData/JobOptionTable.h"

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
  o << "  -i|--input        input [input [input]]       ... source of configuration (mandatory)\n";
  o << "  -2|--comp         input [input [input]]       ... source of a second configuration for comparison\n";
  o << "  -o|--output       xml|cool [output[;cooldb]]  ... output format, name\n";
  o << "                                                ... absolute output file name must contain '/', cooldb can be appended COMP200|OFLP200\n";
  o << "  -v|--loglevel     <string>                    ... log level [NIL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS]\n";
  o << "  -l|--log          <string>                    ... name of a log file\n";
  o << "  --jo                                          ... read and write job options where possible\n";
  o << "  -p|--print        <int>                       ... print configuration with detail 0...5 (default 1)\n";
  o << "  -h|--help                                     ... this output\n";
  o << "  --nomerge                                     ... internally don't merge L2 and EF (by default merge is enabled)\n";
  o << "\n";
  o << "Input can be specified the following\n";
  o << "  -i [l1menu.xml] [hltmenu2.xml]                   ... to read L1 and/or HLT menu from XML [file names must end with '.xml'\n";
  o << "  -i DBALIAS|dbconnection smk[,l1psk,hltpsk,bgsk]  ... to read the menu from a trigger db via alias or explicit connection specification (ORACLE or SQlite)\n";
  o << "  -i COOLDBALIAS|cooldbconnection run[,lb]         ... to read the menu from COOL for a certain run and possbly LB\n";
  o << "\n";
  o << "The dbconnection can be specified as following\n";
  o << "  'sqlite:TriggerMenuSQLiteFile_rel1.sqlite'\n";
  o << "\n";
  o << "\n";
  o << "\n";
  o << "Input for comparison can be specified the same way, using the '-2' option\n";
  o << "\n";
  o << "Output formats can be xml or cool. In case a second input is specified for comparison, the output will be on screen or an xml file with the differences\n";
  o << "  -o xml test                                     ... will produce LVL1config_test.xml and/or HLTconfig_test.xml. When\n";
  o << "                                                      comparing two menus this will produce Diff_test.xml. In this case the\n";
  o << "                                                      specification of '-o test' is sufficient\n";
  o << "  -o cool test                                    ... will produce Menu_test.db\n";
  o << "\n";
  o << "================================================================================\n";
}

class JobConfig {
public:
   enum Format { UNDEF=0x00, DB=0x01, COOL=0x02, XML=0x04 };
   enum ETriggerLevel { LVL1 = 0, HLT = 1, NONE = 2 };
   ~JobConfig(){}
   JobConfig() {}

   std::vector<std::string>  inpar, inpar2, outpar;

   Format       input {UNDEF};
   Format       input2 {UNDEF};
   unsigned int output {UNDEF};
   string       db{""}, db2{""};
   vector<unsigned int> keys, keys2; // smk[,l1key[,hltkey[,bgkey]]]
   string       outBase {""};
   string       l1xmlOutFile { "LVL1Config.xml" };
   string       hltxmlOutFile { "HLTConfig.xml" };
   string       coolOutFile { "trig_cool.db" };
   string       coolDbname { "COMP200" };

   // other
   bool         help {false};
   int          printlevel {-1};
   MSGTC::Level outputlevel {MSGTC::WARNING};
   bool         jo {false};
   string       logFileName {""};
   bool         merge = { true };
   vector<string> error;


   string CheckForCompleteSetup();

   void PrintSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ));

   void parseProgramOptions(int argc, char* argv[]);

   unsigned int getKey(unsigned int which) { return keys.size()>which?keys[which]:0; }

   unsigned int getKey2(unsigned int which) { return keys2.size()>which?keys2[which]:0; }

} gConfig;


namespace {
   bool endswith(const std::string& str, const std::string& sub) {
      if(str.size()<sub.size())
         return false;
      return (str.compare(str.size()-sub.size(),sub.size(),sub) == 0);
   }

   bool startswith(const std::string& str, const std::string& sub) {
      if(str.size()<sub.size())
         return false;
      return (str.compare(0,sub.size(),sub) == 0);
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
                stripped == "nomerge" ||
                stripped == "v" || stripped == "loglevel") ) {
            listofUnknownParameters += " " + currInput;
            continue;
         }

      if(isParam) {
         currentPar = "";
         if(stripped == "h" || stripped == "help" )        { help = true; continue; }
         if(stripped == "nomerge") { merge = false; continue; }
         currentPar = stripped;
      } else {
         if(currentPar == "i" || currentPar == "input")     { inpar.push_back(stripped); continue; }
         if(currentPar == "2" || currentPar == "comp")      { inpar2.push_back(stripped); continue; }
         if(currentPar == "o" || currentPar == "output")    { 
            if(outpar.size()==0 && stripped != "xml" && stripped != "cool") {
               error.push_back("Unknown output type: " + stripped + ". Must be either xml or cool, optionally followed by a base string for the output file name");
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

   if( (inpar.size()==1 && endswith(inpar[0],".xml")) || 
       (inpar.size()==2 && endswith(inpar[1],".xml") && endswith(inpar[1],".xml")) ) {
      input = XML;
   } else if( inpar.size()>=1 && ( endswith(inpar[0],".db") || startswith(inpar[0],"COOLONL_TRIGGER") ) ) {
      input = COOL;
   } else if( inpar.size()==2 ) {
      input = DB;
      db = inpar[0];
      vector<string> ksv;
      boost::split(ksv,inpar[1],boost::is_any_of(","));
      for(const string& ks: ksv) {
         keys.push_back( boost::lexical_cast<unsigned int,string>(ks) );
      };
   }

   if( (inpar2.size()==1 && endswith(inpar2[0],".xml")) || 
       (inpar2.size()==2 && endswith(inpar2[1],".xml") && endswith(inpar2[1],".xml")) ) {
      input2 = XML;
   }


   for(const string& o: outpar) {
      if(o=="xml") { output |= XML; }
      else if(o=="cool") { output |= COOL; }
      else { outBase = o; }
   }

   if (outBase != "") {
      if( output & JobConfig::COOL ) {
         string out = outBase;
         size_t sempos = outBase.find(";");
         if (sempos !=  string::npos) {
            coolDbname = outBase.substr(sempos+1);
            out = outBase.substr(0,sempos);
         }
         if (out.find("/") !=  string::npos) {
            coolOutFile = out;
         } else {
            coolOutFile="trig_cool_" + out + ".db";
         }
      } else {
         l1xmlOutFile  = "LVL1Config_" + outBase + ".xml";
         hltxmlOutFile = "HLTConfig_" + outBase + ".xml";
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
   log << "JOB SETUP: " << (input==DB?"DB":(input==COOL?"COOL":"XML"));
   if(output!=UNDEF)
      log << " --> " << (output==COOL?"COOL":"XML");
   log << lineend;
   log << "----------" << lineend;
   log << "   Input               : ";
   for(string s: inpar) log << s << ", "; log << lineend;
   if( input2 != UNDEF ) {
      log << "   Input for comparison: ";
      for(string s: inpar2) log << s << ", "; log << lineend;
   }
   if( output != UNDEF ) {
      log << "   Output              : ";
      if( output&XML )  log << l1xmlOutFile << ", " << hltxmlOutFile;
      if( output&COOL ) log << coolOutFile << "; " << coolDbname;
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
   ostream& logerr = (outf==0?cerr:*errf);
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
   HLTFrame* hltFrame(0);

   /*------------------
    * from DB
    *-----------------*/
   if (gConfig.input == JobConfig::DB) {
      unique_ptr<StorageMgr> sm(new StorageMgr(gConfig.db, "", "", log));
      log << "Retrieving Lvl1 CTP configuration" << lineend;
      ctpc = new TrigConf::CTPConfig();
      ctpc->setSMK( gConfig.getKey(0) );
      ctpc->setPrescaleSetId( gConfig.getKey(1) );
      ctpc->setBunchGroupSetId( gConfig.getKey(3) );
      DBLoader::setEnv(DBLoader::CTP);
      sm->masterTableLoader().setLevel(gConfig.outputlevel);
      sm->masterTableLoader().load(*ctpc);
      ctpc->muCTPi().setSMK( gConfig.getKey(0) );
      sm->masterTableLoader().load( ctpc->muCTPi() );

      log << "Retrieving HLT menu configuration and prescale set from the TriggerDB" << lineend;
      hltFrame = new HLTFrame();
      hltFrame->setSMK( gConfig.getKey(0) );
      if( gConfig.getKey(2)>0 )
         hltFrame->thePrescaleSetCollection().set_prescale_key_to_load( gConfig.getKey(2) );
      sm->hltFrameLoader().load( *hltFrame );
   }
   /*------------------
    * from XML
    *-----------------*/
   else if (gConfig.input == JobConfig::XML) {
      unique_ptr<XMLStorageMgr> sm( gConfig.inpar.size()==1 ?
                                    new XMLStorageMgr( { xmlpathresolve(gConfig.inpar[0]) } ) :
                                    new XMLStorageMgr( { xmlpathresolve(gConfig.inpar[0]), xmlpathresolve(gConfig.inpar[1]) } )
                                    );

      sm->setLevel(MSGTC::VERBOSE);

      if(sm->hasLVL1()) {
         ctpc = new CTPConfig();
         log << "Retrieving Lvl1 CTP configuration from " << sm->m_xmlL1File << lineend;
         DBLoader::setEnv(DBLoader::CTP);
         sm->masterTableLoader().setLevel(gConfig.outputlevel);
         sm->masterTableLoader().load(*ctpc);
         sm->masterTableLoader().load(ctpc->muCTPi());
         log << "Done reading " << sm->m_xmlL1File << lineend;

      }
      if(sm->hasHLT()) {
         hltFrame = new HLTFrame();
         hltFrame->setMergedHLT(gConfig.merge);
         log << "Retrieving HLT menu from " << sm->m_xmlHLTFile << lineend;
         sm->hltFrameLoader().setLevel(gConfig.outputlevel);
         sm->hltFrameLoader().load( *hltFrame );
         log << "Done reading " << sm->m_xmlHLTFile << lineend;
      }
   }
   /*------------------
    * from COOL
    *-----------------*/
   else if (gConfig.input == JobConfig::COOL) {
      string coolinput = gConfig.inpar[0];
      unsigned int runnumber =  gConfig.inpar.size()>1 ? boost::lexical_cast<unsigned int,string>(gConfig.inpar[1]) : 1;
      string connection("");
      if(coolinput.find('.') != string::npos) {
         connection = "sqlite://;schema="+coolinput+";dbname=COMP200";
         log << "TrigConfReadWrite:                Reading cool from file " << coolinput << " with schema name COMP200" << lineend;
      } else {
         connection = coolinput;
         log << "TrigConfReadWrite:                Reading cool using dblookup with alias " << coolinput << " with schema name COMP200" << lineend;
      }
      TrigConfCoolWriter * coolWriter = new TrigConfCoolWriter( connection );
      string configSource("");
      ctpc = new CTPConfig();
      coolWriter->readL1Payload( runnumber, *ctpc);
      hltFrame = new HLTFrame();
      coolWriter->readHLTPayload(runnumber, *hltFrame);
   }

   if(gConfig.printlevel>=0) {
      if(ctpc)     ctpc->print("  ",gConfig.printlevel);
      if(hltFrame) hltFrame->print("  ",gConfig.printlevel);
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
         TrigConf::DBLoader::setEnv(TrigConf::DBLoader::CTP);
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
      }
      /*------------------
       * from XML
       *-----------------*/
      else if (gConfig.input2 == JobConfig::XML) {
         unique_ptr<XMLStorageMgr> sm( gConfig.inpar2.size()==1 ?
                                       new XMLStorageMgr( { xmlpathresolve(gConfig.inpar2[0]) } ) :
                                       new XMLStorageMgr( { xmlpathresolve(gConfig.inpar2[0]),xmlpathresolve(gConfig.inpar2[1]) } ) );
         
         if(sm->hasLVL1()) {
            ctpc2 = new CTPConfig();
            log << "Retrieving Lvl1 CTP configuration from " << sm->m_xmlL1File << lineend;
            DBLoader::setEnv(DBLoader::CTP);
            sm->masterTableLoader().load(*ctpc2);
            sm->masterTableLoader().load( ctpc2->muCTPi() );
            log << "Done reading " << sm->m_xmlL1File << lineend;
         }

         if(sm->hasHLT()) {
            hltFrame2 = new HLTFrame();
            hltFrame2->setMergedHLT(gConfig.merge);
            log << "Retrieving HLT menu from " << sm->m_xmlHLTFile << lineend;
            sm->hltFrameLoader().load( *hltFrame2 );
            log << "Done reading " << sm->m_xmlHLTFile << lineend;
         }
      
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
   if ( (gConfig.output & JobConfig::XML) != 0 ) {
      /*------------------
       * to XML
       *-----------------*/

      if(ctpc) {
         log << "TrigConfReadWrite:                Writing LVL1 menu " << gConfig.l1xmlOutFile << lineend;
         ctpc->writeXML(gConfig.l1xmlOutFile, 2);
      }

      if(hltFrame) {
         std::ofstream xmlfile;
         xmlfile.open(gConfig.hltxmlOutFile);
         log << "TrigConfReadWrite:                Writing HLT menu " << gConfig.hltxmlOutFile << lineend;
         hltFrame->writeXML(xmlfile);
         xmlfile.close();
      }
   }
   if ( (gConfig.output & JobConfig::COOL) != 0 ) {
      /*------------------
       * to COOL
       *-----------------*/
      log << "TrigConfReadWrite:                Writing sqlite cool file " << gConfig.coolOutFile << " with schema name " << gConfig.coolDbname << lineend;
      TrigConfCoolWriter * coolWriter = new TrigConfCoolWriter( "sqlite://;schema="+gConfig.coolOutFile+";dbname="+gConfig.coolDbname );
      string configSource("");
      if(ctpc) {
         coolWriter->writeL1Payload(0x80000000, // infinite range
                                    *ctpc);
      }
      if(hltFrame) {
         coolWriter->writeHLTPayload(0x80000000, // infinite range
                                     *hltFrame,
                                     configSource);
      }
   }

    
   if( gConfig.jo ) {
      JobOptionTable jotl2,jotef;
      unique_ptr<IStorageMgr> sm( new StorageMgr(gConfig.db,"","",log) );
      
      log << "TrigConf2XMLApp:                  Retrieving L2 JO from the TriggerDB" << lineend;
      jotl2.setSuperMasterTableId( gConfig.getKey(0) );
      jotl2.setTriggerLevel(0); // L2
      sm->jobOptionTableLoader().load( jotl2 );
      if(gConfig.printlevel>0) jotl2.print();
      jotl2.writeToFile( string("L2JO_"+gConfig.outBase+".xml").c_str() );
      
      log << "TrigConf2XMLApp:                  Retrieving EF JO from the TriggerDB" << lineend;
      jotef.setSuperMasterTableId( gConfig.getKey(0) );
      jotef.setTriggerLevel(1); // EF
      sm->jobOptionTableLoader().load( jotef );
      if(gConfig.printlevel>0) jotef.print();
      jotef.writeToFile( string("EFJO_"+gConfig.outBase+".xml").c_str() );

   }

}

