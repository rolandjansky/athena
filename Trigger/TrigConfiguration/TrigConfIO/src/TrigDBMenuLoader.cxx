/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/TrigDBMenuLoader.h"

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

TrigConf::TrigDBMenuLoader::TrigDBMenuLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBMenuLoader", connection)
{}

TrigConf::TrigDBMenuLoader::~TrigDBMenuLoader()
{}



bool
TrigConf::TrigDBMenuLoader::loadL1Menu ( unsigned int smk,
                                         boost::property_tree::ptree & l1menu ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   
   std::unique_ptr< coral::IQuery > query( session->nominalSchema().newQuery() );
   query->addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
   query->addToTableList ( "L1_MASTER_TABLE", "L1MT" );

   // bind list
   coral::AttributeList bindList;
   bindList.extend<int>("smk");
   bindList[0].data<int>() = smk;

   // condition clause
   std::string theCondition = "";
   theCondition += std::string( " SMT.SMT_ID = :smk"        );
   theCondition += std::string( " AND SMT.SMT_L1_MASTER_TABLE_ID = L1MT.L1MT_ID" );

   query->setCondition( theCondition, bindList );

   // output data and types
   coral::AttributeList attList;
   attList.extend<std::string>( "SMT.SMT_NAME" );
   attList.extend<int>        ( "SMT.SMT_VERSION" );
   attList.extend<int>        ( "SMT.SMT_L1_MASTER_TABLE_ID" );
   attList.extend<coral::Blob>( "L1MT.L1MT_MENU" );

   query->defineOutput(attList);
   for( const coral::Attribute & attr : attList) {
      query->addToOutputList(attr.specification().name());
   }

   coral::ICursor& cursor = query->execute();

   if ( ! cursor.next() ) {
      throw std::runtime_error( "TrigDBMenuLoader: SuperMasterKey not available" );
   }
	
   const coral::AttributeList& row = cursor.currentRow();

   const coral::Blob& l1menuBlob = row["L1MT.L1MT_MENU"].data<coral::Blob>();
   boost::iostreams::stream<boost::iostreams::array_source> streamL1( static_cast<const char*> ( l1menuBlob.startingAddress()), l1menuBlob.size());
   boost::property_tree::read_json(streamL1, l1menu);

   session->transaction().commit();

   return true;
}



bool
TrigConf::TrigDBMenuLoader::loadHLTMenu ( unsigned int smk,
                                          boost::property_tree::ptree & hltmenu ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   
   std::unique_ptr< coral::IQuery > query( session->nominalSchema().newQuery() );
   query->addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
   query->addToTableList ( "HLT_MASTER_TABLE", "HMT" );

   // bind list
   coral::AttributeList bindList;
   bindList.extend<int>("smk");
   bindList[0].data<int>() = smk;

   // condition clause
   std::string theCondition = "";
   theCondition += std::string( " SMT.SMT_ID = :smk"        );
   theCondition += std::string( " AND SMT.SMT_HLT_MASTER_TABLE_ID = HMT.HMT_ID" );

   query->setCondition( theCondition, bindList );

   // output data and types
   coral::AttributeList attList;
   attList.extend<std::string>( "SMT.SMT_NAME" );
   attList.extend<int>        ( "SMT.SMT_VERSION" );
   attList.extend<int>        ( "SMT.SMT_HLT_MASTER_TABLE_ID" );
   attList.extend<coral::Blob>( "HMT.HMT_MENU" );

   query->defineOutput(attList);
   for( const coral::Attribute & attr : attList) {
      query->addToOutputList(attr.specification().name());
   }

   coral::ICursor& cursor = query->execute();

   if ( ! cursor.next() ) {
      throw std::runtime_error( "TrigDBMenuLoader: SuperMasterKey not available" );
   }
	
   const coral::AttributeList& row = cursor.currentRow();

   const coral::Blob& hltmenuBlob = row["HMT.HMT_MENU"].data<coral::Blob>();
   boost::iostreams::stream<boost::iostreams::array_source> streamHLT( static_cast<const char*> ( hltmenuBlob.startingAddress()), hltmenuBlob.size());
   boost::property_tree::read_json(streamHLT, hltmenu);

   session->transaction().commit();

   return true;
}



bool
TrigConf::TrigDBMenuLoader::loadL1Menu( unsigned int smk, L1Menu & l1menu ) const
{
   boost::property_tree::ptree ptl1;
   bool success = loadL1Menu( smk, ptl1 );
   if(!success)
      return false;
   if( ! ptl1.empty() ) {
      l1menu.setData(std::move(ptl1));
      l1menu.setSMK(smk);
   }
   return true;
}

bool
TrigConf::TrigDBMenuLoader::loadHLTMenu( unsigned int smk, HLTMenu & hltmenu ) const
{
   boost::property_tree::ptree pthlt;
   bool success = loadHLTMenu( smk, pthlt );
   if(!success)
      return false;
   if( ! pthlt.empty() ) {
      hltmenu.setData(std::move(pthlt));
      hltmenu.setSMK(smk);
   }
   return true;
}
