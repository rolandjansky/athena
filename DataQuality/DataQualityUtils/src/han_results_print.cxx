/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>

#include "DataQualityUtils/HanOutputFile.h"


namespace {

void usage( const std::string& command_name, int exit_code );

class CmdLineArgs {
public:
  CmdLineArgs( int argc, char *argv[] );
  
  std::string command;
  std::string hresults;
};

} // unnamed namespace



int main( int argc, char *argv[] )
{
  CmdLineArgs arg( argc, argv );
  
  std::string resultsName( arg.hresults );
  
  if (!std::filesystem::exists(resultsName)) {
    std::cerr << "File " << resultsName << " does not seem to exist. Exiting" << std::endl;
    return 1;
  }

  dqutils::HanOutputFile of(resultsName);
  of.printAllDQAssessments();
  
  return 0;
}


// ************************************************************
// Private Functions
// ************************************************************

namespace {

void usage( const std::string& command_name, int exit_code )
{
  std::string message;
  message += "\n";
  message += "This program takes as an argument the name of a \'han\' results\n";
  message += "file and prints the statuses of all assessments to stdout.\n";

  std::string::size_type s = command_name.size();
  std::string::size_type p = command_name.rfind( '/', s );
  std::string short_name = command_name.substr( p+1, s );

  std::cout << "\n";
  std::cout << "Usage: " << short_name << " <results_file>\n";
  std::cout << message << "\n";
  exit(exit_code);
}


CmdLineArgs::
CmdLineArgs( int argc, char *argv[] )
{
  command = argv[0];
  if( argc > 2 ) usage( command, 1 );
  if( argc < 2 ) usage( command, 0 );
  
  hresults = argv[1];
}

} // unnamed namespace

