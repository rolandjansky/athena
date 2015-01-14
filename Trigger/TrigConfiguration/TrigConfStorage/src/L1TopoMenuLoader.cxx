/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoMenuLoader.h"

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "DBHelper.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoConfig/L1TopoConfigAlg.h"

#include <iostream>
#include <stdexcept>
#include <typeinfo>

using namespace std;
bool TrigConf::L1TopoMenuLoader::load( TXC::L1TopoMenu& tcaTarget ) {
   try {
     unsigned int schema = triggerDBSchemaVersion();
     TRG_MSG_DEBUG("L1TopoMenuLoader Run " << (isRun1()? 1 : 2) << ", schema version " << schema);
     if(isRun1()) {
        TRG_MSG_INFO("Not loading L1 TopoMenu from a run 1 database");
        return true;
     }
     TRG_MSG_INFO("Loading L1 TopoMenu");
     startSession();
     loadTopoAlgos(tcaTarget);
     commitSession();
   } catch( const std::exception& e ) {
      TRG_MSG_ERROR("L1TopoMenuLoader::load >> Standard C++ exception: " << e.what());
   }
   return 1;
}

bool TrigConf::L1TopoMenuLoader::loadTopoAlgos( TXC::L1TopoMenu& tcaTarget) {

   TRG_MSG_DEBUG("Calling loadTopoAlgos");
      
   try {
      startSession();
      
      unique_ptr<coral::IQuery> query0(m_session.nominalSchema().newQuery());
      query0->addToTableList("SUPER_MASTER_TABLE","SM");
      query0->addToTableList("TOPO_MASTER_TABLE","TMT");
      query0->addToTableList("TOPO_TRIGGER_MENU","TTM");
      query0->addToTableList("TTM_TO_TA","TTM2TA");
      query0->addToTableList("TOPO_ALGO","TA");
      query0->setRowCacheSize(5);
	
      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<unsigned int>("tmtId");
      std::string cond = "TMT_ID = :tmtId";
      bindList0[0].data<unsigned int>() = tcaTarget.getSMK();

      string theCondition0 = "";
      theCondition0 += string(" SM.SMT_ID = :tmtId");
      theCondition0 += string(" AND TMT.TMT_ID = SM.SMT_TOPO_MASTER_TABLE_ID "); 
      theCondition0 += string(" AND TTM.TTM_ID = TMT.TMT_TRIGGER_MENU_ID ");
      theCondition0 += string(" AND TTM2TA.TTM2TA_MENU_ID = TTM.TTM_ID "); 
      theCondition0 += string(" AND TA.TA_ID = TTM2TA.TTM2TA_ALGO_ID "); 
      query0->setCondition( theCondition0, bindList0 );

      coral::AttributeList attList0;
      attList0.extend<int>( "TMT.TMT_TRIGGER_MENU_ID" );
      attList0.extend<int>( "TMT.TMT_VERSION" );
      attList0.extend<string>( "TTM.TTM_NAME" );
      attList0.extend<int>( "TTM.TTM_VERSION" );
      attList0.extend<int>( "TTM.TTM_CTPLINK_ID" );
      attList0.extend<int>( "TA.TA_ID" );
      attList0.extend<string>( "TA.TA_NAME" );
      attList0.extend<string>( "TA.TA_OUTPUT" );
      attList0.extend<string>( "TA.TA_TYPE" );
      attList0.extend<int>( "TA.TA_BITS" );
      attList0.extend<string>( "TA.TA_SORT_DECI" );
      attList0.extend<int>( "TA.TA_ALGO_ID" );
      query0->defineOutput(attList0);     
      fillQuery(query0.get(), attList0);
      TRG_MSG_DEBUG("Executing loadTopoAlgos query ");
      coral::ICursor& cursor0 = query0->execute();

      int ctplinkid = 0;
      while(cursor0.next()){
         const coral::AttributeList& row0 = cursor0.currentRow();
         int ta_id = row0["TA.TA_ID"].data<int>();
         string ta_name = row0["TA.TA_NAME"].data<string>();
         string ta_output = row0["TA.TA_OUTPUT"].data<string>();
         string ta_type = row0["TA.TA_TYPE"].data<string>();
         int ta_bits = row0["TA.TA_BITS"].data<int>();
         string ta_sort_deci = row0["TA.TA_SORT_DECI"].data<string>();
         int ta_algo_id = row0["TA.TA_ALGO_ID"].data<int>();
         ctplinkid = row0["TTM.TTM_CTPLINK_ID"].data<int>();
         tcaTarget.setName(row0["TTM.TTM_NAME"].data<string>());
         tcaTarget.setVersion(std::to_string(ta_id));


         TXC::L1TopoConfigAlg alg(ta_name,ta_type);
         if(ta_sort_deci=="Decision") alg.setAlgKind(TXC::L1TopoConfigAlg::DECISION);
         else if(ta_sort_deci=="Sort") alg.setAlgKind(TXC::L1TopoConfigAlg::SORT);
         else alg.setAlgKind(TXC::L1TopoConfigAlg::NONE);
         alg.setAlgOutput(ta_output);
         alg.setAlgoID(ta_algo_id);
         loadAlgInput(alg,ta_id);
         loadAlgOutput(alg,ta_id,ta_bits);
         loadAlgRegister(alg, ta_id);
         loadAlgFixed(alg, ta_id);
         tcaTarget.addAlgorithm(alg);
      }

      loadTopoConfig(tcaTarget);
      loadOutputList(tcaTarget,ctplinkid);
   } catch( const std::exception& e ) {
      TRG_MSG_ERROR("loadTopoAlgos >> Standard C++ exception: " << e.what());
      commitSession();
      return false; 
   }
   return true;
}

bool TrigConf::L1TopoMenuLoader::loadAlgInput( TXC::L1TopoConfigAlg& tcaTarget, const int& ta_id ) {
   try {
      TRG_MSG_DEBUG("Calling loadAlgInput");
      startSession();
      
      unique_ptr<coral::IQuery> query0(m_session.nominalSchema().newQuery());
      query0->addToTableList("TA_TO_TI","TA2TI");
      query0->addToTableList("TOPO_ALGO_INPUT","TAI");
      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<int>("taId");
      bindList0[0].data<int>() = ta_id;

      string theCondition0 = "";
      theCondition0 += string(" TA2TI.TA2TI_ALGO_ID = :taId" );
      theCondition0 += string(" AND TA2TI.TA2TI_INPUT_ID = TAI.TAI_ID"); 
      query0->setCondition( theCondition0, bindList0 );

      coral::AttributeList attList0;
      attList0.extend<string>( "TAI.TAI_NAME" );
      attList0.extend<string>( "TAI.TAI_VALUE" );
      attList0.extend<int>( "TAI.TAI_POSITION" );
      query0->defineOutput(attList0);     
      fillQuery(query0.get(), attList0);
      //std::cout << "Executing loadAlgInput query " << std::endl;
      coral::ICursor& cursor0 = query0->execute();
      while(cursor0.next()){
         const coral::AttributeList& row0 = cursor0.currentRow();
         //std::cout << "loadAlgInput my row " << row0 << std::endl;
         string tai_name = row0["TAI.TAI_NAME"].data<string>();
         string tai_value = row0["TAI.TAI_VALUE"].data<string>();
         int tai_pos = row0["TAI.TAI_POSITION"].data<int>();
         tcaTarget.addInput(tai_name,tai_value,tai_pos);
     }
   } catch (const exception& e){
      TRG_MSG_ERROR("loadAlgInput >> Standard C++ exception: " << e.what());
     return false;
   }
   return true;
}

bool TrigConf::L1TopoMenuLoader::loadAlgRegister( TXC::L1TopoConfigAlg& tcaTarget, const int& ta_id ) {
   try {
      TRG_MSG_DEBUG("Calling loadAlgRegister");
      startSession();
      
      unique_ptr<coral::IQuery> query0(m_session.nominalSchema().newQuery());
      query0->addToTableList("TA_TO_TP","TA2TP");
      query0->addToTableList("TOPO_PARAMETER","TP");
      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<int>("taId");
      bindList0[0].data<int>() = ta_id;

      string theCondition0 = "";
      theCondition0 += string(" TA2TP.TA2TP_ALGO_ID = :taId" );
      theCondition0 += string(" AND TA2TP.TA2TP_PARAM_ID = TP.TP_ID"); 
      query0->setCondition( theCondition0, bindList0 );

      coral::AttributeList attList0;
      attList0.extend<string>( "TP.TP_NAME" );
      attList0.extend<int>( "TP.TP_VALUE" );
      attList0.extend<int>( "TP.TP_POSITION" );
      attList0.extend<int>( "TP.TP_SELECTION" );
      query0->defineOutput(attList0);     
      fillQuery(query0.get(), attList0);
      //std::cout << "Executing loadAlgRegister query " << std::endl;
      coral::ICursor& cursor0 = query0->execute();
      while(cursor0.next()){
         const coral::AttributeList& row0 = cursor0.currentRow();
         //std::cout << "loadAlgRegister my row " << row0 << std::endl;
         string tp_name = row0["TP.TP_NAME"].data<string>();
         int tp_value = row0["TP.TP_VALUE"].data<int>();
         int tp_pos = row0["TP.TP_POSITION"].data<int>();
         int tp_sel = row0["TP.TP_SELECTION"].data<int>();
         tcaTarget.addParameter(tp_name,to_string(tp_value),tp_pos, tp_sel);
     }
   } catch (const exception& e){
     TRG_MSG_ERROR("loadAlgRegister >> Standard C++ exception: " << e.what());
     return false;
   }
   return true;
}

bool TrigConf::L1TopoMenuLoader::loadAlgOutput( TXC::L1TopoConfigAlg& tcaTarget, const int& ta_id, const int& ta_bits ) {
   try {
      TRG_MSG_DEBUG("Calling loadAlgOutput");
      startSession();
      unique_ptr<coral::IQuery> query0(m_session.nominalSchema().newQuery());
      query0->addToTableList("TA_TO_TO","TA2TO");
      query0->addToTableList("TOPO_ALGO_OUTPUT","TAO");
      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<int>("taId");
      bindList0[0].data<int>() = ta_id;

      string theCondition0 = "";
      theCondition0 += string(" TA2TO.TA2TO_ALGO_ID = :taId" );
      theCondition0 += string(" AND TA2TO.TA2TO_OUTPUT_ID = TAO.TAO_ID"); 
      query0->setCondition( theCondition0, bindList0 );

      coral::AttributeList attList0;
      attList0.extend<string>( "TAO.TAO_NAME" );
      attList0.extend<string>( "TAO.TAO_VALUE" );
      attList0.extend<string>( "TAO.TAO_BITNAME" );
      attList0.extend<int>( "TAO.TAO_SELECTION" );
      query0->defineOutput(attList0);     
      fillQuery(query0.get(), attList0);
      //std::cout << "Executing loadAlgOutput query " << std::endl;
      coral::ICursor& cursor0 = query0->execute();
      while(cursor0.next()){
         const coral::AttributeList& row0 = cursor0.currentRow();
         //std::cout << "loadAlgOutput my row " << row0 << std::endl;
         string tao_name = row0["TAO.TAO_NAME"].data<string>();
         string tao_value = row0["TAO.TAO_VALUE"].data<string>();
         string tao_bitname = row0["TAO.TAO_BITNAME"].data<string>();
         int tao_sel = row0["TAO.TAO_SELECTION"].data<int>();
         tcaTarget.addOutput(tao_name,tao_value,ta_bits,tao_bitname, tao_sel);
     }
   } catch (const exception& e){
     TRG_MSG_ERROR("loadAlgOutput >> Standard C++ exception: " << e.what());
     return false;
   }
   return true;
}

bool TrigConf::L1TopoMenuLoader::loadAlgFixed( TXC::L1TopoConfigAlg& tcaTarget, const int& ta_id ) {
   try {
      TRG_MSG_DEBUG("Calling loadAlgFixed");
      startSession();
      
      unique_ptr<coral::IQuery> query0(m_session.nominalSchema().newQuery());
      query0->addToTableList("TA_TO_TG","TA2TG");
      query0->addToTableList("TOPO_GENERIC","TG");
      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<int>("taId");
      bindList0[0].data<int>() = ta_id;

      string theCondition0 = "";
      theCondition0 += string(" TA2TG.TA2TG_ALGO_ID = :taId" );
      theCondition0 += string(" AND TA2TG.TA2TG_GENERIC_ID = TG.TG_ID"); 
      query0->setCondition( theCondition0, bindList0 );

      coral::AttributeList attList0;
      attList0.extend<string>( "TG.TG_NAME" );
      attList0.extend<string>( "TG.TG_VALUE" );
      query0->defineOutput(attList0);     
      fillQuery(query0.get(), attList0);
      //std::cout << "Executing loadAlgFiexed query " << std::endl;
      coral::ICursor& cursor0 = query0->execute();
      while(cursor0.next()){
         const coral::AttributeList& row0 = cursor0.currentRow();
         //std::cout << "loadAlgFixed my row " << row0 << std::endl;
         string tg_name = row0["TG.TG_NAME"].data<string>();
         string tg_value = row0["TG.TG_VALUE"].data<string>();
         tcaTarget.addFixedParameter(tg_name,tg_value);
     }
   } catch (const exception& e){
     TRG_MSG_ERROR("loadAlgFixed >> Standard C++ exception: " << e.what());
     return false;
   }
   return true;
}

bool TrigConf::L1TopoMenuLoader::loadOutputList( TXC::L1TopoMenu& tcaTarget, const int& ctplinkid) {
   try {
      TRG_MSG_DEBUG("Calling loadOutputList");
      startSession();
      
      unique_ptr<coral::IQuery> query0(m_session.nominalSchema().newQuery());
      query0->addToTableList("TOPO_OUTPUT_LIST","TOL");
      query0->addToTableList("TOPO_OUTPUT_LINK","TOLK");
      query0->addToTableList("TOPO_OUTPUT_LINE","TOLINE");
      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<int>("ctpLId");
      bindList0[0].data<int>() = ctplinkid;

      string theCondition0 = "";
      theCondition0 += string(" TOL.OL_ID = :ctpLId" );
      theCondition0 += string(" AND TOLK.TL_LINK_ID = TOL.OL_ID"); 
      theCondition0 += string(" AND TOLK.TL_OUTPUT_ID = TOLINE.TOL_ID"); 
      query0->setCondition( theCondition0, bindList0 );

      coral::AttributeList attList0;
      attList0.extend<int>( "TOLINE.TOL_ID" );
      attList0.extend<string>( "TOLINE.TOL_ALGO_NAME" );
      attList0.extend<string>( "TOLINE.TOL_TRIGGERLINE" );
      attList0.extend<unsigned int>( "TOLINE.TOL_ALGO_ID" );
      attList0.extend<unsigned int>( "TOLINE.TOL_MODULE" );
      attList0.extend<unsigned int>( "TOLINE.TOL_FPGA" );
      attList0.extend<unsigned int>( "TOLINE.TOL_FIRST_BIT" );
      attList0.extend<unsigned int>( "TOLINE.TOL_CLOCK" );
      query0->defineOutput(attList0);     
      fillQuery(query0.get(), attList0);
      //std::cout << "Executing loadAlgFiexed query " << std::endl;
      coral::ICursor& cursor0 = query0->execute();
      while(cursor0.next()){
         const coral::AttributeList& row0 = cursor0.currentRow();
         //std::cout << "loadOutputList my row " << row0 << std::endl;
         string tol_name = row0["TOLINE.TOL_ALGO_NAME"].data<string>();
         unsigned int tol_algoId = row0["TOLINE.TOL_ALGO_ID"].data<unsigned int>();
         unsigned int tol_module = row0["TOLINE.TOL_MODULE"].data<unsigned int>();
         unsigned int tol_fpga = row0["TOLINE.TOL_FPGA"].data<unsigned int>();
         unsigned int tol_clock = row0["TOLINE.TOL_CLOCK"].data<unsigned int>();
         unsigned int tol_firstbit = row0["TOLINE.TOL_FIRST_BIT"].data<unsigned int>();
         tcaTarget.addL1TopoXMLOutput(TXC::OutputListElement(tol_name,tol_algoId,tol_module,tol_fpga,tol_clock,tol_firstbit));
     }
     tcaTarget.setTriggerList();
   } catch (const exception& e){
     TRG_MSG_ERROR("loadOutputList >> Standard C++ exception: " << e.what());
     return false;
   }
   return true;
}

bool TrigConf::L1TopoMenuLoader::loadTopoConfig( TXC::L1TopoMenu& tcaTarget) {

   try {
      TRG_MSG_DEBUG("Calling loadTopoConfig");
      startSession();
      
      unique_ptr<coral::IQuery> query0(m_session.nominalSchema().newQuery());
      query0->addToTableList("SUPER_MASTER_TABLE","SM");
      query0->addToTableList("TOPO_MASTER_TABLE","TMT");
      query0->addToTableList("TOPO_TRIGGER_MENU","TTM");
      query0->addToTableList("TTM_TO_TC","TTM2TC");
      query0->addToTableList("TOPO_CONFIG","TC");
      query0->setRowCacheSize(5);
	
      //Bind list
      coral::AttributeList bindList0;
      bindList0.extend<unsigned int>("tmtId");
      std::string cond = "TMT_ID = :tmtId";
      bindList0[0].data<unsigned int>() = tcaTarget.getSMK();

      string theCondition0 = "";
      theCondition0 += string(" SM.SMT_ID = :tmtId");
      theCondition0 += string(" AND TMT.TMT_ID = SM.SMT_TOPO_MASTER_TABLE_ID "); 
      theCondition0 += string(" AND TTM.TTM_ID = TMT.TMT_TRIGGER_MENU_ID ");
      theCondition0 += string(" AND TTM2TC.TTM2TC_ID = TTM.TTM_ID ");
      theCondition0 += string(" AND TC.TC_ID = TTM2TC.TTM2TC_CONFIG_ID ");
      query0->setCondition( theCondition0, bindList0 );

      coral::AttributeList attList0;
      attList0.extend<string>( "TC.TC_NAME" ); 
      attList0.extend<string>( "TC.TC_VALUE" );
      query0->defineOutput(attList0);     
      fillQuery(query0.get(), attList0);
      coral::ICursor& cursor0 = query0->execute();

      TXC::L1TopoConfigGlobal* l1topoconfigglobal = new TXC::L1TopoConfigGlobal();
      while(cursor0.next()){
         const coral::AttributeList& row0 = cursor0.currentRow();

         string tc_name = row0["TC.TC_NAME"].data<string>();
         string tc_value = row0["TC.TC_VALUE"].data<string>();

         l1topoconfigglobal->addTopoConfigElement(tc_name,tc_value);
         tcaTarget.addL1TopoConfigGlobal(TXC::TopoConfigElement(tc_name,tc_value));
      }

   } catch( const std::exception& e ) {
      TRG_MSG_ERROR("loadTopoConfig >> Standard C++ exception: " << e.what());
      commitSession();
      return false; 
   }
   return true;
}

