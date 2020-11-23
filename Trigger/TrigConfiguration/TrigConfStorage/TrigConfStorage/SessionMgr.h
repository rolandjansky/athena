/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_SessionMgr
#define TrigConf_SessionMgr

#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>
#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IRelationalDomain.h"
#include "TrigConfBase/TrigConfMessaging.h"

namespace coral {
   class ISessionProxy;
}


namespace TrigConf {

   class ReplicaSorter;
  
   /**@brief Manager of the database session
    * 
    * it uses CORAL to connect to the different DB technologies
    */
   class SessionMgr : public TrigConfMessaging
   {
   public:

      /**@brief constructor */
      SessionMgr();

      /**@brief destructor*/
      ~SessionMgr();

      /**@brief close open sessions*/
      void closeSession();
    
      /**@brief instantiates the session*/
      coral::ISessionProxy& createSession();

      /**@brief SQL -> C++ type conversion mapping for Oracle database to match MySQL*/
      void setTypeConversionRules();

      // setters
      void setConnectionString(const std::string & connStr) { m_connectionString = connStr; }
      void setUseFrontier(bool useFrontier) { m_useFrontier = useFrontier; }
      void setUseSQLite(bool useSQLite) { m_useSQLite = useSQLite; }
      void setRetrialPeriod(int retrialPeriod) { m_retrialPeriod = retrialPeriod; }
      void setRetrialTimeout(int retrialTimeout) { m_retrialTimeout = retrialTimeout; }
      void setConnectionTimeout(int connectionTimeout) { m_connectionTimeout = connectionTimeout; }

      void setDbType(const std::string & s);
      void setDbServer(const std::string & s);
      void setDbName(const std::string & s);
      void setDbUser(const std::string & s);
      void setDbPassword(const std::string & s);

      // accessors
      const std::string& connection() const { return m_connectionString; }
      bool useFrontier() const { return m_useFrontier; }
      bool useSQLite() const { return m_useSQLite; }
      int retrialPeriod() const { return m_retrialPeriod; }
      int retrialTimeout() const { return m_retrialTimeout; }
      int connectionTimeout() const { return m_connectionTimeout; }
      const std::string & dbType() const { return m_dbtype; }
      const std::string & dbServer() const { return m_dbserver; }
      const std::string & dbName() const { return m_dbname; }
      const std::string & dbUser() const { return m_user; }
      const std::string & dbPassword() const { return m_password; }

   private:

      void buildConnectionString();

      /**@brief SQL -> C++ type conversion interface to CORAL*/
      void setCppTypeForSqlType(const std::string& cpp_type, 
                                const std::string& sql_type);

      coral::ISessionProxy * m_sessionproxy { nullptr };     ///< the coral database session

      std::string            m_connectionString { "" };   ///< connection string
      std::string            m_dbtype { "" };      ///< db type
      std::string            m_dbserver { "" };    ///< db server
      std::string            m_dbname { "" };      ///< db name
      std::string            m_user { "" };        ///< user name
      std::string            m_password { "" };    ///< password
      bool                   m_useFrontier { false }; ///< uses frontier instead of oracle
      bool                   m_useSQLite { true }; ///< resolve sqlite in the replicaSorter
      int                    m_retrialPeriod {0};
      int                    m_retrialTimeout {0};
      int                    m_connectionTimeout {0};

      ReplicaSorter *        m_replicaSorter { nullptr }; // private replica sorter in case of standalone usage of SessionMgr

   };
}

#endif
