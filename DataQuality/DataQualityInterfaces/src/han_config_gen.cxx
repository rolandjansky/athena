/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * $Id: han_config_gen.cxx,v 1.3 2009-02-09 15:19:59 ponyisi Exp $
 */

#include <iostream>
#include <string>
#include <cstdlib>

#include "DataQualityInterfaces/HanConfig.h"
#include "DataQualityInterfaces/ConditionsSingleton.h"

namespace {

void usage( const std::string& command_name, int exit_code );

class CmdLineArgs {
public:
  CmdLineArgs( int argc, char *argv[] );
  
  std::string command;
  std::string mconfig;
  std::string conds;
};

} // unnamed namespace


int main( int argc, char *argv[] )
{
  CmdLineArgs arg( argc, argv );
  
  std::string infileName( arg.mconfig );
  dqi::ConditionsSingleton::getInstance().setCondition(arg.conds);
  //std::cout<<__PRETTY_FUNCTION__<<"Input Conditions="<<dqi::ConditionsSingleton::getInstance().getCondition()
  //<<std::endl;
  std::string outfileName;
  
  std::string::size_type s = infileName.size();
  std::string::size_type p = infileName.rfind( '.', s );
  outfileName = infileName.substr( 0, p );
  outfileName += ".hcfg";
  
  dqi::HanConfig config;
  config.AssembleAndSave( infileName, outfileName );
  return 0;
}


// ************************************************************
// Private Functions
// ************************************************************

namespace {

CmdLineArgs::
CmdLineArgs( int argc, char *argv[] )
{
  command = argv[0];
  if( argc > 3 ) usage( command, 1 );
  if( argc < 2 ) usage( command, 0 );
  
  mconfig = argv[1];
  if(argc==3)conds=argv[2];
}


void usage( const std::string& command_name, int exit_code )
{
  std::string message;
  message += "\n";
  message += "This program takes as an argument the name of a file containing\n";
  message += "a MiniConfig configuration and produces a similarly named Root file\n";
  message += "containing a configuration for the \'han\' application\n";

  std::string::size_type s = command_name.size();
  std::string::size_type p = command_name.rfind( '/', s );
  std::string short_name = command_name.substr( p+1, s );

  std::cout << "\n";
  std::cout << "Usage: " << short_name << " <filename>\n";
  std::cout << message << "\n";
  exit(exit_code);
}


} // unnamed namespace
