/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CaloJetInputLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  31. Oct. 2005   
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#include "./CaloJetInputLoader.h"

#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

bool TrigConf::CaloJetInputLoader::load( CaloJetInput& cjiTarget ) {
   msg() << "Loading CaloJetInput via ID. ID = " << cjiTarget.id() << std::endl;
      
   try {
      startSession();
      msg() << "Loading CaloJetInput " << cjiTarget.id() << std::endl;

      //do something here

      commitSession();      
      return true;
      
   } catch( const coral::SchemaException& e ) {
      msg() << "CaloJetInputLoader >> SchemaException: " 
            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;
   } catch( const std::exception& e ) {
      msg() << "CaloJetInputLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      return false; 
   } catch(...) {
      msg() << "CaloJetInputLoader >> unknown C++ exception" << std::endl;
      m_session.transaction().rollback();
      return false; 
   }
}


  
