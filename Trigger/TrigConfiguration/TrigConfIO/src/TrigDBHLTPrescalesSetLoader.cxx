// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBHLTPrescalesSetLoader.h"

TrigConf::TrigDBHLTPrescalesSetLoader::TrigDBHLTPrescalesSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBHLTPrescalesSetLoader", connection)
{}

TrigConf::TrigDBHLTPrescalesSetLoader::~TrigDBHLTPrescalesSetLoader()
{}

namespace {
   std::vector<TrigConf::QueryDefinition>
   getQueryDefinitions() {
      std::vector<TrigConf::QueryDefinition> queries;
      { // query for table dev1 and dev2
         queries.emplace_back();
         auto & q = queries.back();
         // tables
         q.addToTableList ( "HLT_PRESCALE_SET" );
         // bind vars
         q.extendBinding<int>("key");
         // conditions
         q.extendCondition("HPS_ID = :key");
         // attributes
         q.extendOutput<coral::Blob>( "HPS_DATA" );
         // the field with the data
         q.setDataName("HPS_DATA");
      }
      return queries;
   }
}

bool
TrigConf::TrigDBHLTPrescalesSetLoader::loadHLTPrescales ( unsigned int psk, TrigConf::HLTPrescalesSet & pss,
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
            throw std::runtime_error( "TrigDBHLTPrescalesSetLoader: HLT presale key " + std::to_string(psk) + " not available" );
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
      TRG_MSG_ERROR("Could not read the HLT prescale set data from the database, all query attempts failed");
      return false;
   }

   return true;
}
