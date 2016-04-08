/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCollQuery/TrigCollQuery.h"

#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <memory>

using namespace std;

string          app;
TrigCollQuery   queryRemapper;


void usage() {
   cerr << "Usage: "  << app << " [-help] [-d] -runs <runlist> OPTIONS  POOLUtilityName POOLoptions" << endl
        << "  OPTIONS: [-runnumbercol <col_name>] [ -xmlcfg <url>| -trigsvc <url> [-project <MC_project> -amitag <MC_tag>] [-cert <file> -key <file>] ]" << endl;
}

void help() {
   usage();
   cerr <<
"  -help         : print out this help \n"
"  -debug (-d)   : print some debugging messages \n"
"  -runs <runs>  : decode trigger queries for given runs \n"
"                  example: 1,3,5-12,22 \n"
"  -xmlcfg <url> : read trigger configuration XML files from directory <url> \n"
"                  do not use trigger configuration web service \n"
"                  'default' will use " << queryRemapper.defaultXMLConfigDir() << "\n"
"  -trigsvc <url>: use web service <url> for trigger configuration \n"
"                  'default' will use " << queryRemapper.triggerWebService() << "\n"
"    Options relevant to the use of the trigger web service:\n"
"    -project <name>: MC project name \n"
"    -amitag <tag>  : AMI tag for MC project \n"
"    -cert <file>   : path to certificate in PEM format, if X509_USER_PROXY is not defined \n"
"                     (proxy can he created by voms-proxy-init) \n"
"                     note: for non interactive use the certificate should be without a passphrase \n"
"    -key <file>    : path to certificate key in PEM format \n"
"\n"
"  -runnumbercol <attribute_name> \n"
"                : name of the attribute where run number is kept (default is RunNumber)\n"
" \n"
"  POOLUtilityName - name of the POOL Collection utility to run \n"
"  POOLoptions - standard options for the POOL Utility. Expected options: \n"
"    - src       : required to find out storage technology for query format. \n"
"                  Note: PHYSICAL_NAME assumes SQL query, LOGICAL_NAME is not supported \n"
"    - query     : query scanned for trigger names.  Use TriggerPassed(TrigName1,...) \n"
"                  expressions \n"
" \n"
"  More informatin on WWW page: \n"
"  https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TagTriggerQueries \n";
}

void printError(const string& err="", int exitcode=0) {
   cerr << app << ": ERROR - " << err << endl;
   usage();
   if( exitcode )
      exit(exitcode);
}



int main(int argc, const char *argv[])
{
   app =  basename( const_cast<char*>(argv[0]) );
   std::vector<std::unique_ptr<std::string> > remappedV;
   // number of items to skip from the beginning of the command line
   // when invoking a collection tool
   // default = 1 - just the name of the QueryTool itlself
   int  skipArgs = 1;

   bool useXMLConfig(false);
   int i = 1;
   bool	privateOptions = true;
   string certpath, keypath;
   while( privateOptions && argc > i ) {
      string option = argv[i];
      if( option == "-help" ) {
         help();
         exit(0);
      }
      if( option == "-d" || option == "-debug" ) {
	 i++, skipArgs++;
         queryRemapper.setDebug();
      } else if( option == "-xmlcfg" ) {
	 i++, skipArgs++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -xmlcfg option", 2 );
         string url = argv[i];
         if( url == "default" )
            url = queryRemapper.defaultXMLConfigDir();
         queryRemapper.XMLConfigDir( url );
         useXMLConfig = true;
         cout << "Setting Trigger XML Config location to: " << url << endl;
	 i++, skipArgs++;
      } else if( option == "-trigsvc" ) {
	 i++, skipArgs++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -trigsvc option", 2 );
         string url = argv[i];
         if( url == "default" )
            url = queryRemapper.triggerWebService();
         queryRemapper.triggerWebService( url );
	 i++, skipArgs++;
      } else if( option == "-project" ) {
	 i++, skipArgs++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -project option", 2 );
         queryRemapper.setDataPeriod( argv[i] );
	 i++, skipArgs++;
      } else if( option == "-amitag" ) {
	 i++, skipArgs++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -amitag option", 2 );
         queryRemapper.setAMITag( argv[i] );
	 i++, skipArgs++;
      } else if( option == "-cert" ) {
	 i++, skipArgs++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -cert option", 2 );
         certpath = argv[i];
         if( keypath.empty() ) keypath = certpath;
	 i++, skipArgs++;
      } else if( option == "-key" ) {
	 i++, skipArgs++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -key option", 2 );
         keypath = argv[i];
	 i++, skipArgs++;
      } else if( option == "-usectpword" ) {
	 queryRemapper.useCTPWord(true);
	 skipArgs++;
	 i++;
	 cout << "Using CTPWord atribute for L1 trigger" << endl;
      } else if( option == "-runnumbercol" ) {
	 i++, skipArgs++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -runnumbercol option", 2 );
	 queryRemapper.runNumberColumn( argv[i] );
	 i++, skipArgs++;
      } else if( option == "-runs" ) {
	 i++, skipArgs++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -runs option", 2 );
	 queryRemapper.queryRuns( argv[i] );
	 i++, skipArgs++;
      } else
	 privateOptions = false;
   }

   bool mixedTech = false;
   int ii = i;
   string srcTech;
   while( argc > ii ) {
      string option( argv[ii] );
      if( option == "-src" ) {
         while( argc > ii+2 && *argv[ii+1] != '-' ) {
            if( !srcTech.empty() && srcTech != argv[ii+2] ) {
               // error: tech mismatch
               mixedTech = true;
               break;
            }
            ii += 2;
            srcTech = argv[ii];
            if( srcTech == "PHYSICAL_NAME" ) {
               // Assume this is Relational
               srcTech = "RelationalCollection";
            }
            cout << "Setting input collection technology to " << srcTech << endl;
         }
      }
      ++ii;
   }
   if( mixedTech ) 
      printError("can not mix up imput ROOT and Relational collection types when querying triggers", 1 );
   if( srcTech.empty() ) 
      printError("unable to determine collection technology", 10 );

   while( argc > i ) {
      string option( argv[i] );
      if( option == "-query" ) {
         if( ++i >= argc ) 
            printError("missing query parameter", 2 );
         try {
            if( useXMLConfig ) {
               // cout << "Calling read XML Config" << endl;
               queryRemapper.readTriggerMap();
            } else {
               queryRemapper.setCertPath( certpath, keypath );
            }
            std::string remappedQ = queryRemapper.triggerQueryRemap( argv[i], srcTech );
            remappedV.emplace_back (new std::string (remappedQ));
            argv[i] = remappedV.back().get()->c_str();
         }catch( exception &e ) {
            printError(e.what(), 3);
         }
      }
      i++;
   } 

   // cout << ">>>  skipArgs is " << skipArgs << ", argc is " << argc << endl;
   if( skipArgs >= argc ) 
      printError("Utility to execute not specified ", 5 );

   cout << "new command: ";
   for( i = skipArgs; i < argc; i++ ) {
      cout << argv[i] << " ";
   }
   cout << endl;
   
   execvp( argv[skipArgs], (char* const*)(argv+skipArgs) );
   
   perror( (app+": Error - execvp() failed").c_str() ); 
   return 6;
}
