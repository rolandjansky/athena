/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/JsonFileLoader.h
 * @author J. Stelzer
 * @date Feb 2019
 * @brief Loader class for Trigger configuration from Json
 */

#ifndef TRIGCONFSTORAGE_JSONFILELOADER_H
#define TRIGCONFSTORAGE_JSONFILELOADER_H

#include "TrigConfBase/TrigConfMessaging.h"
#include "TrigConfData/DataStructure.h"

#include "boost/property_tree/ptree.hpp"

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class JsonFileLoader : public TrigConfMessaging {
   public:

      /** Constructor */
      JsonFileLoader();

      /** Destructor */
      virtual ~JsonFileLoader();

      /**
       * @brief Load content of json file into a ptree
       * @param filename [in] Name of the json file
       * @param data [out] 
       * @param pathToChild [in] Path to a sub structure for partial loading of data
       *
       * The file is searched for in the path list defined by the environment variable @c DATAPATH.
       * If the optional parameter @c pathToChild is specified, one can load parts if the configuration
       * from the file.
       *@code
       *    TrigConf::JsonFileLoader fileLoader;
       *    const std::string l1_filename = "TriggerMenuXML/LVL1config_Physics_pp_v7.json";
       *    TrigConf::DataStructure metSignificance;
       *    fileLoader.loadFile( l1_filename, metSignificance, "CaloInfo.METSignificance");
       @endcode
       */
      bool loadFile( const std::string & filename,
                     boost::property_tree::ptree & data,
                     const std::string & pathToChild = "") const;

      /**
       * @brief Load content of json file into a ptree
       * @param filename [in] Name of the json file
       * @param data [out] 
       * @param pathToChild [in] Path to a sub structure for partial loading of data
       */
      bool loadFile( const std::string & filename,
                     DataStructure & data,
                     const std::string & pathToChild = "") const;


      /**
       * @brief Save content of DataStructure (underlying ptree) to a file
       * @param filename [in] Name of the json file
       * @param data [in]
       */
      bool saveFile( const std::string & filename,
                     const DataStructure & data ) const;

      /**
       * @brief Checks the trigger level of a given json file
       * @param filename [in] Name of the json file
       */
      std::string getFileType( const std::string & filename ) const;

      /**
       * @brief Checks the trigger level of a given json file
       * @param filename [in] Name of the json file
       * @param level [out] either "L1", "HLT" or "UNKNOWN"
       */
      bool checkTriggerLevel( const std::string & filename,
                              std::string & level ) const;

      void setLevel(MSGTC::Level lvl) { msg().setLevel(lvl); }

      MSGTC::Level outputLevel() const { return msg().level(); }

   private:
      std::string findFile(const std::string & filename) const;

   };

}

#endif

