/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Han is a histogram analysis program using
 * the data-quality monitoring framework (DQMF)
 *
 * $Id: han.cxx,v 1.7 2009-02-09 15:19:59 ponyisi Exp $
 */

#include <iostream>
#include <string>
#include <cstdlib>

#include "DataQualityInterfaces/HanApp.h"
#include "DataQualityInterfaces/ConditionsSingleton.h"

namespace {

void usage( const std::string& command_name, int exit_code );

class CmdLineArgs {
public:
  CmdLineArgs( int argc, char *argv[] );
  
  std::string command;
  std::string hconfig;
  std::string data;
  std::string path;
  std::string output;
  std::string cmdlineConditions;
};

} // unnamed namespace


int main( int argc, char *argv[] )
{
  CmdLineArgs arg( argc, argv );
  
  std::string inputName( arg.data );
  std::string configName( arg.hconfig );
  std::string pathName( arg.path );
  std::string outputName;
  if( arg.output != "" ) {
    outputName = arg.output;
  }
  else {
    std::string::size_type s = inputName.size();
    std::string::size_type p = inputName.rfind( '.', s );
    outputName = inputName.substr( 0, p );
    outputName += "_han.root";
  }
  
  dqi::ConditionsSingleton::getInstance().setCondition(arg.cmdlineConditions);
  if (arg.cmdlineConditions != "") {
    std::cout<<"Input Conditions="<<dqi::ConditionsSingleton::getInstance().getCondition()
	     <<std::endl;
  }
  dqi::HanApp app;
  return app.Analyze( configName, inputName, outputName, pathName );
}


// ************************************************************
// Private Functions
// ************************************************************

namespace {

void usage( const std::string& command_name, int exit_code )
{
  std::string message;
  message += "\n";
  message += "This program takes as arguments the name of a configuration\n";
  message += "file and the name of the data file to be analyzed.\n";
  message += "One may optionally specify a path within the data file to analyze\n";
  message += "and additionally the name of the file with the output results.\n";
  message += "If no output file is specified, a name based on the input is used.\n";

  std::string::size_type s = command_name.size();
  std::string::size_type p = command_name.rfind( '/', s );
  std::string short_name = command_name.substr( p+1, s );

  std::cout << "\n";
  std::cout << "Usage: " << short_name << " <config_file> <data_file> [path [results_file]]\n";
  std::cout << message << "\n";
  exit(exit_code);
}


CmdLineArgs::
CmdLineArgs( int argc, char *argv[] )
{
  command = argv[0];
  if( argc > 6 ) usage( command, 1 );
  if( argc < 3 ) usage( command, 0 );
  
  hconfig = argv[1];
  data = argv[2];
  path = (argc == 4 || argc == 5 ||argc ==6) ? argv[3] : "";
  output = (argc == 5 || argc== 6) ? argv[4] : "";
  cmdlineConditions = (argc == 6) ? argv[5] : "";  
}

} // unnamed namespace

