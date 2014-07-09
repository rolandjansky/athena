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


//#include "GaudiKernel/MsgStream.h"

#include "TrigConfL1Data/HelperFunctions.h"

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/XMLStorageMgr.h"
#include "TrigConfStorage/HLTPrescaleSetLoader.h"


#include "TrigConfStorage/TrigConfCoolWriter.h"


#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"


#include "TrigConfL1Data/Menu.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTChain.h"

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

using std::endl;

class TrigConfError {
 public:
  TrigConfError(std::string s, uint c) : what(s), code(c) {};
  ~TrigConfError(){};
  std::string what;
  uint code;
};

std::string findInPath(const std::string& filename, const std::string & path) {
   struct stat file_stat;
   char buf[2048];
   std::string fullname = path + "/" + filename;
   strcpy (buf, fullname.c_str ());
   if (stat (buf, &file_stat) == 0) return fullname;
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
//std::string test_trig_connection = "mysql://lxmrra3801.cern.ch/Preseries";
std::string test_trig_connection = "mysql://pcatr13.cern.ch/Preseries";


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
  o << "|                                   read          -  reading info from COOL (for debugging)" << lineend;
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
  o << "|                                   test value: " << test_trig_connection << lineend;
  o << "|   --configkey       <int>         Trigger configuration key" << lineend;
  o << "|   --prescalekeylvl1 <int>         Trigger Lvl1 prescale configuration key" << lineend;
  o << "|   --prescalekeyhlt  <int>         Trigger HLT prescale configuration key [if set to 0 the HLT configuration will not be written]" << lineend;
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
  o << "|                                   test value: " << test_cool_connection << lineend;
  o << "|   --run             <int>       * Run number" << lineend;
  o << "|   --xml             <string>    * Write xml file" << lineend;
  o << "| " << lineend;                    
  o << "| ================================================================================" << lineend;
}

class JobConfig {
public:
   enum ETriggerLevel { LVL1 = 0, HLT = 1, NONE = 2 };

   JobConfig() :
      fTest(false),
      fCoolConnection(""),
      fTriggerDBConnection(""),
      fTriggerRunRanges(), // to superseed run number
      fTriggerLumiblock(0),
      fTriggerConfigKey(-1),
      fTriggerLvl1PsConfigKey(-1),
      fTriggerLevel(NONE),
      fTriggerLVL1XML(""),
      fTriggerHltMenuXML(""),
      fLogfileName(""),
      fInfIOV(false),
      fWriteXML("")
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
      if(UseTestDefaults()) return test_trig_connection;
      return fTriggerDBConnection;
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

   void AddTriggerHltPsConfigKeyToCollection(int x) { fTriggerHltPsConfigKeyCollection.push_back(x); }
   const std::vector<int> & HltPsConfigKeyCollection() const {
      return fTriggerHltPsConfigKeyCollection;
   }

   void SetTriggerLvl1PsConfigKey(int x) { fTriggerLvl1PsConfigKey = x; }
   int Lvl1PsConfigKey() const {
      if(UseTestDefaults() && fTriggerLvl1PsConfigKey==-1) return 1;
      return fTriggerLvl1PsConfigKey;
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

   void SetWriteXML(const std::string& s) { fWriteXML = s; }
   const std::string& WriteXML() const { return fWriteXML; }

   void CheckForCompleteSetup();

   //void PrintCompleteSetup(MsgStream & log);
   void PrintCompleteSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ));

private:
   bool               fTest;

   std::string        fEnvironment;
   std::string        fCoolConnection;
   std::string        fTriggerDBConnection;  
   std::vector<std::pair<unsigned int,unsigned int> > fTriggerRunRanges;
   int                fTriggerLumiblock; 
   int                fTriggerConfigKey; 
   std::vector<int>   fTriggerHltPsConfigKeyCollection;
   int                fTriggerLvl1PsConfigKey;
   enum ETriggerLevel fTriggerLevel;  
   std::string        fTriggerLVL1XML;
   std::string        fTriggerHltMenuXML;
   std::string        fLogfileName;
   bool               fInfIOV;
   std::string        fWriteXML;
} gConfig;


void JobConfig::CheckForCompleteSetup() {
  // check if a complete setup has been specified and that the options
  // are consistent 

  // environment parameter
  if(!WriteConfiguration() && !ReadConfiguration())
    throw TrigConfError(std::string("Incomplete setup, no running environment specified"), 2);

  if( CoolConnection() == "" )
    throw TrigConfError(std::string("No COOL DB connection string specified, use option '--cooldb'"), 2); // incomplete setup: code 2
  
  if(WriteConfiguration()) {
     
     if( TriggerDbConnection()=="" )
        throw TrigConfError(std::string("No TriggerDB connection string specified, use option '--trigdb'"), 2);
     if( ConfigKey()<=0 )
        throw TrigConfError(std::string("No config key specified, use option '--configkey'"), 2);
     if( HltPsConfigKeyCollection().size()==0 )
        throw TrigConfError(std::string("No HLT prescale configuration key specified, use option '--prescalekeyhlt'"), 2);
     if( Lvl1PsConfigKey()<=0 )
        throw TrigConfError(std::string("No LVL1 prescale configuration key specified, use option '--prescalekeylvl1'"), 2);
  }
  if(ReadConfiguration()) {
    // if we read the configuration from the database
    if( RunRanges().size()==0 )
      throw TrigConfError(std::string("No RunNumber specified, use option '--run'"), 2); // incomplete setup: code 2
  }

}


void JobConfig::PrintCompleteSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ) ) {
   // check if a complete setup has been specified and that the options
   // are consistent 
   log << "========================================" << lineend;
   log << "JOB SETUP:" << lineend
       << "----------" << lineend;	

   if( WriteConfiguration() ) {
      log << "Will write configuration from the TriggerDB to COOL!" << lineend;
      log << "COOL connection     : " << CoolConnection() << lineend;

      log << "TriggerDB connection: " << TriggerDbConnection() << lineend;
      log << "   configuration key: " << ConfigKey() << lineend;
      log << "   HLT prescale keys : "; for(unsigned int i=0; i<HltPsConfigKeyCollection().size(); i++) log << HltPsConfigKeyCollection()[i] << ", "; log << lineend;
      log << "   Lvl1 prescale key: " << Lvl1PsConfigKey() << lineend;
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
void parseProgramOptions(int argc, char* argv[]) throw(TrigConfError) {

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
      if(stripped=="infiov" )               { currentPar = ""; gConfig.SetUseInfIOV(); continue; }
      if(stripped=="test" )                 { currentPar = ""; gConfig.SetUseTestDefaults(); continue; }
      if(stripped=="schemaversion" )        { currentPar = "schemaversion"; continue; }
      if(stripped=="xml" )                  { currentPar = "writexml"; continue; }
      if(stripped=="restrictToFolders" )    { currentPar = "restrictToFolders"; continue; }

      listofUnknownParameters += " " + stripped;

    } else { // string starts with a character other than '-', so it is a parameter value

			//if(currentPar=="") throw TrigConfError(std::string("No value expected, but found: ")+stripped, 1); // throw error of unknown value: code 1

      if(currentPar == "environment")  { gConfig.AddEnvironment(stripped); continue; }
      if(currentPar == "cooldb")       { gConfig.SetCoolConnection(stripped); continue; }
      if(currentPar == "trigdb")       { gConfig.SetTriggerDbConnection(stripped); continue; }
      if(currentPar == "runnumber")    { gConfig.SetTriggerRunRanges(stripped); continue; }
      if(currentPar == "lumiblock")    { gConfig.SetTriggerLumiblockNumber(convertStringToInt(stripped)); continue; }
      if(currentPar == "configkey")    { gConfig.SetTriggerConfigKey(convertStringToInt(stripped)); continue; }
      if(currentPar == "hltpskey")     { gConfig.AddTriggerHltPsConfigKeyToCollection(convertStringToInt(stripped)); continue; }
      if(currentPar == "lvl1pskey")    { gConfig.SetTriggerLvl1PsConfigKey(convertStringToInt(stripped)); continue; }
      if(currentPar == "triggerlevel") { gConfig.SetTriggerLevel(stripped=="lvl1"?JobConfig::LVL1:JobConfig::HLT); continue; }
      if(currentPar == "logfile")      { gConfig.SetLogfileName(stripped); continue; }
      if(currentPar == "hltmenu")      { gConfig.SetTriggerHltMenuXML(stripped); continue; }
      if(currentPar == "lvl1config")   { gConfig.SetTriggerLVL1XML(stripped); continue; }
      if(currentPar == "writexml")     { gConfig.SetWriteXML(stripped); continue; }
    }
  }

  if(listofUnknownParameters!="")
      throw TrigConfError(std::string("Unknown parameter: ")+listofUnknownParameters, 1); // throw error of unknown parameter: code 1

}




int main( int argc, char* argv[] ) {
  

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
   // WRITING information into the COOL db
   // ========================================
   if(gConfig.WriteConfiguration()) {
         
      std::vector<std::pair<unsigned int,unsigned int> > infinity; infinity.push_back(std::pair<unsigned int,unsigned int>(0x80000000,0x80000000));
         
      // setup the connection to the TriggerDB
      const std::vector<std::pair<unsigned int,unsigned int> >& runRanges = gConfig.UseInfIOV()?infinity:gConfig.RunRanges();  // the run range list

      unsigned int runnumber = runRanges[0].first;
      unsigned int smk = (unsigned int)gConfig.ConfigKey();       // the configuration key for the HLT menu
      //unsigned int lvlPrescaleKey  = (unsigned int)gConfig.Lvl1PsConfigKey(); // the configuration key for the LVL1 prescales
      unsigned int hltPrescaleKey  = (unsigned int)gConfig.HltPsConfigKeyCollection()[0];  // the configuration key for the HLT prescales
      //unsigned int lumiblockNumber = (unsigned int)gConfig.LumiblockNumber(); // the lumiblock number
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


      TrigConf::HLTFrame*           fHLTFrame(TrigConf::HLTFrame::instance());  // pointer to the HLTFrame (could be skipped at the moment since it is still a singleton)
      TrigConf::HLTPrescaleSet*     fHLTPrescaleSet(new TrigConf::HLTPrescaleSet()); // currently active HLT prescale set
      
      TrigConf::TrigConfCoolWriter* fConf2Cool = new TrigConf::TrigConfCoolWriter(gConfig.CoolConnection(),log);

      std::string name("");
      
      try {
         TrigConf::TrigConfCoolWriter coolWriter(gConfig.CoolConnection(),log);
         {

            TrigConf::ThresholdConfig thrcfg;
            TrigConf::CaloInfo ci;
            TrigConf::CTPConfig ctpConfig;
            std::string configurationSource("Test");

            {
               std::vector<TrigConf::HLTChain*>& chainList = fHLTFrame->getHLTChainList().chains();
               std::vector<TrigConf::HLTChain*>::iterator chainIt = chainList.begin();
               for(;chainIt!=chainList.end();chainIt++) delete *chainIt;
               chainList.clear();
            }
            
            std::unique_ptr< TrigConf::StorageMgr > sm(new TrigConf::StorageMgr(csv[0],user,passwd));

            std::cout << "Reading HLT menu from the TriggerDB with SMK " << smk << " and HLT PSK " << hltPrescaleKey << "." << std::endl;
    
            fHLTFrame->setUseNextRun( false );
            fHLTFrame->set_superMaster_id(smk);
            fHLTFrame->set_hltPrescaleSet_id(hltPrescaleKey);
            try {
               sm->hltFrameLoader().load( *fHLTFrame );
            }
            catch(std::exception e) {
               std::cerr << e.what() << std::endl;
               return 0;
            }
            fHLTPrescaleSet->fillFromFrame(fHLTFrame);
            std::cout << "Done reading HLT menu" << std::endl;

            fConf2Cool->writeRunPayload(runnumber,
                                        smk,
                                        hltPrescaleKey,
                                        thrcfg,  // ignored
                                        ctpConfig,
                                        ci, // ignored
                                        *fHLTFrame,
                                        configurationSource);
         }

         for(unsigned int i=1; i<gConfig.HltPsConfigKeyCollection().size(); i++  ) {

            int pskey = gConfig.HltPsConfigKeyCollection()[i];
            int lb = 3 * i;

            std::cout << "Simulating userCommand: 'HLTPRESCALE2COOL' with argument '" << lb << " " << pskey << "'" << std::endl;

            std::unique_ptr< TrigConf::StorageMgr > sm(new TrigConf::StorageMgr(csv[0],user,passwd));

            std::cout << "writeHLTPrescaleSetToCool: HLT psk " << pskey << ", name " << name << " for LB " << lb << std::endl;

            fHLTPrescaleSet->reset();
            fHLTPrescaleSet->resetRerunPrescales();

            std::cout << "HLT PSS status after reset: psk=" << fHLTPrescaleSet->psk() << ", name=" << fHLTPrescaleSet->name()
                      << " #L2 ps " << fHLTPrescaleSet->nL2() << " #EF ps " << fHLTPrescaleSet->nEF() << std::endl;

            fHLTPrescaleSet->set_psk(pskey);
            sm->hltPrescaleSetLoader().load( *fHLTPrescaleSet );
            name = fHLTPrescaleSet->name();

            std::cout << "HLT PSS status after loading: psk=" << fHLTPrescaleSet->psk() << ", name=" << fHLTPrescaleSet->name()
                      << " #L2 ps " << fHLTPrescaleSet->nL2() << " #EF ps " << fHLTPrescaleSet->nEF() << std::endl;

            // set the rerun prescale from the menu (so it is written to cool correctly)
            const std::vector<TrigConf::HLTChain*>& chains = fHLTFrame->getHLTChainList().chains();
            std::vector<TrigConf::HLTChain*>::const_iterator cIt = chains.begin();
            for(;cIt != chains.end();++cIt) {
               TrigConf::HLTChain *ch = *cIt;
               fHLTPrescaleSet->setrrps( ch->level(), ch->chain_counter(), ch->rerun_prescale() );
            }
            
            std::cout << "HLT PSS status after setting rrps: psk=" << fHLTPrescaleSet->psk() << ", name=" << fHLTPrescaleSet->name()
                      << " #L2 ps " << fHLTPrescaleSet->nL2() << " #EF ps " << fHLTPrescaleSet->nEF() << std::endl;

            // as long as we still use the prescales in /TRIGGER/HLT/MENU the HLTFrame needs to be updated
            fHLTFrame->set_hltPrescaleSet(new TrigConf::HLTPrescaleSet(*fHLTPrescaleSet)); // new one needed since HLTFrame takes the ownership
            
            std::cout << "HLT PSS status after setting pss in frame: psk=" << fHLTPrescaleSet->psk() << ", name=" << fHLTPrescaleSet->name()
                      << " #L2 ps " << fHLTPrescaleSet->nL2() << " #EF ps " << fHLTPrescaleSet->nEF() << std::endl;

            std::cout << "PSS pointer " << fHLTPrescaleSet << std::endl;
            std::cout << "NOW CALLING writeHltPrescalePayload( runnumber, lb, *fHLTPrescaleSet)" << runnumber << "  " << lb << std::endl;
            fConf2Cool->writeHltPrescalePayload( runnumber, lb, *fHLTPrescaleSet);

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
   }

   if(outf) outf->close();
   if(errf) errf->close();
   return 0;
}

