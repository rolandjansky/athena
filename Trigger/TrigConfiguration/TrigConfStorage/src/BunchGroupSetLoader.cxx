/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./BunchGroupSetLoader.h"
#include "./BunchGroupLoader.h"
#include "TrigConfStorage/StorageMgr.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/BunchGroupSet.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

bool
TrigConf::BunchGroupSetLoader::load( BunchGroupSet& data ) {
   try {
      BunchGroupSet& bgsTarget = dynamic_cast<BunchGroupSet&>(data);
      msg() << "BunchGroupSetLoader:              Loading BunchGroupSet with ID = " << bgsTarget.id() << std::endl;
      try
         {
            startSession();
            coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_BUNCH_GROUP_SET");
            coral::IQuery* query = table.newQuery();
            query->setRowCacheSize( 5 );

            //Bind list
            coral::AttributeList bindList;
            bindList.extend<int>("bgsId");
            std::string cond = "L1BGS_ID = :bgsId";
            bindList[0].data<int>() = (int)bgsTarget.id();
            query->setCondition( cond, bindList );

            //Output data and types
            coral::AttributeList attList;
            attList.extend<std::string>( "L1BGS_NAME" ); 
            attList.extend<int>( "L1BGS_VERSION" ); 
            attList.extend<std::string>( "L1BGS_COMMENT" );
            query->defineOutput(attList);
            query->addToOutputList( "L1BGS_NAME" );
            query->addToOutputList( "L1BGS_VERSION" );
            query->addToOutputList( "L1BGS_COMMENT" );

            coral::ICursor& cursor = query->execute();
      
            if ( ! cursor.next() ) {
               msg() << "BunchGroupSetLoader >> No such BGS exists " << bgsTarget.id() << std::endl;
               delete query;
               commitSession();
               throw std::runtime_error( "BunchGroupSetLoader >> BunchGroupSet not available" );
            }

            const coral::AttributeList& row = cursor.currentRow();
            std::string name = row["L1BGS_NAME"].data<std::string>();
            int version = row["L1BGS_VERSION"].data<int>();
            std::string comment = row["L1BGS_COMMENT"].data<std::string>();

            if ( cursor.next() ) {
               msg() << "BunchGroupSetLoader >> More than one BunchGroupSet exists " 
                     << bgsTarget.id() << std::endl;
               delete query;
               commitSession();
               throw std::runtime_error( "BunchGroupSetLoader >>  BunchGroupSet not available" );
            }

            // fill the object with data
            bgsTarget.setName( name );
            bgsTarget.setVersion( version );
            bgsTarget.setComment( comment );


            // now touch the l1_bgs_to_bg table to get the bunch groups.
            coral::ITable& table2 = m_session.nominalSchema().tableHandle( "L1_BGS_TO_BG");
            coral::IQuery* query2 = table2.newQuery();
            query2->setRowCacheSize( 5 );

            //binding
            coral::AttributeList bindList2;
            bindList2.extend<int>("bgsId");
            std::string cond2= "L1BGS2BG_BUNCH_GROUP_SET_ID = :bgsId ";
            bindList2[0].data<int>() = (int)bgsTarget.id();
            query2->setCondition( cond2, bindList2 );


            //Output data and types
            coral::AttributeList attList2;
            attList2.extend<long>( "L1BGS2BG_BUNCH_GROUP_ID" );
            attList2.extend<long>( "L1BGS2BG_INTERNAL_NUMBER" );
            query2->defineOutput(attList2);
            query2->addToOutputList( "L1BGS2BG_BUNCH_GROUP_ID" );
            query2->addToOutputList( "L1BGS2BG_INTERNAL_NUMBER" );
                                
            query2->addToOrderList( "L1BGS2BG_INTERNAL_NUMBER" );

            coral::ICursor& cursor2 = query2->execute();

            long bunch_group_id = 0;
            long internal_number = 99;
            BunchGroupLoader& bgldr = dynamic_cast<BunchGroupLoader&>(dynamic_cast<StorageMgr&>(m_storageMgr).bunchGroupLoader() );
            while( cursor2.next() ) {
               const coral::AttributeList& row2 = cursor2.currentRow();
               bunch_group_id = row2["L1BGS2BG_BUNCH_GROUP_ID"].data<long>();
               internal_number = row2["L1BGS2BG_INTERNAL_NUMBER"].data<long>();
               BunchGroup bg;
               bg.setId(bunch_group_id);
               if ( !bgldr.load( bg ) )
                  {
                     msg() << "BunchGroupSetLoader: Error loading BunchGroup " 
                           << bg.id() << std::endl;
                     delete query;
                     delete query2;
                     commitSession();
                     throw std::runtime_error( "BunchGroupSetLoader: Error loading BunchGroup" );
                  }
               // fill the bunch groups to the bgsTarget
               bg.setInternalNumber( internal_number );
               bgsTarget.addBunchGroup(bg);
            }
            delete query;
            delete query2;      
            commitSession();
            return true;
         } catch( const coral::SchemaException& e ) {
         msg() << "BunchGroupSetLoader  >> SchemaException: " 
               << e.what() << std::endl;
         m_session.transaction().rollback();
         return false;
      } catch( const std::exception& e ) {
         msg() << "BunchGroupSetLoader >> Standard C++ exception: " << e.what() << std::endl;
         m_session.transaction().rollback();
         return false; 
      } catch( ... ) {
         msg() << "BunchGroupSetLoader >> Unknown C++ exception" << std::endl;
         m_session.transaction().rollback();
         return false; 
      }
   } catch (std::bad_cast& ex) {
      msg() << "Caught exception in BunchGroupSetLoader : "
            << ex.what() << std::endl;
      data = BunchGroupSet();
      return false;
   }
}

