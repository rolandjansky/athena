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


#include "./ReplicaSorter.h"

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

SessionMgr::~SessionMgr() {
   closeSession();
   // never delete the m_replicaSorter, we will have to live with that
   // one-time memory leak. The problem is the CORAL interface that
   // does keeps a reference to the replicaSorter and keeps using it
   // but doesn't clean it up. And if we delete it too early, then
   // later access leads to a crash
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

   
   if(csc.replicaSortingAlgorithm() == nullptr) { // likely to be standalone, create our own
      TRG_MSG_INFO("Create own ReplicaSortingAlgorithm with useFrontier=" << (m_useFrontier ? "true" : "false") << " and useSQLite=" << ( m_useSQLite ? "true" : "false"));
      m_replicaSorter = new TrigConf::ReplicaSorter();
      m_replicaSorter->setUseFrontier(m_useFrontier);
      m_replicaSorter->setUseSQLite(m_useSQLite);
      csc.setReplicaSortingAlgorithm(*m_replicaSorter);
   }

   buildConnectionString();
   TRG_MSG_INFO("Connecting to " << m_connectionString);
   m_sessionproxy = connSvc.connect(m_connectionString, coral::AccessMode::ReadOnly);

   TRG_MSG_INFO("Opening session " << m_connectionString << " with " << m_retrialPeriod << "/" << m_retrialTimeout << "/" << m_connectionTimeout);

   return *m_sessionproxy;
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

