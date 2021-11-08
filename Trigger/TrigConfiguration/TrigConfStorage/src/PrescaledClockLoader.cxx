/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     PrescaledClockLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005   
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#include "./PrescaledClockLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/PrescaledClock.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

bool TrigConf::PrescaledClockLoader::load( PrescaledClock& pcTarget ) {

   if(verbose())
      msg() << "PrescaledClockLoader:             Load PrescaledClock with ID = " << pcTarget.id() << std::endl;
      
   try {

      startSession();
      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_PRESCALED_CLOCK");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 5 );
        
      //Bind list
      coral::AttributeList bindList;
      bindList.extend<long>("pcId");
      std::string cond = "L1PC_ID = :pcId";
      bindList[0].data<long>() = pcTarget.id();
      query->setCondition( cond, bindList);

      //Output data and types
      coral::AttributeList attList;
      attList.extend<std::string>( "L1PC_NAME" );
      attList.extend<int>( "L1PC_VERSION" );
      attList.extend<int>( "L1PC_CLOCK1" );
      attList.extend<int>( "L1PC_CLOCK2" );
      attList.extend<long>( "L1PC_ID" );
      query->defineOutput(attList);
      
      query->addToOutputList( "L1PC_NAME" );
      query->addToOutputList( "L1PC_VERSION" );
      query->addToOutputList( "L1PC_CLOCK1" );
      query->addToOutputList( "L1PC_CLOCK2" );
      query->addToOutputList( "L1PC_ID" );
      query->addToOrderList( "L1PC_ID" );

      coral::ICursor& cursor = query->execute();
      
      if ( ! cursor.next() ) {
         msg() << "PrescaledClockLoader >> No such prescaled clock exists " 
               << pcTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "PrescaledClockLoader >> PrescaledClock not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name    = row["L1PC_NAME"].data<std::string>();
      int         version = row["L1PC_VERSION"].data<int>();
      int         clock1  = row["L1PC_CLOCK1"].data<int>();
      int         clock2  = row["L1PC_CLOCK2"].data<int>();

      if ( cursor.next() ) {
         msg() << "PrescaledClockLoader >> More than one PrescaledClock exists " 
               << pcTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "PrescaledClockLoader >>  PrescaledClock not available" );
      }

      // Fill the object with data
      pcTarget.setName( name );
      pcTarget.setVersion( version );
      pcTarget.setClock1( clock1 );
      pcTarget.setClock2( clock2 );

      delete query;
      commitSession();
      return true;

   } catch( const coral::SchemaException& e ) {
      msg() << "PrescaledClockLoader >> SchemaException: " 

            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;

   } catch( const std::exception& e ) {
      msg() << "PrescaledClockLoader >> Standard C++ exception: " << e.what() << std::endl;

      m_session.transaction().rollback();
      return false; 

   } catch( ... ) {
      msg() << "PrescaledClockLoader >> Uknown C++ exception" << std::endl;

      m_session.transaction().rollback();
      return false; 
   }   
}

