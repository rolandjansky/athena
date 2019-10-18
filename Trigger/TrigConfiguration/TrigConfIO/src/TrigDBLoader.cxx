/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/TrigDBLoader.h"

#include "CoralBase/Exception.h"

#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ISessionProxy.h"

#include "boost/property_tree/ptree.hpp"

using ptree = boost::property_tree::ptree;

TrigConf::TrigDBLoader::TrigDBLoader(const std::string & loaderName, const std::string & connection) : 
   TrigConfMessaging(loaderName),
   m_connection(connection)
{}


TrigConf::TrigDBLoader::~TrigDBLoader()
{}


std::unique_ptr<coral::ISessionProxy>
TrigConf::TrigDBLoader::createDBSession() const {

   coral::ConnectionService connSvc;
   coral::IConnectionServiceConfiguration& csc = connSvc.configuration();
   csc.setConnectionRetrialPeriod( m_retrialPeriod );
   csc.setConnectionRetrialTimeOut( m_retrialTimeout );
   csc.setConnectionTimeOut( m_connectionTimeout );

   /* TODO
   if(csc.replicaSortingAlgorithm() == nullptr) { // likely to be standalone, create our own
      TRG_MSG_INFO("Create own ReplicaSortingAlgorithm");
      m_replicaSorter = new TrigConf::ReplicaSorter();
      csc.setReplicaSortingAlgorithm(*m_replicaSorter);
   }
   */

   TRG_MSG_INFO("Connecting to " << m_connection);

   auto proxy = std::unique_ptr<coral::ISessionProxy>( connSvc.connect(m_connection, coral::AccessMode::ReadOnly) );

   TRG_MSG_INFO("Opened session " << m_connection << " with " 
                << m_retrialPeriod << "/" << m_retrialTimeout << "/" << m_connectionTimeout);

   return proxy;
}
