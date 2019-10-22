/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <vector>

#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfIO/TrigDBJobOptionsLoader.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1TopoMenu.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "boost/lexical_cast.hpp"


using namespace std;




struct Config {
public:
   ~Config(){}
   Config(){}

   std::vector<std::string> knownParameters { "file", "smk", "db", "write", "help", "h", "d", "detail" };

   // parameters
   // input
   std::vector<std::string> inputFiles {};
   unsigned int smk { 0 };
   std::string  dbalias { "TRIGGERDBDEV2" };

   // output
   std::string  base { "" };

   // other
   bool         help { false };
   bool         detail { false };
   // to keep track of configuration errors
   vector<string> error;

   // parses the commandline 
   void parseProgramOptions(int argc, char* argv[]);

   // help
   void usage();

};


void Config::usage() {

  cout << "The program needs to be run with the following specifications:\n\n";
  cout << "TriggerMenuRW <options>\n";
  cout << "\n";
  cout << "[Input options]\n";
  cout << "  --file                file1 [file2 [file3 ...]]     ... one or multiple json files\n";
  cout << "  --smk                 smk                           ... smk \n";
  cout << "  --db                  dbalias                       ... dbalias (default " << dbalias << ") \n";
  cout << "[Output options]\n";
  cout << "  --write               [base]                        ... to write out json files, e.g. L1menu[_<base>].json. base is optional.\n";
  cout << "[Other options]\n";
  cout << "  -h|--help                                           ... this help\n";
  cout << "  -d|--detail                                         ... prints detailed job options\n";
  cout << "\n\n";
  cout << "Examples\n";
  cout << "  --file L1menu.json HLTMenu.json                     ... read L1Menu.json and HLTMenu.json and show some basic statistics\n";

}

void
Config::parseProgramOptions(int argc, char* argv[]) {

   std::string currentParameter("");
   std::string listofUnknownParameters = "";

   for(int i=1; i<argc; i++) {

      std::string currentWord(argv[i]);
      bool isParam = currentWord[0]=='-'; // string starts with a '-', so it is a parameter name

      // get the parameter name
      int firstChar = currentWord.find_first_not_of('-');
      string paramName = currentWord.substr(firstChar);

      // check if the parameter is known
      if ( isParam && std::find(knownParameters.begin(), knownParameters.end(), paramName) == knownParameters.end() ) {
         listofUnknownParameters += " " + currentWord;
         continue;
      }

      if(isParam) {
         currentParameter = "";
         // check the boolean parameters
         if(paramName == "h" || paramName == "help" ) { help = true; continue; }
         if(paramName == "d" || paramName == "detail" ) { detail = true; continue; }
         currentParameter = paramName;
         continue;
      }

      // now treat the parameter values

      // inputs
      if(currentParameter == "file") { 
         inputFiles.push_back(currentWord);
         continue; 
      }
      if(currentParameter == "smk") { 
         smk = boost::lexical_cast<unsigned int,string>(currentWord);
         continue; 
      }
      if(currentParameter == "db") { 
         dbalias = currentWord;
         continue; 
      }

      // output
      if(currentParameter == "write") { 
         base = currentWord;
         continue; 
      }

   }

   // some sanity checks
   if ( inputFiles.size() == 0 and smk == 0 ) {
      error.push_back("No input specified! Please provide either input file(s) or smk");
   }

   if ( listofUnknownParameters.size() > 0 ) {
      error.push_back( string("Unknown parameter(s):") + listofUnknownParameters);
   }

}




int main(int argc, char** argv) {

   Config cfg;
   cfg.parseProgramOptions(argc, argv);
   if(cfg.help) {
      cfg.usage();
      return 0;
   }
   if(cfg.error.size()!=0) {
      for(const string & e: cfg.error)
         cerr << e << endl;
      cfg.usage();
      return 1;
   }

   if( cfg.inputFiles.size()>0 ) {
      // load config from files

      // file loader
      TrigConf::JsonFileLoader fileLoader;

      for (const std::string & fn : cfg.inputFiles) {

         // check if the file is L1 or HLT
         std::string triggerLevel;
         fileLoader.checkTriggerLevel( fn, triggerLevel);
         
         if(triggerLevel == "L1") {
            TrigConf::L1Menu l1menu;
            fileLoader.loadFile( fn, l1menu);
            cout << "Loaded L1 menu file " << fn << endl;
            l1menu.printStats();
         } else if(triggerLevel == "HLT" ) {
            TrigConf::HLTMenu hltmenu;
            fileLoader.loadFile( fn, hltmenu);
            cout << "Loaded HLT menu file " << fn << " with " << hltmenu.size() << " chains" << endl;
         } else {
            cerr << "File " << fn << " not recognized as being an L1 or HLT menu" << endl;
         }

      }
   }

   if( cfg.smk != 0 ) {
      // load config from DB

      // db menu loader
      TrigConf::TrigDBMenuLoader dbloader(cfg.dbalias);

      TrigConf::L1Menu l1menu;
      TrigConf::HLTMenu hltmenu;
      
      dbloader.loadL1Menu( cfg.smk, l1menu );
      if (l1menu) {
         cout << "Loaded L1 menu with " << l1menu.size() << " items" <<  endl;
         l1menu.printStats();
      } else {
         cout << "Did not load an L1 menu" << endl;
      }

      dbloader.loadHLTMenu( cfg.smk, hltmenu );
      if (hltmenu) {
         cout << "Loaded HLT menu with " << hltmenu.size() << " chains" << endl;
      } else {
         cout << "Did not load an HLT menu" << endl;
      }

      // db job options loader
      TrigConf::TrigDBJobOptionsLoader jodbloader(cfg.dbalias);

      TrigConf::DataStructure jo;
      jodbloader.loadJobOptions( cfg.smk, jo );
      if (jo) {
         cout << "Loaded job options with " << jo.getObject("properties").getKeys().size() << " entries " << endl;
         if( cfg.detail ) {
            for( const auto alg : jo.getObject("properties").data()) {
               std::cout << alg.first << std::endl;
               for( const auto prop : alg.second ) {
                  std::cout << "      " << prop.first << " -> " << prop.second.data() << std::endl;
               }
            }
         }
      } else {
         cout << "Did not load job options" << endl;
      }


   }


   return 0;
}
