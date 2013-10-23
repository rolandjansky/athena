/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef HLTRATES2COOL_HLTTOCOOLWRITER
#define HLTRATES2COOL_HLTTOCOOLWRITER

#include "HLTRates2COOL/HLTCounter.h"

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"

#include <string>
#include <iostream>

namespace hltca {
   class HLTCountCoolWriter {
   public:

      /**@brief constructor
       *
       * @param dbId database connection string
       *
       * @param o output stream for all messages (defaults to cout)
       */
      HLTCountCoolWriter(const std::string& dbId, std::ostream & o = std::cout)  :
         m_dbId(cool::DatabaseId(dbId)),
         m_ostream(o)
      {};
    
      /**@brief destructor*/
      ~HLTCountCoolWriter(){};
    
  
      /**@brief Create a new schema in the COOL database*/
      void createSchema(bool print=false);

      /**@brief Prints the schema in the COOL database
       *
       * @param o output stream
       */
      void printSchema();

      /**@brief Enables the buffered writing of the counter folders*/      
      void setupStorageBuffers();

      /**@brief Enables the buffered writing of the counter folders*/      
      void flushStorageBuffers();
    
      void writeHLTCountersPayload(unsigned int runNumber,
                                   unsigned int lumiblockNumber,
                                   const std::vector<HLTCounter>& counters,
                                   HLTCounter::TriggerLevel level = HLTCounter::BOTH);
  
  
      void writeHLTCountersPayload( cool::ValidityKey since,
                                    cool::ValidityKey until,
                                    const std::vector<HLTCounter>& counters,
                                    HLTCounter::TriggerLevel level = HLTCounter::BOTH);

      bool readHLTCountersPayload(unsigned int runNumber,
                                  unsigned int lumiblockNumber,
                                  std::vector< HLTCounter>& counters,
                                  HLTCounter::TriggerLevel level);



      /**@brief open COOL db connection
       * @param readOnly if @c true the db is opened in readonly mode
       */
      cool::IDatabasePtr openDb( bool readOnly = true );
  
      /**@brief check if db is open
       */
      bool dbIsOpen();
  
      /**@brief delete the COOL database*/
      void dropDb();
  
      /**@brief close the COOL database*/
      //void closeDb(cool::IDatabasePtr& db);
      void closeDb();  // to be used

   private:
  
      /**@brief get COOL db service
       */
      cool::IDatabaseSvc& databaseService();
    
      /**@brief create a new COOL database*/
      cool::IDatabasePtr createDb();
  
      /**@brief create a new COOL database with prior deletion of the old one*/
      cool::IDatabasePtr recreateDb();

      // data members
      // DB connection string: oracle://<server>;schema=<acc_name>;dbname=<db_name>;user=<acc_name>;password=<pwd>
      cool::DatabaseId         m_dbId;  //< db connection string
      cool::IDatabasePtr       m_dbPtr; //< COOL database pointer

      std::ostream & m_ostream; //< output stream for all messages

      void rangeInfo(const std::string& content, cool::ValidityKey since, cool::ValidityKey until);
      
      /**@brief Create a new schema in the COOL database with prior deletion of the old one*/
      void recreateSchema(){};
  
   };

}

#endif
