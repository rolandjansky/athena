/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTRATES2COOL_HLTCOOLFOLDERSPEC
#define HLTRATES2COOL_HLTCOOLFOLDERSPEC

#include "HLTRates2COOL/HLTCounter.h"

#include <iostream>

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/RecordSpecification.h"

/**@brief Definition of the COOL folder structure for the trigger configuration data*/

namespace hltca {
   class HLTCoolFolderSpec
   {
   public:
  
      /**@brief creates the folder structure in the COOL database
       *
       * @param db COOL database
       *
       * @returns pointer to the folder structure
       */
      static cool::IFolderSetPtr createFolderStructure(cool::IDatabasePtr db);
  
      /**@brief prints the folder structure that is in the COOL database
       *
       * @param db COOL database
       *
       * @param o output stream
       *
       * @returns pointer to the folder
       */
      static void printFolderStructure(cool::IDatabasePtr db, std::ostream & o);
  
  
      /**@brief get a handle to the HLT Rates folder in the COOL db
       *
       * @param db COOL database
       *
       * @returns pointer to the folder
       */
      //  static cool::IFolderPtr getHLTRatesFolder(cool::IDatabasePtr db);
  
      static cool::IFolderPtr getL2CountersFolder(cool::IDatabasePtr db);

      static cool::IFolderPtr getEFCountersFolder(cool::IDatabasePtr db);

      static cool::Record createHLTCountersPayload(unsigned char format, const std::vector<hltca::HLTCounter>& counters, hltca::HLTCounter::TriggerLevel level);

      static void readHLTCountersPayload(const cool::IRecord & payload, std::vector<hltca::HLTCounter>& counters, hltca::HLTCounter::TriggerLevel level);

   private:

      /**@brief default constructor*/
      HLTCoolFolderSpec(){};
  
      /**@brief copy constructor*/
      HLTCoolFolderSpec(const HLTCoolFolderSpec &) {};

      static cool::RecordSpecification getHLTCountersFolderSpecification();

      static void pack(unsigned char format, const std::vector<hltca::HLTCounter>& counters, cool::Blob64k& blob, hltca::HLTCounter::TriggerLevel level);
      static void unpack(unsigned char format, const cool::Blob64k& blob, std::vector<hltca::HLTCounter>& counters, hltca::HLTCounter::TriggerLevel level);

   };
}

#endif
