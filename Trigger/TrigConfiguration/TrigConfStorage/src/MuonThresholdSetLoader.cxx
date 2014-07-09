/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     MuonThresholdSetLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   D.Berge (CERN)	berge@cern.ch 
//CREATED:  03. Aug. 2008
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#include "./MuonThresholdSetLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/MuonThresholdSet.h"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

std::vector<TrigConf::MuonThresholdSet>
TrigConf::MuonThresholdSetLoader::loadAll() {

   std::vector<TrigConf::MuonThresholdSet> vSets;
   unsigned int schema = triggerDBSchemaVersion();
      
   if(schema > 6) {
      startSession();

      std::vector<int> sm_keys;
      std::vector<std::string> alias;
      std::vector<bool> def;
      try {
         msg() << "Load all sm keys from trigger_alias table." << std::endl;
         coral::ITable& table = m_session.nominalSchema().tableHandle( "TRIGGER_ALIAS");
         coral::IQuery* query = table.newQuery();
         query->setRowCacheSize( 5 );

         //Set types
         coral::AttributeList attList;
         attList.extend<int>( "TAL_SUPER_MASTER_TABLE_ID" );
         attList.extend<std::string>( "TAL_TRIGGER_ALIAS" );
         attList.extend<int>( "TAL_DEFAULT" );
         query->defineOutput(attList);

         query->addToOutputList( "TAL_SUPER_MASTER_TABLE_ID" );		  
         query->addToOutputList( "TAL_TRIGGER_ALIAS" );
         query->addToOutputList( "TAL_DEFAULT" );
         coral::ICursor& cursor = query->execute();

         while (cursor.next()) {
            const coral::AttributeList& row = cursor.currentRow();
            sm_keys.push_back(row["TAL_SUPER_MASTER_TABLE_ID"].data<int>());
            alias.push_back(row["TAL_TRIGGER_ALIAS"].data<std::string>());
            def.push_back(static_cast<bool>(row["TAL_DEFAULT"].data<int>()));
         }

         for(size_t i = 0; i<sm_keys.size(); ++i) {

            msg() << "Found sm_key " << sm_keys[i] << " alias '" << alias[i] 
                  << "' default " << def[i] << std::endl;
	       
            TrigConf::MuonThresholdSet muons;
            muons.setSuperMasterTableId(sm_keys[i]);
            try {
               if ( !this->load( muons ) ) {
                  msg() << "Muon threshold set for sm key " 
                        << sm_keys[i] << " not available" << std::endl;
               }
            } catch(std::bad_cast& ex) {
               msg() << " Failed casting : " << ex.what();
            }
            vSets.push_back(muons);
         }

         delete query;
         commitSession();
      } catch( const coral::SchemaException& e ) {
         msg() << "MuonThresholdSetLoader  >> SchemaException: " 
               << e.what() << std::endl;
         m_session.transaction().rollback();
      } catch( const std::exception& e ) {
         msg() << "MuonThresholdSetLoader >> Standard C++ exception: " << e.what() << std::endl;
         m_session.transaction().rollback();
      } catch( ... ) {
         msg() << "MuonThresholdSetLoader >> unknown C++ exception" << std::endl;
         m_session.transaction().rollback();
      }
   }
   return vSets;
}
   
bool TrigConf::MuonThresholdSetLoader::load(MuonThresholdSet& target) {
   msg() << "MuonThresholdSetLoader start loading data" << std::endl;
	 
   unsigned int schema = triggerDBSchemaVersion();

   startSession();

   if(schema > 6) {

      // check if we need to load ID first
      if(target.id() == 0) {

         try {
            int Lvl1MasterKey = 0;
            int setId = 0;
            { // load L1 master key from super master key

               //msg() << "Load L1 master key for sm key " << target.superMasterTableId() << std::endl;
               coral::ITable& table = m_session.nominalSchema().tableHandle( "SUPER_MASTER_TABLE");
               coral::IQuery* query = table.newQuery();
               query->setRowCacheSize( 5 );

               coral::AttributeList conditionData;
               conditionData.extend<int>("smtid");
               query->setCondition( "SMT_ID = :smtid" , conditionData );
               conditionData[0].data<int>() = target.superMasterTableId();

               coral::AttributeList attList;
               attList.extend<int>( "SMT_L1_MASTER_TABLE_ID" );		  
               query->defineOutput(attList);
               query->addToOutputList( "SMT_L1_MASTER_TABLE_ID" );		  

               coral::ICursor& cursor = query->execute();
               if ( ! cursor.next() ) {
                  msg() << "MuonThresholdSetLoader >> No such SuperMaster key exists " 
                        << target.superMasterTableId() << std::endl;
                  delete query;
                  commitSession();
                  throw std::runtime_error( "MuonThresholdSetLoader >> SuperMasterKey not available" );
               }
               const coral::AttributeList& row = cursor.currentRow();
               Lvl1MasterKey = row["SMT_L1_MASTER_TABLE_ID"].data<int>();

               //msg() << "L1 master key " << Lvl1MasterKey << std::endl;
               delete query;
            }
            { // load muon threshold set id from L1 master key

               //msg() << "Load muon set id for L1 master key " << Lvl1MasterKey << std::endl;

               coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_MASTER_TABLE");
               coral::IQuery* query = table.newQuery();
               query->setRowCacheSize( 5 );

               coral::AttributeList conditionData;
               conditionData.extend<int>("l1mtid");
               query->setCondition( "L1MT_ID = :l1mtid" , conditionData );
               conditionData[0].data<int>() = Lvl1MasterKey;

               coral::AttributeList attList;
               attList.extend<int>( "L1MT_MUON_THRESHOLD_SET_ID" ); 
               query->defineOutput(attList);

               query->addToOutputList( "L1MT_MUON_THRESHOLD_SET_ID" );		  
               coral::ICursor& cursor = query->execute();
               if ( ! cursor.next() ) {
                  msg() << "MuonThresholdSetLoader >> No such L1 master key exists " 
                        << Lvl1MasterKey << std::endl;
                  delete query;
                  commitSession();
                  throw std::runtime_error( "MuonThresholdSetLoader >> L1MasterKey not available" );
               }
               const coral::AttributeList& row = cursor.currentRow();
               setId = row["L1MT_MUON_THRESHOLD_SET_ID"].data<int>();
               //msg() << "Muon set id " << setId << std::endl;
               delete query;
            }
            target.setId(setId);
         } catch( const coral::SchemaException& e ) {
            msg() << "MuonThresholdSetLoader  >> SchemaException: " 
                  << e.what() << std::endl;
            m_session.transaction().rollback();
            return false;
         } catch( const std::exception& e ) {
            msg() << "MuonThresholdSetLoader >> Standard C++ exception: " << e.what() << std::endl;
            m_session.transaction().rollback();
            return false; 
         } catch( ... ) {
            msg() << "MuonThresholdSetLoader >> unknown C++ exception" << std::endl;
            m_session.transaction().rollback();
            return false; 
         }
      }

      try {
         msg() << "Super master key " << target.superMasterTableId() 
               << " MuonThresholdSet id " << target.id() << std::endl;

         coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_MUON_THRESHOLD_SET");	    
         coral::IQuery* query = table.newQuery();
         query->setRowCacheSize( 5 );	    

         std::string cond = "L1MTS_ID =  :threshId";
         coral::AttributeList alist;
         alist.extend<int>("threshId");
         alist[0].data<int>() = target.id();
         query->setCondition( cond, alist );

         coral::AttributeList attList;
         attList.extend<std::string>( "L1MTS_NAME" );
         attList.extend<int>( "L1MTS_VERSION" );
         attList.extend<int>( "L1MTS_RPC_AVAILABLE" );
         attList.extend<int>( "L1MTS_RPC_AVAILABLE_ONLINE" );
         attList.extend<int>( "L1MTS_TGC_AVAILABLE" );
         attList.extend<int>( "L1MTS_TGC_AVAILABLE_ONLINE" );
         attList.extend<int>( "L1MTS_RPC_SET_EXT_ID");
         attList.extend<std::string>( "L1MTS_RPC_SET_NAME" );
         attList.extend<int>( "L1MTS_TGC_SET_EXT_ID");
         attList.extend<std::string>( "L1MTS_TGC_SET_NAME" );
         attList.extend<int>( "L1MTS_RPC_PT1_EXT_ID" );
         attList.extend<int>( "L1MTS_RPC_PT2_EXT_ID" );
         attList.extend<int>( "L1MTS_RPC_PT3_EXT_ID" );
         attList.extend<int>( "L1MTS_RPC_PT4_EXT_ID" );
         attList.extend<int>( "L1MTS_RPC_PT5_EXT_ID" );
         attList.extend<int>( "L1MTS_RPC_PT6_EXT_ID" );
         query->defineOutput(attList);

         query->addToOutputList( "L1MTS_NAME" );
         query->addToOutputList( "L1MTS_VERSION" );
         query->addToOutputList( "L1MTS_RPC_AVAILABLE" );
         query->addToOutputList( "L1MTS_RPC_AVAILABLE_ONLINE" );
         query->addToOutputList( "L1MTS_TGC_AVAILABLE" );
         query->addToOutputList( "L1MTS_TGC_AVAILABLE_ONLINE" );
         query->addToOutputList( "L1MTS_RPC_SET_EXT_ID" );
         query->addToOutputList( "L1MTS_RPC_SET_NAME" );
         query->addToOutputList( "L1MTS_TGC_SET_EXT_ID" );
         query->addToOutputList( "L1MTS_TGC_SET_NAME" );
         query->addToOutputList( "L1MTS_RPC_PT1_EXT_ID" );
         query->addToOutputList( "L1MTS_RPC_PT2_EXT_ID" );
         query->addToOutputList( "L1MTS_RPC_PT3_EXT_ID" );
         query->addToOutputList( "L1MTS_RPC_PT4_EXT_ID" );
         query->addToOutputList( "L1MTS_RPC_PT5_EXT_ID" );
         query->addToOutputList( "L1MTS_RPC_PT6_EXT_ID" );

         coral::ICursor& cursor = query->execute();
         if ( ! cursor.next() ) {
            msg() << "MuonThresholdSetLoader >> No such threshold set exists : " 
                  << target.id() << std::endl;
            delete query;
            commitSession();
            throw std::runtime_error( "MuonThresholdSetLoader >> ThresholdSet not available" );
         }
	    
         const coral::AttributeList& row = cursor.currentRow();

         target.setName( row["L1MTS_NAME"].data<std::string>() );
         target.setVersion( row["L1MTS_VERSION"].data<int>() );

         target.setRpcAvailable( static_cast<bool>(row["L1MTS_RPC_AVAILABLE"].data<int>()) );
         target.setRpcAvailableOnline( static_cast<bool>(row["L1MTS_RPC_AVAILABLE_ONLINE"].data<int>()) );
         target.setTgcAvailable( static_cast<bool>(row["L1MTS_TGC_AVAILABLE"].data<int>()) );
         target.setTgcAvailableOnline( static_cast<bool>(row["L1MTS_TGC_AVAILABLE_ONLINE"].data<int>()) );

         target.setRpcSetId( row["L1MTS_RPC_SET_EXT_ID"].data<int>() );
         target.setTgcSetId( row["L1MTS_TGC_SET_EXT_ID"].data<int>() );

         target.setRpcSetName( row["L1MTS_RPC_SET_NAME"].data<std::string>() );
         target.setTgcSetName( row["L1MTS_TGC_SET_NAME"].data<std::string>() );

         target.setRpcPt1Id( row["L1MTS_RPC_PT1_EXT_ID"].data<int>() );
         target.setRpcPt2Id( row["L1MTS_RPC_PT2_EXT_ID"].data<int>() );
         target.setRpcPt3Id( row["L1MTS_RPC_PT3_EXT_ID"].data<int>() );
         target.setRpcPt4Id( row["L1MTS_RPC_PT4_EXT_ID"].data<int>() );
         target.setRpcPt5Id( row["L1MTS_RPC_PT5_EXT_ID"].data<int>() );
         target.setRpcPt6Id( row["L1MTS_RPC_PT6_EXT_ID"].data<int>() );

         delete query;
         commitSession();
         return true;
      } catch( const coral::SchemaException& e ) {
         msg() << "MuonThresholdSetLoader  >> SchemaException: " 
               << e.what() << std::endl;
         m_session.transaction().rollback();
         return false;
      } catch( const std::exception& e ) {
         msg() << "MuonThresholdSetLoader >> Standard C++ exception: " << e.what() << std::endl;
         m_session.transaction().rollback();
         return false; 
      } catch( ... ) {
         msg() << "MuonThresholdSetLoader >> unknown C++ exception" << std::endl;
         m_session.transaction().rollback();
         return false; 
      } 
   } else {
      msg() << "Schema version " << schema << " too old, don't load MuonThresholdSet" << std::endl;
      target = MuonThresholdSet();
      return false;
   }
}

