/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCollQuery/TrigCollQuery.h"

#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

string          app;
TrigCollQuery   queryRemapper;


void usage() {
   cerr << "Usage: "  << app << " OPTIONS -runs <runlist> -query <query>" << endl
        << "  OPTIONS: -help -d -syntax <syntax> -runnumbercol <col_name> -xmlcfg <url> -trigsvc <url> -project <MC_project> -amitag <MC_tag> -cert <file> -key <file>" << endl;
}

void help() {
   usage();
   cerr <<
"  -runs <runs>  : decode triggers for given runs \n"
"                  example: 1,3,5-12,22 \n"
"  -query        : trigger query to be decoded.\n"
"                  example:  TriggerPassed(TrigName1, TrigName2, ...) && TriggerNotPassed(TrigNameX) \n"
"  -syntax       : query syntax:  ROOT (C++) or Relational.  Default is ROOT \n"
" \n"
"  -help         : print out this help \n"
"  -debug (-d)   : print debugging messages \n"
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
   string query;
   string srcTech("Root");
   bool useXMLConfig(false);
   string certpath, keypath;
   int i = 1;
   while( argc > i ) {
      string option = argv[i];
      if( option == "-help" ) {
         help();
         exit(0);
      }
      if( option == "-d" || option == "-debug" ) {
	 i++;
         queryRemapper.setDebug();
      } else if( option == "-xmlcfg" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -xmlcfg option", 2 );
         string url = argv[i];
         if( url == "default" )
            url = queryRemapper.defaultXMLConfigDir();
         queryRemapper.XMLConfigDir( url );
         useXMLConfig = true;
         cout << "Setting Trigger XML Config location to: " << url << endl;
	 i++;
      } else if( option == "-trigsvc" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -trigsvc option", 2 );
         string url = argv[i];
         if( url == "default" )
            url = queryRemapper.triggerWebService();
         queryRemapper.triggerWebService( url );
	 i++;
      } else if( option == "-project" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -project option", 2 );
         queryRemapper.setDataPeriod( argv[i] );
	 i++;
      } else if( option == "-amitag" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -amitag option", 2 );
         queryRemapper.setAMITag( argv[i] );
	 i++;
      } else if( option == "-cert" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -cert option", 2 );
         certpath = argv[i];
         if( keypath.empty() ) keypath = certpath;
	 i++;
      } else if( option == "-key" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -key option", 2 );
         keypath = argv[i];
	 i++;
      } else if( option == "-runnumbercol" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -runnumbercol option", 2 );
	 queryRemapper.runNumberColumn( argv[i] );
	 i++;
      } else if( option == "-runs" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -runs option", 2 );
	 queryRemapper.queryRuns( argv[i] );
	 i++;
      } else if( option == "-syntax" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -syntax option", 2 );
         srcTech = argv[i];
	 i++;
         // cout << "Setting input collection technology to " << srcTech << endl;
      } else if( option == "-query" ) {
	 i++;
	 if( argc <= i ) 
	    printError( "Missing parameter for -query option", 2 );
         query = argv[i];
	 i++;
      } 
   }

   std::transform(srcTech.begin(), srcTech.end(), srcTech.begin(), ::toupper);
   if( srcTech.substr(0,3) == "ROO" )
      srcTech = "RootCollection";
   else if( srcTech.substr(0,3) == "REL" || srcTech.substr(0,3) == "SQL" )
      srcTech = "RelationalCollection";
   else
      printError("Unrecongnized query syntax.  Use ROOT or RELATIONAL", 30);
         
   try {
      if( useXMLConfig ) {
         // cout << "Calling read XML Config" << endl;
         queryRemapper.readTriggerMap();
      } else {
         queryRemapper.setCertPath( certpath, keypath );
      }
      cout << queryRemapper.triggerQueryRemap( query, srcTech ) << endl;
   }catch( exception &e ) {
      printError(e.what(), 3);
   }
}
