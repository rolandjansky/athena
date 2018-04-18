/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdexcept>

#include "PersistencySvc/SimpleUtilityBase.h"

#include "StorageSvc/IStorageSvc.h"
#include "StorageSvc/IStorageExplorer.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbOption.h"
#include "StorageSvc/FileDescriptor.h"

using namespace pool;


SimpleUtilityBase::SimpleUtilityBase( int argc, char* argv[] ):
      technologyName( pool::ROOT_StorageType.storageName() )
{
   if( argc > 0 && argv[0] )
      executableName = argv[0];
   for ( int i = 1; i < argc; ++i )
      args.push_back( std::string( argv[i] ) );
}


SimpleUtilityBase::~SimpleUtilityBase()
{
   if( storageExplorer ) {
      if( session ) storageExplorer->endSession( session );
      storageExplorer->release();
   }
   if( storageSvc ) storageSvc->release();
}


bool SimpleUtilityBase::parseArguments()
{
   if( fileNames.empty() )
      return false;
   pool::DbType theType = pool::DbType::getType( technologyName );
   if ( theType.match( pool::TEST_StorageType ) ) {
      throw std::runtime_error( "Unsupported file type : " + technologyName );
   }
   return true;
}


void SimpleUtilityBase::startSession()
{
   storageSvc = pool::createStorageSvc("StorageSvc");
   if( !storageSvc ) {
      throw std::runtime_error( "Could not create a StorageSvc object" );
   }
   storageSvc->addRef();
   void* pVoid = 0;
   pool::DbStatus sc = storageSvc->queryInterface( pool::IStorageExplorer::interfaceID(), &pVoid );
   storageExplorer = static_cast<pool::IStorageExplorer*>(pVoid);
   if( !( sc.isSuccess() && storageExplorer ) ) {
      throw std::runtime_error( "Could not retrieve a IStorageExplorer interface" );
   }
   long technologyId = pool::DbType::getType( technologyName ).majorType();
   if( ! storageExplorer->startSession( pool::READ, technologyId, session ).isSuccess() ) {
      throw std::runtime_error( "Could not start a new session" );
   }
   if( technologyId == pool::ROOT_StorageType.majorType() ) {
      // Set ERROR_LEVEL ROOT domain option to kError (==3000)
      // to disable warnings about unknown classes when opening the file
      // ----  (NOT needed when nostreamers option is used)
      pool::DbOption        err_lvl("ERROR_LEVEL", "", 3000); 
      storageExplorer->setDomainOption(session, err_lvl);
       
      // Tell ROOT to not autoload dictionaries (speedup)
      pool::DbOption        no_streamers("FILE_READSTREAMERINFO", "", 0);
      //  (DISABLED! Seems to break ROOT 5.20 ability to read 5.18 files)
      // storageExplorer->setDomainOption(session, no_streamers);
   }
}


std::string SimpleUtilityBase::readFileGUID( const std::string& pfn )
{
   std::string fid;
   pool::FileDescriptor fd( fid, pfn );
   if( ! storageExplorer->connect( session, pool::READ, fd ).isSuccess() ) {
      throw std::runtime_error( "Could not open file \"" + pfn + "\"" );
   }
   if( ! storageExplorer->dbParam( fd, "FID", fid ).isSuccess() ) {
      storageExplorer->disconnect( fd );
      throw std::runtime_error( "Could not retrieve the FID from file \"" + pfn + "\"" );
   }
   storageExplorer->disconnect( fd );
   return fid;
}


void SimpleUtilityBase::readFileGUIDs()
{
   for( const auto& pfn : fileNames ) {
      std::string fid = readFileGUID( pfn );
      fidAndPfn.push_back( std::make_pair( fid, pfn ) );
   }
}


int SimpleUtilityBase::run()
{
   try {
      if( parseArguments() ) {
         execute();
      }
      else printSyntax();
   }
   catch ( std::exception& error ) {
      std::cerr << executableName << ": " << error.what() << std::endl;
      return 1;
   }
   catch ( ... ) {
      std::cerr << executableName << ": Unrecognized exception!" << std::endl;
      return 1;
   }
   return 0;
}


//  ------------------   Utils -----------------------
std::string Utils::readFileGUID( const std::string& pfn )
{
   SimpleUtilityBase util;
   util.startSession();
   return util.readFileGUID( pfn );
}
