/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CaloSinCosLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005   
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#include "./CaloSinCosLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/CaloSinCos.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

bool
TrigConf::CaloSinCosLoader::load( CaloSinCos& cscTarget ) {
   msg() << "CaloSinCosLoader started loading data via ID. ID = " << cscTarget.id() << std::endl;

   try {
      startSession();
      msg() << "Loading CaloSinCos " << cscTarget.id() << std::endl;

      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_CALO_SIN_COS");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 5 );

      // the query condition
      std::string condition = "L1CSC_ID = :l1cscid";
	
      // binding
      coral::AttributeList boundvars;
      boundvars.extend<int>("l1cscid");
      boundvars[0].data<int>() = (int)cscTarget.id();

      query->setCondition( condition, boundvars );      

      //Output
      coral::AttributeList attList;
      attList.extend<int>( "L1CSC_VERSION" );
      attList.extend<std::string>( "L1CSC_NAME" ); 
      attList.extend<int>( "L1CSC_VAL1" ); 
      attList.extend<int>( "L1CSC_VAL2" ); 
      attList.extend<int>( "L1CSC_VAL3" ); 
      attList.extend<int>( "L1CSC_VAL4" ); 
      attList.extend<int>( "L1CSC_VAL5" ); 
      attList.extend<int>( "L1CSC_VAL6" );
      attList.extend<int>( "L1CSC_VAL7" );  	
      attList.extend<int>( "L1CSC_VAL8" );
      attList.extend<int>( "L1CSC_ETA_MIN" ); 
      attList.extend<int>( "L1CSC_ETA_MAX" );
      attList.extend<int>( "L1CSC_PHI_MIN" );
      attList.extend<int>( "L1CSC_PHI_MAX" );
      query->defineOutput(attList);

      query->addToOutputList( "L1CSC_VERSION" );
      query->addToOutputList( "L1CSC_NAME" );
      query->addToOutputList( "L1CSC_VAL1" );
      query->addToOutputList( "L1CSC_VAL2" );
      query->addToOutputList( "L1CSC_VAL3" );
      query->addToOutputList( "L1CSC_VAL4" );
      query->addToOutputList( "L1CSC_VAL5" );
      query->addToOutputList( "L1CSC_VAL6" );
      query->addToOutputList( "L1CSC_VAL7" );
      query->addToOutputList( "L1CSC_VAL8" );
      query->addToOutputList( "L1CSC_ETA_MIN" );
      query->addToOutputList( "L1CSC_ETA_MAX" );
      query->addToOutputList( "L1CSC_PHI_MIN" );
      query->addToOutputList( "L1CSC_PHI_MAX" );

      query->addToOrderList( "L1CSC_ID" );

      coral::ICursor& cursor = query->execute();

      if ( ! cursor.next() ) {
         msg() << "CaloSinCosLoader >> No such deadtime exists " << cscTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "CaloSinCosLoader >> CaloSinCos not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name = row["L1CSC_NAME"].data<std::string>();
      int version = row["L1CSC_VERSION"].data<int>();
      int val1 = row["L1CSC_VAL1"].data<int>();
      int val2 = row["L1CSC_VAL2"].data<int>();
      int val3 = row["L1CSC_VAL3"].data<int>();
      int val4 = row["L1CSC_VAL4"].data<int>();
      int val5 = row["L1CSC_VAL5"].data<int>();
      int val6 = row["L1CSC_VAL6"].data<int>();
      int val7 = row["L1CSC_VAL7"].data<int>();
      int val8 = row["L1CSC_VAL8"].data<int>();
      int eta_min = row["L1CSC_ETA_MIN"].data<int>();
      int eta_max = row["L1CSC_ETA_MAX"].data<int>();
      int phi_min = row["L1CSC_PHI_MIN"].data<int>();
      int phi_max = row["L1CSC_PHI_MAX"].data<int>();

      if ( cursor.next() ) {
         msg() << "CaloSinCosLoader >> More than one CaloSinCos exists " 
               << cscTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "CaloSinCosLoader >>  CaloSinCos not available" );
      }

      // fill the object with data
      cscTarget.setName( name );
      cscTarget.setVersion( version );
      cscTarget.setVal1( val1 );
      cscTarget.setVal2( val2 );
      cscTarget.setVal3( val3 );
      cscTarget.setVal4( val4 );
      cscTarget.setVal5( val5 );
      cscTarget.setVal6( val6 );
      cscTarget.setVal7( val7 );
      cscTarget.setVal8( val8 );
      cscTarget.setEtaMin(eta_min);
      cscTarget.setEtaMax(eta_max);
      cscTarget.setPhiMin(phi_min);
      cscTarget.setPhiMax(phi_max);

      delete query;      
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "CaloSinCosLoader >> SchemaException: " 
            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;
   } catch( const std::exception& e ) {
      msg() << "CaloSinCosLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      return false; 
   } catch(...) {
      msg() << "CaloSinCosLoader >> unknown C++ exception" << std::endl;
      m_session.transaction().rollback();
      return false; 
   }
}


