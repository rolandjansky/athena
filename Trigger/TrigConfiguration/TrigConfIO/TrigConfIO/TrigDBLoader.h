/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/TrigDBLoader.h
 * @author J. Stelzer
 * @date Sep 2019
 * @brief Loader class for Trigger configuration from the Trigger DB
 */

#ifndef TRIGCONFSTORAGE_DBLOADER_H
#define TRIGCONFSTORAGE_DBLOADER_H

#include "GaudiKernel/StatusCode.h"

#include "boost/property_tree/ptree.hpp"
#include "TrigConfBase/TrigConfMessaging.h"

#include "TrigConfData/DataStructure.h"


namespace coral {
   class ISessionProxy;
}

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class TrigDBLoader : public TrigConfMessaging {
   public:

      /** Constructor */
      TrigDBLoader(const std::string & connection);

      /** Destructor */
      virtual ~TrigDBLoader();

      /**
       * @brief Load content from the Trigger DB into a ptree for a given SuperMasterKey (SMK)
       * @param smk [in] the SMK
       * @param data [out] 
       * @param pathToChild [in] Path to a sub structure for partial loading of data
       *
       * The file is searched for in the path list defined by the environment variable @c DATAPATH.
       * If the optional parameter @c pathToChild is specified, one can load parts if the configuration
       * from the file.
       *@code
       *    TrigConf::TrigDBLoader fileLoader;
       *    TrigConf::DataStructure metSignificance;
       *    fileLoader.loadFile( l1_filename, metSignificance, "CaloInfo.METSignificance");
       @endcode
       */
      StatusCode loadMenu ( unsigned int smk,
                            boost::property_tree::ptree & l1data,
                            boost::property_tree::ptree & hltdata
                            );

      /**
       * @brief Load content of json file into a ptree
       * @param filename [in] Name of the json file
       * @param data [out] 
       * @param pathToChild [in] Path to a sub structure for partial loading of data
       */
      StatusCode loadMenu ( unsigned int smk,
                            DataStructure & l1menu,
                            DataStructure & hltmenu);

      void setLevel(MSGTC::Level lvl) { msg().setLevel(lvl); }

      MSGTC::Level outputLevel() const { return msg().level(); }

   private:

      /** @brief create (if needed) DB session and return the session proxy */
      coral::ISessionProxy& getDBSession();

      /**@brief close open sessions*/
      void closeDBSession();


      // private variables
      std::string            m_connection {"TRIGGERDB"};
      int                    m_retrialPeriod {0};
      int                    m_retrialTimeout {0};
      int                    m_connectionTimeout {0};

      coral::ISessionProxy * m_sessionproxy { nullptr };     ///< the coral database session

   };

}

#endif

