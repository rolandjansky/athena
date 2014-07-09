/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     CTPFilesLoader.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   D.Berge (CERN)	berge@cern.ch 
//CREATED:  04. Aug. 2008   
//                                                     
//PURPOSE: This Loader loads the CTPFiles Object from the TriggerDB
//
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// TODO:
// =====
// 2. load smx files, too
// 3. try and use proper stringstreams for string parsing -> speed?
//
////////////////////////////////////////////////////////////////////////

#include "./CTPFilesLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "TrigConfL1Data/CTPFiles.h"

#include <stdexcept>
#include <sstream>
#include <typeinfo>

bool TrigConf::CTPFilesLoader::load( CTPFiles& data) {
   try {
      CTPFiles& gfTarget = dynamic_cast<CTPFiles&>(data);
      bool retVal = true;
      if(gfTarget.loadCtpcoreFiles()) retVal = loadCtpcoreData(gfTarget);
      if(gfTarget.loadCtpinFiles()) retVal = loadCtpinData(gfTarget);
      if(gfTarget.loadCtpmonFiles()) retVal = loadCtpmonData(gfTarget);
      return retVal;
   } catch (std::bad_cast& ex) {
      msg() << "Caught exception in CTPFilesLoader : "
            << ex.what() << std::endl;
      data = CTPFiles();
      return false;
   }
}

// could make a templated function to be used by everybody for all
// fields of the master table...
int TrigConf::CTPFilesLoader::getMenuIdFromMaster(int mid) {

   coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_MASTER_TABLE");
   coral::IQuery* query = table.newQuery();
   query->setRowCacheSize( 5 );

   //Binding
   coral::AttributeList emptyBindVariableList0;
   std::string condition = "L1MT_ID = :mtId";
   emptyBindVariableList0.extend<int>("mtId");
   emptyBindVariableList0[0].data<int>() = mid;
   query->setCondition( condition, emptyBindVariableList0 );

   coral::AttributeList attList;
   attList.extend<int>( "L1MT_TRIGGER_MENU_ID" );
   query->defineOutput(attList);
   query->addToOutputList( "L1MT_TRIGGER_MENU_ID" );

   coral::ICursor& cursor = query->execute();
   if ( ! cursor.next() ) {
      msg() << "CTPFilesLoader >> No such Master_Table exists " 
            << mid << std::endl;
      delete query;
      commitSession();
      throw std::runtime_error( "CTPFilesLoader" );
   }
   const coral::AttributeList& row = cursor.currentRow();
   int menuid = row["L1MT_TRIGGER_MENU_ID"].data<int>();	
   if ( cursor.next() ) {
      msg() << "CTPFilesLoader >> More than one Master_Table exists " 
            << mid << std::endl;
      delete query;
      commitSession();
      throw std::runtime_error( "CTPFilesLoader >>  Master_Table not available" );
   }
   return menuid;
}

long TrigConf::CTPFilesLoader::getFilesIdFromMenu(int mid) {
   coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_TRIGGER_MENU");
   coral::IQuery* query = table.newQuery();
   query->setRowCacheSize( 5 );

   //Binding
   coral::AttributeList emptyBindVariableList0;
   std::string condition = "L1TM_ID = :tmId";
   emptyBindVariableList0.extend<int>("tmId");
   emptyBindVariableList0[0].data<int>() = mid;
   query->setCondition( condition, emptyBindVariableList0 );

   coral::AttributeList attList;
   attList.extend<int>( "L1TM_CTP_FILES_ID" );
   query->defineOutput(attList);
   query->addToOutputList( "L1TM_CTP_FILES_ID" );
      
   coral::ICursor& cursor = query->execute();
   if ( ! cursor.next() ) {
      msg() << "CTPFilesLoader >> No such Trigger_Menu exists " << mid << std::endl;
      delete query;
      commitSession();
      throw std::runtime_error( "CTPFilesLoader" );
   }
   const coral::AttributeList& row = cursor.currentRow();
   return static_cast<long>(row["L1TM_CTP_FILES_ID"].data<int>());
}

long TrigConf::CTPFilesLoader::getSmxIdFromMenu(int mid) {
   coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_TRIGGER_MENU");
   coral::IQuery* query = table.newQuery();
   query->setRowCacheSize( 5 );

   //Binding
   coral::AttributeList emptyBindVariableList0;
   std::string condition = "L1TM_ID = :tmId";
   emptyBindVariableList0.extend<int>("tmId");
   emptyBindVariableList0[0].data<int>() = mid;
   query->setCondition( condition, emptyBindVariableList0 );

   coral::AttributeList attList;
   attList.extend<int>( "L1TM_CTP_SMX_ID" );
   query->defineOutput(attList);
   query->addToOutputList( "L1TM_CTP_SMX_ID" );

   coral::ICursor& cursor = query->execute();
   if ( ! cursor.next() ) {
      msg() << "CTPFilesLoader >> No such Trigger_Menu exists " << mid << std::endl;
      delete query;
      commitSession();
      throw std::runtime_error( "CTPFilesLoader" );
   }
   const coral::AttributeList& row = cursor.currentRow();
   return static_cast<long>(row["L1TM_CTP_SMX_ID"].data<int>());
}

bool TrigConf::CTPFilesLoader::loadCtpcoreData( CTPFiles& ctpFiles) {
   msg() << "CTPFilesLoader : start loading CTPCORE data" << std::endl;
   try {
      startSession();
      // first check if we need to obtain the CTP files ID, or if it
      // was passed in
      if(ctpFiles.id() <= 0) {
         int l1id = ctpFiles.lvl1MasterTableId();
         int menuid = getMenuIdFromMaster(l1id);
         long filesid = getFilesIdFromMenu(menuid);
         ctpFiles.setId(filesid);
      }

      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_CTP_FILES");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 10 );

      //Binding
      coral::AttributeList emptyBindVariableList0;
      emptyBindVariableList0.extend<long>("fId");
      emptyBindVariableList0[0].data<long>() = ctpFiles.id();
      query->setCondition( "L1CF_ID = :fId", emptyBindVariableList0 );

      coral::AttributeList attList;
      attList.extend<std::string>( "L1CF_NAME" );
      attList.extend<int>( "L1CF_VERSION" );
      attList.extend<std::string>( "L1CF_LUT" );
      attList.extend<std::string>( "L1CF_CAM" );
      query->defineOutput(attList);
      query->addToOutputList( "L1CF_NAME" );
      query->addToOutputList( "L1CF_VERSION" );
      query->addToOutputList( "L1CF_LUT" );
      query->addToOutputList( "L1CF_CAM" );

      coral::ICursor& cursor = query->execute();

      if ( ! cursor.next() ) {
         msg() << "CTPFilesLoader >> No such L1_CTP_Files exists " << ctpFiles.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >> CTPFiles not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name = row["L1CF_NAME"].data<std::string>();
      int version = row["L1CF_VERSION"].data<int>();
      std::string ctpcoreLUT_str = row["L1CF_LUT"].data<std::string>();
      std::string ctpcoreCAM_str = row["L1CF_CAM"].data<std::string>();

      if ( cursor.next() ) {
         msg() << "CTPFilesLoader >> More than one GetFile exists "   << ctpFiles.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >>  CTPFiles not available" );
      }

      std::vector<u_int> lutVec(CTPFiles::ALL_CTPCORELUT_SIZE);
      parseHexString(lutVec,ctpcoreLUT_str);
      std::vector<u_int> camVec(CTPFiles::ALL_CTPCORECAM_SIZE);
      parseHexString(camVec,ctpcoreCAM_str);

      // Fill the object with data
      ctpFiles.setName( name );
      ctpFiles.setVersion( version );
      ctpFiles.setCtpcoreLUT(lutVec);
      ctpFiles.setCtpcoreCAM(camVec);

      delete query;
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "CTPFilesLoader >> SchemaException: " 
            << e.what() << std::endl;
      commitSession();
      return false;
   } catch( const std::exception& e ) {
      msg() << "CTPFilesLoader >> Standard C++ exception: " << e.what() << std::endl;
      commitSession();
      return false; 
   } catch( ... ) {
      msg() << "CTPFilesLoader >> unknown C++ exception" << std::endl;
      commitSession();
      return false; 
   }
}

bool TrigConf::CTPFilesLoader::loadCtpinData( CTPFiles& ctpFiles) {
   msg() << "CTPFilesLoader : start loading CTPIN data" << std::endl;
   try {
      startSession();
      // first check if we need to obtain the CTP files ID, or if it
      // was passed in
      if(ctpFiles.id() <= 0 || ctpFiles.smxId() <= 0) {
         int l1id = ctpFiles.lvl1MasterTableId();
         int menuid = getMenuIdFromMaster(l1id);
         if(ctpFiles.id() <= 0) {
            long filesid = getFilesIdFromMenu(menuid);
            ctpFiles.setId(filesid);
         }
         if(ctpFiles.smxId() <= 0) {
            long id = getSmxIdFromMenu(menuid);
            ctpFiles.setSmxId(id);
         }
      }

      { // restrict scope to re-use variables
         coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_CTP_FILES");
         coral::IQuery* query = table.newQuery();
         query->setRowCacheSize( 13 );

         //Binding
         coral::AttributeList emptyBindVariableList0;
         std::string condition = "L1CF_ID = :fId";
         emptyBindVariableList0.extend<long>("fId");
         emptyBindVariableList0[0].data<long>() = ctpFiles.id();
         query->setCondition( condition, emptyBindVariableList0 );
	    
         coral::AttributeList attList;
         attList.extend<std::string>( "L1CF_NAME" );
         attList.extend<int>( "L1CF_VERSION" );
         attList.extend<std::string>( "L1CF_MON_SEL_SLOT7" );
         attList.extend<std::string>( "L1CF_MON_SEL_SLOT8" );
         attList.extend<std::string>( "L1CF_MON_SEL_SLOT9" );
         attList.extend<std::string>( "L1CF_MON_DEC_SLOT7" );
         attList.extend<std::string>( "L1CF_MON_DEC_SLOT8" );
         attList.extend<std::string>( "L1CF_MON_DEC_SLOT9" );
         query->defineOutput(attList);
         query->addToOutputList( "L1CF_NAME" );
         query->addToOutputList( "L1CF_VERSION" );
         query->addToOutputList( "L1CF_MON_SEL_SLOT7" );
         query->addToOutputList( "L1CF_MON_SEL_SLOT8" );
         query->addToOutputList( "L1CF_MON_SEL_SLOT9" );
         query->addToOutputList( "L1CF_MON_DEC_SLOT7" );
         query->addToOutputList( "L1CF_MON_DEC_SLOT8" );
         query->addToOutputList( "L1CF_MON_DEC_SLOT9" );

         coral::ICursor& cursor = query->execute();
         if ( ! cursor.next() ) {
            msg() << "CTPFilesLoader >> No such L1_CTP_Files exists " << ctpFiles.id() << std::endl;
            delete query;
            commitSession();
            throw std::runtime_error( "CTPFilesLoader >> CTPFiles not available" );
         }

         const coral::AttributeList& row = cursor.currentRow();
         std::string name = row["L1CF_NAME"].data<std::string>();
         int version = row["L1CF_VERSION"].data<int>();
         std::string strMonSel7 = row["L1CF_MON_SEL_SLOT7"].data<std::string>();
         std::string strMonSel8 = row["L1CF_MON_SEL_SLOT8"].data<std::string>();
         std::string strMonSel9 = row["L1CF_MON_SEL_SLOT9"].data<std::string>();
         std::string strMonDec7 = row["L1CF_MON_DEC_SLOT7"].data<std::string>();
         std::string strMonDec8 = row["L1CF_MON_DEC_SLOT8"].data<std::string>();
         std::string strMonDec9 = row["L1CF_MON_DEC_SLOT9"].data<std::string>();
	
         if ( cursor.next() ) {
            msg() << "CTPFilesLoader >> More than one GetFile exists "   << ctpFiles.id() << std::endl;
            delete query;
            commitSession();
            throw std::runtime_error( "CTPFilesLoader >>  CTPFiles not available" );
         }

         std::vector<u_int> monDecSlot7(CTPFiles::ALL_CTPINMONDEC_SIZE);
         parseHexString(monDecSlot7,strMonDec7);
         std::vector<u_int> monDecSlot8(CTPFiles::ALL_CTPINMONDEC_SIZE);
         parseHexString(monDecSlot8,strMonDec8);
         std::vector<u_int> monDecSlot9(CTPFiles::ALL_CTPINMONDEC_SIZE);
         parseHexString(monDecSlot9,strMonDec9);
		
         std::vector<u_int> monSelSlot7(CTPFiles::ALL_CTPINMONSEL_SIZE);
         parseHexString(monSelSlot7,strMonSel7);
         std::vector<u_int> monSelSlot8(CTPFiles::ALL_CTPINMONSEL_SIZE);
         parseHexString(monSelSlot8,strMonSel8);
         std::vector<u_int> monSelSlot9(CTPFiles::ALL_CTPINMONSEL_SIZE);
         parseHexString(monSelSlot9,strMonSel9);

         // Fill the object with data
         ctpFiles.setName( name );
         ctpFiles.setVersion( version );
         ctpFiles.setCtpinMonDecoderSlot7(monDecSlot7);
         ctpFiles.setCtpinMonDecoderSlot8(monDecSlot8);
         ctpFiles.setCtpinMonDecoderSlot9(monDecSlot9);
         ctpFiles.setCtpinMonSelectorSlot7(monSelSlot7);
         ctpFiles.setCtpinMonSelectorSlot8(monSelSlot8);
         ctpFiles.setCtpinMonSelectorSlot9(monSelSlot9);
         delete query;
      }
	 
      {
         coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_CTP_SMX");
         coral::IQuery* query = table.newQuery();
         query->setRowCacheSize( 13 );

         //Binding
         coral::AttributeList emptyBindVariableList0;
         std::string condition = "L1SMX_ID = :sId";
         emptyBindVariableList0.extend<long>("sId");
         emptyBindVariableList0[0].data<long>() = ctpFiles.smxId();
         query->setCondition( condition, emptyBindVariableList0 );
	    
         coral::AttributeList attList;
         attList.extend<std::string>( "L1SMX_NAME" );
         attList.extend<std::string>( "L1SMX_OUTPUT" );
         attList.extend<std::string>( "L1SMX_VHDL_SLOT7" );
         attList.extend<std::string>( "L1SMX_VHDL_SLOT8" );
         attList.extend<std::string>( "L1SMX_VHDL_SLOT9" );
         attList.extend<std::string>( "L1SMX_SVFI_SLOT7" );
         attList.extend<std::string>( "L1SMX_SVFI_SLOT8" );
         attList.extend<std::string>( "L1SMX_SVFI_SLOT9" );
         query->defineOutput(attList);
         query->addToOutputList( "L1SMX_NAME" );
         query->addToOutputList( "L1SMX_OUTPUT" );

         query->addToOutputList( "L1SMX_VHDL_SLOT7" );
         query->addToOutputList( "L1SMX_VHDL_SLOT8" );
         query->addToOutputList( "L1SMX_VHDL_SLOT9" );
         query->addToOutputList( "L1SMX_SVFI_SLOT7" );
         query->addToOutputList( "L1SMX_SVFI_SLOT8" );
         query->addToOutputList( "L1SMX_SVFI_SLOT9" );


         coral::ICursor& cursor = query->execute();
         if ( ! cursor.next() ) {
            msg() << "CTPFilesLoader >> No such L1_CTP_SMX entry " << ctpFiles.smxId() << " exists" << std::endl;
            delete query;
            commitSession();
            throw std::runtime_error( "CTPFilesLoader >> CTPFiles not available" );
         }

         const coral::AttributeList& row = cursor.currentRow();
         ctpFiles.setSmxName(row["L1SMX_NAME"].data<std::string>());
         ctpFiles.setCtpinSmxOutput(row["L1SMX_OUTPUT"].data<std::string>());

         ctpFiles.setCtpinSmxVhdlSlot7(row["L1SMX_VHDL_SLOT7"].data<std::string>());
         ctpFiles.setCtpinSmxVhdlSlot8(row["L1SMX_VHDL_SLOT8"].data<std::string>());
         ctpFiles.setCtpinSmxVhdlSlot9(row["L1SMX_VHDL_SLOT9"].data<std::string>());
         ctpFiles.setCtpinSmxSvfiSlot7(row["L1SMX_SVFI_SLOT7"].data<std::string>());
         ctpFiles.setCtpinSmxSvfiSlot8(row["L1SMX_SVFI_SLOT8"].data<std::string>());
         ctpFiles.setCtpinSmxSvfiSlot9(row["L1SMX_SVFI_SLOT9"].data<std::string>());
         delete query;
      }
	 
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "CTPFilesLoader >> SchemaException: " 
            << e.what() << std::endl;
      commitSession();
      return false;
   } catch( const std::exception& e ) {
      msg() << "CTPFilesLoader >> Standard C++ exception: " << e.what() << std::endl;
      commitSession();
      return false; 
   } catch( ... ) {
      msg() << "CTPFilesLoader >> unknown C++ exception" << std::endl;
      commitSession();
      return false; 
   }
}

bool TrigConf::CTPFilesLoader::loadCtpmonData( CTPFiles& ctpFiles) {
   msg() << "CTPFilesLoader : start loading CTPMON data" << std::endl;
   try {
      startSession();
      // first check if we need to obtain the CTP files ID, or if it
      // was passed in
      if(ctpFiles.id() <= 0) {
         int l1id = ctpFiles.lvl1MasterTableId();
         int menuid = getMenuIdFromMaster(l1id);
         long filesid = getFilesIdFromMenu(menuid);
         ctpFiles.setId(filesid);
      }

      coral::ITable& table = m_session.nominalSchema().tableHandle( "L1_CTP_FILES");
      coral::IQuery* query = table.newQuery();
      query->setRowCacheSize( 13 );

      //Binding
      coral::AttributeList emptyBindVariableList0;
      std::string condition = "L1CF_ID = :fId";
      emptyBindVariableList0.extend<long>("fId");
      emptyBindVariableList0[0].data<long>() = ctpFiles.id();
      query->setCondition( condition, emptyBindVariableList0 );
	    
      coral::AttributeList attList;
      attList.extend<std::string>( "L1CF_NAME" );
      attList.extend<int>( "L1CF_VERSION" );
      attList.extend<std::string>( "L1CF_MON_SEL_CTPMON" );
      attList.extend<std::string>( "L1CF_MON_DEC_CTPMON" );
      query->defineOutput(attList);
      query->addToOutputList( "L1CF_NAME" );
      query->addToOutputList( "L1CF_VERSION" );
      query->addToOutputList( "L1CF_MON_SEL_CTPMON" );
      query->addToOutputList( "L1CF_MON_DEC_CTPMON" );

      coral::ICursor& cursor = query->execute();
      if ( ! cursor.next() ) {
         msg() << "CTPFilesLoader >> No such L1_CTP_Files exists " << ctpFiles.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >> CTPFiles not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();

      std::string name = row["L1CF_NAME"].data<std::string>();
      int version = row["L1CF_VERSION"].data<int>();
      std::string ctpmonSelector_str = row["L1CF_MON_SEL_CTPMON"].data<std::string>();
      std::string ctpmonDecoder_str = row["L1CF_MON_DEC_CTPMON"].data<std::string>();
	
      if ( cursor.next() ) {
         msg() << "CTPFilesLoader >> More than one GetFile exists "   << ctpFiles.id() << std::endl;
         delete query;
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >>  CTPFiles not available" );
      }
	 
      std::vector<u_int> monDecVec(CTPFiles::ALL_CTPMONDECODER_SIZE);
      parseHexString(monDecVec,ctpmonDecoder_str);

      //Selector for CTPMON - note the different format requires
      //different parsing here!
      std::vector<u_int> monSelVec(CTPFiles::ALL_CTPMONSELECTOR_SIZE);
      std::stringstream stream(ctpmonSelector_str);
      std::string val;
      u_int index = 0;
      u_int data = 0;
      while(stream >> val) {
         // get rid of possible '0x'
         std::string::size_type loc = val.find("0x");
         if(loc != std::string::npos) val.replace(loc,2,"");
         if(!convert_string<u_int>(data, val)) {
            msg() << "CTPFilesLoader>> Conversion of CTPMONSELECTOR no" << index << " failed!" << std::endl;
            commitSession();
            throw std::runtime_error( "CTPFilesLoader >> CTPMONSELECTOR conversion failed." );	  
         }
         if( index >=  CTPFiles::ALL_CTPMONSELECTOR_SIZE) {
            msg() << "CTPFilesLoader>> CTPMON selector word no" 
                  << index << " out of bounds" << std::endl;
            commitSession();
            throw std::runtime_error( "CTPFilesLoader >> CTPMONSELECTOR conversion failed." );
         }
         monSelVec[index] = data;
         ++index;
      }
	
      // Fill the object with data
      ctpFiles.setName( name );
      ctpFiles.setVersion( version );
      ctpFiles.setCtpmonDecoder(monDecVec);
      ctpFiles.setCtpmonSelector(monSelVec); 

      delete query;
      commitSession();
      return true;
   } catch( const coral::SchemaException& e ) {
      msg() << "CTPFilesLoader >> SchemaException: " 
            << e.what() << std::endl;
      commitSession();
      return false;
   } catch( const std::exception& e ) {
      msg() << "CTPFilesLoader >> Standard C++ exception: " << e.what() << std::endl;
      commitSession();
      return false; 
   } catch( ... ) {
      msg() << "CTPFilesLoader >> unknown C++ exception" << std::endl;
      commitSession();
      return false; 
   }
}

void TrigConf::CTPFilesLoader::parseHexString(std::vector<u_int>& vec,const std::string& str) {
   u_int data;
   u_int significantBits = 8;
   u_int preFix = 2;
   u_int postFix = 1;
   u_int wordsize = (preFix + significantBits + postFix);	    
   for(u_int i=0; i < vec.size(); ++i) {
      if(!convert_hex_string<u_int>(data, str.substr(i*wordsize+preFix,significantBits))) {
         msg() << "CTPFilesLoader>> Conversion no " << i << " failed!" << std::endl;
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >> Conversion failed." );	 
      }
      vec[i] = data;
   }
}

