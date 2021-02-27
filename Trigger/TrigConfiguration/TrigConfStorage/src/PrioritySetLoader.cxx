/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./PrioritySetLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITableDescription.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

bool
TrigConf::PrioritySetLoader::load( PrioritySet& psTarget ) {

   msg() << "PrioritySetLoader started loading data via MasterID = " << psTarget.lvl1MasterTableId() << std::endl;
   try {

      startSession();
      // will create a view into the query result; this saves us one C++ class
      coral::IQuery* Query = m_session.nominalSchema().newQuery();

      // set the tables that are used
      Query->addToTableList ( "L1_TRIGGER_ITEM", "TI"  );
      Query->addToTableList ( "L1_TM_TO_TI",     "TMTI");
      Query->addToTableList ( "L1_MASTER_TABLE", "MT"  );

      // Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("mtId");
      bindList[0].data<int>() = psTarget.lvl1MasterTableId();
      std::string theCondition = "";
      theCondition += std::string( " MT.L1MT_ID = :mtId"                                         );
      theCondition += std::string( " AND MT.L1MT_TRIGGER_MENU_ID = TMTI.L1TM2TI_TRIGGER_MENU_ID" );
      theCondition += std::string( " AND TMTI.L1TM2TI_TRIGGER_ITEM_ID = TI.L1TI_ID"              );
      Query->setCondition( theCondition, bindList );
   
      // the table columns of the output and their types
      coral::AttributeList attList;
      attList.extend<long>( "TI.L1TI_CTP_ID" );
      attList.extend<std::string>( "TI.L1TI_PRIORITY" );
      Query->defineOutput(attList);
      Query->addToOutputList( "TI.L1TI_CTP_ID" );
      Query->addToOutputList( "TI.L1TI_PRIORITY" );

      // the ordering
      std::string theOrder = "";
      theOrder += "  TI.L1TI_CTP_ID";
      //       theOrder += "  L1_TM_TO_TI.L1TM2TI_CTP_ID";
      Query->addToOrderList( theOrder );

      // process the query
      coral::ICursor& Cursor = Query->execute();
      std::vector<std::string> prio(PrioritySet::N_PRIORITIES,std::string("HIGH"));
      while (Cursor.next()) {
         const coral::AttributeList& row = Cursor.currentRow();
         if(verbose()>1)
            row.toOutputStream( msg() ) << std::endl;
         long ctp_id = row["TI.L1TI_CTP_ID"].data<long>();
         prio.at(ctp_id) = row["TI.L1TI_PRIORITY"].data<std::string>();
      }
      psTarget.setPriorities( prio );
      delete Query;
      commitSession();
      return true;
   } catch (const coral::SchemaException& e) {
      msg() << "PrioritySetLoader >> SchemaException: " 
            << e.what() << std::endl;      
      m_session.transaction().rollback();
      return false;
   } catch (const std::exception& e) {
      msg() << "PrioritySetLoader >> Standard C++ exception: " << e.what() << std::endl;
      m_session.transaction().rollback();
      return false; 
   } catch (...) {
      msg() << "PrioritySetLoader >> unknown C++ exception" << std::endl;
      m_session.transaction().rollback();
      return false; 

   }

}
