/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/SessionMgr.h"

#include "RelationalAccess/AccessMode.h"
#include "RelationalAccess/IConnection.h"
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

#include "CoralKernel/Context.h"
#include "CoralBase/Exception.h"

#include <boost/algorithm/string/case_conv.hpp>

#include <iostream>
#include <stdlib.h>
#include <string.h>


using namespace std;
using namespace TrigConf;

SessionMgr::SessionMgr( const std::string& cs,
                        const std::string& user,
                        const std::string& pass,
                        std::ostream & o)
   : m_session( 0 )
   , m_connection( 0 )
   , m_cs( cs )
   , m_dbtype( "" )
   , m_dbserver( "" )
   , m_dbname( "" )
   , m_user( user)
   , m_password( pass )
   , m_ostream( o )
{}

SessionMgr::SessionMgr( const std::string& type,
                        const std::string& server,
                        const std::string& name,
                        const std::string& user,
                        const std::string& pass,
                        std::ostream & o)
   : m_session( 0 )
   , m_connection( 0 )
   , m_cs( "" )
   , m_dbtype( boost::to_lower_copy(type) )
   , m_dbserver( server )
   , m_dbname( name )
   , m_user( user)
   , m_password( pass )
   , m_ostream( o )
{
   if(m_dbserver=="") { 
      m_cs = m_dbname; 
   } else if(m_dbtype == "dblookup") { 
      m_cs = m_dbserver; 
   } else { 
      m_cs = m_dbtype + "://" + m_dbserver + "/" + m_dbname;     
   }
}

SessionMgr::~SessionMgr() {
   closeSession();
}

void 
SessionMgr::setUseFrontier(bool useFrontier) { 
   m_useFrontier = useFrontier; 
} 

void
SessionMgr::closeSession() {
   if( m_session ) {
      try {
         if( m_session->isUserSessionActive() )
            m_session->endUserSession();
         delete m_session;
         m_session=0;

         if ( m_connection->isConnected( true ) ) {
            m_connection->disconnect();
            m_ostream << "<TriggerDB SessionMgr> Closing connection " << m_cs << std::endl;
         }

         delete m_connection;
         m_connection=0;
      }
      catch ( coral::Exception& e ) {
         m_ostream << "CORAL exception " << e.what() << std::endl;
         throw;
      }
      catch ( std::exception& e ) {
         m_ostream << "Standard C++ exception " << e.what() << std::endl;
         throw;
      }
      catch( ... ) {
         m_ostream << "Unknown exception ..." << std::endl;
         throw;
      }
   }
}

  
coral::ISession&
TrigConf::SessionMgr::createSession(int retrialPeriod, int retrialTimeout, int connectionTimeout) {

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

   if(m_user == "" && m_cs.find("sqlite", 0) == std::string::npos  ) { // no user and not explicit sqlite

      bool useAuth = true;

      if( m_cs.find(':') == std::string::npos ) { // no ':' is found, the connection string is an ALIAS
         
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
       
         const coral::IDatabaseServiceSet * svcSet = lookupsvc->lookup( m_cs, coral::ReadOnly);
         if(svcSet->numberOfReplicas()==0)
            throw std::runtime_error( std::string("Alias '") + m_cs + "' has no entries in dblookup.xml" );

         

         // check if FRONTIER_SERVER is set, if so, allow generic replicas
         const char* cUseFrontier = getenv("TRIGGER_USE_FRONTIER");
         bool triggerUseFrontierEnv = (cUseFrontier && strcmp(cUseFrontier,"")!=0);
         m_ostream << "SessionMgr                                           INFO Checking if trigger should use frontier" << endl 
                   << "SessionMgr                                           INFO environment TRIGGER_USE_FRONTIER : " << (triggerUseFrontierEnv?"YES":"NO") << endl 
                   << "SessionMgr                                           INFO SessionMgr::useFrontier flag     : " << (m_useFrontier?"YES":"NO") << endl 
                   << "SessionMgr                                           INFO ==> " << ((triggerUseFrontierEnv || m_useFrontier)?"":"not ") << "going to use Frontier" << endl; 
         
         bool triggerUseFrontier = (triggerUseFrontierEnv || m_useFrontier); 
         	 
         bool frontierDefined(false); 
         const char* cfrontier=0; 
         if(triggerUseFrontier) { 
            cfrontier=getenv("FRONTIER_SERVER"); 
            frontierDefined = (cfrontier && strcmp(cfrontier,"")!=0); 
            m_ostream << "SessionMgr                                           INFO Checking for environment FRONTIER_SERVER : " << (frontierDefined?"YES":"NO") << std::endl; 
         } 
         triggerUseFrontier = triggerUseFrontier && frontierDefined; 

         bool foundFrontier = false;
         if (triggerUseFrontier) {
            m_ostream << "Frontier server at " << cfrontier << " will be considered for Trigger DB connection" << std::endl;

            for(int i = 0; i < svcSet->numberOfReplicas(); i++) {
               const std::string& conn = svcSet->replica( i ).connectionString();
               if (conn.find("frontier://")==std::string::npos) continue;

               coral::IRelationalDomain& domain = relsvc->domainForConnection( conn );

               cs = domain.decodeUserConnectionString( conn );

               m_connection = domain.newConnection( cs.first );
               //try to connect to the database
               try{
                  m_ostream << "Trying frontier for TriggerDB with connection string: " << conn << std::endl;
                  m_connection->connect();
                  coral::AccessMode mode = coral::ReadOnly; // possible options: coral::Update, coral::ReadOnly
                  m_session = m_connection->newSession( cs.second, mode );
                  m_cs = conn;
                  foundFrontier = true;
                  break;
               } catch (std::exception& exc){
                  m_ostream << "Failed to connect to alias " << i << " (" << exc.what() << ")" << std::endl;
               }

            }

            if(!foundFrontier)
               m_ostream << "frontier:// not specified in dblookup, fall back to ORACLE." << std::endl;
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
                  m_ostream << "Trying alias number " << i << " for TriggerDB with connection string: " << test_connectionString << std::endl;
                  m_connection->connect();
                  coral::AccessMode mode = coral::ReadOnly; // possible options: coral::Update, coral::ReadOnly
                  m_session = m_connection->newSession( cs.second, mode );
                  m_cs = test_connectionString;
                  canopen = true;
                  if(domain.flavorName()=="Oracle") {
                     useAuth = true;
                  }
                  break;
               } catch (std::exception& exc){
                  m_ostream << "Failed to connect to alias " << i << " (" << exc.what() << ")" << std::endl;
               }
            }
            if(!canopen){
               throw std::runtime_error( "Connection can not be established to any of the TriggerDB aliases" );
            } else {
               m_ostream << "<TriggerDB SessionMgr> Established a connection to TriggerDB with connection string " << m_cs << std::endl;
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
            const coral::IAuthenticationCredentials& crds = authsvc->credentials( m_cs );
            m_user     = crds.valueForItem("user");
            m_password = crds.valueForItem("password");
         }
      }
      // =============================================
      // connection string (user, pw) already supplied
      // =============================================

   } else {

      coral::IRelationalDomain& domain = relsvc->domainForConnection( m_cs );
      // Sanity check for TNS_ADMIN
      if(domain.flavorName()=="Oracle" && getenv("TNS_ADMIN")==0) 
         throw std::runtime_error("TNS_ADMIN undefined: Oracle connections will not work!");   
      // (DB, schemaName)
      cs = domain.decodeUserConnectionString( m_cs );
      // connect to the database
      m_connection = domain.newConnection( cs.first );
      m_connection->connect();
      m_ostream << "<TriggerDB SessionMgr> Established a connection to TriggerDB with specified connection string " << m_cs << std::endl;
      coral::AccessMode mode = coral::ReadOnly; // possible options: coral::Update, coral::ReadOnly
      m_session = m_connection->newSession( cs.second, mode );
   }

   // ===============================
   // create and start a session for
   // a certain schema
   // ===============================

   m_session->startUserSession( m_user, m_password );
   
   return *m_session; 
   
}
