/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdexcept>

#include "HLTRates2COOL/HLTCountCoolWriter.h"
#include "HLTRates2COOL/HLTCoolFolderSpec.h"
#include "HLTRates2COOL/HLTCounter.h"

#include "CoolApplication/Application.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoralKernel/Context.h"

#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ILookupService.h"
#include "RelationalAccess/IAuthenticationService.h"
#include "RelationalAccess/IAuthenticationCredentials.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"
#include "RelationalAccess/IDatabaseServiceSet.h"

void
hltca::HLTCountCoolWriter::createSchema(bool print) {

   bool needToOpen = !dbIsOpen();

   if(needToOpen) {
      try {
         openDb(false);
      }
      catch(cool::DatabaseDoesNotExist& e) {
         m_ostream << "*** : " << e.what() << std::endl;
         m_ostream << "Will create a new database: " << m_dbId << std::endl;
         createDb();
         openDb(false);
      }
      catch(cool::Exception& e) {
         m_ostream << "*** COOL  exception caught: " << e.what() << std::endl;
         m_ostream << "Coudn't create a new schema: " << m_dbId << std::endl;
         throw;
      }
   } // End if(needToOpen)

   try {
      m_ostream << "Creating schema '" << m_dbId << "'" << std::endl;
      HLTCoolFolderSpec::createFolderStructure(m_dbPtr);
      if(print) printSchema();
   }
   catch(cool::Exception& e) {
      m_ostream << "*** COOL  exception caught: " << e.what() << std::endl;
      m_ostream << "Coudn't create a new schema: " << m_dbId << std::endl;
      throw;
   }
   if(needToOpen) { 
      closeDb();
   }

   return;
}



//------------------------------------------------------------
// printSchema(ostream &)
// ------------------------------------------------------------
void
hltca::HLTCountCoolWriter::printSchema() {

  bool needToOpen = !dbIsOpen();

  bool readonly = true;
  if(needToOpen) openDb(readonly);

  HLTCoolFolderSpec::printFolderStructure(m_dbPtr, m_ostream);

  if(needToOpen) closeDb();

  return;
}


//------------------------------------------------------------
// dbIsOpen()
// ------------------------------------------------------------
bool
hltca::HLTCountCoolWriter::dbIsOpen() {
  return m_dbPtr.use_count()>0 && m_dbPtr->isOpen();
}




cool::IDatabaseSvc&
hltca::HLTCountCoolWriter::databaseService()
{
   static cool::Application app;
   return app.databaseService();
}

// ------------------------------------------------------------
// dropDb()
// ------------------------------------------------------------
void
hltca::HLTCountCoolWriter::dropDb()
{
//   m_ostream << "Dropping database '" << m_dbId << "'" << std::endl;
//   try
//     {
//       cool::IDatabaseSvc& dbSvc = databaseService();
//       dbSvc.dropDatabase( m_dbId ); // do nothing if the database does not exist
//     }

//   catch(std::exception& e)
//     {
//       m_ostream << "*** COOL  exception caught: " << e.what() << std::endl;
//       m_ostream << "Couldn't drop conditions database: " << m_dbId << std::endl;
//     }

  return;
}


// ------------------------------------------------------------
// openDb(bool readOnly)
// ------------------------------------------------------------
cool::IDatabasePtr
hltca::HLTCountCoolWriter::openDb( bool readOnly )
{
   if(dbIsOpen())
      return m_dbPtr;

   cool::IDatabaseSvc& dbSvc = databaseService();
  
   try {
      m_ostream << "Opening database '" << m_dbId << "'" << std::endl;
      m_dbPtr = dbSvc.openDatabase( m_dbId, readOnly );   // If the database does not exist, an exception is thrown
      return m_dbPtr;
   } 
   catch(cool::DatabaseDoesNotExist& e) {
      if(readOnly) {
         m_ostream << "Database '" << m_dbId << "' does not exist" << std::endl;
         throw;
      }
      else {
         m_ostream << "Database '" << m_dbId << "' does not exist, try to create it" << std::endl;
         m_dbPtr = dbSvc.createDatabase( m_dbId );
         m_ostream << "Created DB" << std::endl;
         return m_dbPtr;
      }
   }
   catch(cool::Exception& e) {
      m_ostream << "*** COOL  exception caught: " << e.what() << std::endl;
      m_ostream << "Couldn't open database " << m_dbId << std::endl;
      throw;
   }
}




// ------------------------------------------------------------
// closeDb()
// ------------------------------------------------------------
void
hltca::HLTCountCoolWriter::closeDb()
{
   if(dbIsOpen()) {
      try {
         m_ostream << "Closing database '" << m_dbId << std::endl;
         m_dbPtr->closeDatabase();
      }
      catch(std::exception& e) {
         m_ostream << "*** COOL  exception caught: " << e.what() << std::endl
                   << "Couldn't close conditions database: " << m_dbId << std::endl;
      }
   }
   return;
}



//------------------------------------------------------------
// createDb()
// ------------------------------------------------------------
cool::IDatabasePtr
hltca::HLTCountCoolWriter::createDb()
{
  m_ostream << "Creating database '" << m_dbId << "'" << std::endl;
  try
    {
      cool::IDatabaseSvc& dbSvc = databaseService();
      m_dbPtr = dbSvc.createDatabase( m_dbId );
    }
  catch(std::exception& e)
    {
      m_ostream << "*** COOL  exception caught: " << e.what() << std::endl;
      m_ostream << "Coudn't create a new conditions database: " << m_dbId << std::endl;
      throw;
    }

  return m_dbPtr;
}



void
hltca::HLTCountCoolWriter::rangeInfo(const std::string& content,
                           cool::ValidityKey since,
                           cool::ValidityKey until) 
{
   
   if(since==cool::ValidityKeyMin && until==cool::ValidityKeyMax) {
      std::cout << "Writing " << content 
                << " for infinite range [0,oo]" 
                << std::endl;
      return;
   }

   unsigned long long lb[2];
   unsigned long long run[2];

   lb[0] = since & 0xFFFFFFFF;
   lb[1] = until & 0xFFFFFFFF;

   run[0] = since>>32;
   run[0] &= 0x7FFFFFFF;
   run[1] = until>>32;
   run[1] &= 0x7FFFFFFF;

   if(lb[0]==0 && lb[1]==0) {
      unsigned int first = run[0];
      unsigned int last  = run[1]-1;
      if(first==last) {
         std::cout << "IOV Info: writing " << content 
                   << " for run " << first 
                   << std::endl;
      } else {
         std::cout << "IOV Info: writing " << content 
                   << " for run range [" << first << "," << last << "]" 
                   << std::endl;
      }
   }
   else {
      if(run[0]==run[1]) {
         if(lb[0]==lb[1]-1) {
            std::cout << "IOV Info: writing " << content 
                      << " for run " << run[0] 
                      << " and LB " << lb[0] 
                      << std::endl;
         } else {
            std::cout << "IOV Info: writing " << content 
                      << " for run " << run[0] 
                      << " and LB range [" << lb[0] << "," << lb[1]-1 << "]" 
                      << std::endl;
         }
      } else {
         std::cout << "IOV Info: writing " << content 
                   << " for range [" << run[0] << "/" 
                   << lb[0] << "," << run[1] << "/" << lb[1] << "[" 
                   << std::endl;
      }
   }
}

void
hltca::HLTCountCoolWriter::writeHLTCountersPayload(unsigned int runNumber,
                                                   unsigned int lumiblockNumber,
                                                   const std::vector< HLTCounter>& counters,
                                                   HLTCounter::TriggerLevel level)
{
  bool infiniteRange = (runNumber & 0x80000000) != 0; // highest bin==1 indicates infinite run range to be set (would not give valid iov anyway)
  cool::ValidityKey since(runNumber);
  cool::ValidityKey until(runNumber);
  if(infiniteRange) {
     since = cool::ValidityKeyMin;
     until = cool::ValidityKeyMax;
  } else {
     since <<= 32;
     until <<= 32;
     since += lumiblockNumber;
     until += lumiblockNumber+1;
  }

  bool needToOpen = !dbIsOpen();
  if(needToOpen) openDb(false);
  HLTCoolFolderSpec::createFolderStructure(m_dbPtr); // just in case it doesn't exist
  writeHLTCountersPayload( since, until, counters, level );
  if(needToOpen) closeDb();
}

 
void
hltca::HLTCountCoolWriter::writeHLTCountersPayload( cool::ValidityKey since,
                                                    cool::ValidityKey until,
                                                    const std::vector<HLTCounter>& counters,
                                                    HLTCounter::TriggerLevel level)
{

   bool needToOpen = !dbIsOpen();
   if(needToOpen) openDb(false);

   // writing configuration keys and prescales
   try {

      unsigned char packingFormat(1);

      if(level==HLTCounter::L2 || level==HLTCounter::BOTH) {
         rangeInfo("L2 counts", since, until);
         cool::IFolderPtr l2CountersFolder = HLTCoolFolderSpec::getL2CountersFolder(m_dbPtr);
         cool::Record l2payload = HLTCoolFolderSpec::createHLTCountersPayload( packingFormat, counters, HLTCounter::L2);
         l2CountersFolder->storeObject(since, until, l2payload, 0);
      }

      if(level==HLTCounter::EF || level==HLTCounter::BOTH) {
         rangeInfo("EF counts", since, until);
         cool::IFolderPtr efCountersFolder = HLTCoolFolderSpec::getEFCountersFolder(m_dbPtr);
         cool::Record efpayload = HLTCoolFolderSpec::createHLTCountersPayload( packingFormat, counters, HLTCounter::EF);
         efCountersFolder->storeObject(since, until, efpayload, 0);
      }
    
   } 
   catch( cool::Exception & e) {
      m_ostream << "Caught cool::Exception: " << e.what() << std::endl;
      if(counters.size()>0) {
         m_ostream << "Failed to write "<<(*counters.begin()).level()<<" counters to COOL" << std::endl;
      } else {
         m_ostream << "Failed to write "<<(*counters.begin()).level()<<" counters to COOL" << std::endl;
      }
      throw;
   } 
   catch(std::exception & e) {
      m_ostream << "Caught std::exception: " << e.what() << std::endl;
      m_ostream << "Failed to write counters to COOL" << std::endl;
      throw;
   }

   if(needToOpen) closeDb();
}


bool
hltca::HLTCountCoolWriter::readHLTCountersPayload(unsigned int runNumber,
                                                  unsigned int lumiblockNumber,
                                                  std::vector< HLTCounter>& counters,
                                                  HLTCounter::TriggerLevel level)
{

   counters.clear();

   cool::ValidityKey pointInTime(runNumber);
   pointInTime <<= 32;
   pointInTime += lumiblockNumber;

   bool needToOpen = !dbIsOpen();
   if(needToOpen) openDb(true);

   // writing configuration keys and prescales
   try {

      cool::IFolderPtr f;
      if(level==HLTCounter::L2) {
         f = HLTCoolFolderSpec::getL2CountersFolder(m_dbPtr);
      } else if(level==HLTCounter::EF) {
         f = HLTCoolFolderSpec::getEFCountersFolder(m_dbPtr);
      } else {
         return false;
      }

      cool::IObjectPtr o = f->findObject( pointInTime, 0);
      const cool::IRecord& payload = o->payload();
      HLTCoolFolderSpec::readHLTCountersPayload(payload, counters, level);

   } 
   catch( cool::Exception & e) {
      m_ostream << "Caught cool::Exception: " << e.what() << std::endl;
      m_ostream << "Failed to read counters from COOL" << std::endl;
      throw;
   } 
   catch(std::exception & e) {
      m_ostream << "Caught std::exception: " << e.what() << std::endl;
      m_ostream << "Failed to read counters to COOL" << std::endl;
      throw;
   }

   if(needToOpen) closeDb();
   return  true;
}



void
hltca::HLTCountCoolWriter::setupStorageBuffers() {
   HLTCoolFolderSpec::getL2CountersFolder(m_dbPtr)->setupStorageBuffer();
   HLTCoolFolderSpec::getEFCountersFolder(m_dbPtr)->setupStorageBuffer();
}

void
hltca::HLTCountCoolWriter::flushStorageBuffers() {
   HLTCoolFolderSpec::getL2CountersFolder(m_dbPtr)->flushStorageBuffer();
   HLTCoolFolderSpec::getEFCountersFolder(m_dbPtr)->flushStorageBuffer();
}
