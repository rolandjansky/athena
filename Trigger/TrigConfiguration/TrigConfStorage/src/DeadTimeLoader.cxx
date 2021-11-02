/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     DeadTimeLoader.cpp 
//PACKAGE:  TTrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005   
//                                                     
//PURPOSE:
//UPDATED:  8 Dec 2008 Paul Bell for sqlite access 
//          (use of defineOutput method to set data type)
//
//////////////////////////////////////////////////////////////////////

#include "./DeadTimeLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/DeadTime.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

using std::string;

bool TrigConf::DeadTimeLoader::load( DeadTime& dtTarget) {
   if(verbose())
      msg() << "DeadTimeLoader:                   Start loading data via ID. ID=" << dtTarget.id() << std::endl;

   try {
      startSession();

      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_DEAD_TIME");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<long>("dtId");
      std::string cond = "L1DT_ID = :dtId";
      bindList[0].data<long>() = dtTarget.id();
      query->setCondition( cond, bindList );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<int>( "L1DT_VERSION" );
      attList.extend<string>( "L1DT_NAME" );
      attList.extend<int>( "L1DT_SIMPLE" );
      attList.extend<int>( "L1DT_COMPLEX1_LEVEL" );
      attList.extend<int>( "L1DT_COMPLEX1_RATE" );
      attList.extend<int>( "L1DT_COMPLEX2_LEVEL" );
      attList.extend<int>( "L1DT_COMPLEX2_RATE" );
      attList.extend<long>( "L1DT_ID" );
      query->defineOutput(attList);

      query->addToOutputList( "L1DT_VERSION" );
      query->addToOutputList( "L1DT_NAME" );
      query->addToOutputList( "L1DT_SIMPLE" );
      query->addToOutputList( "L1DT_COMPLEX1_LEVEL" );
      query->addToOutputList( "L1DT_COMPLEX1_RATE" );
      query->addToOutputList( "L1DT_COMPLEX2_LEVEL" );
      query->addToOutputList( "L1DT_COMPLEX2_RATE" );
      query->addToOutputList( "L1DT_ID" );
      query->addToOrderList( "L1DT_ID" );

      coral::ICursor& cursor = query->execute();

      if ( ! cursor.next() ) {
         msg() << "DeadTimeLoader >> No such deadtime exists " << dtTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "DeadTimeLoader >> DeadTime not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name = row["L1DT_NAME"].data<std::string>();
      int version = row["L1DT_VERSION"].data<int>();
      int simple  = row["L1DT_SIMPLE"].data<int>();
      int complex1_level = row["L1DT_COMPLEX1_LEVEL"].data<int>();
      int complex1_rate = row["L1DT_COMPLEX1_RATE"].data<int>();
      int complex2_level = row["L1DT_COMPLEX2_LEVEL"].data<int>();
      int complex2_rate = row["L1DT_COMPLEX2_RATE"].data<int>();

      if ( cursor.next() ) {
         msg() << "DeadTimeLoader >> More than one DeadTime exists " 
               << dtTarget.id() << std::endl;
         commitSession();
         throw std::runtime_error( "DeadTimeLoader >>  DeadTime not available" );
      }

      // fill the object with data
      dtTarget.setName( name );
      dtTarget.setVersion( version );
      dtTarget.setSimple( simple );
      dtTarget.setComplex1Level( complex1_level );
      dtTarget.setComplex1Rate( complex1_rate );
      dtTarget.setComplex2Level( complex2_level );
      dtTarget.setComplex2Rate( complex2_rate );

      delete query;
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "DeadTimeLoader >> SchemaException: " 
            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;
   } catch( const std::exception& e ) {
      msg() << "DeadTimeLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      return false; 
   } catch( ... ) {
      msg() << "DeadTimeLoader >> Unknown C++ exception" << std::endl;
      m_session.transaction().rollback();
      return false; 
   }
   
}

