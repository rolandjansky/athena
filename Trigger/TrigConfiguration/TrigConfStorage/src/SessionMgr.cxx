/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/SessionMgr.h"

#include "RelationalAccess/AccessMode.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"
#include "RelationalAccess/IDatabaseServiceSet.h"
#include "RelationalAccess/ILookupService.h"
#include "RelationalAccess/IAuthenticationService.h"
#include "RelationalAccess/IAuthenticationCredentials.h"
#include "RelationalAccess/ITypeConverter.h"
#include "RelationalAccess/IRelationalDomain.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"

#include "RelationalAccess/ISessionProxy.h"


#include "CoralKernel/Context.h"
#include "CoralBase/Exception.h"

#include <boost/algorithm/string/case_conv.hpp>

#include <iostream>
#include <stdlib.h>
#include <string.h>


using namespace std;
using namespace TrigConf;

SessionMgr::SessionMgr() :
   TrigConfMessaging("SessionMgr")
{}

// SessionMgr::SessionMgr( const std::string& connectionString,
//                         const std::string& user,
//                         const std::string& pass) :
//    TrigConfMessaging("SessionMgr"),
//    m_connectionString( connectionString ),
//    m_user( user),
//    m_password( pass )
// {}

// SessionMgr::SessionMgr( const std::string& type,
//                         const std::string& server,
//                         const std::string& name,
//                         const std::string& user,
//                         const std::string& pass) :
//    TrigConfMessaging("SessionMgr"),
//    m_dbtype( boost::to_lower_copy(type) ),
//    m_dbserver( server ),
//    m_dbname( name ),
//    m_user( user ),
//    m_password( pass )
// {
//    if(m_dbserver=="") { 
//       m_connectionString = m_dbname; 
//    } else if(m_dbtype == "dblookup") { 
//       m_connectionString = m_dbserver; 
//    } else { 
//       m_connectionString = m_dbtype + "://" + m_dbserver + "/" + m_dbname;     
//    }
// }

SessionMgr::~SessionMgr() {
   closeSession();
}

void
SessionMgr::closeSession() {

   if(m_sessionproxy) {
      try{
         delete m_sessionproxy;
         m_sessionproxy = nullptr;
         TRG_MSG_INFO("Closing session " << m_connectionString);
      }
      catch ( coral::Exception& e ) {
         TRG_MSG_WARNING("CORAL exception " << e.what());
         throw;
      }
   }

//    if( m_session ) {
//       try {
//          if( m_session->isUserSessionActive() )
//             m_session->endUserSession();
//          delete m_session;
//          m_session=0;

//          if ( m_connection->isConnected( true ) ) {
//             m_connection->disconnect();
//             TRG_MSG_INFO("Closing connection " << m_connectionString);
//          }

//          delete m_connection;
//          m_connection=0;
//       }
//       catch ( coral::Exception& e ) {
//          TRG_MSG_WARNING("CORAL exception " << e.what());
//          throw;
//       }
//       catch ( std::exception& e ) {
//          TRG_MSG_WARNING("Standard C++ exception " << e.what());
//          throw;
//       }
//       catch( ... ) {
//          TRG_MSG_WARNING("Unknown exception ...");
//          throw;
//       }
//    }
}

void
TrigConf::SessionMgr::setDbType(const std::string & s) {
   m_dbtype = boost::to_lower_copy(s);
   m_connectionString = "";
}

void
TrigConf::SessionMgr::setDbServer(const std::string & s) {
   m_dbserver = s;
   m_connectionString = "";
}

void
TrigConf::SessionMgr::setDbName(const std::string & s) { 
   m_dbname = s;
   m_connectionString = "";
}

void
TrigConf::SessionMgr::setDbUser(const std::string & s) {
   m_user = s;
   m_connectionString = "";
}

void
TrigConf::SessionMgr::setDbPassword(const std::string & s) {
   m_password = s;
   m_connectionString = "";
}


void
TrigConf::SessionMgr::buildConnectionString() {

   if(m_connectionString!="")
      return;

   if(m_dbserver=="") { 
      m_connectionString = m_dbname; 
   } else if(m_dbtype == "dblookup") { 
      m_connectionString = m_dbserver; 
   } else { 
      m_connectionString = m_dbtype + "://" + m_dbserver + "/" + m_dbname;     
   }
}


coral::ISessionProxy&
TrigConf::SessionMgr::createSession() {

   if( m_sessionproxy ) return *m_sessionproxy;

   coral::ConnectionService connSvc;
   coral::IConnectionServiceConfiguration& csc = connSvc.configuration();
   csc.setConnectionRetrialPeriod( m_retrialPeriod );
   csc.setConnectionRetrialTimeOut( m_retrialTimeout );
   csc.setConnectionTimeOut( m_connectionTimeout );

   buildConnectionString();
   m_sessionproxy = connSvc.connect(m_connectionString, coral::AccessMode::ReadOnly);
   TRG_MSG_INFO("Opening session " << m_connectionString);

   return *m_sessionproxy;   
}

/*
coral::ISession&
TrigConf::SessionMgr::createSessionOld(int retrialPeriod, int retrialTimeout, int connectionTimeout) {

   if( m_session ) return *m_session;

   coral::Context& context = coral::Context::instance();
   //coral::MessageStream::setMsgVerbosity(coral::Error);
   //coral::MessageStream::setMsgVerbosity(coral::Debug);

   // ===============================
   // load the relational service
   // ===============================
   const std::string relSvcStr("CORAL/Services/RelationalService");
   coral::IHandle<coral::IRelationalService> relsvc = context.query<coral::IRelationalService>();
   if (!relsvc.isValid()) {
      context.loadComponent(relSvcStr);
      relsvc = context.query<coral::IRelationalService>();
   }
   if (!relsvc.isValid())
      throw std::runtime_error( "Could not locate any relational service" );

   coral::ConnectionService conSvcH;
   coral::IConnectionServiceConfiguration& csConfig = conSvcH.configuration();
   csConfig.setConnectionRetrialPeriod( retrialPeriod );    //time interval between two connection retrials
   // time out for the connection retrials before the connection service fails over to the next available replica or quits
   csConfig.setConnectionRetrialTimeOut( retrialTimeout );
   csConfig.setConnectionTimeOut( connectionTimeout );
 
   // (DB, schemaName)
   std::pair<std::string,std::string> cs;

   // =================================
   // user-less login
   // =================================

   if(m_user == "" && m_connectionString.find("sqlite", 0) == std::string::npos  ) { // no user and not explicit sqlite

      bool useAuth = true;

      if( m_connectionString.find(':') == std::string::npos ) { // no ':' is found, the connection string is an ALIAS
         
         useAuth = false;

         // =================================
         // load the lookup service
         // =================================
         const std::string lookSvcStr("CORAL/Services/XMLLookupService");
         coral::IHandle<coral::ILookupService> lookupsvc = context.query<coral::ILookupService>();
         if (!lookupsvc.isValid()) {
            context.loadComponent(lookSvcStr);
            lookupsvc = context.query<coral::ILookupService>();
         }
         if (!lookupsvc.isValid())
            throw std::runtime_error( "Could not locate any lookup service" );
       
         const coral::IDatabaseServiceSet * svcSet = lookupsvc->lookup( m_connectionString, coral::ReadOnly);
         if(svcSet->numberOfReplicas()==0)
            throw std::runtime_error( std::string("Alias '") + m_connectionString + "' has no entries in dblookup.xml" );

         

         // check if FRONTIER_SERVER is set, if so, allow generic replicas
         const char* cUseFrontier = getenv("TRIGGER_USE_FRONTIER");
         bool triggerUseFrontierEnv = (cUseFrontier && strcmp(cUseFrontier,"")!=0);
         TRG_MSG_INFO("Checking if trigger should use frontier");
         TRG_MSG_INFO("environment TRIGGER_USE_FRONTIER : " << (triggerUseFrontierEnv?"YES":"NO"));
         TRG_MSG_INFO("SessionMgr::useFrontier flag     : " << (m_useFrontier?"YES":"NO"));
         TRG_MSG_INFO("==> " << ((triggerUseFrontierEnv || m_useFrontier)?"":"not ") << "going to use Frontier");
         
         bool triggerUseFrontier = (triggerUseFrontierEnv || m_useFrontier); 
         	 
         bool frontierDefined(false); 
         const char* cfrontier=0; 
         if(triggerUseFrontier) { 
            cfrontier=getenv("FRONTIER_SERVER"); 
            frontierDefined = (cfrontier && strcmp(cfrontier,"")!=0); 
            TRG_MSG_INFO("Checking for environment FRONTIER_SERVER : " << (frontierDefined?"YES":"NO")); 
         } 
         triggerUseFrontier = triggerUseFrontier && frontierDefined; 

         bool foundFrontier = false;
         if (triggerUseFrontier) {
            TRG_MSG_INFO("Frontier server at " << cfrontier << " will be considered for Trigger DB connection");

            for(int i = 0; i < svcSet->numberOfReplicas(); i++) {
               const std::string& conn = svcSet->replica( i ).connectionString();
               if (conn.find("frontier://")==std::string::npos) continue;

               coral::IRelationalDomain& domain = relsvc->domainForConnection( conn );

               cs = domain.decodeUserConnectionString( conn );

               m_connection = domain.newConnection( cs.first );
               //try to connect to the database
               try{
                  TRG_MSG_INFO("Trying frontier for TriggerDB with connection string: " << conn);
                  m_connection->connect();
                  coral::AccessMode mode = coral::ReadOnly; // possible options: coral::Update, coral::ReadOnly
                  m_session = m_connection->newSession( cs.second, mode );
                  m_connectionString = conn;
                  foundFrontier = true;
                  break;
               } catch (std::exception& exc){
                  TRG_MSG_WARNING("Failed to connect to alias " << i << " (" << exc.what() << ")");
               }

            }

            if(!foundFrontier)
               TRG_MSG_INFO("frontier:// not specified in dblookup, fall back to ORACLE.");
         }

         if(!foundFrontier) {
            bool canopen = false;
            for(int i = 0; i < svcSet->numberOfReplicas(); i++) {
               const std::string& test_connectionString = svcSet->replica( i ).connectionString();
               coral::IRelationalDomain& domain = relsvc->domainForConnection( test_connectionString );
               // Sanity check for TNS_ADMIN
               if(domain.flavorName()=="Oracle" && getenv("TNS_ADMIN")==0) 
                  throw std::runtime_error("TNS_ADMIN undefined: Oracle connections will not work!");   
               // (DB, schemaName)
               cs = domain.decodeUserConnectionString( test_connectionString );
               // connect to the database
               m_connection = domain.newConnection( cs.first );
               //try to connect to the database
               try{
                  TRG_MSG_INFO("Trying alias number " << i << " for TriggerDB with connection string: " << test_connectionString);
                  m_connection->connect();
                  coral::AccessMode mode = coral::ReadOnly; // possible options: coral::Update, coral::ReadOnly
                  m_session = m_connection->newSession( cs.second, mode );
                  m_connectionString = test_connectionString;
                  canopen = true;
                  if(domain.flavorName()=="Oracle") {
                     useAuth = true;
                  }
                  break;
               } catch (std::exception& exc){
                  TRG_MSG_WARNING("Failed to connect to alias " << i << " (" << exc.what() << ")");
               }
            }
            if(!canopen){
               throw std::runtime_error( "Connection can not be established to any of the TriggerDB aliases" );
            } else {
               TRG_MSG_INFO("Established a connection to TriggerDB with connection string " << m_connectionString);
            }
         }


      }

      // =================================
      // load the authentification service (if we didn't just find an sqlite connection)
      // =================================
      if(useAuth) {
         const char* coral_auth_path = getenv("CORAL_AUTH_PATH");
         if ( coral_auth_path != 0 && (strlen(coral_auth_path)>0) ) { 
            const std::string authSvcStr("CORAL/Services/XMLAuthenticationService");
            coral::IHandle<coral::IAuthenticationService> authsvc = context.query<coral::IAuthenticationService>();
            if (!authsvc.isValid()) {
               context.loadComponent(authSvcStr);
               authsvc = context.query<coral::IAuthenticationService>();
            }
            if (!authsvc.isValid())
               throw std::runtime_error( "Could not locate any authentication service" );
            const coral::IAuthenticationCredentials& crds = authsvc->credentials( m_connectionString );
            m_user     = crds.valueForItem("user");
            m_password = crds.valueForItem("password");
         }
      }
      // =============================================
      // connection string (user, pw) already supplied
      // =============================================

   } else {

      coral::IRelationalDomain& domain = relsvc->domainForConnection( m_connectionString );
      // Sanity check for TNS_ADMIN
      if(domain.flavorName()=="Oracle" && getenv("TNS_ADMIN")==0) 
         throw std::runtime_error("TNS_ADMIN undefined: Oracle connections will not work!");   
      // (DB, schemaName)
      cs = domain.decodeUserConnectionString( m_connectionString );
      // connect to the database
      m_connection = domain.newConnection( cs.first );
      m_connection->connect();
      TRG_MSG_INFO("Established a connection to TriggerDB with specified connection string " << m_connectionString);
      coral::AccessMode mode = coral::ReadOnly; // possible options: coral::Update, coral::ReadOnly
      m_session = m_connection->newSession( cs.second, mode );
   }

   // ===============================
   // create and start a session for
   // a certain schema
   // ===============================

   m_session->startUserSession( m_user, m_password );
   
   return *m_session; 
   
}*/

