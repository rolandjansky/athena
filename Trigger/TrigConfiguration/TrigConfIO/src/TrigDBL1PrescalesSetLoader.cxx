// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBL1PrescalesSetLoader.h"

TrigConf::TrigDBL1PrescalesSetLoader::TrigDBL1PrescalesSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBL1PrescalesSetLoader", connection)
{
   { // query for all schema versions
      auto & q = m_queries[1];
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
}

TrigConf::TrigDBL1PrescalesSetLoader::~TrigDBL1PrescalesSetLoader() = default;

bool
TrigConf::TrigDBL1PrescalesSetLoader::loadL1Prescales ( unsigned int psk, TrigConf::L1PrescalesSet & pss,
                                                        const std::string & outFileName ) const
{
   boost::property_tree::ptree pt;
   {
      auto session = createDBSession();
      session->transaction().start( /*bool readonly=*/ true);
      QueryDefinition qdef = getQueryDefinition(session.get(), m_queries);
      try {
         qdef.setBoundValue<int>("key", psk);
         auto q = qdef.createQuery( session.get() );
         auto & cursor = q->execute();
         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("Tried reading L1 prescales, but L1 prescale key " << psk << " is not available" );
            throw TrigConf::NoL1PSKException("TrigDBL1PrescalesSetLoader: L1 PSK " + std::to_string(psk) + " not available");
         }
         const coral::AttributeList& row = cursor.currentRow();
         const coral::Blob& dataBlob = row[qdef.dataName()].data<coral::Blob>();
         writeRawFile( dataBlob, outFileName );
         blobToPtree( dataBlob, pt );
      }
      catch(coral::QueryException & ex) {
         TRG_MSG_ERROR("When reading L1 prescales for L1 PSK " << psk << " a coral::QueryException was caught ( " << ex.what() <<" )" );
         throw TrigConf::QueryException("TrigDBL1PrescalesSetLoader: " + std::string(ex.what()));
      }
   }
   try {
      pss.setData(std::move(pt));
      pss.setPSK(psk);
   }
   catch(std::exception & ex) {
      pss.clear();
      TRG_MSG_ERROR("When reading L1 prescales for L1 PSK " << psk << " a parsing error occured ( " << ex.what() <<" )" );
      throw TrigConf::ParsingException("TrigDBL1PrescalesSetLoader: parsing error " + std::string(ex.what()));
   }

   return true;
}
