/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/TrigDBL1BunchGroupSetLoader.h"

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

TrigConf::TrigDBL1BunchGroupSetLoader::TrigDBL1BunchGroupSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBL1BunchGroupSetLoader", connection)
{}

TrigConf::TrigDBL1BunchGroupSetLoader::~TrigDBL1BunchGroupSetLoader()
{}


bool
TrigConf::TrigDBL1BunchGroupSetLoader::loadBunchGroupSet ( unsigned int bgsk,
                                                           TrigConf::L1BunchGroupSet & bgs ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);

   std::unique_ptr<coral::IQuery> query( session->nominalSchema().tableHandle("L1_BUNCH_GROUP_SET").newQuery() );
   query->setRowCacheSize( 5 );
   
   // bind list
   coral::AttributeList bindList;
   bindList.extend<int>("bgsk");
   bindList[0].data<int>() = bgsk;

   // condition clause
   std::string theCondition = "L1BGS_ID = :bgsk";
   query->setCondition( theCondition, bindList );

   // output data and types
   coral::AttributeList attList;
   attList.extend<std::string>( "L1BGS_NAME" );
   attList.extend<int>        ( "L1BGS_VERSION" );
   attList.extend<std::string>( "L1BGS_COMMENT" );
   attList.extend<coral::Blob>( "L1BGS_DATA" );
   query->defineOutput(attList);
   for( const coral::Attribute & attr : attList) {
      query->addToOutputList(attr.specification().name());
   }

   coral::ICursor& cursor = query->execute();

   if ( ! cursor.next() ) {
      throw std::runtime_error( "TrigDBL1BunchGroupSetLoader: L1 Bunchgroup with key " + std::to_string(bgsk) + " not available" );
   }
	
   const coral::AttributeList& row = cursor.currentRow();

   const coral::Blob& l1psBlob = row["L1BGS_DATA"].data<coral::Blob>();
   boost::iostreams::stream<boost::iostreams::array_source> stream( static_cast<const char*> ( l1psBlob.startingAddress()), l1psBlob.size());

   boost::property_tree::ptree bgs_pt;
   boost::property_tree::read_json(stream, bgs_pt);
   bgs.setData(std::move(bgs_pt));
   bgs.setBGSK(bgsk);

   session->transaction().commit();

   return true;
}
