/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/TrigDBHLTPrescalesSetLoader.h"

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
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITransaction.h"


#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/iostreams/stream.hpp"

#include <memory>
#include <exception>

TrigConf::TrigDBHLTPrescalesSetLoader::TrigDBHLTPrescalesSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBHLTPrescalesSetLoader", connection)
{}

TrigConf::TrigDBHLTPrescalesSetLoader::~TrigDBHLTPrescalesSetLoader()
{}


bool
TrigConf::TrigDBHLTPrescalesSetLoader::loadHLTPrescales ( unsigned int hltpsk,
                                                          TrigConf::HLTPrescalesSet & hltpss ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);

   std::unique_ptr<coral::IQuery> query( session->nominalSchema().tableHandle("HLT_PRESCALE_SET").newQuery() );
   query->setRowCacheSize( 5 );
   
   // bind list
   coral::AttributeList bindList;
   bindList.extend<int>("hltpsk");
   bindList[0].data<int>() = hltpsk;

   // condition clause
   std::string theCondition = "HPS_ID = :hltpsk";
   query->setCondition( theCondition, bindList );

   // output data and types
   coral::AttributeList attList;
   attList.extend<std::string>( "HPS_NAME" );
   attList.extend<int>        ( "HPS_VERSION" );
   attList.extend<std::string>( "HPS_COMMENT" );
   attList.extend<coral::Blob>( "HPS_DATA" );
   query->defineOutput(attList);
   for( const coral::Attribute & attr : attList) {
      query->addToOutputList(attr.specification().name());
   }

   coral::ICursor& cursor = query->execute();

   if ( ! cursor.next() ) {
      throw std::runtime_error( "TrigDBHLTPrescalesSetLoader: HLT PSK " + std::to_string(hltpsk) + " not available" );
   }
	
   const coral::AttributeList& row = cursor.currentRow();

   const coral::Blob& hltpsBlob = row["HPS_DATA"].data<coral::Blob>();
   boost::iostreams::stream<boost::iostreams::array_source> stream( static_cast<const char*> ( hltpsBlob.startingAddress()), hltpsBlob.size());

   boost::property_tree::ptree hltpss_pt;
   boost::property_tree::read_json(stream, hltpss_pt);
   hltpss.setData(hltpss_pt);

   session->transaction().commit();

   return true;
}
