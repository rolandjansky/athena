/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>

#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCLeaf.h"
#include "FileCatalog/FCImpl.h"
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/URIParser.h"

#include "PersistencySvc/SimpleUtilityBase.h"

using namespace pool;

class InsertFileToCatalogApplication : public SimpleUtilityBase {
public:
  InsertFileToCatalogApplication( int argc, char* argv[] );

  bool parseArguments(); 
  void execute();
  void printSyntax();

private:
  std::string 		m_URL; 
};



InsertFileToCatalogApplication::InsertFileToCatalogApplication( int argc, char* argv[] )
      : SimpleUtilityBase( argc, argv )
{ }

   
bool
InsertFileToCatalogApplication::parseArguments()
{
  unsigned int excludedArgument = 0;
  for ( unsigned int iArg = 0; iArg < args.size(); ++iArg ) {
     if ( iArg > 0 && iArg == excludedArgument )
	continue;
     const std::string& arg = args[iArg];
     if ( arg == "-u" ) {
	unsigned int nextArgumentIndex = iArg + 1;
	if( nextArgumentIndex < args.size() ) {
	   excludedArgument = nextArgumentIndex;
	   m_URL = args[nextArgumentIndex];
	}
     }
     else if( arg == "-t" ) {
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
InsertFileToCatalogApplication::execute()
{
   startSession();
   readFileGUIDs();
   
  // Open the file catalog and insert the pfn/fid/technology
  pool::URIParser p( m_URL );
  p.parse();
  std::auto_ptr<pool::IFileCatalog> catalog( new pool::IFileCatalog );
  catalog->setWriteCatalog( p.contactstring() );
  catalog->connect();
  catalog->start();    
  pool::FCLeaf* leaf = dynamic_cast< pool::FCLeaf* >( catalog->getWriteCatalog() );
  if (!leaf)
    std::cerr << "InsertFileToCatalogApplication: dynamic_cast failed.\n";
  else {
    for ( std::vector< std::pair< std::string, std::string > >::const_iterator iFile = fidAndPfn.begin();
          iFile != fidAndPfn.end(); ++iFile ) {
      pool::PFNEntry entry( iFile->second, iFile->first, technologyName );
      leaf->getImpl()->insertPFN( entry );
    }    
  }
  catalog->commit();
}


void
InsertFileToCatalogApplication::printSyntax()
{
  std::cout << "Syntax : " << executableName << " [-u fileCatalog] [-t technologyType] files" << std::endl;
}




int main( int argc, char* argv[] )
{
   InsertFileToCatalogApplication	app( argc, argv );
   return app.run();
}
