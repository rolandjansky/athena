/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./BunchGroupLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/BunchGroup.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

bool TrigConf::BunchGroupLoader::load(BunchGroup& bgTarget) {
   msg() << "BunchGroupLoader:                 Loading BunchGroup with ID = " << bgTarget.id() << std::endl;
   try {
      startSession();
      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_BUNCH_GROUP");
      coral::IQuery* query = table.newQuery();
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
      query->defineOutput(attList);
 
      query->addToOutputList( "L1BG_NAME" );
      query->addToOutputList( "L1BG_VERSION" );

      coral::ICursor& cursor = query->execute();
      
      if ( ! cursor.next() ) {
         msg() << "BunchGroupLoader >> No such deadtime exists " << bgTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "BunchGroupLoader >> BunchGroup not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name    = row["L1BG_NAME"].data<std::string>();
      int         version = row["L1BG_VERSION"].data<int>();

      if ( cursor.next() ) {
         msg() << "BunchGroupLoader >> More than one BunchGroup exists " 
               << bgTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "BunchGroupLoader >>  BunchGroup not available" );
      }

      // fill the object with data
      bgTarget.setName( name );
      bgTarget.setVersion( version );

      // now touch the l1_bg_to_b table to get the bunches.
      coral::ITable& table2 = m_session.nominalSchema().tableHandle( "L1_BG_TO_B");
      coral::IQuery* query2 = table2.newQuery();
      query2->setRowCacheSize( 5 );

      //binding
      coral::AttributeList bindList2;
      bindList2.extend<long>("bgId");
      std::string cond2 = "L1BG2B_BUNCH_GROUP_ID = :bgId";
      bindList2[0].data<long>() = bgTarget.id();
      query2->setCondition( cond2, bindList2 );

      //Output data and types
      coral::AttributeList attList2;
      attList2.extend<long>( "L1BG2B_BUNCH_NUMBER" );
      query2->defineOutput(attList2);
      query2->addToOutputList( "L1BG2B_BUNCH_NUMBER" );

      coral::ICursor& cursor2 = query2->execute();

      short bunch_number = 0;
      while( cursor2.next() ) {
         const coral::AttributeList& row2 = cursor2.currentRow();
         bunch_number = row2["L1BG2B_BUNCH_NUMBER"].data<long>();
         // fill the bunch numbers to the bgTarget
         bgTarget.addBunch(bunch_number);
      }
      
      delete query;
      delete query2;
      
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "BunchGroupLoader >> SchemaException: " 
            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;
   } catch( const std::exception& e ) {
      msg() << "BunchGroupLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      return false; 
   } catch( ... ) {
      msg() << "BunchGroupLoader >> Unknown C++ exception" << std::endl;
      m_session.transaction().rollback();
      return false; 
   }   
} 



