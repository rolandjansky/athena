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
#include "RelationalAccess/ISession.h"
#include "RelationalAccess/IConnection.h"

namespace TrigConf
{
  
  /**@brief Manager of the database session
   * 
   * it uses CORAL to connect to the different DB technologies
   */
  class SessionMgr
  {
  public:
    /**@brief constructor
     *
     * @param cs connection string
     * @param user user name, if not specified the XML authentication will be used
     * @param password password string (only used in connection with a user name)
     * @param o output stream for all messages
     */
    SessionMgr( const std::string& cs, const std::string& user = "",const std::string& password = "", std::ostream & o = std::cout);

    /**@brief constructor
     *
     * @param type Server technology 
     * @param server Server name
     * @param name Database name, or schema name, or, if server and type are not specified, alias name for lookup
     * @param user user name, if not specified the XML authentication will be used
     * @param password password string (only used in connection with a user name)
     * @param o output stream for all messages
     */
    SessionMgr( const std::string& type, const std::string& server, 
                const std::string& name, const std::string& user , 
                const std::string& password , std::ostream & o = std::cout);

    /**@brief destructor*/
    ~SessionMgr();

    /**@brief close open sessions*/
    void closeSession();
    
    /**@brief instantiates the session*/
    coral::ISession& createSession(int retrialPeriod=30, int retrialTimeout=300, int connectionTimeout=5);

    /**@brief SQL -> C++ type conversion mapping for Oracle database to match MySQL*/
    void setTypeConversionRules();

    const std::string& connection() const { return m_cs; }

     void setUseFrontier(bool useFrontier);
     bool useFrontier() const { return m_useFrontier; }

  private:
    /**@brief SQL -> C++ type conversion interface to CORAL*/
    void setCppTypeForSqlType(const std::string& cpp_type, 
                              const std::string& sql_type);

    coral::ISession *    m_session;     ///< the coral database session
    coral::IConnection * m_connection;  ///< the coral connection
    std::string          m_cs;          ///< connection string
    std::string          m_dbtype;      ///< db type
    std::string          m_dbserver;    ///< db server
    std::string          m_dbname;      ///< db name
    std::string          m_user;        ///< user name
    std::string          m_password;    ///< password
    bool                 m_useFrontier; ///< uses frontier instead of oracle
    std::ostream &       m_ostream;     ///< output stream
  };
}

#endif
