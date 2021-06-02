/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_ConfigSvcBase
#define TrigConfigSvc_ConfigSvcBase

#include "AthenaBaseComps/AthService.h"

namespace TrigConf {
   class TrigDBConnectionConfig;
   class IStorageMgr;
   class TrigDBConnectionConfig;

   class ConfigSvcBase : public AthService {
   public:
      ConfigSvcBase(const std::string& name, ISvcLocator* pSvcLocator);
      virtual ~ConfigSvcBase();

      virtual StatusCode initialize();

      virtual void declareCommonProperties();

      // to create db storage manager
      StatusCode initStorageMgr();
      StatusCode freeStorageMgr();

      bool fromDB() const { return m_dbconfig != nullptr; }

   protected:

      // properties
      std::string        m_configSourceString { "XML" };
      std::string        m_xmlFile { "NONE" };
      std::string        m_dbServer { "" };
      std::string        m_dbUser { "" };
      std::string        m_dbPassword { "" };
      std::string        m_dbTable { "" };
      int                m_dbSMKey { 0 };
      int                m_dbLVL1PSKey { 0 };
      int                m_dbHLTPSKey { 0 };
      std::string        m_dbHLTPSKeySet { "[]" };
      BooleanProperty    m_useFrontier { false };
      BooleanProperty    m_useSQLite { true };
      int                m_printMenuLevel { 1 };
      std::unique_ptr<TrigDBConnectionConfig> m_dbconfig;

      // the storage manager
      IStorageMgr*       m_storageMgr { nullptr };

   };
}

#endif
