/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/TrigDBL1PrescalesSetLoader.h"

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

TrigConf::TrigDBL1PrescalesSetLoader::TrigDBL1PrescalesSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBL1PrescalesSetLoader", connection)
{}

TrigConf::TrigDBL1PrescalesSetLoader::~TrigDBL1PrescalesSetLoader()
{}


bool
TrigConf::TrigDBL1PrescalesSetLoader::loadL1Prescales ( unsigned int l1psk,
                                                        TrigConf::L1PrescalesSet & l1pss ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);

   std::unique_ptr<coral::IQuery> query( session->nominalSchema().tableHandle("L1_PRESCALE_SET").newQuery() );
   query->setRowCacheSize( 5 );
   
   // bind list
   coral::AttributeList bindList;
   bindList.extend<int>("l1psk");
   bindList[0].data<int>() = l1psk;

   // condition clause
   std::string theCondition = "L1PS_ID = :l1psk";
   query->setCondition( theCondition, bindList );

   // output data and types
   coral::AttributeList attList;
   attList.extend<std::string>( "L1PS_NAME" );
   attList.extend<int>        ( "L1PS_VERSION" );
   attList.extend<std::string>( "L1PS_COMMENT" );
   attList.extend<coral::Blob>( "L1PS_DATA" );
   query->defineOutput(attList);
   for( const coral::Attribute & attr : attList) {
      query->addToOutputList(attr.specification().name());
   }

   coral::ICursor& cursor = query->execute();

   if ( ! cursor.next() ) {
      throw std::runtime_error( "TrigDBL1PrescalesSetLoader: L1 PSK " + std::to_string(l1psk) + " not available" );
   }
	
   const coral::AttributeList& row = cursor.currentRow();

   const coral::Blob& l1psBlob = row["L1PS_DATA"].data<coral::Blob>();
   boost::iostreams::stream<boost::iostreams::array_source> stream( static_cast<const char*> ( l1psBlob.startingAddress()), l1psBlob.size());

   boost::property_tree::ptree l1pss_pt;
   boost::property_tree::read_json(stream, l1pss_pt);
   l1pss.setData(l1pss_pt);

   session->transaction().commit();

   return true;
}
