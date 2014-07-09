/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     ThresholdMonitorLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   G.Fischer (DESY)	Gordon.Fischer@cern.ch 
//CREATED:  16. Oct. 2007   
//                                                     
//PURPOSE:
//UPDATED:  8 Dec 2008 Paul Bell for sqlite access 
//          (use of defineOutput method to set data type)
//
//////////////////////////////////////////////////////////////////////


#include "./ThresholdMonitorLoader.h"
#include "TrigConfStorage/StorageMgr.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"


#include "TrigConfL1Data/ThresholdMonitor.h"
#include "TrigConfL1Data/L1DataDef.h"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <typeinfo>


bool TrigConf::ThresholdMonitorLoader::load( ThresholdMonitor& tmTarget ) {
  
   if(verbose()>=2)
      msg() << "ThresholdMonitorLoader started loading data via ID. ID = " 
            << tmTarget.id() << " for MenuId = " 
            << m_MenuId << std::endl;
        
   try {
      unsigned int schema = triggerDBSchemaVersion();

      startSession();
      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_TM_TO_TT_MON");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 6 );

      //Bind list
      coral::AttributeList bindList;
      bindList.extend<int>("tmId");
      bindList.extend<int>("menuId");
      bindList[0].data<int>() = tmTarget.id();
      bindList[1].data<int>() = m_MenuId;
      std::string theCondition = "";
      theCondition += std::string( " L1TM2TTM_ID = :tmId"                    );
      theCondition += std::string( " AND L1TM2TTM_TRIGGER_MENU_ID = :menuId" );
      query->setCondition( theCondition, bindList );

      //Output types
      coral::AttributeList attList;
      attList.extend<std::string>( "L1TM2TTM_NAME" );
      attList.extend<long>( "L1TM2TTM_TRIGGER_THRESHOLD_ID" );
      attList.extend<int>( "L1TM2TTM_INTERNAL_COUNTER" );
      attList.extend<int>( "L1TM2TTM_MULTIPLICITY" );
      if(schema <= 6) attList.extend<long>( "L1TM2TTM_BUNCH_GROUP_SET_ID" );
      if(schema > 6) attList.extend<long>( "L1TM2TTM_BUNCH_GROUP_ID" );
      attList.extend<std::string>( "L1TM2TTM_COUNTER_TYPE" );
      query->defineOutput(attList);

      query->addToOutputList("L1TM2TTM_NAME" );
      query->addToOutputList("L1TM2TTM_TRIGGER_THRESHOLD_ID" );
      query->addToOutputList("L1TM2TTM_INTERNAL_COUNTER" );
      query->addToOutputList("L1TM2TTM_MULTIPLICITY" );
      if(schema <= 6) query->addToOutputList("L1TM2TTM_BUNCH_GROUP_SET_ID" );
      else if(schema > 6) query->addToOutputList("L1TM2TTM_BUNCH_GROUP_ID" );
      query->addToOutputList("L1TM2TTM_COUNTER_TYPE" );

      coral::ICursor& cursor = query->execute();
	
      if ( ! cursor.next() ) {
         msg() << "ThresholdMonitorLoader >> No such ThresholdMonitor exists " 
               << tmTarget.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "ThresholdMonitorLoader >> ThresholdMonitor not available" );
      }
        	
      const coral::AttributeList& row = cursor.currentRow();
      long trigger_threshold_id =0;
      trigger_threshold_id = row["L1TM2TTM_TRIGGER_THRESHOLD_ID"].data<long>();
      int internal_counter =0;
      internal_counter = row["L1TM2TTM_INTERNAL_COUNTER"].data<int>();
      int multiplicity;
      multiplicity = row["L1TM2TTM_MULTIPLICITY"].data<int>();
      long bunch_group_id=0;
      if(schema <= 6) bunch_group_id = row["L1TM2TTM_BUNCH_GROUP_SET_ID"].data<long>();
      else if(schema > 6) bunch_group_id = row["L1TM2TTM_BUNCH_GROUP_ID"].data<long>();
      std::string countertype="";
      countertype = row["L1TM2TTM_COUNTER_TYPE"].data<std::string>();
      std::string name="";
      name = row["L1TM2TTM_NAME"].data<std::string>();

      // Fill the vector with data
      tmTarget.setName(name);
      tmTarget.setThresholdId(trigger_threshold_id);
      tmTarget.setInternalCounter(internal_counter );
      tmTarget.setMultiplicity(multiplicity);

      tmTarget.setCounterType(countertype );
      tmTarget.setBunchGroupId(bunch_group_id );

      // now given the threshold id get the name, type, bit,
      // etc. of the threshold
      coral::ITable& tableThresh = m_session.nominalSchema().tableHandle( "L1_TRIGGER_THRESHOLD");
      coral::IQuery* queryThresh = tableThresh.newQuery();
      queryThresh->setRowCacheSize( 4 );

      //Binding
      std::string cond = "L1TT_ID =  :threshId";
      coral::AttributeList alist;
      alist.extend<long>("threshId");
      alist[0].data<long>() = static_cast<long>(trigger_threshold_id);
      queryThresh->setCondition( cond, alist );

      //Output and types
      coral::AttributeList attList1;
      attList1.extend<std::string>( "L1TT_NAME" );
      attList1.extend<int>( "L1TT_ACTIVE" );
      queryThresh->defineOutput(attList1);
      queryThresh->addToOutputList( "L1TT_NAME" );
      queryThresh->addToOutputList( "L1TT_ACTIVE" );

      coral::ICursor& cursorThresh = queryThresh->execute();
      if ( ! cursorThresh.next() ) {
         msg() << "ThresholdMonitorLoader >> No such trigger threshold exists : " << trigger_threshold_id << std::endl;
         delete query;
         delete queryThresh;
         commitSession();
         throw std::runtime_error( "ThresholdMonitorLoader >> TriggerThreshold not available" );
      }
	    
      const coral::AttributeList& rowThresh = cursorThresh.currentRow();
      tmTarget.setThresholdName( rowThresh["L1TT_NAME"].data<std::string>() );
      tmTarget.setThresholdActive( rowThresh["L1TT_ACTIVE"].data<int>() );	    

      // now retrieve the first and last bit of the threshold, and
      // the ctpin mapping
      coral::ITable& tableTMTOTT = m_session.nominalSchema().tableHandle( "L1_TM_TO_TT");
      coral::IQuery* queryTMTOTT = tableTMTOTT.newQuery();
      queryTMTOTT->setRowCacheSize( 5 );

      //Binding
      cond = "L1TM2TT_TRIGGER_THRESHOLD_ID = :ttId AND L1TM2TT_TRIGGER_MENU_ID = :menuId";
      coral::AttributeList alistTMTOTT;
      alistTMTOTT.extend<long>("ttId");
      alistTMTOTT.extend<long>("menuId");
      alistTMTOTT[0].data<long>() = trigger_threshold_id;
      alistTMTOTT[1].data<long>() = m_MenuId;
      queryTMTOTT->setCondition( cond, alistTMTOTT );

      //Output and types
      coral::AttributeList attList2;
      attList2.extend<std::string>( "L1TM2TT_CABLE_CTPIN" ); 
      attList2.extend<std::string>( "L1TM2TT_CABLE_CONNECTOR" );
      attList2.extend<int>( "L1TM2TT_CABLE_START" ); 
      attList2.extend<int>( "L1TM2TT_CABLE_END" );
      queryTMTOTT->defineOutput(attList2);
      queryTMTOTT->addToOutputList( "L1TM2TT_CABLE_CTPIN" );
      queryTMTOTT->addToOutputList( "L1TM2TT_CABLE_CONNECTOR" );
      queryTMTOTT->addToOutputList( "L1TM2TT_CABLE_START" );
      queryTMTOTT->addToOutputList( "L1TM2TT_CABLE_END" );

      coral::ICursor& cursorTMTOTT = queryTMTOTT->execute();
      if ( ! cursorTMTOTT.next() ) {
         msg() << "ThresholdMonitorLoader >> No such trigger threshold ( " << trigger_threshold_id 
               << " ) or menu ( " << m_MenuId << " ) exists in table L1_TM_TO_TT" << std::endl;
         delete query;
         delete queryThresh;
         delete queryTMTOTT;
         commitSession();
         throw std::runtime_error( "ThresholdMonitorLoader >> TriggerThreshold link entry not available" );
      }
      const coral::AttributeList& rowTMTOTT = cursorTMTOTT.currentRow();

      std::string slotString = rowTMTOTT["L1TM2TT_CABLE_CTPIN"].data<std::string>();
      uint16_t slot = 0;
      if(slotString.find("SLOT7") != std::string::npos) {
         slot = 7;
      } else if(slotString.find("SLOT8") != std::string::npos) {
         slot = 8;
      } else if(slotString.find("SLOT9") != std::string::npos) {
         slot = 9;
      } else {
         msg() << "Unknown CTPIN string '" << slotString << "'" << std::endl;
         delete query;
         delete queryThresh;
         delete queryTMTOTT;
         commitSession();
         throw std::runtime_error( "ThresholdMonitorLoader: Error loading Counters " );
      }
      tmTarget.setCtpinSlot( slot );	    
      std::string conString = rowTMTOTT["L1TM2TT_CABLE_CONNECTOR"].data<std::string>();
      uint16_t con = 99;
      if(conString.find("CON0") != std::string::npos) {
         con = 0;
      } else if(conString.find("CON1") != std::string::npos) {
         con = 1;
      } else if(conString.find("CON2") != std::string::npos) {
         con = 2;
      } else if(conString.find("CON3") != std::string::npos) {
         con = 3;
      } else {
         msg() << "Unknown CTPIN connector string '" << conString << "'" << std::endl;
         delete query;
         delete queryThresh;
         delete queryTMTOTT;
         commitSession();
         throw std::runtime_error( "ThresholdMonitorLoader: Error loading Counters " );
      }
      tmTarget.setCtpinConnector(con);
      tmTarget.setThresholdStartBit( rowTMTOTT["L1TM2TT_CABLE_START"].data<int>() );
      tmTarget.setThresholdEndBit( rowTMTOTT["L1TM2TT_CABLE_END"].data<int>() );

      delete query;
      delete queryThresh;
      delete queryTMTOTT;
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "ThresholdMonitorLoader >> SchemaException: " 

            << e.what() << std::endl;
      m_session.transaction().rollback();
      return false;
   } catch( const std::exception& e ) {
      msg() << "ThresholdMonitorLoader >> Standard C++ exception: " << e.what() << std::endl;

      m_session.transaction().rollback();
      return false; 
   } catch( ... ) {
      msg() << "ThresholdMonitorLoader >> unknown C++ exception" << std::endl;

      m_session.transaction().rollback();
      return false; 
   }
}

void TrigConf::ThresholdMonitorLoader::setMenuId(const long& id) {
   m_MenuId = id;
}  

long TrigConf::ThresholdMonitorLoader::menuId() {
   return m_MenuId;
}  

