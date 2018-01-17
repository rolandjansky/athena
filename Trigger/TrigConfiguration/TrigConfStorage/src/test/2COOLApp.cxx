/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     TrigConf2COOLApp.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Stelzer (CERN)	Joerg.Stelzer@cern.ch 
//CREATED:  07. Jan. 2005
//                                                     
//PURPOSE: 
//
// This standalone application is designed to write the trigger
// configuration (L1+HLT) for a given run/LB to the COOL database.
// The information can be read from either xml files (intented for
// Monte Carlo production) or from the TriggerDB in online running.
//
// Online running: The application has to be called at the beginning
// of a new run. In addition L1 prescales can be written at LB block
// boundaries. The information is retrieved from the TriggerDB. The
// application is called by the Run Control.
//
// MC production: There are no LBs in MC production, the application
// is called only at the beginning of the production job. The
// information is retrieved from XML files or from the TriggerDB
// depending on wether the simulation uses the TriggerDB or not.
//
//////////////////////////////////////////////////////////////////////


// TODO for Joerg: add the bunchgroupset key, like the prescale key, and
// load the bunchgroupset separately; then call BG write functions

#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/CTPConfigOnline.h"

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/XMLStorageMgr.h"


#include "TrigConfStorage/TrigConfCoolWriter.h"

#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"

#include "TrigConfL1Data/Menu.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <ctime>
#include <map>
#include <vector>
#include <sys/stat.h> 

using namespace std;

class TrigConfError {
public:
   TrigConfError(const std::string& s, uint c) : what(s), code(c) {};
   ~TrigConfError(){};
   std::string what;
   uint code;
};

string
findInPath(const std::string& filename, const string & path) {
   struct stat file_stat;
   string fullname = path + "/" + filename;
   if (stat (fullname.c_str(), &file_stat) == 0) return fullname;
   return "";
}

std::vector<std::string> splitpath(const std::string& line, std::string del=":")
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

std::string test_cool_connection = "oracle://devdb;schema=atltrig;dbname=COOLTEST;user=atltrig;password=trigconf2007";

// ================================================================================
//
// The following arguments must or can be given to the application:
//
// Mandatory:
//
// -e|--env         onl|sim|cre            Running environment
//                                         onl  -  online
//                                         sim  -  simulation
//                                         cre  -  create COOL schema 
// ================================================================================


// --------------------------------------------------------------------------------
// printhelp
// --------------------------------------------------------------------------------
void printhelp(std::ostream & o, std::ostream& (*lineend) ( std::ostream& os )) {
  o << "================================================================================" << lineend;
  o << "| The program needs to be run with the following specifications:" << lineend;
  o << "| TrigConf2CoolApp.exe <arguments>" << lineend << lineend;
  o << "| where the arguments are (*-required):" << lineend;
  o << "| --------------------------------------------------------------------------------" << lineend;
  o << "| " << lineend;
  o << "| " << lineend;
  o << "|   -e|--env         <string>     * Running environment [write|sim|read|create]" << lineend;
  o << "|                                   write         -  writing info into COOL" << lineend;
  o << "|                                   writel1       -  writing L1 info only into COOL" << lineend;
  o << "|                                   writehlt      -  writing HLT info only into COOL" << lineend;
  o << "|                                   read          -  reading info from COOL (for debugging)" << lineend;
  o << "|                                   sim           -  simulation" << lineend;
  o << "|                                   create        -  create COOL schema" << lineend;
  o << "|                                   printschema   -  print COOL schema" << lineend;
  o << "|                                   schemaversion -  print COOL schema version" << lineend;
  o << "|   --log            <string>     * name of the log file" << lineend;
  o << "| " << lineend;                    
  o << "| Create new schema" << lineend;   
  o << "| -----------------" << lineend;   
  o << "|   --cooldb         <string>     * COOL DB, format: 'oracle://<server>;schema=<schema>;dbname=<DBNAME>;user=<user>;password=<pw>'" << lineend;
  o << "|                                   test value: " << test_cool_connection << lineend;
  o << "|   --schemaversion  <int>          schema version (default will be used if not specified - recommended)" << lineend;
  o << "| " << lineend;                    
  o << "| Print schema/schemaversion" << lineend;   
  o << "| --------------------------" << lineend;   
  o << "|   --cooldb         <string>     * COOL DB, format: 'oracle://<server>;schema=<schema>;dbname=<DBNAME>;user=<user>;password=<pw>'" << lineend;
  o << "|                                   test value: " << test_cool_connection << lineend;
  o << "| " << lineend;                    
  o << "| Write to COOL" << lineend;      
  o << "| -------------" << lineend;      
  o << "|   --cooldb          <string>    * COOL DB, format: 'oracle://<server>;schema=<schema>;dbname=<DBNAME>;user=<user>;password=<pw>'" << lineend;
  o << "|                                   test value: " << test_cool_connection << lineend;
  o << "|   --run             <string>    * Run number, string that can be interpreted as single int or range, or list of either ('14-16,23,30-31')" << lineend;
  o << "| From the TriggerDB" << lineend;      
  o << "|   --trigdb          <string>    * Trigger DB" << lineend;
  o << "|   --usefrontier                   uses frontier instead of oracle (disables sqlite)" << lineend;
  o << "|   --configkey       <int>         Trigger configuration key" << lineend;
  o << "|   --prescalekeylvl1 <int>         Trigger Lvl1 prescale configuration key" << lineend;
  o << "|   --prescalekeyhlt  <int>         Trigger HLT prescale configuration key [if set to 0 the HLT configuration will not be written]" << lineend;
  o << "|   --bgkey           <int>         Trigger bunchgroup configuration key" << lineend;
  o << "|   --level           <string>      Trigger level [lvl1|hlt]" << lineend;
  o << "|   --lumiblock       <int>         Luminosity block (if specified, only Lvl1 prescales are written)" << lineend;
  o << "|   --restrictToFolders <string>    Komma-separated list of folders, if specified, only those folders are written" << lineend;
  o << "| From xml" << lineend;
  o << "|   --lvl1config      <string>      xml file containing the LVL1 configuration (including prescales)" << lineend;
  o << "|   --hltmenu         <string>      xml file containing the HLT menu (including prescales)" << lineend;
  o << "|   --run             <int>         Run number" << lineend;
  o << "|   --infiov                        can be used if you don't have a runnumber" << lineend;
  o << "| " << lineend;
  o << "| Read from COOL" << lineend;      
  o << "| --------------" << lineend;      
  o << "|   --cooldb          <string>    * COOL DB, format: 'oracle://<server>;schema=<schema>;dbname=<DBNAME>;user=<user>;password=<pw>'" << lineend;
  o << "|                                   e.g.: COOLONL_TRIGGER/COMP200" << lineend;
  o << "|                                   test value: " << test_cool_connection << lineend;
  o << "|   --run             <int>       * Run number" << lineend;
  o << "|   --xml             <string>    * Write xml file" << lineend;
  o << "| " << lineend;                    
  o << "| ================================================================================" << lineend;
}

class JobConfig {
 public:
  enum ETriggerLevel { NONE = 0, LVL1 = 1, HLT = 2, BOTH = 3 };

  JobConfig() :
    fTest(false),
    fCoolConnection(""),
    fTriggerDBConnection(""),
    fUseFrontier(false),
    fTriggerRunRanges(), // to superseed run number
    fTriggerLumiblock(0),
    fTriggerConfigKey(-1),
    fTriggerHltPsConfigKey(-1),
    fTriggerLvl1PsConfigKey(-1),
    fTriggerLvl1BgKey(1),
    fTriggerLevel(NONE),
    fTriggerLVL1XML(""),
    fTriggerHltMenuXML(""),
    fLogfileName(""),
    fInfIOV(false),
    fWriteXML(""),
    fNewSchemaVersion(0),
    fExclusiveFolders()
  {}

  ~JobConfig(){}

  bool CreateDBSchema() { return fEnvironment.find("create") != std::string::npos; }
  bool DropDBSchema() { return fEnvironment.find("drop") != std::string::npos; }
  bool PrintDBSchema() { return fEnvironment.find("printschema") != std::string::npos; }
  bool PrintSchemaVersion() { return fEnvironment.find("schemaversion") != std::string::npos; }
  bool WriteConfiguration() { return fEnvironment.find("write") != std::string::npos; }
  bool ReadConfiguration() { return fEnvironment.find("read") != std::string::npos; }
  void AddEnvironment(const std::string & e) { 
    if(fEnvironment.size()>0) fEnvironment.append(" ");
    fEnvironment.append(e);
  }

  ETriggerLevel WriteLevel() { 
    if(fEnvironment.find("writel1") != std::string::npos) return LVL1;
    if(fEnvironment.find("writehlt") != std::string::npos) return HLT;
    if(fEnvironment.find("write") != std::string::npos) return BOTH;
    return NONE;
  }
  bool UseTestDefaults() const { return fTest; }
  void SetUseTestDefaults(bool x=true) { fTest = x; }

  bool UseInfIOV() const { return fInfIOV; }
  void SetUseInfIOV(bool x=true) { fInfIOV = x; }

  void SetCoolConnection(const std::string & x) { fCoolConnection = x; }
  std::string CoolConnection() const {
    if(UseTestDefaults()) return test_cool_connection;
    return fCoolConnection;
  }

  void SetTriggerDbConnection(const std::string & x) { fTriggerDBConnection = x; }
  std::string TriggerDbConnection() const {
     return fTriggerDBConnection;
  }

  void SetUseFrontier(bool useFrontier = true) { fUseFrontier = useFrontier; } 
  bool UseFrontier() const { 
    return fUseFrontier; 
  } 

  void SetTriggerRunRanges(const std::string & runs) {
    std::vector<std::string> runRangeList = splitpath(runs, ",");
    std::vector<std::string>::iterator rrIt = runRangeList.begin();
    std::vector<std::string>::iterator rrItEnd = runRangeList.end();
    for(;rrIt!=rrItEnd;rrIt++) {
      std::vector<std::string> startend = splitpath((*rrIt), "-");
      unsigned int first = (unsigned int)convertStringToInt(startend[0]);
      unsigned int last  = (unsigned int)startend.size()==1?first:convertStringToInt(startend[1]);
      fTriggerRunRanges.push_back(std::pair<unsigned int,unsigned int>(first,last));
    }
  }
  const std::vector<std::pair<unsigned int,unsigned int> >& RunRanges() const { return fTriggerRunRanges; }

  void SetTriggerLumiblockNumber(int x) { fTriggerLumiblock = x; }
  int LumiblockNumber() const {
    if(UseTestDefaults() && fTriggerLumiblock==-1) return 0;
    return fTriggerLumiblock;
  }

  void SetTriggerConfigKey(int x) { fTriggerConfigKey = x; }
  int ConfigKey() const {
    if(UseTestDefaults() && fTriggerConfigKey==-1) return 1;
    return fTriggerConfigKey;
  }

  void SetTriggerHltPsConfigKey(int x) { fTriggerHltPsConfigKey = x; }
  int HltPsConfigKey() const {
    if(UseTestDefaults() && fTriggerHltPsConfigKey==-1) return 1;
    return fTriggerHltPsConfigKey;
  }

  void SetTriggerLvl1PsConfigKey(int x) { fTriggerLvl1PsConfigKey = x; }
  int Lvl1PsConfigKey() const {
    if(UseTestDefaults() && fTriggerLvl1PsConfigKey==-1) return 1;
    return fTriggerLvl1PsConfigKey;
  }

  void SetTriggerLvl1BunchGroupKey(int x) { fTriggerLvl1BgKey = x; }
  int BunchGroupKey() const {
    return fTriggerLvl1BgKey;
  }
  

  void SetTriggerLevel(enum ETriggerLevel x) { fTriggerLevel = x; }
  enum ETriggerLevel TriggerLevel() const {
    if(UseTestDefaults()) return HLT;
    return fTriggerLevel;
  }

  bool UseXML() { return (HltMenuXML() != "") || (LVL1XML() != ""); }
  void SetTriggerHltMenuXML(const std::string & x) { fTriggerHltMenuXML = x; }
  const std::string & HltMenuXML() const { return fTriggerHltMenuXML; }

  void SetTriggerLVL1XML(const std::string & x) { fTriggerLVL1XML = x; }
  const std::string & LVL1XML() const { return fTriggerLVL1XML; }

  void SetLogfileName(const std::string & fn) { fLogfileName = fn; }
  const std::string & LogfileName() const { return fLogfileName; }

  void SetNewSchemaVersion(int sv) { fNewSchemaVersion = sv; }
  int NewSchemaVersion() const { return fNewSchemaVersion; }

  void SetWriteXML(const std::string& s) { fWriteXML = s; }
  const std::string& WriteXML() const { return fWriteXML; }

  void AddWriteFolder(const std::string& s) { fExclusiveFolders.push_back(s); }
	const std::vector<std::string>& ListOfWriteFolders() const { return fExclusiveFolders; }
	
  void CheckForCompleteSetup();

  //void PrintCompleteSetup(MsgStream & log);
  void PrintCompleteSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ));

 private:
  bool               fTest;

  std::string        fEnvironment;
  std::string        fCoolConnection;
  std::string        fTriggerDBConnection;  
  bool               fUseFrontier;
  std::vector<std::pair<unsigned int,unsigned int> > fTriggerRunRanges;
  int                fTriggerLumiblock; 
  int                fTriggerConfigKey; 
  int                fTriggerHltPsConfigKey;
  int                fTriggerLvl1PsConfigKey;
  unsigned int       fTriggerLvl1BgKey;
  enum ETriggerLevel fTriggerLevel;  
  std::string        fTriggerLVL1XML;
  std::string        fTriggerHltMenuXML;
  std::string        fLogfileName;
  bool               fInfIOV;
  std::string        fWriteXML;
  int                fNewSchemaVersion;
  std::vector<std::string> fExclusiveFolders;

} gConfig;


void JobConfig::CheckForCompleteSetup() {
  // check if a complete setup has been specified and that the options
  // are consistent 

  // environment parameter
  if(!CreateDBSchema() && !WriteConfiguration() && !ReadConfiguration() && !PrintDBSchema() && !PrintSchemaVersion() && !DropDBSchema()) {
    throw TrigConfError(std::string("Incomplete setup, no running environment specified"), 2); // incomplete setup: code 2
  }  

  if( CoolConnection() == "" ) {
    throw TrigConfError(std::string("No COOL DB connection string specified, use option '--cooldb'"), 2); // incomplete setup: code 2
  }  

  if(WriteConfiguration()) {
    if(UseXML()) {
      // XML -> COOL
      if( RunRanges().size()==0 && !UseInfIOV() && CoolConnection() != "none" )
        throw TrigConfError(std::string("No RunNumber specified, use option '--run'"), 2); // incomplete setup: code 2
      if( (HltMenuXML()=="") || (LVL1XML()=="") )
        throw TrigConfError(std::string("Both, LVL1 and HLT xml need to be specified"), 2); // incomplete setup: code 2
    } else {
      // DB -> COOL
      if( TriggerDbConnection()=="" )
        throw TrigConfError(std::string("No TriggerDB connection string specified, use option '--trigdb'"), 2); // incomplete setup: code 2

      if( ConfigKey()<0 )
        throw TrigConfError(std::string("No config key specified, use option '--configkey'"), 2); // incomplete setup: code 2
      if( HltPsConfigKey()<0 ) // if it is equal 0 then HLT menu will not be written
        throw TrigConfError(std::string("No HLT prescale configuration key specified, use option '--prescalekeyhlt'"), 2); // incomplete setup: code 2
      if(fEnvironment!="writehlt") {
        if( Lvl1PsConfigKey()<0 ) // if it is equal 0 then L1 prescales (nor keys) will be written
          throw TrigConfError(std::string("No LVL1 prescale configuration key specified, use option '--prescalekeylvl1'"), 2); // incomplete setup: code 2
      }

    }
  }
  if(ReadConfiguration()) {
    // if we read the configuration from the database
    if( RunRanges().size()==0)
      throw TrigConfError(std::string("No RunNumber specified, use option '--run'"), 2); // incomplete setup: code 2
  }

}


//void JobConfig::PrintCompleteSetup(MsgStream & log) {
void JobConfig::PrintCompleteSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ) ) {
  // check if a complete setup has been specified and that the options
  // are consistent 
  log << "========================================" << lineend;
  log << "JOB SETUP:" << lineend
      << "----------" << lineend;	

  if(CreateDBSchema()) {
    log << "Will create schema!" << lineend;
    log << "COOL connection: " << CoolConnection() << lineend;
    log << "Version (0-default): " << NewSchemaVersion() << lineend;
  }  

  if(PrintDBSchema()) {
    log << "Will print schema!" << lineend;
    log << "COOL connection: " << CoolConnection() << lineend;
  }  

  if(PrintSchemaVersion()) {
    log << "Will print schema version!" << lineend;
    log << "COOL connection: " << CoolConnection() << lineend;
  }  

  if( WriteConfiguration() ) {
    log << "Will write configuration from " << (UseXML()?"XML":"the TriggerDB") << " to COOL!" << lineend;
    log << "COOL connection     : " << CoolConnection() << lineend;
    log << "Write level         : " << WriteLevel() << lineend;
    if(UseXML()) {
      if(UseInfIOV()) {
        log << "Run number        : infinity" << lineend;
      } else {
        log << "Run numbers         : ";
        std::vector<std::pair<unsigned int,unsigned int> >::const_iterator rrIt = RunRanges().begin();
        std::vector<std::pair<unsigned int,unsigned int> >::const_iterator rrItEnd = RunRanges().end();
        for(;rrIt!=rrItEnd; rrIt++) {
          if(rrIt != RunRanges().begin()) log << ", ";
          int first = (*rrIt).first;
          int last = (*rrIt).second;
          if(first==last) {
            log << first;
          } else {
            log << first << "-" << last;
          }
        }
        log << lineend;
      }
      log << "LVL1 configuration: " << LVL1XML() << lineend;      
      log << "HLT configuration : " << HltMenuXML() << lineend;      
    } else {
      log << "TriggerDB connection: " << TriggerDbConnection() << lineend;
      log << "   configuration key: " << ConfigKey() << lineend;
      log << "   HLT prescale key : " << HltPsConfigKey() << lineend;
      log << "   Lvl1 prescale key: " << Lvl1PsConfigKey() << lineend;
      log << "   Bunchgroup key   : " << BunchGroupKey() << lineend;
      log << "Run numbers       : ";
      std::vector<std::pair<unsigned int,unsigned int> >::const_iterator rrIt = RunRanges().begin();
      std::vector<std::pair<unsigned int,unsigned int> >::const_iterator rrItEnd = RunRanges().end();
      for(;rrIt!=rrItEnd; rrIt++) {
        if(rrIt != RunRanges().begin()) log << ", ";
        int first = (*rrIt).first;
        int last = (*rrIt).second;
        if(first==last) {
          log << first;
        } else {
          log << first << "-" << last;
        }
      }
      log << lineend;
      log << "Lumiblock number    : " << LumiblockNumber() << lineend;
    } 
		if(ListOfWriteFolders().size()>0) {
    log << "Writing will be restricted to the following folders:" << lineend;
		std::vector<std::string>::const_iterator wfIt = ListOfWriteFolders().begin();
		for(;wfIt!=ListOfWriteFolders().end();wfIt++) {
		  log << "  " << (*wfIt) << lineend;
		}
		}
  }

  if( ReadConfiguration() ) {
    log << "Will read configuration from COOL!" << lineend;
    log << "Run number          : " << RunRanges()[0].first << lineend;
    log << "Lumiblock number    : " << LumiblockNumber() << lineend;
    log << "Writing of XML file : " << WriteXML() << lineend;
  }  
  log << "========================================" << lineend;

}




// --------------------------------------------------------------------------------
// parseProgramOptions
// --------------------------------------------------------------------------------
void parseProgramOptions(int argc, char* argv[]) {

  // building the option map
  std::string currentPar("");
	std::string listofUnknownParameters = "";

  for(int i=1; i<argc; i++) {
    std::string currInput(argv[i]);
    int fchar = currInput.find_first_not_of('-');
    std::string stripped = currInput.substr(fchar);
    bool isParam = (fchar!=0); // string starts with a '-', so it is a parameter name

    if(isParam) { 
      if(stripped=="e" || stripped=="env" ) { currentPar = "environment"; continue; }
      if(stripped=="cooldb" )               { currentPar = "cooldb"; continue; }
      if(stripped=="trigdb" )               { currentPar = "trigdb"; continue; }
      if(stripped=="run" )                  { currentPar = "runnumber"; continue; }
      if(stripped=="lumiblock" )            { currentPar = "lumiblock"; continue; }
      if(stripped=="configkey" )            { currentPar = "configkey"; continue; }
      if(stripped=="prescalekeyhlt" )       { currentPar = "hltpskey"; continue; }
      if(stripped=="prescalekeylvl1" )      { currentPar = "lvl1pskey"; continue; }
      if(stripped=="level" )                { currentPar = "triggerlevel"; continue; }
      if(stripped=="log" )                  { currentPar = "logfile"; continue; }
      if(stripped=="hltmenu" )              { currentPar = "hltmenu"; continue; }
      if(stripped=="lvl1config" )           { currentPar = "lvl1config"; continue; }
      if(stripped=="bgkey" )                { currentPar = "bgkey"; continue; }
      if(stripped=="infiov" )               { currentPar = ""; gConfig.SetUseInfIOV(); continue; }
      if(stripped=="test" )                 { currentPar = ""; gConfig.SetUseTestDefaults(); continue; }
      if(stripped=="usefrontier" )          { currentPar = ""; gConfig.SetUseFrontier(); continue; } 
      if(stripped=="schemaversion" )        { currentPar = "schemaversion"; continue; }
      if(stripped=="xml" )                  { currentPar = "writexml"; continue; }
      if(stripped=="restrictToFolders" )    { currentPar = "restrictToFolders"; continue; }

      listofUnknownParameters += " " + stripped;

    } else { // string starts with a character other than '-', so it is a parameter value

      if(currentPar == "environment")  { gConfig.AddEnvironment(stripped); continue; }
      if(currentPar == "cooldb")       { gConfig.SetCoolConnection(stripped); continue; }
      if(currentPar == "trigdb")       { gConfig.SetTriggerDbConnection(stripped); continue; }
      if(currentPar == "runnumber")    { gConfig.SetTriggerRunRanges(stripped); continue; }
      if(currentPar == "lumiblock")    { gConfig.SetTriggerLumiblockNumber(convertStringToInt(stripped)); continue; }
      if(currentPar == "configkey")    { gConfig.SetTriggerConfigKey(convertStringToInt(stripped)); continue; }
      if(currentPar == "hltpskey")     { gConfig.SetTriggerHltPsConfigKey(convertStringToInt(stripped)); continue; }
      if(currentPar == "lvl1pskey")    { gConfig.SetTriggerLvl1PsConfigKey(convertStringToInt(stripped)); continue; }
      if(currentPar == "bgkey")        { gConfig.SetTriggerLvl1BunchGroupKey(convertStringToInt(stripped)); continue; }
      if(currentPar == "triggerlevel") { gConfig.SetTriggerLevel(stripped=="lvl1"?JobConfig::LVL1:JobConfig::HLT); continue; }
      if(currentPar == "logfile")      { gConfig.SetLogfileName(stripped); continue; }
      if(currentPar == "hltmenu")      { gConfig.SetTriggerHltMenuXML(stripped); continue; }
      if(currentPar == "lvl1config")   { gConfig.SetTriggerLVL1XML(stripped); continue; }
      if(currentPar == "schemaversion"){ gConfig.SetNewSchemaVersion(convertStringToInt(stripped)); continue; }
      if(currentPar == "writexml")     { gConfig.SetWriteXML(stripped); continue; }
      if(currentPar == "restrictToFolders") { gConfig.AddWriteFolder(stripped); continue; }
    }
  }

  if(listofUnknownParameters!="")
      throw TrigConfError(std::string("Unknown parameter: ")+listofUnknownParameters, 1); // throw error of unknown parameter: code 1

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

   //  MsgStream log(0,"TrigConf2COOL"); log.report(int(MSG::INFO));// log.format("% F%48W%S%7W%R%T %0W%M")

   // ========================================
   // Getting the program parameters
   // ========================================
   std::ofstream *outf(0), *errf(0);
   try {
      parseProgramOptions(argc, argv);
   }
   catch(TrigConfError & e) {
      if(gConfig.LogfileName() != "") {
         std::string outfn = gConfig.LogfileName()+".out";
         std::string errfn = gConfig.LogfileName()+".err";
         outf = new std::ofstream(outfn.c_str());
         errf = new std::ofstream(errfn.c_str());
      }
      //std::ostream & log = (outf==0?std::cout:*outf);
      std::ostream & logerr = (outf==0?std::cerr:*errf);
      logerr << std::endl << "===> Error in the option parsing: " << e.what << std::endl << std::endl;
      printhelp(logerr, std::endl);
      if(outf) outf->close();
      if(errf) errf->close();
      return e.code;
   }

   if(gConfig.LogfileName() != "") {
      std::string outfn = gConfig.LogfileName()+".out";
      std::string errfn = gConfig.LogfileName()+".err";
      outf = new std::ofstream(outfn.c_str());
      errf = new std::ofstream(errfn.c_str());
   }
   std::ostream & log = (outf==0?std::cout:*outf);
   std::ostream & logerr = (outf==0?std::cerr:*errf);
  
   std::ostream& (*lineend) ( std::ostream& os ) = &std::endl;
   //std::ostream& endl ( std::ostream& os ); 


   try {
      gConfig.CheckForCompleteSetup();
   }
   catch(TrigConfError & e) {
      logerr << lineend << "===> Error in the option specification: " << e.what << lineend << lineend;
      printhelp(log, lineend);
      if(outf) outf->close();
      if(errf) errf->close();
      return e.code;
   }
  
   gConfig.PrintCompleteSetup(log, lineend);
  
   // ========================================
   // DROPPING a schema from the COOL db (DO NOT USE ON THE ONLINE COOL DATABASE !!!!!)
   // ========================================
//    if(gConfig.DropDBSchema()) {
//       try {
//          if( gConfig.CoolConnection().find("atonr") == std::string::npos &&
//              gConfig.CoolConnection().find("ATONR") == std::string::npos ) {
//             TrigConf::TrigConfCoolWriter coolWriter(gConfig.CoolConnection());
//             coolWriter.recreateSchema();
//          }
//       }
//       catch(std::exception & e) {
//          log << e.what() << lineend;
//          if(outf) outf->close();
//          if(errf) errf->close();
//          return 1;
//       }
//    }

   // ========================================
   // CREATING a new schema on the COOL db
   // ========================================
   if(gConfig.CreateDBSchema()) {
      try {
         TrigConf::TrigConfCoolWriter coolWriter(gConfig.CoolConnection());
         coolWriter.createSchema(gConfig.NewSchemaVersion());
      }
      catch(std::exception & e) {
         log << e.what() << lineend;
         if(outf) outf->close();
         if(errf) errf->close();
         return 1;
      }
   }
  
   // ========================================
   // PRINT the schema in the COOL db
   // ========================================
   if(gConfig.PrintDBSchema()) {
      try {      
         TrigConf::TrigConfCoolWriter coolWriter(gConfig.CoolConnection());
         coolWriter.printSchema(log);
      }
      catch(std::exception & e) {
         log << e.what() << lineend;
         if(outf) outf->close();
         if(errf) errf->close();
         return 1;
      }
   }

   // ========================================
   // PRINT the schema version of the COOL db
   // ========================================
   if(gConfig.PrintSchemaVersion()) {
      try {      
         TrigConf::TrigConfCoolWriter coolWriter(gConfig.CoolConnection());
         coolWriter.printSchemaVersion(log);
      }
      catch(std::exception & e) {
         log << e.what() << lineend;
         if(outf) outf->close();
         if(errf) errf->close();
         return 1;
      }
   }




   // ========================================
   //
   // DB,XML -> COOL 
   //
   // WRITING information into the COOL db
   // ========================================
   if(gConfig.WriteConfiguration()) {
      if(!gConfig.UseXML()) {
         //
         // DB -> COOL
         //
         std::vector<std::pair<unsigned int,unsigned int> > infinity; infinity.push_back(std::pair<unsigned int,unsigned int>(0x80000000,0x80000000));
         
         // setup the connection to the TriggerDB
         unsigned int masterConfigKey = (unsigned int)gConfig.ConfigKey();       // the configuration key for the HLT menu
         unsigned int lvlPrescaleKey  = (unsigned int)gConfig.Lvl1PsConfigKey(); // the configuration key for the LVL1 prescales
         unsigned int hltPrescaleKey  = (unsigned int)gConfig.HltPsConfigKey();  // the configuration key for the HLT prescales
         unsigned int bgKey           = (unsigned int)gConfig.BunchGroupKey();  // the configuration key for the HLT prescales
         const std::vector<std::pair<unsigned int,unsigned int> >& runRanges = gConfig.UseInfIOV()?infinity:gConfig.RunRanges();  // the run range list
         unsigned int lumiblockNumber = (unsigned int)gConfig.LumiblockNumber(); // the lumiblock number
         std::string  configSource = gConfig.TriggerDbConnection();

         std::vector<std::string> csv = TrigConf::split(configSource, ";");
         std::string user = "";
         std::string passwd = "";
         std::vector<std::string>::iterator csIt = csv.begin();
         for(;csIt!=csv.end();csIt++) {
            std::string& s = *csIt;
            if( s.find("user")==0 ) user=TrigConf::split(s, "=")[1];
            if( s.find("passwd")==0 ) passwd=TrigConf::split(s, "=")[1];
         }
      
         try {
            std::unique_ptr< TrigConf::IStorageMgr > sm = 
               std::unique_ptr< TrigConf::IStorageMgr >(new TrigConf::StorageMgr(csv[0],user,passwd,log));

            // setup the coolWriter
            TrigConf::TrigConfCoolWriter coolWriter(gConfig.CoolConnection(),log);
            std::vector<std::string>::const_iterator wfIt = gConfig.ListOfWriteFolders().begin();
            for(;wfIt!=gConfig.ListOfWriteFolders().end();wfIt++) 
               coolWriter.addWriteFolder(*wfIt);
											
            if( lumiblockNumber == 0 ) { // write runwise configuration information

               TrigConf::ThresholdConfig thrcfg;
               TrigConf::PrescaleSet lvl1ps;
               TrigConf::CTPConfig ctpc;
               TrigConf::CaloInfo ci;

               log << lineend
                   << "Loading information from Trigger DB" << lineend
                   << "===================================" << lineend
                   << lineend;

               //get the LVL1 trigger information
               //log << "Retrieving Lvl1 threshold configuration" << lineend;
               //thrcfg.setSuperMasterTableId(masterConfigKey);
               //sm->masterTableLoader().load(thrcfg);
               //thrcfg.print();
      
               if(gConfig.WriteLevel() & JobConfig::LVL1) {
                 log << "Retrieving Lvl1 CTP configuration" << lineend;
                 log << "NB: BG set is hardcoded to 1 so better make sure its in the DB!" << lineend;
                 bool useCTPConfigOnline = false;
                 if(useCTPConfigOnline) {
                   TrigConf::CTPConfigOnline ctpconl;
                   ctpconl.setSuperMasterTableId(masterConfigKey);
                   sm->masterTableLoader().load(ctpconl);
                   ctpc.setMenu( ctpconl.menu() );
                   ctpc.setPrescaleSet( ctpconl.prescaleSet() );
                   ctpc.setBunchGroupSet( ctpconl.bunchGroupSet() );
                   ctpc.setPrescaledClock( ctpconl.prescaledClock() );
                   ctpc.setDeadTime( ctpconl.deadTime() );
                   ctpc.setRandom( ctpconl.random() );
                   ctpc.setLvl1MasterTableId( ctpconl.lvl1MasterTableId() );
                 } else {
                   ctpc.setSuperMasterTableId(masterConfigKey);
                   ctpc.setPrescaleSetId(lvlPrescaleKey);
                   ctpc.setBunchGroupSetId(bgKey);
                   sm->masterTableLoader().load(ctpc);
                 }
                 //ctpc.print("  ",5);
               }

               // get the HLT trigger information
               TrigConf::HLTFrame hltFrame;
               if(hltPrescaleKey>0) {
                  log << "Retrieving HLT menu configuration ("<<masterConfigKey<<") and prescale set ("<<hltPrescaleKey<<")" << lineend;
                  hltFrame.setSMK(masterConfigKey);
                  hltFrame.thePrescaleSetCollection().set_prescale_key_to_load(hltPrescaleKey);
                  sm->hltFrameLoader().load( hltFrame );
                  //hltFrame.print();
               }

               log << lineend
                   << "Writing information to COOL" << lineend
                   << "===========================" << lineend
                   << lineend;

               if(hltPrescaleKey>0) {
                  coolWriter.writeRunPayload(runRanges,
                                             masterConfigKey,
                                             hltPrescaleKey,
                                             thrcfg,
                                             ctpc,
                                             ci,
                                             hltFrame,
                                             configSource);

                  //                   TrigConf::HLTPrescaleSet *hltpss = hltFrame.chains().extractPrescaleSet();
                  //                   coolWriter.writeHltPrescalePayload( runRanges, *hltpss);
                  //                   delete hltpss;
               } else {
                  coolWriter.writeL1MenuPayload(runRanges,ctpc.menu());
               }

               
//                if(gConfig.WriteLevel() & JobConfig::LVL1) {
//                  if(lvlPrescaleKey>0) {
//                    coolWriter.writeL1PrescalePayload(runRanges,
//                                                      lvlPrescaleKey,
//                                                      lvl1ps);
//                  }
//                }


            } else { // write lumiblock-wise information (prescales only)

               log << lineend
                   << "Loading information from TriggerDB" << lineend
                   << "==================================" << lineend
                   << lineend;

               TrigConf::PrescaleSet lvl1ps;
               if(gConfig.WriteLevel() & JobConfig::LVL1) {
                 log << "Retrieving Lvl1 prescale set from the TriggerDB" << lineend;
                 lvl1ps.setId(lvlPrescaleKey);
                 sm->prescaleSetLoader().load(lvl1ps);
                 lvl1ps.print();
               }

               unique_ptr<TrigConf::HLTPrescaleSet> hltpss;
               if(gConfig.WriteLevel() & JobConfig::HLT) {
                 if(hltPrescaleKey>0) {
                   log << "Retrieving HLT menu configuration ("<<masterConfigKey<<") and prescale set ("<<hltPrescaleKey<<")" << lineend;
                   TrigConf::HLTFrame hltFrame;
                   hltFrame.setSMK(masterConfigKey);
                   hltFrame.thePrescaleSetCollection().set_prescale_key_to_load(hltPrescaleKey);
                   sm->hltFrameLoader().load( hltFrame );
                   hltpss = unique_ptr<TrigConf::HLTPrescaleSet>(hltFrame.chains().extractPrescaleSet());
                   cout << endl << *hltpss << endl;
                 }
               }

               unsigned int runNumber = runRanges[0].first; // need to think about this
               
               log << lineend
                   << "Writing information to COOL" << lineend
                   << "===========================" << lineend
                   << lineend;
               
               // write the payload
               if(gConfig.WriteLevel() & JobConfig::LVL1) {
                 coolWriter.writeL1PrescalePayload(runNumber,
                                                   lumiblockNumber,
                                                   lvlPrescaleKey,
                                                   lvl1ps);
               }
               if(gConfig.WriteLevel() & JobConfig::HLT) {
                 if(hltPrescaleKey>0) {
                   coolWriter.writeHltPrescalePayload( runNumber,
                                                       lumiblockNumber,
                                                       *hltpss.get());
                 }
               }
            }
         }
         catch(TrigConfError & e) {
            log << e.what << lineend;
            printhelp(log, lineend);
            if(outf) outf->close();
            if(errf) errf->close();
            return e.code;
         }
         catch(std::exception & e) {
            log << "Caught std::exception: " << e.what() << lineend;
            if(outf) outf->close();
            if(errf) errf->close();
            return 1;
         }
      } else {

         //
         // XML -> COOL
         //
         if(gConfig.CoolConnection()!="none") {
            
            try {

               std::vector<std::pair<unsigned int,unsigned int> > infinity; infinity.push_back(std::pair<unsigned int,unsigned int>(0x80000000,0x80000000));

               const std::vector<std::pair<unsigned int,unsigned int> >& runRanges = gConfig.UseInfIOV()?infinity:gConfig.RunRanges(); // the run range list

               //unsigned int lumiblockNumber = 0;                                       // the lumiblock number (in MC the LB is 0)
               std::string  lvl1ConfigXml   = xmlpathresolve(gConfig.LVL1XML()); // the LVL1 configuration xml file
               std::string  hltMenuXml      = xmlpathresolve(gConfig.HltMenuXML()); // the HLT menu xml file
               std::string  configSource    = gConfig.LVL1XML() + " and " + gConfig.HltMenuXML();

               if( lvl1ConfigXml == "" ) throw TrigConfError("ERROR: LVL1 menu file " + gConfig.LVL1XML() + " not found",10);
               if( hltMenuXml == "" ) throw TrigConfError("ERROR: HLT menu file " + gConfig.HltMenuXML() + " not found",10);

               // dummy values that don't make sense when reading from xml
               unsigned int masterConfigKey = 0;       // the configuration key for the HLT menu
               unsigned int lvlPrescaleKey  = 0; // the configuration key for the LVL1 prescales
               unsigned int hltPrescaleKey  = 0;  // the configuration key for the HLT prescales

               std::unique_ptr< TrigConf::IStorageMgr > sm = 
                  std::unique_ptr< TrigConf::IStorageMgr >(new TrigConf::XMLStorageMgr( {lvl1ConfigXml, hltMenuXml} ));
        
               // setup the coolWriter
               TrigConf::TrigConfCoolWriter * coolWriter = new TrigConf::TrigConfCoolWriter(gConfig.CoolConnection(),log);

               TrigConf::ThresholdConfig thrcfg;

               TrigConf::CaloInfo ci;
               log << "Retrieving CaloInfo configuration from " << lvl1ConfigXml << lineend;
               sm->caloInfoLoader().load(ci);
      
               TrigConf::CTPConfig ctpc;
               log << "Retrieving Lvl1 CTP configuration from " << lvl1ConfigXml << lineend;
               sm->masterTableLoader().load(ctpc);
               ctpc.menu().setCaloInfo(ci);
               ci.print();
               //ctpc.print();

               // get the HLT trigger information
               log << "Retrieving HLT menu configuration and prescale set from " << hltMenuXml << lineend;
               TrigConf::HLTFrame hltFrame;
               sm->hltFrameLoader().load( hltFrame );
               //hltFrame.print();

               // write the payload
               coolWriter->writeRunPayload(runRanges,
                                           masterConfigKey,
                                           hltPrescaleKey,
                                           thrcfg,
                                           ctpc,
                                           ci,
                                           hltFrame,
                                           configSource);

               unique_ptr<TrigConf::HLTPrescaleSet> hltpss(hltFrame.chains().extractPrescaleSet());
               coolWriter->writeHltPrescalePayload( runRanges, *hltpss.get());
               
               log << "Retrieving Lvl1 prescale set from " << lvl1ConfigXml << lineend;
               TrigConf::PrescaleSet lvl1ps;
               lvl1ps.setId(lvlPrescaleKey);
               sm->prescaleSetLoader().load(lvl1ps);
               //lvl1ps.print();
               
               // write the payload
//                if(runRanges.size()==1 && runRanges[0].first==runRanges[0].second) {
//                   coolWriter->writeL1PrescalePayload(runRanges[0].first,
//                                                      lumiblockNumber,
//                                                      lvlPrescaleKey,
//                                                      lvl1ps);
//                } else {
                  coolWriter->writeL1PrescalePayload(runRanges,
                                                     lvlPrescaleKey,
                                                     lvl1ps);
//                }

            }
            catch(TrigConfError & e) {
               log << e.what << lineend;
               if(e.code<10) printhelp(log, lineend);
               if(outf) outf->close();
               if(errf) errf->close();
               return e.code;
            }
            catch(std::exception & e) {
               log << "Caught std::exception: " << e.what() << lineend;
               if(outf) outf->close();
               if(errf) errf->close();
               return 1;
            }
         }
      }
      

      if(outf) outf->close();
      if(errf) errf->close();
      return 0;
   }




   // ========================================
   // READING information from the COOL DB for
   // a certain run and print
   // ========================================
   if(gConfig.ReadConfiguration()) {

      try {


         // the run number
         unsigned int runNumber       = gConfig.RunRanges()[0].first;       
         unsigned int lumiblockNumber = (unsigned int)gConfig.LumiblockNumber(); // the lumiblock number

         // setup the coolReader
         TrigConf::TrigConfCoolWriter coolReader(gConfig.CoolConnection());
         
         if( lumiblockNumber == 0 ) { // read runwise configuration information

            TrigConf::CTPConfig ctpc;

            // the LVL1 menu information
            log << "Retrieving Lvl1 menu configuration from cool" << lineend;
            coolReader.readL1Menu( runNumber, ctpc );

            // read the prescales
            log << "Retrieving Lvl1 prescale set from cool" << lineend;
            TrigConf::PrescaleSet * psSet = new TrigConf::PrescaleSet;
            unsigned int lvl1PrescaleKey(0);
            coolReader.readL1PrescalePayload(runNumber,lumiblockNumber,lvl1PrescaleKey,*psSet);             

            log << "Retrieving Lvl1 bunchgroups from cool" << lineend;
            // read the bunchgroups information
            TrigConf::BunchGroupSet bgs;
            int bgKey;
            coolReader.readL1BunchGroupLBPayload( runNumber, lumiblockNumber, bgKey, bgs);

            ctpc.setPrescaleSet(*psSet);
            ctpc.setBunchGroupSet(bgs);
            ctpc.setBunchGroupSetId(bgKey);
            //ctpc.print();

            TrigConf::CaloInfo ci;




            if(gConfig.WriteXML()!="") {
               std::string l1menuxml = "L1Menu_" + gConfig.WriteXML() + ".xml";
               std::ofstream xmlfile;
               xmlfile.open (l1menuxml.c_str());
               xmlfile << "<?xml version=\"1.0\" ?>" << endl;
               xmlfile << "<!-- File is auto-generated from the TrigDB -->" << endl;
               xmlfile << "<LVL1Config name=\"lumi1E31\">" << endl;
               ctpc.writeXML(xmlfile,1,2);
               xmlfile << "</LVL1Config>" << endl;
               xmlfile.close();
            }

            // the HLT trigger information
            TrigConf::HLTFrame hltFrame;
            unsigned int masterConfigKey(0);
            unsigned int hltPrescaleKey(0);
            std::string  configSource("");

            log << "Retrieving HLT Menu from cool" << lineend;
            // read the payload
            coolReader.readRunPayload(runNumber,
                                      hltFrame,
                                      masterConfigKey,
                                      hltPrescaleKey,
                                      configSource );


            if(coolReader.HLTPrescaleFolderExists()) {
               TrigConf::HLTPrescaleSet *hltpss = new TrigConf::HLTPrescaleSet();
               coolReader.readHltPrescalePayload( runNumber, lumiblockNumber, *hltpss);
               hltFrame.thePrescaleSetCollection().setPrescaleSet(hltpss); // frame takes ownership of hltpss
            }

            log << "Configuration source:" << lineend;
            log << "---------------------" << lineend;
            log << configSource << lineend;
            log << lineend;
            log << "Configuration keys:" << lineend;
            log << "-------------------" << lineend;
            log << "   Master        : " << masterConfigKey << lineend;
            log << "   HLT Prescale  : " << hltPrescaleKey << lineend;
            // hltFrame.print();
            if(gConfig.WriteXML()!="") {
               std::string hltmenuxml = "HLTMenu_" + gConfig.WriteXML() + ".xml";
               std::ofstream xmlfile;
               xmlfile.open (hltmenuxml.c_str());
               xmlfile << "<?xml version=\"1.0\" ?>" << endl;
               xmlfile << "<!-- File is auto-generated from the TrigDB -->" << endl;
               xmlfile << "<HLT_MENU>" << endl;
               hltFrame.writeXML(xmlfile);
               xmlfile << "</HLT_MENU>" << endl;
               xmlfile.close();
            }

         }
      }
      catch(TrigConfError & e) {
         log << e.what << lineend;
         printhelp(log, lineend);
         if(outf) outf->close();
         return e.code;
      }
      catch(std::exception & e) {
         log << "Caught std::exception: " << e.what() << lineend;
         if(outf) outf->close();
         return 1;
      }
      if(outf) outf->close();
      return 0;
   }

   if(outf) outf->close();
   return 0;
}

