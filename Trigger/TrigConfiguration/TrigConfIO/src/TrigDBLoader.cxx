/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBLoader.h"

#include "CoralBase/Exception.h"
#include "CoralBase/Blob.h"

#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISchema.h"

#include "boost/property_tree/ptree.hpp"
#include <fstream>

using ptree = boost::property_tree::ptree;

TrigConf::TrigDBLoader::TrigDBLoader(const std::string & loaderName, const std::string & connection) : 
   TrigConfMessaging(loaderName),
   m_connection(connection)
{}


TrigConf::TrigDBLoader::~TrigDBLoader()
{}

namespace {
   bool startswith(const std::string& str, const std::string& sub) {
      if(str.size()<sub.size())
         return false;
      return (str.compare(0,sub.size(),sub) == 0);
   }
}

size_t
TrigConf::TrigDBLoader::schemaVersion(coral::ISessionProxy* session) const {

   static const std::string versionTagPrefix("Trigger-Run3-Schema-v");

   // if schema version has been set, then return it
   if(m_schemaVersion>0) {
      TRG_MSG_INFO("TriggerDB schema version: " << m_schemaVersion);
      return m_schemaVersion;
   }

   // if database has no schema version, then we return 0
   if(! session->nominalSchema().existsTable("TRIGGER_SCHEMA") ) {
      throw std::runtime_error( "Trigger schema has no schema version table" );
   }
   
   TrigConf::QueryDefinition qdef;
   // tables
   qdef.addToTableList ( "TRIGGER_SCHEMA" );
   // attributes
   qdef.extendOutput<std::string>( "TS_TAG" );

   auto query = qdef.createQuery( session );
   auto & cursor = query->execute();
   if ( ! cursor.next() ) {
      throw std::runtime_error( "Trigger schema has schema version table but it is empty" );
   }

   const coral::AttributeList& row = cursor.currentRow();
   std::string versionTag = row["TS_TAG"].data<std::string>();
   if( ! startswith(versionTag, versionTagPrefix)) {
      throw std::runtime_error( "Tag format error: Trigger schema version tag " + versionTag + "does not start with " + versionTagPrefix);      
   }
   
   std::string vstr = versionTag.substr(versionTagPrefix.size()); // the part of the string containing the version 
   try {
      m_schemaVersion = std::stoi(vstr);
   }
   catch (const std::invalid_argument& ia) {
      TRG_MSG_ERROR("Invalid argument when interpreting the version part " << vstr << " of schema tag " << versionTag << ". " << ia.what());
      throw;
   }

   TRG_MSG_INFO("TriggerDB schema version: " << m_schemaVersion);
   return m_schemaVersion;
}

bool
TrigConf::TrigDBLoader::writeRawFile(const coral::Blob & data, const std::string & outFileName) const
{
   if( outFileName.empty() ) {
      return true;
   }
   std::ofstream outFile;
   outFile.open( outFileName, std::ofstream::binary );
   outFile.write( static_cast<const char*> ( data.startingAddress()), data.size() );
   outFile.close();
   TRG_MSG_INFO("Wrote file " << outFileName);
   return true;
}

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

   TRG_MSG_INFO("Opened session " << m_connection << " with  retrialPeriod/retrialTimeout/connectionTimeout: " 
                << m_retrialPeriod << "/" << m_retrialTimeout << "/" << m_connectionTimeout);

   return proxy;
}


TrigConf::QueryDefinition
TrigConf::TrigDBLoader::getQueryDefinition(coral::ISessionProxy* session, const std::map<size_t, TrigConf::QueryDefinition> & queries) const
{
   size_t sv = schemaVersion( session );
   // find the largest version key in the map of defined queries that is <= the schemaVersion 
   size_t maxDefVersion = 0;
   for(auto & entry : queries) {
      size_t vkey = entry.first;
      if(vkey>maxDefVersion and vkey<=sv) {
         maxDefVersion = vkey;
      }
   }
   // if nothing found, throw an error
   if( maxDefVersion==0 ) {
      TRG_MSG_ERROR("No query for schema version " << sv << " defined" );
      throw TrigConf::NoQueryException( "No query available for schema version" + std::to_string(sv) );      
   }
   return queries.at(maxDefVersion);
}
