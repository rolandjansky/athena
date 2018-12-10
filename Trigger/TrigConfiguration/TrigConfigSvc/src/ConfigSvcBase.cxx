/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfigSvc/ConfigSvcBase.h"
#include "TrigConfBase/TrigDBConnectionConfig.h"
#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/XMLStorageMgr.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/lexical_cast.hpp"

using namespace std;
using namespace TrigConf;

ConfigSvcBase::ConfigSvcBase(const std::string& name, ISvcLocator* pSvcLocator) :
   AthService(name, pSvcLocator),
   m_dbconfig { nullptr }
{}

ConfigSvcBase::~ConfigSvcBase()
{
  delete m_storageMgr;
}

void
ConfigSvcBase::declareCommonProperties() {
   declareProperty( "ConfigSource",     m_configSourceString,
                    "Source of trigger configuration; can be \"XML\", \"MySQL\", \"Oracle\", \"DBLookup\" or \"RUN3_DUMMY\" ");
   declareProperty( "XMLMenuFile",      m_xmlFile,
                    "XML file containing the trigger configuration.");
   declareProperty( "DBServer",         m_dbServer,
                    "Database server to use.");
   declareProperty( "DBUser",           m_dbUser,
                    "User name for database connection. If empty, use XMLAuthenticationService.");
   declareProperty( "DBPassword",       m_dbPassword,
                    "Password for database connection. Only used if \"DBUser\" is specified and not empty.");
   declareProperty( "DBTable",          m_dbTable );

   declareProperty( "DBSMKey",          m_dbSMKey,
                    "The SuperMaster key");
   declareProperty( "UseFrontier",      m_useFrontier,
                    "Tries to use Frontier for accessing the TriggerDB");
   declareProperty( "PrintMenu",        m_printMenuLevel,
                    "Prints menu with detail level x=0..5 [default = " + boost::lexical_cast<string,int>(m_printMenuLevel) + "]");
}



StatusCode
ConfigSvcBase::initialize() {

   CHECK(AthService::initialize());

   ATH_MSG_INFO("=================================");
   ATH_MSG_INFO("Initializing " << name() << " service");
   ATH_MSG_INFO("Version: " << PACKAGE_VERSION);
   ATH_MSG_INFO("=================================");

   string s(boost::to_lower_copy(m_configSourceString)); // lower case

  if (s == "run3_dummy") {
      ATH_MSG_WARNING("Configured to use Run-3 Dummy menu. This should never be seen in production");
   } else if(s != "xml") {
      TrigDBConnectionConfig::DBType dbtype(TrigDBConnectionConfig::DBLookup);
      if (s == "oracle") { dbtype = TrigDBConnectionConfig::Oracle; }
      else if (s == "mysql")  { dbtype = TrigDBConnectionConfig::MySQL; }
      else if (s == "sqlite") { dbtype = TrigDBConnectionConfig::SQLite; }

      TrigDBConnectionConfig * tmpptr{nullptr};
      if(m_dbHLTPSKey)
        tmpptr = new TrigDBConnectionConfig(dbtype, m_dbServer, m_dbSMKey,
                                            m_dbHLTPSKey);
      else
        tmpptr = new TrigDBConnectionConfig(dbtype, m_dbServer, m_dbSMKey,
                                            m_dbHLTPSKeySet);
      m_dbconfig = std::unique_ptr<TrigDBConnectionConfig>{std::move(tmpptr)};
      m_dbconfig->m_useFrontier = m_useFrontier;
   }

   ATH_MSG_INFO("    ConfigSource        = " << m_configSourceString);
   if(m_dbconfig==nullptr) {
      ATH_MSG_INFO("    XMLMenuFile         = " << m_xmlFile);
   } else {
      ATH_MSG_INFO("    DB Server           = " << m_dbServer);
      ATH_MSG_INFO("    DB User             = " << m_dbUser);
      ATH_MSG_INFO("    DB Table            = " << m_dbTable);
      ATH_MSG_INFO("    UseFrontier         = " << m_useFrontier.value());
      ATH_MSG_INFO("    DB connection       = " << m_dbconfig->toString());
      ATH_MSG_INFO("    DB SuperMasterKey   = " << m_dbSMKey);
   }
   ATH_MSG_INFO("    PrintMenu level     = " << m_printMenuLevel);

   return StatusCode::SUCCESS;
}



StatusCode
ConfigSvcBase::initStorageMgr() {
   if(fromDB()) {
      string connectionString;
      if(m_dbconfig->m_type == TrigDBConnectionConfig::DBLookup)
         connectionString = m_dbconfig->m_server;
      else
         connectionString = m_dbconfig->typeToString() + "://" +
            m_dbconfig->m_server + "/" + m_dbconfig->m_schema;

      ATH_MSG_INFO("Connection: " << connectionString);
      StorageMgr * sm = new StorageMgr( connectionString, m_dbconfig->m_user, m_dbconfig->m_password );

      sm->setUseFrontier(m_dbconfig->m_useFrontier);

      sm->setRetrialPeriod( m_dbconfig->m_retrialPeriod );
      sm->setRetrialTimeout( m_dbconfig->m_retrialPeriod * (m_dbconfig->m_maxRetrials + 1) );
      sm->setConnectionTimeout( 0 );

      m_storageMgr = sm;
   } else {
      if (m_xmlFile == "") {
         ATH_MSG_ERROR("If you need the configuration and ConfigSource is 'XML', you need to specify a menu xml file");
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("XML file: " << m_xmlFile);
      m_storageMgr = new XMLStorageMgr( { m_xmlFile } );
   }
   return StatusCode::SUCCESS;
}


// close storage manager
StatusCode
ConfigSvcBase::freeStorageMgr() {
   delete m_storageMgr;
   m_storageMgr = 0;
   return StatusCode::SUCCESS;
}
