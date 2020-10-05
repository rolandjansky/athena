/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBJobOptionsLoader.h"

TrigConf::TrigDBJobOptionsLoader::TrigDBJobOptionsLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBJobOptionsLoader", connection)
{
   { // query for schema version 1
      auto & q = m_queries[1];
      // tables
      q.addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
      q.addToTableList ( "JO_MASTER_TABLE", "JOMT" );
      // bind vars
      q.extendBinding<int>("smk");
      // conditions
      q.extendCondition("SMT.SMT_ID = :smk");
      q.extendCondition(" AND SMT.SMT_JO_MASTER_TABLE_ID = JOMT.JO_ID");
      // attributes
      q.extendOutput<std::string>( "SMT.SMT_NAME" );
      q.extendOutput<int>        ( "SMT.SMT_JO_MASTER_TABLE_ID" );
      q.extendOutput<coral::Blob>( "JOMT.JO_CONTENT" );
      // the field with the data
      q.setDataName("JOMT.JO_CONTENT");
   }
   { // query for schema version 2
      auto & q = m_queries[2];
      // tables
      q.addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
      q.addToTableList ( "HLT_JOBOPTIONS", "HJO" );
      // bind vars
      q.extendBinding<int>("smk");
      // conditions
      q.extendCondition("SMT.SMT_ID = :smk");
      q.extendCondition("AND HJO.HJO_ID=SMT.SMT_HLT_JOBOPTIONS_ID");
      // attributes
      q.extendOutput<std::string>( "SMT.SMT_NAME" );
      q.extendOutput<int>        ( "SMT.SMT_HLT_JOBOPTIONS_ID" );
      q.extendOutput<coral::Blob>( "HJO.HJO_DATA" );
      // the field with the data
      q.setDataName("HJO.HJO_DATA");
   }
}

TrigConf::TrigDBJobOptionsLoader::~TrigDBJobOptionsLoader() = default;

bool
TrigConf::TrigDBJobOptionsLoader::loadJobOptions ( unsigned int smk,
                                                   boost::property_tree::ptree & jobOptions,
                                                   const std::string & outFileName ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   QueryDefinition qdef = getQueryDefinition(session.get(), m_queries);
   try {
      qdef.setBoundValue<int>("smk", smk);
      auto q = qdef.createQuery( session.get() );
      auto & cursor = q->execute();
      if ( ! cursor.next() ) {
         TRG_MSG_ERROR("Tried reading HLT job options, but SuperMasterKey " << smk << " is not available" );
         throw TrigConf::NoSMKException("TrigDBJobOptionsLoader: SMK " + std::to_string(smk) + " not available");
      }
      const coral::AttributeList& row = cursor.currentRow();
      const coral::Blob& dataBlob = row[qdef.dataName()].data<coral::Blob>();
      writeRawFile( dataBlob, outFileName );
      blobToPtree( dataBlob, jobOptions );
   }
   catch(coral::QueryException & ex) {
      TRG_MSG_ERROR("When reading HLT job options for SMK " << smk << " a coral::QueryException was caught ( " << ex.what() <<" )" );
      throw TrigConf::QueryException("TrigDBJobOptionsLoader: " + std::string(ex.what()));
   }
   return true;
}


bool
TrigConf::TrigDBJobOptionsLoader::loadJobOptions ( unsigned int smk,
                                                   DataStructure & jobOptions,
                                                   const std::string & outFileName ) const
{

   boost::property_tree::ptree ptJobOptions;
   loadJobOptions( smk, ptJobOptions, outFileName );
   try {
      jobOptions.setData(std::move(ptJobOptions));
   }
   catch(std::exception & ex) {
      jobOptions.clear();
      TRG_MSG_ERROR("When reading HLT job options for SMK " << smk << " a parsing error occured ( " << ex.what() <<" )" );
      throw TrigConf::ParsingException("TrigDBJobOptionsLoader: parsing error " + std::string(ex.what()));
   }
   return true;
}

