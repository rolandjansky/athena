/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     TrigConfConsistencyChecker.cxx 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHORs:  J.Stelzer (CERN)	  Joerg.Stelzer@cern.ch 
//          T.Bold (CERN)	      Tomasz.Bold@cern.ch 
//CREATED:  29. July 2007
//                                                     
//PURPOSE: 
//
// This standalone application is designed to read the trigger
// configuration (L1+HLT) from various sources and check its consistency.
//
//////////////////////////////////////////////////////////////////////

#include "../XMLBoostHelper.h"


#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <ctime>
#include <map>
#include <sys/stat.h> 
#include <set>

#include "boost/lexical_cast.hpp"
#include "boost/regex.hpp"

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/XMLStorageMgr.h"
#include "TrigConfStorage/TrigConfCoolWriter.h"

#include "TrigConfL1Data/L1DataDef.h"

#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChainList.h"

#include "TrigConfStorage/ConfigurationCheck.h"


using namespace std;
using namespace TrigConf;
using namespace boost::property_tree;

std::string findInPath(const std::string& filename, const std::string & path) {
   struct stat file_stat;
   std::string fullname = path + "/" + filename;
   if (stat (fullname.c_str(), &file_stat) == 0) return fullname;
   return "";
}

std::vector<std::string> splitpath(std::string line, std::string del=":")
{
  std::vector<std::string> res;
  std::string::size_type old_pos = 0, pos = line.find(del,0);
  while( pos != std::string::npos ) {
    res.push_back(line.substr(old_pos,pos-old_pos));
    old_pos = pos + del.size();
    pos = line.find(del, old_pos);
  }
  // last entry
  if (old_pos < line.size())
    res.push_back(line.substr(old_pos,line.size()-old_pos));
  return res;
}

std::string pathresolve(const std::string& filename, const std::string & searchpath) {
   std::string fullname = findInPath(filename,".");
   if( fullname != "" ) { return fullname; }
   std::vector<std::string> listofpaths = splitpath(searchpath);
   std::vector<std::string>::const_iterator path    = listofpaths.begin();
   std::vector<std::string>::const_iterator pathEnd = listofpaths.end();
   for(;path!=pathEnd;path++) {
      fullname = findInPath(filename,*path);
      if( fullname != "" ) { return fullname; }      
   }
   return "";
}

std::string xmlpathresolve(const std::string& filename) {
   // if path starts with '/' then it is absolute
   if( filename.find('/') == 0 ) return filename;

   std::string xmlpath = ::getenv("XMLPATH");
   if(filename.find('/')==std::string::npos) {
      // if there is no "/" in the filename we try to things
      std::string fullname = pathresolve(filename, xmlpath);
      if(fullname == "")
         fullname = pathresolve("TriggerMenuXML/"+filename, xmlpath);
      return fullname;
   } else {
      return pathresolve(filename, xmlpath);
   }
}



// --------------------------------------------------------------------------------
// printhelp
// --------------------------------------------------------------------------------
void printhelp(std::ostream & o) {
  o << "================================================================================" << std::endl;
  o << "| The program needs to be run with the following specifications:" << std::endl;
  o << "| TrigConf2CoolApp.exe <arguments>" << std::endl << std::endl;
  o << "| where the arguments are ( * required ):" << std::endl;
  o << "| ------------------------------------------------------------------------------" << std::endl;
  o << "| " << std::endl;
  o << "| " << std::endl;
  o << "| Input sources [in order of precedence xml, triggerdb, cooldb]" << std::endl;   
  o << "| -------------------------------------------------------------" << std::endl;   
  o << "|   --lvl1xml         <string>      LVL1 XML configuration files" << std::endl;
  o << "|   --hltxml          <string>      HLT XML configuration files" << std::endl;
  o << "| " << std::endl;                    
  o << "|   --trigdb          [<string>]    Trigger DB (e.g. TRIGGERDB or 'oracle://ATLR;')" << std::endl;
  o << "|                                   online TRIGGERDB taken if connection omitted" << std::endl;
  o << "|   --keys            <int int>     Trigger configuration keys (two arguments smk, hlt-psk)" << std::endl;
  o << "| " << std::endl;                    
  o << "|   --cool            [<string>]    read from COOL, format: 'oracle://<server>;schema=<schema>;dbname=<DBNAME>;user=<user>;password=<pw>'" << std::endl;
  o << "|                                   online COOL taken if connection omitted" << std::endl;
  o << "|   --run             <int>         Run number" << std::endl;
  o << "| " << std::endl;                    
  o << "|   --listtests                     Lists available test" << std::endl; 
  o << "| " << std::endl;
  o << "|   --disable         <string>      Disable certain test (this option can be repeated)" << std::endl; 
  o << "| " << std::endl;
  o << "|   --verbose         <int>         Verbosity: 0-silent, 1-basic, 2-detailed, 3-debug, 4-printCTP, 5-printHLT" << std::endl; 
  o << "| " << std::endl;
  o << "|   --silenceseverity               if set, the severity output is suppressed [only if you want to fool NICOS]" << std::endl; 
  o << "| " << std::endl;
  o << "|   --exceptions      <string>      file with exceptions i.e. failed tests which should be ignored. The " << std::endl; 
  o << "| " << std::endl;
  o << "|   --help                          this output" << std::endl; 
  o << "| " << std::endl;
  o << "================================================================================" << std::endl;
}

class TrigConfError {
public:
   TrigConfError(std::string s, uint c) : what(s), code(c) {};
   ~TrigConfError(){};
   std::string what;
   uint code;
};


class JobConfig {
public:
   enum ETriggerLevel { LVL1 = 0, HLT = 1, NONE = 2 };

   JobConfig() :
      fCoolConnection(""),
      fTriggerDBConnection(""),
      fTriggerRunNumber(-1),
      fTriggerConfigKey(),
      fTriggerLvl1MenuXML(""),
      fTriggerHltMenuXML(""),
      fSilenceSeverity(false),
      fHelp(false),
      fVerbose(0)
   {}

   ~JobConfig(){}

   bool UseLVL1()      const { return UseLVL1XML() || UseTriggerDB() || UseCOOL(); }
   bool UseHLT()       const { return UseHLTXML()  || UseTriggerDB() || UseCOOL(); }
   bool UseLVL1XML()   const { return Lvl1MenuXML() != ""; }
   bool UseHLTXML()    const { return HltMenuXML() != ""; }
   bool UseCOOL()      const { return CoolConnection() != "" && RunNumber()>=0; }
   bool UseTriggerDB() const { return TriggerDbConnection() != "" && ( ConfigKey().size()>0 || UseCOOL()); }
   std::string LVL1Source() const { 
      return UseLVL1XML()?Lvl1MenuXML():UseTriggerDB()?TriggerDbConnection():CoolConnection()+", run "+boost::lexical_cast<std::string,int>(RunNumber());
   }
   std::string HLTSource()  const {
      return UseHLTXML()?HltMenuXML():UseTriggerDB()?TriggerDbConnection():CoolConnection()+", run "+boost::lexical_cast<std::string,int>(RunNumber());
   }

   void SetHelp(bool x=true) { fHelp = x; }
   bool Help() const { return fHelp; }

   void SetCoolConnection(const std::string & x) { fCoolConnection = x; }
   std::string CoolConnection() const { return fCoolConnection; }

   void SetTriggerDbConnection(const std::string & x) { fTriggerDBConnection = x; }
   std::string TriggerDbConnection() const { return fTriggerDBConnection; }

   void SetTriggerRunNumber(int x) { fTriggerRunNumber = x; }
   int RunNumber() const { return fTriggerRunNumber; }

   void AddTriggerConfigKey(int x) { fTriggerConfigKey.push_back(x); }
   const std::vector<int>& ConfigKey() const { return fTriggerConfigKey; }

   void SetHltMenuXML(const std::string & x) { fTriggerHltMenuXML = x; }
   const std::string & HltMenuXML() const { return fTriggerHltMenuXML; }

   void SetLvl1MenuXML(const std::string & x) { fTriggerLvl1MenuXML = x; }
   const std::string & Lvl1MenuXML() const { return fTriggerLvl1MenuXML; }

   void DisableTest(const std::string& test) { fDisabledTests.insert(test); }
   std::set<std::string>& DisabledTests() { return fDisabledTests; }

   void SetSilenceSeverity(bool silence=true) { fSilenceSeverity = silence; }
   bool SilenceSeverity() { return true; } //fSilenceSeverity; }

   void SetVerbose(unsigned int v = 1) { fVerbose=v; }
   unsigned int Verbose() { return fVerbose; }

  void SetExceptionsFileName(const std::string& name) { fExceptionsFileName = name; }
  const std::string& ExceptionsFileName() const { return fExceptionsFileName; }


   void CheckForCompleteSetup();
  
   void PrintCompleteSetup(std::ostream & log);

private:
   std::string           fCoolConnection;
   std::string           fTriggerDBConnection;  
   int                   fTriggerRunNumber; 
   std::vector<int>      fTriggerConfigKey; 
   std::string           fTriggerLvl1MenuXML;
   std::string           fTriggerHltMenuXML;
   std::set<std::string> fDisabledTests;
   bool                  fSilenceSeverity;
   bool                  fHelp;
   unsigned int          fVerbose;
   std::string           fExceptionsFileName;
   
} gConfig;


void JobConfig::CheckForCompleteSetup() {
  // check if a complete setup has been specified and that the options
  // are consistent 

  // environment parameter
  if(!UseHLT() && !UseLVL1())
    throw TrigConfError(std::string("No checks will be performed since no configuration source was specified"), 2);

  if( UseTriggerDB() && (ConfigKey().size()!=2 || ConfigKey()[0]==0 || ConfigKey()[1]==0) )
    throw TrigConfError(std::string("Two configuration keys (SMK, HLT PSK) must be specified and not zero"), 2);

}



template <class T>
bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&)) {
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

int convertStringToInt(const std::string & s) {
  int theInt;
  if(from_string<int>(theInt, s, std::fixed)) return theInt; // std::hex also possible
  throw TrigConfError(std::string("Could not convert to integer: ")+s, 1);
}

float convertStringToFloat(const std::string & s) {
  float theFloat;
  if(from_string<float>(theFloat, s, std::dec)) return theFloat;
  throw TrigConfError(std::string("Could not convert to float: ")+s, 1);
}


// --------------------------------------------------------------------------------
// parseProgramOptions
// --------------------------------------------------------------------------------
void parseProgramOptions(int argc, char* argv[]) {

  // building the option map
  std::string currentPar("");
  for(int i=1; i<argc; i++) {
    std::string currInput(argv[i]);
    int fchar = currInput.find_first_not_of('-');
    std::string stripped = currInput.substr(fchar);
    bool isParam = (fchar!=0); // string starts with a '-', so it is a parameter name

    if(isParam) { 
      if(stripped=="help" )                 { gConfig.SetHelp(); currentPar = ""; continue; }
      if(stripped=="cool" )                 { currentPar = "cool"; gConfig.SetCoolConnection("COOLONL_TRIGGER/COMP200"); continue; }
      if(stripped=="trigdb" )               { currentPar = "trigdb"; gConfig.SetTriggerDbConnection("TRIGGERDB"); continue; }
      if(stripped=="run" )                  { currentPar = "runnumber"; continue; }
      if(stripped=="keys" )                 { currentPar = "configkey"; continue; }
      if(stripped=="lvl1xml" )              { currentPar = "lvl1xml"; continue; }
      if(stripped=="hltxml" )               { currentPar = "hltxml"; continue; }
      if(stripped=="disable" )              { currentPar = "disable"; continue; }
      if(stripped=="silenceseverity" )      { gConfig.SetSilenceSeverity(); currentPar = ""; continue; }
      if(stripped=="verbose" )              { currentPar = "verbose"; continue; }
      if(stripped=="exceptions" )           { currentPar = "exceptions"; continue; }
      
      throw TrigConfError(std::string("Unknown parameter ")+stripped, 1); // throw error of unknown parameter: code 1

    } else { // string starts with a character other than '-', so it is a parameter value

      if(currentPar=="") throw TrigConfError(std::string("No value expected, but found: ")+stripped, 1); // throw error of unknown value: code 1

      if(currentPar == "cool")         { gConfig.SetCoolConnection(stripped); continue; }
      if(currentPar == "trigdb")       { gConfig.SetTriggerDbConnection(stripped); continue; }
      if(currentPar == "runnumber")    { gConfig.SetTriggerRunNumber(convertStringToInt(stripped)); continue; }
      if(currentPar == "configkey")    { gConfig.AddTriggerConfigKey(convertStringToInt(stripped)); continue; }
      if(currentPar == "lvl1xml")      { gConfig.SetLvl1MenuXML(xmlpathresolve(stripped)); continue; }
      if(currentPar == "hltxml")       { gConfig.SetHltMenuXML(xmlpathresolve(stripped)); continue; }
      if(currentPar == "disable")      { gConfig.DisableTest(stripped); continue; }
      if(currentPar == "verbose")      { gConfig.SetVerbose(convertStringToInt(stripped)); continue; }
      if(currentPar == "exceptions")   { gConfig.SetExceptionsFileName(xmlpathresolve(stripped)); continue; }
    }
  }
}


void JobConfig::PrintCompleteSetup(std::ostream & log) {
  // check if a complete setup has been specified and that the options
  // are consistent 
  std::string prefix("TrigConfConsistencyChecker:       INFO ");
  log << prefix << "============================================================" << std::endl;
  if(UseLVL1() && UseHLT()) {
    log << prefix << "Consistency checks will be performed on LVL 1 and HLT configuration" << std::endl;
  } else if(UseLVL1()) {
    log << prefix << "Consistency checks will be performed on LVL 1 only" << std::endl;
  } else {
    log << prefix << "Consistency checks will be performed on HLT only" << std::endl;
  }
  if(UseLVL1())
    log << prefix << "LVL 1 source : " << LVL1Source() << std::endl;
  if(UseHLT())
    log << prefix << "HLT source   : " << HLTSource() << std::endl;
  log << prefix << "Exceptions   : " << ExceptionsFileName() << std::endl;
  log << prefix << "============================================================" << std::endl;

}


typedef std::pair<std::string,std::string> Key_t;
typedef std::map< Key_t, std::vector<std::string> > ExcMap_t;
typedef std::pair< Key_t, std::vector<std::string> > ExcMapPair_t;

namespace {
   void parseExceptionsFile( const std::string& filename, std::map< Key_t, std::vector<std::string> >& exc, std::ostream & log ) {

      log << "TrigConfConsistencyChecker:       INFO Loading exceptions file " << filename << std::endl;

      try {
         ptree doc;
         read_xml<ptree>(filename, doc);

         for(const ptree::value_type& v: doc.get_child("exceptions")) {
 
            string testname(""), menu("");
            TrigConf::readAttribute(v.second, "name", testname);
            TrigConf::readAttribute(v.second, "menu", menu);
            
            Key_t key = Key_t(testname,menu);
            vector<string>& exceptions_v = exc[key];
            
            for(ptree::value_type ex: v.second) {
               if(ex.first=="<xmlattr>") continue;
               string excCh("");
               TrigConf::readAttribute(ex.second, "name", excCh);
               exceptions_v.push_back(excCh);
            }
         }

      }
      catch (const exception& ex) {
         log << "TrigConfConsistencyChecker:       Exception occured while parsing exceptions xml file: " << ex.what() << endl; 
         throw;
      }

      if(gConfig.Verbose()>=1 && gConfig.Verbose()<=3) {
         for(const ExcMapPair_t& e: exc) {
            log << "TrigConfConsistencyChecker:       INFO In the test '" << e.first.first << "' for menu '" << e.first.second << "' skipping:\n";
            for(const string& s: e.second ) {
               log << "TrigConfConsistencyChecker:                         " << s << endl;
            }
         }
      } else {
         log << "TrigConfConsistencyChecker:       INFO Found exceptions for tests ";
         for(ExcMapPair_t e: exc) {
            log << e.first.first;
            if(e.first.second.size()!=0) log << "(menu: " << e.first.second << ")";
            log << ", ";
         }
         log << endl;
      }
   }
}

std::vector<string>
getExceptions(const ExcMap_t exc, const string& testname, const string& l1menu, const string& hltmenu) {
   std::vector<string> combined;
   for(ExcMapPair_t e: exc) {
      if(e.first.first!=testname) continue;
      if(e.first.second.size()==0) {
         copy(e.second.begin(), e.second.end(), inserter(combined, combined.begin()));
         continue;
      }
      if(e.first.second.size()==0) {
         copy(e.second.begin(), e.second.end(), inserter(combined, combined.begin()));
         continue;
      }
      boost::regex regexp(e.first.second+"$");
      if(regex_match(l1menu, regexp) || regex_match(hltmenu, regexp)) {
         copy(e.second.begin(), e.second.end(), inserter(combined, combined.begin()));
         continue;
      }
   }
   return combined;
}








int main( int argc, char* argv[] ) {

   //  MSG::NIL = 0,
   //  MSG::VERBOSE,
   //  MSG::DEBUG,
   //  MSG::INFO,
   //  MSG::WARNING,
   //  MSG::ERROR,
   //  MSG::FATAL,
   //  MSG::ALWAYS,
   //  MSG::NUM_LEVELS

   std::ostream& log( std::cout );
   for(int i=0; i<argc; i++)
      log << " " << argv[i];
   log << std::endl;

   // ========================================
   // Getting the program parameters
   // ========================================
   try {
      parseProgramOptions(argc, argv);
      if(gConfig.Help()) {
         printhelp(std::cout);
         return 0;
      }
      gConfig.CheckForCompleteSetup();
   }
   catch(TrigConfError & e) {
      log << std::endl << "TrigConfConsistencyChecker:       ERROR error in the option specification: " << e.what << std::endl << std::endl;
      printhelp(log);
      return e.code;
   }


   gConfig.PrintCompleteSetup(log);

   // ========================================
   // Reading the LVL1 configuration information 
   // ========================================
   TrigConf::CTPConfig* ctpc(0);
   if(gConfig.UseLVL1()) {

      try {
      bool newcabling = true;
      if(newcabling) {
         TrigConf::L1DataDef::setNewJEP3Cabling();
         log << "TrigConfConsistencyChecker:       INFO Using new XS cabling (8TE, 8XE, 8XS)" << std::endl;
      } else {
         TrigConf::L1DataDef::setOldJEP3Cabling();
         log << "TrigConfConsistencyChecker:       INFO Using old XS cabling (4TE, 8XE, 0XS)" << std::endl;
      }


      ctpc = new TrigConf::CTPConfig();
      if(gConfig.UseLVL1XML()) {
         log << "TrigConfConsistencyChecker:       INFO Loading L1 configuration" << std::endl;        

         try {      
            std::unique_ptr< TrigConf::IStorageMgr > sm(new TrigConf::XMLStorageMgr( {gConfig.Lvl1MenuXML()} ));
            sm->masterTableLoader().setVerbose(gConfig.Verbose()<4?gConfig.Verbose():0);
            sm->masterTableLoader().load(*ctpc);
         }
         catch(std::exception & e) {
            log << e.what() << std::endl;
            return 1;
         }
      } else if(gConfig.UseCOOL()) {
         try {
            uint runNumber = (uint)gConfig.RunNumber();
            log << "TrigConfConsistencyChecker:       INFO Retrieving Lvl1 menu configuration from cool" << std::endl;
            TrigConf::TrigConfCoolWriter coolReader(gConfig.CoolConnection());
            coolReader.readL1Menu(runNumber, *ctpc);
         }
         catch(std::exception & e) {
            log << e.what() << std::endl;
            return 1;
         }
      } else if(gConfig.UseTriggerDB()) {
         try {
            TrigConf::IStorageMgr * sm = new TrigConf::StorageMgr(gConfig.TriggerDbConnection());
            uint masterConfigKey = (uint)gConfig.ConfigKey()[0];
      
            log << "TrigConfConsistencyChecker:       INFO Retrieving Lvl1 CTP configuration" << std::endl;
            ctpc->setSuperMasterTableId(masterConfigKey);
            sm->masterTableLoader().setVerbose(gConfig.Verbose()<4?gConfig.Verbose():0);
            sm->masterTableLoader().load(*ctpc);

            delete sm;
         }
         catch(std::exception & e) {
            log << e.what() << std::endl;
            return 1;
         }
      }
      if(gConfig.Verbose()==4)
         ctpc->print();
      }
      catch(...) {
         log << "TrigConfConsistencyChecker:       ERROR while retrieving Lvl1 CTP configuration, will abort" << std::endl;
         return 1;
      }
   }

   // ========================================
   // Reading the HLT configuration information 
   // ========================================
   TrigConf::HLTFrame* hltFrame(0);
   if(gConfig.UseHLT()) {
      try {
         hltFrame = new TrigConf::HLTFrame();
         if(gConfig.UseHLTXML()) {
            log << "TrigConfConsistencyChecker:       INFO Loading HLT configuration" << std::endl;        
            try {      
               std::unique_ptr< TrigConf::IStorageMgr > sm(new TrigConf::XMLStorageMgr( { gConfig.HltMenuXML() } ));
               sm->hltFrameLoader().setVerbose(gConfig.Verbose()<4?gConfig.Verbose():0);
               sm->hltFrameLoader().load( *hltFrame );
            }
            catch(std::exception & e) {
               log << e.what() << std::endl;
               return 1;
            }
         } else {
            if(gConfig.UseCOOL()) {
               try {
                  log << "TrigConfConsistencyChecker:       INFO Retrieving HLT configuration from cool" << std::endl;
                  uint runNumber = (uint)gConfig.RunNumber();
                  TrigConf::TrigConfCoolWriter coolReader(gConfig.CoolConnection());
                  uint        masterConfigKey(0);
                  uint        hltPrescaleKey(0);
                  std::string configSource("");

                  // read the payload
                  coolReader.readRunPayload(runNumber,
                                            *hltFrame,
                                            masterConfigKey,
                                            hltPrescaleKey,
                                            configSource );
                  log << "TrigConfConsistencyChecker:       INFO SMK " << masterConfigKey << ", HLT PSK " << hltPrescaleKey << std::endl;

               }
               catch(std::exception & e) {
                  log << e.what() << std::endl;
                  return 1;
               }
            }
            if(gConfig.UseTriggerDB()) {
               try {
                  std::unique_ptr< TrigConf::IStorageMgr > sm = 
                     std::unique_ptr< TrigConf::IStorageMgr >(new TrigConf::StorageMgr(gConfig.TriggerDbConnection()));
                  log << "TrigConfConsistencyChecker:       Retrieving HLT menu configuration" << std::endl;
                  hltFrame->setSMK(gConfig.ConfigKey()[0]);
                  hltFrame->thePrescaleSetCollection().set_prescale_key_to_load(gConfig.ConfigKey()[1]);
                  sm->hltFrameLoader().setVerbose(gConfig.Verbose()<4?gConfig.Verbose():0);
                  sm->hltFrameLoader().load( *hltFrame );
               }
               catch(std::exception & e) {
                  log << e.what() << std::endl;
                  return 1;
               }
            }
            if(gConfig.Verbose()==5)
               hltFrame->print();
         }
      }
      catch(...) {
         log << "TrigConfConsistencyChecker:       ERROR while retrieving HLT menu configuration, will abort" << std::endl;
         return 1;
      }
   }

   // read false positives file
   ExcMap_t exceptions_map;
   if ( ! gConfig.ExceptionsFileName().empty()) {
      try {
         parseExceptionsFile(gConfig.ExceptionsFileName(), exceptions_map, log);
      }
      catch(...) {
         log << "TrigConfConsistencyChecker:       ERROR while retrieving exceptions, will abort" << std::endl;
         return 1;
      }
   }

   // ################################################################################
   // ###
   // ### configuring tests i.e. place them in the map name -> ptr to class doing it
   // ###
   // ################################################################################
  
   TrigConf::ConfigurationCheck mt ( ctpc, hltFrame );

   int returnCode = 0;

   log << "TrigConfConsistencyChecker:       Starting tests" << std::endl;        

   std::vector<TrigConf::TrigConfTest* >::iterator testIt;
   
   for( TrigConfTest* T: mt.tests() ) {

      if ( gConfig.DisabledTests().count( T->name() ) != 0 ) T->enable(false);

      std::string l1name = ctpc?ctpc->name():"";
      std::string hltname = hltFrame?hltFrame->name():"";
      T->run(getExceptions(exceptions_map, T->name(), l1name, hltname));

      log << "TrigConfConsistencyChecker:       REGTEST  " 
          << std::setw(25) << std::left << T->name() 
          << "[" << std::setw(65) << std::left << T->description() << "]"
          << " => " << (T->status() ? "PASSED" : "NOT PASSED");

      if ( ! T->status() )
         log << std::endl 
	   //<< "TrigConfConsistencyChecker:       " << ((gConfig.SilenceSeverity() && T->code())?"WARNING":T->severity()) << " " << T->error();
	     << "TrigConfConsistencyChecker:       " << T->severity() << " " << T->error();
      log << std::endl;

      returnCode = std::max<int>(returnCode,T->code());
   }

   delete ctpc;
   delete hltFrame;

   return returnCode;  
}

