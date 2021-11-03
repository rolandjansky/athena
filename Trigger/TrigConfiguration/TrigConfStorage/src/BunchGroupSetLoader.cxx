/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./BunchGroupSetLoader.h"
#include "./BunchGroupLoader.h"
#include "./DBHelper.h"
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfL1Data/BunchGroupSet.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

using namespace std;

bool
TrigConf::BunchGroupSetLoader::load( BunchGroupSet& data ) {

   TRG_MSG_INFO("Loading BunchGroupSet with ID = " << data.id());
   try {

      {
         startSession();
         unique_ptr<coral::IQuery> q(m_session.nominalSchema().tableHandle( "L1_BUNCH_GROUP_SET").newQuery());
         q->setRowCacheSize( 5 );

         //Bind list
         coral::AttributeList bindList;
         bindList.extend<int>("bgsId");
         bindList[0].data<int>() = (int)data.id();
         q->setCondition( "L1BGS_ID = :bgsId", bindList );

         //Output data and types
         coral::AttributeList attList;
         attList.extend<std::string>( "L1BGS_NAME" ); 
         attList.extend<int>( "L1BGS_VERSION" ); 
         attList.extend<std::string>( "L1BGS_COMMENT" );
         fillQuery(q.get(),attList);

         coral::ICursor& cursor = q->execute();
      
         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("No such BGS exists " << data.id());
            commitSession();
            throw std::runtime_error( "BunchGroupSetLoader >> BunchGroupSet not available" );
         }

         const coral::AttributeList& row = cursor.currentRow();
         string name = row["L1BGS_NAME"].data<std::string>();
         int version = row["L1BGS_VERSION"].data<int>();
         string comment = row["L1BGS_COMMENT"].data<std::string>();

         if ( cursor.next() ) {
            TRG_MSG_ERROR("More than one BunchGroupSet exists " << data.id());
            commitSession();
            throw std::runtime_error( "BunchGroupSetLoader >>  BunchGroupSet not available" );
         }

         // fill the object with data
         data.setName( name );
         data.setVersion( version );
         data.setComment( comment );
      }

      // now touch the l1_bgs_to_bg table to get the bunch groups.
      {
         unique_ptr<coral::IQuery> q( m_session.nominalSchema().tableHandle( "L1_BGS_TO_BG").newQuery() );
         q->setRowCacheSize( 5 );

         //binding
         coral::AttributeList bindList;
         bindList.extend<int>("bgsId");
         bindList[0].data<int>() = (int)data.id();
         q->setCondition( "L1BGS2BG_BUNCH_GROUP_SET_ID = :bgsId ", bindList );

         //Output data and types
         coral::AttributeList attList;
         attList.extend<long>( "L1BGS2BG_BUNCH_GROUP_ID" );
         attList.extend<long>( "L1BGS2BG_INTERNAL_NUMBER" );
         if(isRun2()) {
            attList.extend<string>( "L1BGS2BG_BUNCH_GROUP_NAME" );
         }
         fillQuery(q.get(),attList);
      
         q->addToOrderList( "L1BGS2BG_INTERNAL_NUMBER" );

         coral::ICursor& cursor = q->execute();

         BunchGroupLoader& bgldr = dynamic_cast<BunchGroupLoader&>(dynamic_cast<StorageMgr&>(m_storageMgr).bunchGroupLoader() );
         while( cursor.next() ) {
            BunchGroup bg;
            const coral::AttributeList& row = cursor.currentRow();

            bg.setId(row["L1BGS2BG_BUNCH_GROUP_ID"].data<long>());
            bg.setInternalNumber( row["L1BGS2BG_INTERNAL_NUMBER"].data<long>() );
            if(isRun2()) {
               bg.setName( row["L1BGS2BG_BUNCH_GROUP_NAME"].data<string>() );
            }
            if ( !bgldr.load( bg ) ) {
               TRG_MSG_ERROR("loading BunchGroup " << bg.id() );
               commitSession();
               throw std::runtime_error( "BunchGroupSetLoader: Error loading BunchGroup" );
            }

            data.addBunchGroup(bg);
         }
         commitSession();
      }

   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Coral::Exception: " << e.what());
      m_session.transaction().rollback();
      throw;
   }
   
   return true;
}

