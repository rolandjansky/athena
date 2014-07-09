/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CaloInfoLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  26. Jan. 2005   
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#include "boost/lexical_cast.hpp"

#include "./CaloInfoLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/CaloInfo.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <iterator>

bool
TrigConf::CaloInfoLoader::load( CaloInfo& data ) {

   // cast to CaloInfo object
   CaloInfo *ci(0);
   try {
      ci = &dynamic_cast<CaloInfo&>(data);
   } catch (std::bad_cast& ex) {
      msg() << "Caught exception in CaloInfoLoader : "
            << ex.what() << std::endl;
      data = CaloInfo();
      return false;
   }

   CaloInfo& ciTarget = * ci;
   bool useSMK = false;
   if(ciTarget.id()!=0) {
      if(verbose())
         msg() << "CaloInfoLoader:                   Loading CaloInfo with ID = " << ciTarget.id() << std::endl;
   } else {
      if(ciTarget.superMasterTableId()!=0) {
         useSMK = true;
         if(verbose())
            msg() << "CaloInfoLoader:                   Loading CaloInfo attached to SuperMasterKey " << ciTarget.superMasterTableId() << std::endl;
      } else {
         if(verbose())
            msg() << "CaloInfoLoader:                   ERROR can not load CaloInfo which has no id or SMK specified" << std::endl;
         return false;
      }
   }

   try {
      unsigned int schema = triggerDBSchemaVersion();

      startSession();

      coral::IQuery* query = m_session.nominalSchema().newQuery();
      query->addToTableList( "L1_CALO_INFO", "L1CI" );
      query->setRowCacheSize( 5 );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("Id");
      std::string cond("");
      if(useSMK) {
         query->addToTableList( "SUPER_MASTER_TABLE", "SMT" );
         query->addToTableList( "L1_MASTER_TABLE", "L1M" );
         cond  = "SMT.SMT_ID = :Id";
         cond += " AND SMT.SMT_L1_MASTER_TABLE_ID = L1M.L1MT_ID";
         cond += " AND L1CI.L1CI_ID = L1M.L1MT_CALO_INFO_ID";
         bindList[0].data<int>() = ciTarget.superMasterTableId();
      } else {
         cond = "L1CI.L1CI_ID = :Id";
         bindList[0].data<int>() = ciTarget.id();
      }
      query->setCondition( cond, bindList );

      //Output data and types
      coral::AttributeList attList;
      attList.extend<std::string>( "L1CI.L1CI_NAME" ); 
      attList.extend<int>( "L1CI.L1CI_VERSION" ); 
      attList.extend<std::string>( "L1CI.L1CI_GLOBAL_SCALE" ); 

      query->addToOutputList( "L1CI.L1CI_NAME" );
      query->addToOutputList( "L1CI.L1CI_VERSION" );
      query->addToOutputList( "L1CI.L1CI_GLOBAL_SCALE" );

      if(schema>=10) {
         query->addToOutputList( "L1CI.L1CI_XS_SIGMA_SCALE" );
         query->addToOutputList( "L1CI.L1CI_XS_SIGMA_OFFSET" );
         query->addToOutputList( "L1CI.L1CI_XS_XE_MIN" );
         query->addToOutputList( "L1CI.L1CI_XS_XE_MAX" );
         query->addToOutputList( "L1CI.L1CI_XS_TESQRT_MIN" );
         query->addToOutputList( "L1CI.L1CI_XS_TESQRT_MAX" );
         attList.extend<int>( "L1CI.L1CI_XS_SIGMA_SCALE" ); 
         attList.extend<int>( "L1CI.L1CI_XS_SIGMA_OFFSET" );
         attList.extend<int>( "L1CI.L1CI_XS_XE_MIN" );
         attList.extend<int>( "L1CI.L1CI_XS_XE_MAX" );
         attList.extend<int>( "L1CI.L1CI_XS_TESQRT_MIN" );
         attList.extend<int>( "L1CI.L1CI_XS_TESQRT_MAX" );
      }
      for(unsigned int index = 1; index <= 12 ;index++) {
         std::stringstream helpstring;
         helpstring << "L1CI.L1CI_JET_WEIGHT" << index;
         attList.extend<int>( helpstring.str() ); 
         query->addToOutputList( helpstring.str() );
      }

      query->defineOutput(attList);
      coral::ICursor& cursor = query->execute();      

      if(!cursor.next()) {
         if(useSMK) {
            msg() << "CaloInfoLoader >> No CaloInfo exists for SMK " << ciTarget.superMasterTableId() << std::endl;
         } else {
            msg() << "CaloInfoLoader >> No CaloInfo exists with ID" << ciTarget.id() << std::endl;
         }
         delete query;
         commitSession();
         throw std::runtime_error( "CaloInfoLoader >> CaloInfo not available" );
      }
        
      // fill the object with data
      const coral::AttributeList& row = cursor.currentRow();
      ciTarget.setName       ( row["L1CI.L1CI_NAME"].data<std::string>()  );
      ciTarget.setVersion    ( row["L1CI.L1CI_VERSION"].data<int>()      );
      if(schema <= 6) ciTarget.setGlobalScale( row["L1CI.L1CI_GLOBAL_SCALE"].data<float>());        
      else ciTarget.setGlobalScale( boost::lexical_cast<float,std::string>(row["L1CI.L1CI_GLOBAL_SCALE"].data<std::string>()));        
      for(unsigned int index = 1; index <= 12 ;index++) {
         std::stringstream helpstring;
         helpstring << "L1CI.L1CI_JET_WEIGHT" << index;
         ciTarget.addJetWeight( static_cast<int>(row[helpstring.str()].data<int>()) );
      }

      if(schema>=10) {
         int XSSigmaScale  = row["L1CI.L1CI_XS_SIGMA_SCALE"].data<int>();
         int XSSigmaOffset = row["L1CI.L1CI_XS_SIGMA_OFFSET"].data<int>();
         int XEmin         = row["L1CI.L1CI_XS_XE_MIN"].data<int>();
         int XEmax         = row["L1CI.L1CI_XS_XE_MAX"].data<int>();
         int TESqrtMin     = row["L1CI.L1CI_XS_TESQRT_MIN"].data<int>();
         int TESqrtMax     = row["L1CI.L1CI_XS_TESQRT_MAX"].data<int>();
         ciTarget.metSigParam().setValues( XSSigmaScale, XSSigmaOffset,
                                           XEmin, XEmax, TESqrtMin, TESqrtMax);
      }

      // check for uniqness of CaloInfo object
      if ( cursor.next() ) {
         msg() << "CaloInfoLoader >> More than one CaloInfo exists " 
               << ciTarget.id() << std::endl;
         throw std::runtime_error( "CaloInfoLoader >>  Too many CaloInfo objects are available" );
      }
        
      //msg() << "CaloInfoLoader:                   Loaded " << ciTarget.jetWeights().size() << " jetweights" << std::endl;
      //std::copy(ciTarget.jetWeights().begin(), ciTarget.jetWeights().end(), std::ostream_iterator<int>(msg(), ", "));

      delete query;
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "CaloInfoLoader  >> SchemaException: " 
            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;
   } catch( const std::exception& e ) {
      msg() << "CaloInfoLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      return false; 
   } catch( ... ) {
      msg() << "CaloInfoLoader >> Unknown C++ exception" << std::endl;
      m_session.transaction().rollback();
      return false; 
   }
}









