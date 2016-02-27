/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdexcept>
#include <memory>

#include "PersistentDataModel/Guid.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCLeaf.h"
#include "FileCatalog/FCImpl.h"
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/URIParser.h"

class InsertFileToCatalogApplication {
public:
  InsertFileToCatalogApplication( int argc, char* argv[] );
  ~InsertFileToCatalogApplication(){}
  bool parseArguments();
  void execute();
  void printSyntax();
private:
  const std::string m_executableName;
  std::string m_technologyName;
  std::string m_fcURL;
  std::vector< std::string > m_fileNames;
  std::string m_guid;
};

InsertFileToCatalogApplication::InsertFileToCatalogApplication( int argc, char* argv[] ):
  m_executableName( std::string( argv[0] ) ),
  m_technologyName( "PoolCollection" ),
  m_fcURL( "" ),
  m_fileNames()
{
  for ( int i = 1; i < argc; ++i )
    m_fileNames.push_back( std::string( argv[i] ) );
}

bool
InsertFileToCatalogApplication::parseArguments()
{
  std::vector< std::string > theFiles;
  std::vector< std::string > args = m_fileNames;
  unsigned int excludedArgument = 0;
  Guid dummy;
  Guid::create(dummy);
  m_guid = dummy.toString();
  for ( unsigned int iArg = 0; iArg < args.size(); ++iArg ) {
    if ( iArg > 0 && iArg == excludedArgument )
      continue;
    const std::string& arg = args[iArg];
    if ( arg == "-u" ) {
      unsigned int nextArgumentIndex = iArg + 1;
      if ( nextArgumentIndex < args.size() ) {
        excludedArgument = nextArgumentIndex;
        m_fcURL = args[nextArgumentIndex];
      }
    }
    else if ( arg == "-t" ) {
      unsigned int nextArgumentIndex = iArg + 1;
      if ( nextArgumentIndex < args.size() ) {
        excludedArgument = nextArgumentIndex;
        m_technologyName = args[nextArgumentIndex];
      }
    }
    else if ( arg == "-g" ) {
      unsigned int nextArgumentIndex = iArg + 1;
      if ( nextArgumentIndex < args.size() ) {
        excludedArgument = nextArgumentIndex;
        if (args[nextArgumentIndex].length()==36) m_guid = args[nextArgumentIndex];
        else std::cout << "-g argument does not match needed GUID length" << std::endl;
      }
    }
    else {
      theFiles.push_back( arg );
    }
  }

  // Check now the values.
  if ( theFiles.empty() ) return false;
  if ( theFiles.size() > 1 ) {
    std::cout << "Too many files, taking first only! " << std:: endl; 
    return false;
  }
  m_fileNames.clear();
  m_fileNames.push_back(theFiles[0]);


  return true;
}


void
InsertFileToCatalogApplication::execute()
{
  // Do the job here

  bool allOK = true;

  // Open the file catalog and insert the pfn/fid/technology
  if ( allOK ) {
    pool::URIParser p( m_fcURL );
    p.parse();
    std::auto_ptr<pool::IFileCatalog> catalog( new pool::IFileCatalog );
    if ( ! catalog.get() ) 
      throw std::runtime_error( "Could not create a file catalog" );
    catalog->setWriteCatalog( p.contactstring() );
    catalog->connect();
    catalog->start();
    
    pool::PFNEntry entry( m_fileNames[0], m_guid, m_technologyName );
    pool::FCLeaf* leaf = 
      dynamic_cast< pool::FCLeaf* >( catalog->getWriteCatalog() );
    if (leaf)
      leaf->getImpl()->insertPFN( entry );
    
    catalog->commit();
  }
}

void
InsertFileToCatalogApplication::printSyntax()
{
  std::cout << "Syntax : " << m_executableName << " [-u fileCatalog] [-t technologyType] [-g input guid] file" << std::endl;
}




int main( int argc, char* argv[] )
{
  try {
    InsertFileToCatalogApplication app( argc, argv );
    if ( app.parseArguments() ) {
      app.execute();
    }
    else app.printSyntax();
  }
  catch ( std::exception& error ) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
  catch ( ... ) {
    std::cerr << "Funny error ..." << std::endl;
    return 1;
  }
  return 0;
}
