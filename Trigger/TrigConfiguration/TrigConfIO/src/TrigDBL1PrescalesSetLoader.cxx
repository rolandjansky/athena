// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBL1PrescalesSetLoader.h"

TrigConf::TrigDBL1PrescalesSetLoader::TrigDBL1PrescalesSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBL1PrescalesSetLoader", connection)
{}

TrigConf::TrigDBL1PrescalesSetLoader::~TrigDBL1PrescalesSetLoader()
{}

namespace {
   std::vector<TrigConf::QueryDefinition>
   getQueryDefinitions() {
      std::vector<TrigConf::QueryDefinition> queries;
      { // query for table dev1 and dev2
         queries.emplace_back();
         auto & q = queries.back();
         // tables
         q.addToTableList ( "L1_PRESCALE_SET" );
         // bind vars
         q.extendBinding<int>("key");
         // conditions
         q.extendCondition("L1PS_ID = :key");
         // attributes
         q.extendOutput<coral::Blob>( "L1PS_DATA" );
         // the field with the data
         q.setDataName("L1PS_DATA");
      }
      return queries;
   }
}

bool
TrigConf::TrigDBL1PrescalesSetLoader::loadL1Prescales ( unsigned int psk, TrigConf::L1PrescalesSet & pss,
                                                        const std::string & outFileName ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   bool querySuccess { false };
   for( auto & qdef : getQueryDefinitions() ) {
      try {
         qdef.setBoundValue<int>("key", psk);
         auto q = qdef.createQuery( session.get() );
         auto & cursor = q->execute();
         querySuccess = true;
         if ( ! cursor.next() ) {
            throw std::runtime_error( "TrigDBL1PrescalesSetLoader: L1 presale key " + std::to_string(psk) + " not available" );
         }
         const coral::AttributeList& row = cursor.currentRow();
         const coral::Blob& dataBlob = row[qdef.dataName()].data<coral::Blob>();
         writeRawFile( dataBlob, outFileName );
         boost::property_tree::ptree pt;
         blobToPtree( dataBlob, pt );
         pss.setData(std::move(pt));
         pss.setPSK(psk);
         break;
      }
      catch(coral::QueryException & ex) {
         continue;
      }
   }
   if( ! querySuccess ) {
      TRG_MSG_ERROR("Could not read the L1 prescale set data data from the database, all query attempts failed");
      return false;
   }

   return true;
}
