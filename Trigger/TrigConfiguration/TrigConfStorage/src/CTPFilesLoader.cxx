/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "./DBHelper.h"

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

using namespace std;

bool TrigConf::CTPFilesLoader::load( CTPFiles& data) {
   bool retVal = true;
   triggerDBSchemaVersion();
   if(isRun1()) {
      data.setFileSizes(1);
   } else {
      data.setFileSizes(2);
   }

   if( data.loadCtpcoreFiles() ) retVal &= loadCtpcoreData(data);
   if( data.loadCtpinFiles()   ) retVal &= loadCtpinData(data);
   if( data.loadCtpmonFiles()  ) retVal &= loadCtpmonData(data);
   return retVal;
}

// could make a templated function to be used by everybody for all
// fields of the master table...
int TrigConf::CTPFilesLoader::getMenuIdFromMaster(int mid) {

   unique_ptr<coral::IQuery> query( m_session.nominalSchema().tableHandle( "L1_MASTER_TABLE").newQuery() );
   query->setRowCacheSize( 5 );

   //Binding
   coral::AttributeList emptyBindVariableList0;
   std::string condition = "L1MT_ID = :mtId";
   emptyBindVariableList0.extend<int>("mtId");
   emptyBindVariableList0[0].data<int>() = mid;
   query->setCondition( condition, emptyBindVariableList0 );

   coral::AttributeList attList;
   attList.extend<int>( "L1MT_TRIGGER_MENU_ID" );
   fillQuery(query.get(),attList);

   coral::ICursor& cursor = query->execute();
   if ( ! cursor.next() ) {
      TRG_MSG_ERROR("No such Master_Table exists " << mid);
      commitSession();
      throw std::runtime_error( "CTPFilesLoader" );
   }
   const coral::AttributeList& row = cursor.currentRow();
   int menuid = row["L1MT_TRIGGER_MENU_ID"].data<int>();	
   if ( cursor.next() ) {
      TRG_MSG_ERROR("More than one Master_Table exists " << mid);
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
      TRG_MSG_ERROR("No such Trigger_Menu exists " << mid);
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
      TRG_MSG_ERROR("No such Trigger_Menu exists " << mid);
      delete query;
      commitSession();
      throw std::runtime_error( "CTPFilesLoader" );
   }
   const coral::AttributeList& row = cursor.currentRow();
   return static_cast<long>(row["L1TM_CTP_SMX_ID"].data<int>());
}



bool
TrigConf::CTPFilesLoader::loadCtpcoreData( CTPFiles& ctpFiles) {

   TRG_MSG_INFO("start loading CTPCORE data");
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

      unique_ptr<coral::IQuery> query( m_session.nominalSchema().tableHandle( "L1_CTP_FILES").newQuery() );
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
      fillQuery(query.get(),attList);

      coral::ICursor& cursor = query->execute();

      if ( ! cursor.next() ) {
         TRG_MSG_ERROR("No such L1_CTP_Files exists " << ctpFiles.id());
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >> CTPFiles not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();
      std::string name = row["L1CF_NAME"].data<std::string>();
      int version = row["L1CF_VERSION"].data<int>();
      std::string ctpcoreLUT_str = row["L1CF_LUT"].data<std::string>();
      std::string ctpcoreCAM_str = row["L1CF_CAM"].data<std::string>();

      if ( cursor.next() ) {
         TRG_MSG_ERROR("More than one GetFile exists "   << ctpFiles.id());
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >>  CTPFiles not available" );
      }

      std::vector<u_int> lutVec(CTPFiles::ALL_CTPCORELUT_SIZE);
      TRG_MSG_INFO("Loading LUT of size " << lutVec.size());
      parseHexString(lutVec, ctpcoreLUT_str, true);

      std::vector<u_int> camVec(CTPFiles::ALL_CTPCORECAM_SIZE);
      TRG_MSG_INFO("Loading CAM of size " << camVec.size());
      parseHexString(camVec,ctpcoreCAM_str);

      // Fill the object with data
      ctpFiles.setName( name );
      ctpFiles.setVersion( version );
      ctpFiles.setCtpcoreLUT(lutVec);
      ctpFiles.setCtpcoreCAM(camVec);

      commitSession();
   } catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("coral::Exception: " << e.what());
      commitSession();
      throw;
   }
   return true;
}

bool TrigConf::CTPFilesLoader::loadCtpinData( CTPFiles& ctpFiles) {
   TRG_MSG_INFO("start loading CTPIN data");
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
         unique_ptr<coral::IQuery> query( m_session.nominalSchema().tableHandle( "L1_CTP_FILES").newQuery() );
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
         fillQuery(query.get(),attList);

         coral::ICursor& cursor = query->execute();
         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("No such L1_CTP_Files exists " << ctpFiles.id());
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
            TRG_MSG_ERROR("More than one GetFile exists "   << ctpFiles.id());
            commitSession();
            throw std::runtime_error( "CTPFilesLoader >>  CTPFiles not available" );
         }

         std::vector<u_int> monDecSlot7(CTPFiles::ALL_CTPINMONDEC_SIZE);
         TRG_MSG_INFO("Loading MON Slot7 of size " << monDecSlot7.size());
         parseHexString(monDecSlot7,strMonDec7);
         std::vector<u_int> monDecSlot8(CTPFiles::ALL_CTPINMONDEC_SIZE);
         TRG_MSG_INFO("Loading MON Slot8 of size " << monDecSlot8.size());
         parseHexString(monDecSlot8,strMonDec8);
         std::vector<u_int> monDecSlot9(CTPFiles::ALL_CTPINMONDEC_SIZE);
         TRG_MSG_INFO("Loading MON Slot9 of size " << monDecSlot9.size());
         parseHexString(monDecSlot9,strMonDec9);
		
         std::vector<u_int> monSelSlot7(CTPFiles::ALL_CTPINMONSEL_SIZE);
         TRG_MSG_INFO("Loading MON SEL Slot7 of size " << monSelSlot7.size());
         parseHexString(monSelSlot7,strMonSel7);
         std::vector<u_int> monSelSlot8(CTPFiles::ALL_CTPINMONSEL_SIZE);
         TRG_MSG_INFO("Loading MON SEL Slot8 of size " << monSelSlot8.size());
         parseHexString(monSelSlot8,strMonSel8);
         std::vector<u_int> monSelSlot9(CTPFiles::ALL_CTPINMONSEL_SIZE);
         TRG_MSG_INFO("Loading MON SEL Slot9 of size " << monSelSlot9.size());
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
      }
	 
      {
         unique_ptr<coral::IQuery> query( m_session.nominalSchema().tableHandle( "L1_CTP_SMX").newQuery() );
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
         fillQuery(query.get(),attList);

         coral::ICursor& cursor = query->execute();
         if ( ! cursor.next() ) {
            TRG_MSG_ERROR("No such L1_CTP_SMX entry " << ctpFiles.smxId() << " exists");
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
      }
	 
      commitSession();
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("coral::Exception: " << e.what());
      commitSession();
      throw;
   }
   return true;
}



bool TrigConf::CTPFilesLoader::loadCtpmonData( CTPFiles& ctpFiles) {
   TRG_MSG_INFO("start loading CTPMON data");
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

      unique_ptr<coral::IQuery> query( m_session.nominalSchema().tableHandle( "L1_CTP_FILES").newQuery() );
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
      fillQuery(query.get(),attList);


      coral::ICursor& cursor = query->execute();
      if ( ! cursor.next() ) {
         TRG_MSG_ERROR("No such L1_CTP_Files exists " << ctpFiles.id());
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >> CTPFiles not available" );
      }

      const coral::AttributeList& row = cursor.currentRow();

      std::string name = row["L1CF_NAME"].data<std::string>();
      int version = row["L1CF_VERSION"].data<int>();
      std::string ctpmonSelector_str = row["L1CF_MON_SEL_CTPMON"].data<std::string>();
      std::string ctpmonDecoder_str = row["L1CF_MON_DEC_CTPMON"].data<std::string>();
	
      if ( cursor.next() ) {
         TRG_MSG_ERROR("More than one GetFile exists "   << ctpFiles.id());
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >>  CTPFiles not available" );
      }
	 
      std::vector<u_int> monDecVec(CTPFiles::ALL_CTPMONDECODER_SIZE);
      TRG_MSG_INFO("Loading MON DEC vec of size " << monDecVec.size());
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
            TRG_MSG_ERROR("Conversion of CTPMONSELECTOR no" << index << " failed!");
            commitSession();
            throw std::runtime_error( "CTPFilesLoader >> CTPMONSELECTOR conversion failed." );	  
         }
         if( index >=  CTPFiles::ALL_CTPMONSELECTOR_SIZE) {
            TRG_MSG_ERROR("CTPMON selector word no" << index << " out of bounds");
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

      commitSession();
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("coral::Exception: " << e.what());
      commitSession();
      throw;
   }
   return true;
}


void TrigConf::CTPFilesLoader::parseHexString(std::vector<u_int>& vec,const std::string& str, bool verbose) {
   u_int data;
   const static u_int significantBits = 8;
   const static u_int preFix = 2;
   const static u_int postFix = 1;
   const static u_int wordsize = (preFix + significantBits + postFix);

   if(str.size() != 0) {
      if(str.size() != wordsize * vec.size()) {
         TRG_MSG_ERROR("File content from DB of size " << str.size() << ", but expects " << wordsize * vec.size() << " [11 * " << vec.size() << "]");
         throw std::runtime_error( "CTPFilesLoader: file of unexpected size" );
      }
   } else {
      TRG_MSG_INFO("DB File content is 0, which is expected for MC, Reprocessing, and ATN databases");
      return;
   }

   for(u_int i=0; i < vec.size(); ++i) {
      if( ! convert_hex_string<u_int>(data, str.substr(i*wordsize+preFix,significantBits))) {
         TRG_MSG_ERROR("Conversion no " << i << " failed!");
         commitSession();
         throw std::runtime_error( "CTPFilesLoader >> Conversion failed." );	 
      }
      if(verbose) {
         //cout << "joerg " << str.substr(i*wordsize+preFix,significantBits) << "   ----->   " << i << "  " << data << endl;
      }
      vec[i] = data;
   }
}

