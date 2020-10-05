/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/TrigDBLoader.h
 * @author J. Stelzer
 * @date Sep 2019
 * @brief Loader class for Trigger configuration from the Trigger DB
 */

#ifndef TRIGCONFIO_TRIGDBMENULOADER_H
#define TRIGCONFIO_TRIGDBMENULOADER_H

#include "TrigConfIO/TrigDBLoader.h"

#include "boost/property_tree/ptree.hpp"

#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTMenu.h"


namespace TrigConf {

   class QueryDefinition;

   /**
    * @brief Loader of trigger menu configurations from the database
    */
   class TrigDBMenuLoader : public TrigConf::TrigDBLoader {
   public:

      /** Constructor */
      TrigDBMenuLoader(const std::string & connection);

      /** Destructor */
      virtual ~TrigDBMenuLoader();

      /**
       * @brief Load L1 menu content from the Trigger DB into a ptree for a given SuperMasterKey (SMK)
       * @param smk [in] the SMK that should be loaded
       * @param l1menu [out] the loaded L1 menu
       * @return true if loading was successfull
       */
      bool loadL1Menu ( unsigned int smk,
                        boost::property_tree::ptree & l1menu,
                        const std::string & outFileName = "") const;

      /**
       * @brief Load HLT menu content from the Trigger DB into two ptrees for a given SuperMasterKey (SMK)
       * @param smk [in] the SMK that should be loaded
       * @param hltmenu [out] the loaded HLT menu
       * @return true if loading was successfull
       */
      bool loadHLTMenu ( unsigned int smk,
                         boost::property_tree::ptree & hltmenu,
                         const std::string & outFileName = "") const;

      /**
       * @brief Load content from the Trigger DB into an L1Menu for a given SuperMasterKey (SMK)
       * @param smk [in] the SMK that should be loaded
       * @param l1menu [out] the loaded L1 menu
       * @param outFileName [in] if set, an outputfile with the raw data blob is written
       * @return true if loading was successfull
       */
      bool loadL1Menu ( unsigned int smk,
                        L1Menu & l1menu,
                        const std::string & outFileName = "") const;

      /**
       * @brief Load content from the Trigger DB into an HLTMenu for a given SuperMasterKey (SMK)
       * @param smk [in] the SMK that should be loaded
       * @param hltmenu [out] the loaded HLT menu
       * @param outFileName [in] if set, an outputfile with the raw data blob is written
       * @return true if loading was successfull
       */
      bool loadHLTMenu ( unsigned int smk,
                         HLTMenu & hltmenu,
                         const std::string & outFileName = "") const;
   private:
      std::map<size_t, QueryDefinition> m_l1queries;
      std::map<size_t, QueryDefinition> m_hltqueries;
   };

}

#endif

