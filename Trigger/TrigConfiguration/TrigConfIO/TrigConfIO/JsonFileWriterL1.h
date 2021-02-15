/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Write Json file from L1Menu object
 * To validate correct loading of the L1 menu
 */

#ifndef TRIGCONFSTORAGE_JSONFILEWRITERL1_H
#define TRIGCONFSTORAGE_JSONFILEWRITERL1_H

#include "TrigConfBase/TrigConfMessaging.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1BunchGroupSet.h"
#include "TrigConfData/L1PrescalesSet.h"

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class JsonFileWriterL1 : public TrigConfMessaging {
   public:

      /** Constructor */
      JsonFileWriterL1();

      bool writeJsonFile(const std::string & filename, const L1Menu & l1menu) const;
      bool writeJsonFile(const std::string & filename, const L1BunchGroupSet & l1bgs) const;
      bool writeJsonFile(const std::string & filename, const L1PrescalesSet & l1bgs) const;

   };

}
#endif
