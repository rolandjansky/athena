// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBHLTPrescalesSetLoader.h"

TrigConf::TrigDBHLTPrescalesSetLoader::TrigDBHLTPrescalesSetLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBHLTPrescalesSetLoader", connection)
{
   { // query for all schema versions
      auto & q = m_queries[1];
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
}

TrigConf::TrigDBHLTPrescalesSetLoader::~TrigDBHLTPrescalesSetLoader() = default;

bool
TrigConf::TrigDBHLTPrescalesSetLoader::loadHLTPrescales ( unsigned int psk, TrigConf::HLTPrescalesSet & pss,
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
            TRG_MSG_ERROR("Tried reading HLT prescales, but HLT prescale key " << psk << " is not available" );
            throw TrigConf::NoHLTPSKException("TrigDBHLTPrescalesSetLoader: HLT PSK " + std::to_string(psk) + " not available");
         }
         const coral::AttributeList& row = cursor.currentRow();
         const coral::Blob& dataBlob = row[qdef.dataName()].data<coral::Blob>();
         writeRawFile( dataBlob, outFileName );
         blobToPtree( dataBlob, pt );
      }
      catch(coral::QueryException & ex) {
         TRG_MSG_ERROR("When reading HLT prescales for HLT PSK " << psk << " a coral::QueryException was caught ( " << ex.what() <<" )" );
         throw TrigConf::QueryException("TrigDBHLTPrescalesSetLoader: " + std::string(ex.what()));
      }
   }
   try {
      pss.setData(std::move(pt));
      pss.setPSK(psk);
   }
   catch(std::exception & ex) {
      pss.clear();
      TRG_MSG_ERROR("When reading HLT prescales for HLT PSK " << psk << " a parsing error occured ( " << ex.what() <<" )" );
      throw TrigConf::ParsingException("TrigDBHLTPrescalesSetLoader: parsing error " + std::string(ex.what()));
   }
   return true;
}
