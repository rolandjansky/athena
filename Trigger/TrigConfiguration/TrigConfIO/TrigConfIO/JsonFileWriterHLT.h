/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Write Json file from HLTMenu object
 * To validate correct loading of the HLT menu
 */

#ifndef TRIGCONFSTORAGE_JSONFILEWRITERHLT_H
#define TRIGCONFSTORAGE_JSONFILEWRITERHLT_H

#include "TrigConfBase/TrigConfMessaging.h"
#include "TrigConfData/HLTMenu.h"

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class JsonFileWriterHLT : public TrigConfMessaging {
   public:

      /** Constructor */
      JsonFileWriterHLT();

      bool writeJsonFile(const std::string & filename, const HLTMenu & menu) const;
   };

}
#endif
