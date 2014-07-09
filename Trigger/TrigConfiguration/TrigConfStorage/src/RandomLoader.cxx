/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     RandomLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005   
//                                                     
//PURPOSE:
//UPDATED:  8 Dec 2008 Paul Bell for sqlite access 
//          (use of defineOutput method to set dat
//
//////////////////////////////////////////////////////////////////////

#include "./RandomLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/Random.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

bool TrigConf::RandomLoader::load( Random& rdTarget ) {

   if(verbose())
      msg() << "RandomLoader:                     Start loading data via ID. ID = " << rdTarget.id() << std::endl;
      
   try {
      startSession();
      
      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_RANDOM");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 5 );
	
      //Bind list
      coral::AttributeList bindList;
      bindList.extend<long>("rdId");
      std::string cond = "L1R_ID = :rdId";
      bindList[0].data<long>() = rdTarget.id();
      query->setCondition( cond, bindList );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<std::string>( "L1R_NAME" ); 
      attList.extend<int>( "L1R_VERSION" ); 
      attList.extend<long>( "L1R_RATE1" ); 
      attList.extend<long>( "L1R_RATE2" ); 
      query->defineOutput(attList);

      query->addToOutputList( "L1R_NAME" );
      query->addToOutputList( "L1R_VERSION" );
      query->addToOutputList( "L1R_RATE1" );
      query->addToOutputList( "L1R_RATE2" );

      coral::ICursor& cursor = query->execute();
      
      if ( ! cursor.next() ) {
         msg() << "RandomLoader >> No such random exists " << rdTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "RandomLoader >> Random not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name ="";
      name = row["L1R_NAME"].data<std::string>();
      int version = 0;
      version = row["L1R_VERSION"].data<int>();
      long rate1 = 0;
      rate1 = row["L1R_RATE1"].data<long>();
      long rate2 = 0;
      rate2 = row["L1R_RATE2"].data<long>();

      if ( cursor.next() ) {
         msg() << "RandomLoader >> More than one Random exists " 
               << rdTarget.id() << std::endl;
         commitSession();
         throw std::runtime_error( "RandomLoader >>  Random not available" );
      }

      // Fill the object with data
      rdTarget.setName( name );
      rdTarget.setVersion( version );
      rdTarget.setRate1( rate1 );
      rdTarget.setRate2( rate2 );

      delete query;
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "RandomLoader >> SchemaException: " 

            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;

   } catch( const std::exception& e ) {
      msg() << "RandomLoader >> Standard C++ exception: " << e.what() << std::endl;

      m_session.transaction().rollback();
      return false;

   } catch( ... ) {
      msg() << "RandomLoader >> Uknown C++ exception" << std::endl;

      m_session.transaction().rollback();
      return false;
   }
}
