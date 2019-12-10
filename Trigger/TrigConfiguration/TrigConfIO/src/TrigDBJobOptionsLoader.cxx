/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/TrigDBJobOptionsLoader.h"

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

TrigConf::TrigDBJobOptionsLoader::TrigDBJobOptionsLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBJobOptionsLoader", connection)
{}

TrigConf::TrigDBJobOptionsLoader::~TrigDBJobOptionsLoader()
{}


bool
TrigConf::TrigDBJobOptionsLoader::loadJobOptions ( unsigned int smk,
                                                   boost::property_tree::ptree & jobOptions ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   
   std::unique_ptr< coral::IQuery > query( session->nominalSchema().newQuery() );
   query->addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
   query->addToTableList ( "JO_MASTER_TABLE", "JOMT" );

   // bind list
   coral::AttributeList bindList;
   bindList.extend<int>("smk");
   bindList[0].data<int>() = smk;

   // condition clause
   std::string theCondition = "";
   theCondition += std::string( " SMT.SMT_ID = :smk"        );
   theCondition += std::string( " AND SMT.SMT_JO_MASTER_TABLE_ID = JOMT.JO_ID" );

   query->setCondition( theCondition, bindList );

   // output data and types
   coral::AttributeList attList;
   attList.extend<std::string>( "SMT.SMT_NAME" );
   attList.extend<int>        ( "SMT.SMT_VERSION" );
   attList.extend<int>        ( "SMT.SMT_JO_MASTER_TABLE_ID" );
   attList.extend<coral::Blob>( "JOMT.JO_CONTENT" );

   query->defineOutput(attList);
   for( const coral::Attribute & attr : attList) {
      query->addToOutputList(attr.specification().name());
   }

   coral::ICursor& cursor = query->execute();

   if ( ! cursor.next() ) {
      throw std::runtime_error( "TrigDBJobOptionsLoader: SuperMasterKey not available" );
   }
	
   const coral::AttributeList& row = cursor.currentRow();

   const coral::Blob& joBlob = row["JOMT.JO_CONTENT"].data<coral::Blob>();
   boost::iostreams::stream<boost::iostreams::array_source> stream( static_cast<const char*> ( joBlob.startingAddress()), joBlob.size());
   boost::property_tree::read_json(stream, jobOptions);

   session->transaction().commit();

   return true;
}


bool
TrigConf::TrigDBJobOptionsLoader::loadJobOptions ( unsigned int smk,
                                                   DataStructure & jobOptions ) const
{

   boost::property_tree::ptree ptJobOptions, pthlt;

   bool success = this -> loadJobOptions( smk, ptJobOptions);

   if(!success)
      return false;

   if( ! ptJobOptions.empty() )
      jobOptions.setData(ptJobOptions);

   return true;
}

