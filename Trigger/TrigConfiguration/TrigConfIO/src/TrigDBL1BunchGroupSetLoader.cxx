// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBL1BunchGroupSetLoader.h"

TrigConf::TrigDBL1BunchGroupSetLoader::TrigDBL1BunchGroupSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBL1BunchGroupSetLoader", connection)
{
   { // query for all schema versions
      auto & q = m_queries[1];
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
}

TrigConf::TrigDBL1BunchGroupSetLoader::~TrigDBL1BunchGroupSetLoader() = default;

bool
TrigConf::TrigDBL1BunchGroupSetLoader::loadBunchGroupSet ( unsigned int bgsk,
                                                           TrigConf::L1BunchGroupSet & bgs,
                                                           const std::string & outFileName ) const
{
   boost::property_tree::ptree pt;
   {
      auto session = createDBSession();
      session->transaction().start( /*bool readonly=*/ true);
      QueryDefinition qdef = getQueryDefinition(session.get(), m_queries);
      try {
         qdef.setBoundValue<int>("key", bgsk);
         auto q = qdef.createQuery( session.get() );
         auto & cursor = q->execute();
         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("Tried reading L1 bunchgroup set, but L1 bunchgroup key " << bgsk << " is not available" );
            throw TrigConf::NoBGSKException("TrigDBL1BunchGroupSetLoader: L1 bunchgroup key " + std::to_string(bgsk) + " not available");
         }
         const coral::AttributeList& row = cursor.currentRow();
         const coral::Blob& dataBlob = row[qdef.dataName()].data<coral::Blob>();
         writeRawFile( dataBlob, outFileName );
         blobToPtree( dataBlob, pt );
      }
      catch(coral::QueryException & ex) {
         TRG_MSG_ERROR("When reading L1 bunchgroup set for L1 bunchgroup key " << bgsk << " a coral::QueryException was caught ( " << ex.what() <<" )" );
         throw TrigConf::QueryException("TrigDBL1BunchGroupSetLoader: " + std::string(ex.what()));
      }
   }
   try {
      bgs.setData(std::move(pt));
      bgs.setBGSK(bgsk);
   }
   catch(std::exception & ex) {
      bgs.clear();
      TRG_MSG_ERROR("When reading L1 bunchgroup set for L1 BGSK " << bgsk << " a parsing error occured ( " << ex.what() <<" )" );
      throw TrigConf::ParsingException("TrigDBL1BunchGroupSetLoader: parsing error " + std::string(ex.what()));
   }

   return true;
}
