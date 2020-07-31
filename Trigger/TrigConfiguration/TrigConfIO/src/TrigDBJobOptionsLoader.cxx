/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBJobOptionsLoader.h"

TrigConf::TrigDBJobOptionsLoader::TrigDBJobOptionsLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBJobOptionsLoader", connection)
{}

TrigConf::TrigDBJobOptionsLoader::~TrigDBJobOptionsLoader()
{}


namespace {
   std::vector<TrigConf::QueryDefinition>
   getQueryDefinitions() {
      std::vector<TrigConf::QueryDefinition> queries;

      { // query for table dev1
         queries.emplace_back();
         auto & q = queries.back();
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

      { // query for table dev2
         queries.emplace_back();
         auto & q = queries.back();
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
      return queries;
   }
}


bool
TrigConf::TrigDBJobOptionsLoader::loadJobOptions ( unsigned int smk,
                                                   boost::property_tree::ptree & jobOptions,
                                                   const std::string & outFileName ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   bool querySuccess { false };
   for( auto & qdef : getQueryDefinitions() ) {
      try {
         qdef.setBoundValue<int>("smk", smk);
         auto q = qdef.createQuery( session.get() );
         auto & cursor = q->execute();
         querySuccess = true;
         if ( ! cursor.next() ) {
            throw std::runtime_error( "TrigDBMenuLoader: SuperMasterKey not available" );
         }
         const coral::AttributeList& row = cursor.currentRow();
         const coral::Blob& dataBlob = row[qdef.dataName()].data<coral::Blob>();
         writeRawFile( dataBlob, outFileName );
         blobToPtree( dataBlob, jobOptions );
         break;
      }
      catch(coral::QueryException & ex) {
         TRG_MSG_INFO("Trying next query after coral::QueryException caught ( " << ex.what() <<" )" );
         continue;
      }
      catch(std::exception & ex) {
         TRG_MSG_INFO("Trying next query after std::exception caught ( " << ex.what() <<" )" );
         continue;
      }
   }
   if( ! querySuccess ) {
      TRG_MSG_ERROR("Could not read the HLT JobOptions data from the database, all query attempts failed");
      return false;
   }
   return true;
}


bool
TrigConf::TrigDBJobOptionsLoader::loadJobOptions ( unsigned int smk,
                                                   DataStructure & jobOptions,
                                                   const std::string & outFileName ) const
{

   boost::property_tree::ptree ptJobOptions;

   bool success = loadJobOptions( smk, ptJobOptions, outFileName );

   if(!success)
      return false;

   if( ! ptJobOptions.empty() )
      jobOptions.setData(std::move(ptJobOptions));

   return true;
}

