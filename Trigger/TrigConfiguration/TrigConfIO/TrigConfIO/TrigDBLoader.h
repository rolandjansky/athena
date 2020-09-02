/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/TrigDBLoader.h
 * @author J. Stelzer
 * @date Sep 2019
 * @brief Loader class for Trigger configuration from the Trigger DB
 */

#ifndef TRIGCONFIO_TRIGDBLOADER_H
#define TRIGCONFIO_TRIGDBLOADER_H

#include "TrigConfBase/TrigConfMessaging.h"

#include <memory>

namespace coral {
   class ISessionProxy;
   class Blob;
}

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class TrigDBLoader : public TrigConfMessaging {
   public:

      /** Constructor */
      TrigDBLoader(const std::string & loaderName, const std::string & connection);

      /** Destructor */
      virtual ~TrigDBLoader();

      /** write data blob into file
          This can be used to write the DB content to file without going through a ptree
       */
      bool writeRawFile(const coral::Blob & data, const std::string & outFileName) const;

      void setLevel(MSGTC::Level lvl) { msg().setLevel(lvl); }

      MSGTC::Level outputLevel() const { return msg().level(); }

   protected:

      /** @brief create (if needed) DB session and return the session proxy */
      std::unique_ptr<coral::ISessionProxy> createDBSession() const;

   private:

      // private variables
      std::string            m_connection {"TRIGGERDB"};
      int                    m_retrialPeriod {0};
      int                    m_retrialTimeout {0};
      int                    m_connectionTimeout {0};

   };

}

#endif

