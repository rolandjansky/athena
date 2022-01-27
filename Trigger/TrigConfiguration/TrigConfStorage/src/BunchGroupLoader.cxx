/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./BunchGroupLoader.h"
#include "./DBHelper.h"

#include "TrigConfL1Data/BunchGroup.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

using namespace std;

bool TrigConf::BunchGroupLoader::load(BunchGroup& bgTarget) {

   TRG_MSG_DEBUG("Loading BunchGroup with ID = " << bgTarget.id());

   try {
      startSession();
      if(isRun1()) {
         unique_ptr<coral::IQuery> query( m_session.nominalSchema().tableHandle( "L1_BUNCH_GROUP").newQuery() );
         query->setRowCacheSize( 5 );

         //Bind list
         coral::AttributeList bindList;
         bindList.extend<long>("bgId");
         std::string cond = "L1BG_ID = :bgId";
         bindList[0].data<long>() = bgTarget.id();
         query->setCondition( cond, bindList );

         //Output data and types
         coral::AttributeList attList;
         attList.extend<std::string>( "L1BG_NAME" );
         attList.extend<int>( "L1BG_VERSION" );
         fillQuery(query.get(),attList);
 
         coral::ICursor& cursor = query->execute();
      
         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("No bunchgroup exists with ID " << bgTarget.id());
            commitSession();
            throw std::runtime_error( "BunchGroupLoader >> BunchGroup not available" );
         }

         const coral::AttributeList& row = cursor.currentRow();
         std::string name    = row["L1BG_NAME"].data<std::string>();
         int         version = row["L1BG_VERSION"].data<int>();

         // fill the object with data
         bgTarget.setName( name );
         bgTarget.setVersion( version );
      }


      {
         // now touch the l1_bg_to_b table to get the bunches.
         unique_ptr<coral::IQuery> q( m_session.nominalSchema().tableHandle( "L1_BG_TO_B").newQuery() );
         q->setRowCacheSize( 5 );

         //binding
         coral::AttributeList bindList;
         bindList.extend<long>("bgId");
         bindList[0].data<long>() = bgTarget.id();
         q->setCondition( "L1BG2B_BUNCH_GROUP_ID = :bgId", bindList );

         //Output data and types
         coral::AttributeList attList;
         attList.extend<int>( "L1BG2B_BUNCH_NUMBER" );
         fillQuery(q.get(),attList);

         coral::ICursor& cursor = q->execute();

         while( cursor.next() ) {
            const coral::AttributeList& row = cursor.currentRow();
            bgTarget.addBunch( row["L1BG2B_BUNCH_NUMBER"].data<int>() );
         }
      
      }
      commitSession();
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Coral::Exception: " << e.what());
      m_session.transaction().rollback();
      throw;
   }
   
   return true;

} 



