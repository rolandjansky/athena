/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>

#include "PersistencySvc/SimpleUtilityBase.h"

using namespace pool;


class ExtractFileIdentifierApplication  : public SimpleUtilityBase {
public:
  ExtractFileIdentifierApplication( int argc, char* argv[] );

  bool parseArguments();
  void execute();
  void printSyntax();
};


ExtractFileIdentifierApplication::ExtractFileIdentifierApplication( int argc, char* argv[] )
  : SimpleUtilityBase( argc, argv )
{ }


bool
ExtractFileIdentifierApplication::parseArguments()
{
  unsigned int excludedArgument = 0;
  for ( unsigned int iArg = 0; iArg < args.size(); ++iArg ) {
    if ( iArg > 0 && iArg == excludedArgument )
      continue;
    const std::string& arg = args[iArg];
    if ( arg == "-t" ) {
      unsigned int nextArgumentIndex = iArg + 1;
      if ( nextArgumentIndex < args.size() ) {
        excludedArgument = nextArgumentIndex;
        technologyName = args[nextArgumentIndex];
      }
    }
    else {
       fileNames.push_back( arg );
    }
  }
  return SimpleUtilityBase::parseArguments();
}



void
ExtractFileIdentifierApplication::execute()
{
   startSession();
   readFileGUIDs();
   for( std::vector< std::pair< std::string, std::string > >::const_iterator iFile = fidAndPfn.begin();
	iFile != fidAndPfn.end(); ++iFile ) {
      std::cout << iFile->first << " (" << iFile->second << ")" << std::endl; 
   }
}


void
ExtractFileIdentifierApplication::printSyntax()
{
  std::cerr << "Syntax : " << executableName << " [-t technologyType] files" << std::endl;
}




int main( int argc, char* argv[] )
{
   ExtractFileIdentifierApplication	app( argc, argv );
   return app.run();
}
