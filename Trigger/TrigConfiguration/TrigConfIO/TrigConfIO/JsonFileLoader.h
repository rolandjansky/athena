/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/JsonFileLoader.h
 * @author J. Stelzer
 * @date Feb 2019
 * @brief Loader class for Trigger configuration from Json
 */

#ifndef TRIGCONFSTORAGE_JSONFILELOADER_H
#define TRIGCONFSTORAGE_JSONFILELOADER_H

#include "GaudiKernel/StatusCode.h"

#include "boost/property_tree/ptree.hpp"
#include "TrigConfBase/TrigConfMessaging.h"

#include "TrigConfData/DataStructure.h"

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
      StatusCode loadFile( const std::string & filename,
                           boost::property_tree::ptree & data,
                           const std::string & pathToChild = "");

      /**
       * @brief Load content of json file into a ptree
       * @param filename [in] Name of the json file
       * @param data [out] 
       * @param pathToChild [in] Path to a sub structure for partial loading of data
       */
      StatusCode loadFile( const std::string & filename,
                           DataStructure & data,
                           const std::string & pathToChild = "");


      void setLevel(MSGTC::Level lvl) { msg().setLevel(lvl); }

      MSGTC::Level outputLevel() const { return msg().level(); }

   };

}

#endif

