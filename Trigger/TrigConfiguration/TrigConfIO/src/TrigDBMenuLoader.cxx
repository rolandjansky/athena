// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBMenuLoader.h"

TrigConf::TrigDBMenuLoader::TrigDBMenuLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBMenuLoader", connection)
{}

TrigConf::TrigDBMenuLoader::~TrigDBMenuLoader()
{}


namespace {
   std::vector<TrigConf::QueryDefinition>
   getL1QueryDefinitions() {
      std::vector<TrigConf::QueryDefinition> queries;

      { // query for table dev1
         queries.emplace_back();
         auto & q = queries.back();
         // tables
         q.addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
         q.addToTableList ( "L1_MENU", "L1TM" );
         // bind vars
         q.extendBinding<int>("smk");
         // conditions
         q.extendCondition("SMT.SMT_ID = :smk");
         q.extendCondition(" AND SMT.SMT_L1_MENU_ID = L1TM.L1TM_ID");
         // attributes
         q.extendOutput<std::string>( "SMT.SMT_NAME" );
         q.extendOutput<int>        ( "SMT.SMT_VERSION" );
         q.extendOutput<int>        ( "SMT.SMT_L1_MENU_ID" );
         q.extendOutput<coral::Blob>( "L1TM.L1TM_DATA" );
         // the field with the data
         q.setDataName("L1TM.L1TM_DATA");
      }

      { // query for table dev2
         queries.emplace_back();
         auto & q = queries.back();
         // tables
         q.addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
         q.addToTableList ( "L1_MASTER_TABLE", "L1MT" );
         // bind vars
         q.extendBinding<int>("smk");
         // conditions
         q.extendCondition("SMT.SMT_ID = :smk");
         q.extendCondition(" AND SMT.SMT_L1_MASTER_TABLE_ID = L1MT.L1MT_ID");
         // attributes
         q.extendOutput<std::string>( "SMT.SMT_NAME" );
         q.extendOutput<int>        ( "SMT.SMT_L1_MASTER_TABLE_ID" );
         q.extendOutput<coral::Blob>( "L1MT.L1MT_MENU" );
         // the field with the data
         q.setDataName("L1MT.L1MT_MENU");
      }
      return queries;
   }

   std::vector<TrigConf::QueryDefinition>
   getHLTQueryDefinitions() {
      std::vector<TrigConf::QueryDefinition> queries;

      { // query for table dev1
         queries.emplace_back();
         auto & q = queries.back();
         // tables
         q.addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
         q.addToTableList ( "HLT_MENU", "HTM" );
         // bind vars
         q.extendBinding<int>("smk");
         // conditions
         q.extendCondition("SMT.SMT_ID = :smk");
         q.extendCondition(" AND SMT.SMT_HLT_MENU_ID = HTM.HTM_ID");
         // attributes
         q.extendOutput<std::string>( "SMT.SMT_NAME" );
         q.extendOutput<int>        ( "SMT.SMT_VERSION" );
         q.extendOutput<int>        ( "SMT.SMT_HLT_MENU_ID" );
         q.extendOutput<coral::Blob>( "HTM.HTM_DATA" );
         // the field with the data
         q.setDataName("HTM.HTM_DATA");
      }

      { // query for table dev2
         queries.emplace_back();
         auto & q = queries.back();
         // tables
         q.addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
         q.addToTableList ( "HLT_MASTER_TABLE", "HMT" );
         // bind vars
         q.extendBinding<int>("smk");
         // conditions
         q.extendCondition("SMT.SMT_ID = :smk");
         q.extendCondition(" AND SMT.SMT_HLT_MASTER_TABLE_ID = HMT.HMT_ID");
         // attributes
         q.extendOutput<std::string>( "SMT.SMT_NAME" );
         q.extendOutput<int>        ( "SMT.SMT_HLT_MASTER_TABLE_ID" );
         q.extendOutput<coral::Blob>( "HMT.HMT_MENU" );
         // the field with the data
         q.setDataName("HMT.HMT_MENU");
      }
      return queries;
   }

}

bool
TrigConf::TrigDBMenuLoader::loadL1Menu ( unsigned int smk,
                                         boost::property_tree::ptree & l1menu,
                                         const std::string & outFileName ) const
{

   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   bool querySuccess { false };
   for( auto & qdef : getL1QueryDefinitions() ) {
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
         blobToPtree( dataBlob, l1menu );
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
      TRG_MSG_ERROR("Could not read the L1Menu data from the database, all query attempts failed");
      return false;
   }

   return true;
}



bool
TrigConf::TrigDBMenuLoader::loadHLTMenu ( unsigned int smk,
                                          boost::property_tree::ptree & hltmenu,
                                          const std::string & outFileName ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   bool querySuccess { false };
   for( auto & qdef : getHLTQueryDefinitions() ) {
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
         blobToPtree( dataBlob, hltmenu );
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
      TRG_MSG_ERROR("Could not read the HLTMenu data from the database, all query attempts failed");
      return false;
   }

   return true;
}



bool
TrigConf::TrigDBMenuLoader::loadL1Menu( unsigned int smk, L1Menu & l1menu,
                                        const std::string & outFileName ) const
{
   boost::property_tree::ptree ptl1;
   bool success = loadL1Menu( smk, ptl1, outFileName );
   if(!success)
      return false;
   if( ! ptl1.empty() ) {
      l1menu.setData(std::move(ptl1));
      l1menu.setSMK(smk);
   }
   return true;
}

bool
TrigConf::TrigDBMenuLoader::loadHLTMenu( unsigned int smk, HLTMenu & hltmenu,
                                         const std::string & outFileName ) const
{
   boost::property_tree::ptree pthlt;
   bool success = loadHLTMenu( smk, pthlt, outFileName );
   if(!success)
      return false;
   if( ! pthlt.empty() ) {
      hltmenu.setData(std::move(pthlt));
      hltmenu.setSMK(smk);
   }
   return true;
}
