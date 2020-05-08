/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/TrigDBJobOptionsLoader.h
 * @author J. Stelzer
 * @date Sep 2019
 * @brief Loader class for Trigger configuration from the Trigger DB
 */

#ifndef TRIGCONFIO_TRIGDBJOBOPTIONSLOADER_H
#define TRIGCONFIO_TRIGDBJOBOPTIONSLOADER_H

#include "TrigConfData/DataStructure.h"

#include "boost/property_tree/ptree.hpp"

#include "TrigConfIO/TrigDBLoader.h"

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class TrigDBJobOptionsLoader : public TrigDBLoader {
   public:

      /** Constructor */
      TrigDBJobOptionsLoader(const std::string & connection);

      /** Destructor */
      virtual ~TrigDBJobOptionsLoader();

      /**
       * @brief Load content from the Trigger DB into two ptrees for a given SuperMasterKey (SMK)
       * @param smk [in] the SMK that should be loaded
       * @param jobOptions [out] the loaded job options
       */
      bool loadJobOptions ( unsigned int smk,
                            boost::property_tree::ptree & jobOptions,
                            const std::string & outFileName = "") const;

      /**
       * @brief Load content from the Trigger DB into an L1Menu and an HLTMenu for a given SuperMasterKey (SMK)
       * @param smk [in] the SMK that should be loaded
       * @param jobOptions [out] the loaded job options
       */
      bool loadJobOptions ( unsigned int smk,
                            DataStructure & jobOptions,
                            const std::string & outFileName = "") const;
   };

}

#endif

