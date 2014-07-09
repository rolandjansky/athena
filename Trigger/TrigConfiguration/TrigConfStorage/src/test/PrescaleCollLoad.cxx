/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     PrescaleCollLoad.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Stelzer (CERN)	Joerg.Stelzer@cern.ch 
//CREATED:  29. Jan. 2009
//                                                     
//PURPOSE: 
//
// This standalone application is designed to read all HLT prescale
// set that are listed in the PrescaleSetCollection table.
//
//////////////////////////////////////////////////////////////////////


#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/XMLStorageMgr.h"

#include "TrigConfStorage/IStorageMgr.h"

#include "TrigConfHLTData/HLTPrescaleSetCollection.h"
#include "TrigConfStorage/IHLTPrescaleSetCollectionLoader.h"


#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"

#include <iostream>
#include <fstream>
#include <string>

using std::endl;

class TrigConfError {
 public:
  TrigConfError(std::string s, uint c) : what(s), code(c) {};
  ~TrigConfError(){};
  std::string what;
  uint code;
};

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

std::string test_trig_connection = "TRIGGERDB";


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
  o << "|   --log            <string>     * name of the log file" << lineend;
  o << "| " << lineend;                    
  o << "|   --trigdb          <string>    * Trigger DB" << lineend;
  o << "|                                   test value: " << test_trig_connection << lineend;
  o << "|   --part            <string>      Partition name [default TDAQ_PARTITION if defined, otherwise USER]" << lineend;
  o << "| " << lineend;                    
  o << "| ================================================================================" << lineend;
}

class JobConfig {
public:
   
   JobConfig() :
      fTriggerDBConnection(""),
      fPartitionName(""),
      fLogfileName("")
   {
      const char* part = getenv("TDAQ_PARTITION");
      if(part) {
         fPartitionName=std::string(part);
      } else {
         const char* user = getenv("USER");
         if(user) fPartitionName=std::string(user);
      }

   }

   ~JobConfig(){}

   void SetTriggerDbConnection(const std::string & x) { fTriggerDBConnection = x; }
   std::string TriggerDbConnection() const { return fTriggerDBConnection; }

   void SetLogfileName(const std::string & fn) { fLogfileName = fn; }
   const std::string & LogfileName() const { return fLogfileName; }

   void SetPartitionName(const std::string & pn) { fPartitionName = pn; }
   const std::string & PartitionName() const { return fPartitionName; }
	
   void CheckForCompleteSetup();

   void PrintCompleteSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ));

private:
   std::string        fTriggerDBConnection;
   std::string        fPartitionName;
   std::string        fLogfileName;
} gConfig;


void JobConfig::CheckForCompleteSetup() {
  // check if a complete setup has been specified and that the options
  // are consistent 
}


//void JobConfig::PrintCompleteSetup(MsgStream & log) {
void JobConfig::PrintCompleteSetup(std::ostream & log, std::ostream& (*lineend) ( std::ostream& os ) ) {
  // check if a complete setup has been specified and that the options
  // are consistent 
  log << "========================================" << lineend;
  log << "JOB SETUP:" << lineend
      << "----------" << lineend;	
  log << "Will read prescale set collection!" << lineend;
  log << "TriggerDB connection: " << TriggerDbConnection() << lineend;
  log << "Partition name: " << PartitionName() << lineend;
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
      if(stripped=="trigdb" || stripped=="log" || stripped=="part" )
         { currentPar = stripped; continue; }
      
      listofUnknownParameters += " " + stripped;

    } else { // string starts with a character other than '-', so it is a parameter value

			//if(currentPar=="") throw TrigConfError(std::string("No value expected, but found: ")+stripped, 1); // throw error of unknown value: code 1

      if(currentPar == "trigdb")     { gConfig.SetTriggerDbConnection(stripped); continue; }
      if(currentPar == "log")        { gConfig.SetLogfileName(stripped); continue; }
      if(currentPar == "part")       { gConfig.SetPartitionName(stripped); continue; }
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
  
   // setup the connection to the TriggerDB
   std::string  configSource = gConfig.TriggerDbConnection();

   TrigConf::HLTPrescaleSetCollection* prescaleSetCollection = new TrigConf::HLTPrescaleSetCollection();
      
   try {

      uint requestcount = 1;

      TrigConf::StorageMgr* sm = new TrigConf::StorageMgr(gConfig.TriggerDbConnection(),"","",log);

      char c='1';
      while(c!='0') {  
         c='1';
         std::cout << "0) ..... Quit" << std::endl;
         std::cout << "1) ..... Continue" << std::endl << std::endl;
         std::cout << "Choice: "; std::cin >> c;

         sm->hltPrescaleSetCollectionLoader().load( *prescaleSetCollection, requestcount++, gConfig.PartitionName() );

      }


   }
   catch(std::exception & e) {
      log << "Caught std::exception: " << e.what() << lineend;
      if(outf) outf->close();
      if(errf) errf->close();
      return 1;
   }

   if(outf) outf->close();
   if(errf) errf->close();
   return 0;
}

