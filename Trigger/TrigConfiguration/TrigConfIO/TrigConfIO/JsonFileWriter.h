/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Write Json file from L1Menu object
 * To validate correct loading of the L1 menu
 */

#ifndef TRIGCONFSTORAGE_JSONFILEWRITER_H
#define TRIGCONFSTORAGE_JSONFILEWRITER_H

#include "TrigConfBase/TrigConfMessaging.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1BunchGroupSet.h"

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class JsonFileWriter : public TrigConfMessaging {
   public:

      /** Constructor */
      JsonFileWriter();

      bool writeJsonFile(const std::string & filename, const L1Menu & l1menu) const;
      bool writeJsonFile(const std::string & filename, const L1BunchGroupSet & l1bgs) const;

   };

}
#endif
