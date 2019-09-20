/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/TrigDBLoader.h"

#include "CoralBase/Exception.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Blob.h"

#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IRelationalDomain.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITransaction.h"


#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/iostreams/stream.hpp"

#include <memory>
#include <exception>

using ptree = boost::property_tree::ptree;

TrigConf::TrigDBLoader::TrigDBLoader(const std::string & connection) : 
   TrigConfMessaging("TrigDBLoader"),
   m_connection(connection)
{}

TrigConf::TrigDBLoader::~TrigDBLoader()
{}



StatusCode
TrigConf::TrigDBLoader::loadMenu ( unsigned int smk,
                                   boost::property_tree::ptree & l1menu, boost::property_tree::ptree & hltmenu )
{
   StatusCode sc = StatusCode::SUCCESS;

   bool mySession = false;
   auto & session = getDBSession();
   if ( ! session.transaction().isActive() ) {
      session.transaction().start(true);
      mySession = true;
   }
   
   std::unique_ptr< coral::IQuery > query( session.nominalSchema().newQuery() );
   query->addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
   query->addToTableList ( "L1_MASTER_TABLE", "L1MT" );
   query->addToTableList ( "HLT_MASTER_TABLE", "HMT" );

   // bind list
   coral::AttributeList bindList;
   bindList.extend<int>("smk");
   bindList[0].data<int>() = smk;

   // condition clause
   std::string theCondition = "";
   theCondition += std::string( " SMT.SMT_ID = :smk"        );
   theCondition += std::string( " AND SMT.SMT_L1_MASTER_TABLE_ID = L1MT.L1MT_ID" );
   theCondition += std::string( " AND SMT.SMT_HLT_MASTER_TABLE_ID = HMT.HMT_ID" );

   query->setCondition( theCondition, bindList );

   // output data and types
   coral::AttributeList attList;
   attList.extend<std::string>( "SMT.SMT_NAME" );
   attList.extend<int>        ( "SMT.SMT_VERSION" );
   attList.extend<int>        ( "SMT.SMT_L1_MASTER_TABLE_ID" );
   attList.extend<int>        ( "SMT.SMT_HLT_MASTER_TABLE_ID" );
   attList.extend<coral::Blob>( "L1MT.L1MT_MENU" );
   attList.extend<coral::Blob>( "HMT.HMT_MENU" );

   query->defineOutput(attList);
   for( const coral::Attribute & attr : attList) {
      query->addToOutputList(attr.specification().name());
   }

   coral::ICursor& cursor = query->execute();

   if ( ! cursor.next() ) {
      throw std::runtime_error( "TrigDBLoader: SuperMasterKey not available" );
   }
	
   const coral::AttributeList& row = cursor.currentRow();

   const coral::Blob& l1menuBlob = row["L1MT.L1MT_MENU"].data<coral::Blob>();
   boost::iostreams::stream<boost::iostreams::array_source> streamL1( static_cast<const char*> ( l1menuBlob.startingAddress()), l1menuBlob.size());
   boost::property_tree::read_json(streamL1, l1menu);

   const coral::Blob& hltmenuBlob = row["HMT.HMT_MENU"].data<coral::Blob>();
   boost::iostreams::stream<boost::iostreams::array_source> streamHLT( static_cast<const char*> ( hltmenuBlob.startingAddress()), hltmenuBlob.size());
   boost::property_tree::read_json(streamHLT, hltmenu);

   if ( mySession ) session.transaction().commit();

   return sc;
}


StatusCode
TrigConf::TrigDBLoader::loadMenu ( unsigned int smk,
                               DataStructure & l1menu, DataStructure & hltmenu )
{

   boost::property_tree::ptree ptl1, pthlt;

   StatusCode sc = this -> loadMenu( smk, ptl1, pthlt);

   if(!sc.isSuccess())
      return sc;

   if( ! ptl1.empty() )
      l1menu.setData(ptl1);
   if( ! pthlt.empty() )
      hltmenu.setData(pthlt);

   return sc;
}



coral::ISessionProxy&
TrigConf::TrigDBLoader::getDBSession() {

   // if we have a session proxy, use it
   if( m_sessionproxy ) 
      return *m_sessionproxy;


   coral::ConnectionService connSvc;
   coral::IConnectionServiceConfiguration& csc = connSvc.configuration();
   csc.setConnectionRetrialPeriod( m_retrialPeriod );
   csc.setConnectionRetrialTimeOut( m_retrialTimeout );
   csc.setConnectionTimeOut( m_connectionTimeout );

   
   if(csc.replicaSortingAlgorithm() == nullptr) { // likely to be standalone, create our own
      TRG_MSG_INFO("Create own ReplicaSortingAlgorithm");
      // m_replicaSorter = new TrigConf::ReplicaSorter();
      // csc.setReplicaSortingAlgorithm(*m_replicaSorter);
   }

   TRG_MSG_INFO("Connecting to " << m_connection);
   m_sessionproxy = connSvc.connect(m_connection, coral::AccessMode::ReadOnly);
   TRG_MSG_INFO("Opening session " << m_connection << " with " 
                << m_retrialPeriod << "/" << m_retrialTimeout << "/" << m_connectionTimeout);

   return *m_sessionproxy;
}


void
TrigConf::TrigDBLoader::closeDBSession() {

   if(m_sessionproxy) {
      try{
         delete m_sessionproxy;
         m_sessionproxy = nullptr;
         TRG_MSG_INFO("Closing session " << m_connection);
      }
      catch ( coral::Exception& e ) {
         TRG_MSG_WARNING("CORAL exception " << e.what());
         throw;
      }
   }

}
