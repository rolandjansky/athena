// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBL1BunchGroupSetLoader.h"

TrigConf::TrigDBL1BunchGroupSetLoader::TrigDBL1BunchGroupSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBL1BunchGroupSetLoader", connection)
{}

TrigConf::TrigDBL1BunchGroupSetLoader::~TrigDBL1BunchGroupSetLoader()
{}

namespace {
   std::vector<TrigConf::QueryDefinition>
   getQueryDefinitions() {
      std::vector<TrigConf::QueryDefinition> queries;
      { // query for table dev1 and dev2
         queries.emplace_back();
         auto & q = queries.back();
         // tables
         q.addToTableList ( "L1_BUNCH_GROUP_SET" );
         // bind vars
         q.extendBinding<int>("key");
         // conditions
         q.extendCondition("L1BGS_ID = :key");
         // attributes
         q.extendOutput<coral::Blob>( "L1BGS_DATA" );
         // the field with the data
         q.setDataName("L1BGS_DATA");
      }
      return queries;
   }
}

bool
TrigConf::TrigDBL1BunchGroupSetLoader::loadBunchGroupSet ( unsigned int bgsk,
                                                           TrigConf::L1BunchGroupSet & bgs,
                                                           const std::string & outFileName ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   bool querySuccess { false };
   for( auto & qdef : getQueryDefinitions() ) {
      try {
         qdef.setBoundValue<int>("key", bgsk);
         auto q = qdef.createQuery( session.get() );
         auto & cursor = q->execute();
         querySuccess = true;
         if ( ! cursor.next() ) {
            throw std::runtime_error( "TrigDBL1BunchGroupSetLoader: L1 bunchgroup key " + std::to_string(bgsk) + " not available" );
         }
         const coral::AttributeList& row = cursor.currentRow();
         const coral::Blob& dataBlob = row[qdef.dataName()].data<coral::Blob>();
         writeRawFile( dataBlob, outFileName );
         boost::property_tree::ptree pt;
         blobToPtree( dataBlob, pt );
         bgs.setData(std::move(pt));
         bgs.setBGSK(bgsk);
         break;
      }
      catch(coral::QueryException & ex) {
         continue;
      }
   }
   if( ! querySuccess ) {
      TRG_MSG_ERROR("Could not read the L1 bunchgroup set data from the database, all query attempts failed");
      return false;
   }

   return true;
}
