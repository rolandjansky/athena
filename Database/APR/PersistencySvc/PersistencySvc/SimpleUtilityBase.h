/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_SIMPLEUTILITYBASE
#define POOL_SIMPLEUTILITYBASE

#include <iostream>
#include <stdexcept>

#include "StorageSvc/IStorageSvc.h"
#include "StorageSvc/IStorageExplorer.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbOption.h"
#include "StorageSvc/FileDescriptor.h"
#include "StorageSvc/poolDb.h"


/* Common code for the POOL utilites */
namespace pool {
   
   class SimpleUtilityBase
   {
  public:
     SimpleUtilityBase( int argc = 0, char* argv[] = 0 );
     virtual ~SimpleUtilityBase();

     virtual int	run();

     virtual bool 	parseArguments(); 
     virtual void 	startSession();
     virtual void 	readFileGUIDs();
     virtual std::string readFileGUID( const std::string& pfn );

     virtual void 	execute() {}
     virtual void 	printSyntax() {}

  protected:
     const std::string	executableName;
     std::string 		technologyName;

     pool::Session* 	session;
     pool::IStorageSvc* 	storageSvc;
     pool::IStorageExplorer* storageExplorer;

     std::vector< std::string > args;
     std::vector< std::string > fileNames;
     std::vector< std::pair< std::string, std::string > > fidAndPfn;
   };



   SimpleUtilityBase::SimpleUtilityBase( int argc, char* argv[] ):
         executableName( std::string( argv[0] ) ),
         technologyName( pool::ROOT_StorageType.storageName() ),
         session( 0 ),
         storageSvc( 0 ),
         storageExplorer( 0 )
   {
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


   void
   SimpleUtilityBase::startSession()
   {
      storageSvc = pool::createStorageSvc( 0, "StorageSvc");
      if( !storageSvc ) {
         throw std::runtime_error( "Could not create a StorageSvc object" );
      }
      storageSvc->addRef();
      void* pVoid = 0;
      pool::DbStatus sc = storageSvc->queryInterface( pool::IStorageExplorer::interfaceID(), &pVoid );
      storageExplorer = (pool::IStorageExplorer*)pVoid;
      if( !( sc.isSuccess() && storageExplorer ) ) {
         throw std::runtime_error( "Could not retrieve a IStorageExplorer interface" );
      }
      long technologyId = pool::DbType::getType( technologyName ).majorType();
      if( ! storageExplorer->startSession( poolDb::READ, technologyId, session ).isSuccess() ) {
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
      if( ! storageExplorer->connect( session, poolDb::READ, fd ).isSuccess() ) {
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
      for( std::vector< std::string >::const_iterator iFile = fileNames.begin();
           iFile != fileNames.end(); ++iFile ) {
         const std::string& pfn = *iFile;
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

} // pool namespace
#endif
