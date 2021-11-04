/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./MenuLoader.h"
#include "./DBHelper.h"
#include "./TriggerThresholdLoader.h"
#include "./ThresholdConfigLoader.h"
#include "./ThresholdMonitorLoader.h"
#include "./LogicExpression.h"

#include "TrigConfStorage/StorageMgr.h"

#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/ThresholdMonitor.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/TIP.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <list>

using namespace std;

bool
TrigConf::MenuLoader::load( Menu& menu ) {

   if(menu.smk()<=0) return false;
   try {
      startSession();

      loadMenuAttributes(menu);

      loadCaloInfo(menu);

      loadThresholds(menu);

      loadItems(menu);

      loadMonitoring(menu);
   
      commitSession();
   }
   catch( const coral::Exception& e ) {
      TRG_MSG_ERROR("Caught coral exception: " << e.what());
      commitSession();
      throw;
   }
   catch( const std::exception& e ) {
      TRG_MSG_ERROR("Caught std exception: " << e.what());
      commitSession();
      throw;
   }
   return true;
} 


void
TrigConf::MenuLoader::loadItems(TrigConf::Menu& menu) {
   TRG_MSG_INFO("Loading CTP Items");

   // to later load internal triggers
   TriggerThresholdLoader& ttldr = dynamic_cast<TriggerThresholdLoader&>( (dynamic_cast<StorageMgr&>(m_storageMgr))
                                                                          .triggerThresholdLoader() );
   ttldr.setMenuId(menu.id());
   ttldr.setLoadCableInfo(false);

   // at this point the internal triggers are not found via the
   // TM->TT link, hence load them when needed, and cache them
   std::map<int,TriggerThreshold*> thresholdNotFoundCache;

   unique_ptr< coral::IQuery > query( m_session.nominalSchema().newQuery() );
   query->addToTableList ( "L1_TM_TO_TI",     "TM2TI" );
   query->addToTableList ( "L1_TRIGGER_ITEM", "TI"    );
   query->addToTableList ( "L1_TI_TO_TT",     "TI2TT" );

   // bind list
   coral::AttributeList bindList;
   bindList.extend<int>("menuId");
   bindList[0].data<int>() = menu.id();

   std::string theCondition = "";
   theCondition += std::string( " TM2TI.L1TM2TI_TRIGGER_MENU_ID = :menuId"        );
   theCondition += std::string( " AND TM2TI.L1TM2TI_TRIGGER_ITEM_ID = TI.L1TI_ID" );
   theCondition += std::string( " AND TI2TT.L1TI2TT_TRIGGER_ITEM_ID = TI.L1TI_ID" );

   query->setCondition( theCondition, bindList );

   // output data and types
   coral::AttributeList attList;
   attList.extend<int>        ( "TI.L1TI_ID"                         );
   attList.extend<std::string>( "TI.L1TI_NAME"                       );
   attList.extend<int>        ( "TI.L1TI_VERSION"                    );
   if(isRun2()) {
      attList.extend<int>     ( "TI.L1TI_PARTITION"                  );
      attList.extend<string>  ( "TI.L1TI_MONITOR"                    );
   }
   attList.extend<int>        ( "TI.L1TI_CTP_ID"                     );
   attList.extend<std::string>( "TI.L1TI_PRIORITY"                   );
   attList.extend<std::string>( "TI.L1TI_DEFINITION"                 );
   attList.extend<int>        ( "TI.L1TI_TRIGGER_TYPE"               );
   attList.extend<int>        ( "TI2TT.L1TI2TT_TRIGGER_THRESHOLD_ID" );
   attList.extend<int>        ( "TI2TT.L1TI2TT_POSITION"             );
   attList.extend<int>        ( "TI2TT.L1TI2TT_MULTIPLICITY"         );
   fillQuery(query.get(), attList);

   // the ordering
   std::string theOrder = "";
   // to get the correct number of items
   theOrder += "  TI.L1TI_CTP_ID ASC";
   // to get the correct TI definition  
   theOrder += ", TI2TT.L1TI2TT_POSITION ASC";
   query->addToOrderList( theOrder );

   //query->setRowCacheSize(1000);
   query->setRowCacheSize(500);

   query->setDistinct();

   coral::ICursor& cursor = query->execute();

   // lists to store <ctpId, vector> for later use
   vector<int> ctpIDs;
   map<int,vector<ThrInfo> > item_thrInfo;

   // create the items
   while (cursor.next()) {

      const coral::AttributeList& row = cursor.currentRow();
      int ctpid = row["TI.L1TI_CTP_ID"].data<int>();
      TriggerItem* item = menu.item(ctpid);
      if(! item) {
         item = new TriggerItem();
         item->setCtpId      (ctpid);
         item->setId         (row["TI.L1TI_ID"].data<int>());
         item->setName       (row["TI.L1TI_NAME"].data<string>());
         item->setVersion    (row["TI.L1TI_VERSION"].data<int>());
         if(isRun2()) {
            item->setPartition  (row["TI.L1TI_PARTITION"].data<int>());
            string mon = row["TI.L1TI_MONITOR"].data<string>();
            unsigned short monMask = 0;

            const short TBP = 0x1;
            const short TAP = 0x2;
            const short TAV = 0x4;
            
            vector<string> monLfHf;
            boost::split(monLfHf, mon, boost::is_any_of(":|"));
            //copy(monLfHf.begin(),monLfHf.end(), ostream_iterator<string>(cout,"\n") );
            
            if(monLfHf.size()==4 && monLfHf[0]=="LF" && monLfHf[2]=="HF" && monLfHf[1].size()==3 && monLfHf[3].size()==3) {
               // LF
               if( monLfHf[1][2]=='1' )  monMask |= TBP;
               if( monLfHf[1][1]=='1' )  monMask |= TAP;
               if( monLfHf[1][0]=='1' )  monMask |= TAV;
               // HF
               if( monLfHf[3][2]=='1' )  monMask |= TBP << 3;
               if( monLfHf[3][1]=='1' )  monMask |= TAP << 3;
               if( monLfHf[3][0]=='1' )  monMask |= TAV << 3;
            } else {
               // this is for the temporary solution
               if(mon.find("TBP") != string::npos) monMask |= TBP;
               if(mon.find("TAP") != string::npos) monMask |= TAP;
               if(mon.find("TAV") != string::npos) monMask |= TAV;
            }
            item->setMonitor( monMask );
         }

         string priority = row["TI.L1TI_PRIORITY"].data<string>();
         if(priority=="0" || priority=="HIGH") {
            item->setComplexDeadtime(0);
         } else if(priority=="1" || priority=="LOW") {
            item->setComplexDeadtime(1);
         }
         item->setDefinition (row["TI.L1TI_DEFINITION"].data<string>());
         item->setTriggerType(row["TI.L1TI_TRIGGER_TYPE"].data<int>());
         menu.addTriggerItem(item);
         if(verbose()>1)
            msg() << "MenuLoader:                       Created Item " << item->name() << " with CTPID " << item->ctpId() << endl;
         item_thrInfo[ctpid] = vector<ThrInfo>();
         ctpIDs.push_back(ctpid);
      }

      ThrInfo thr_info;
      thr_info.thrId = row["TI2TT.L1TI2TT_TRIGGER_THRESHOLD_ID"].data<int>();
      thr_info.thrPos = row["TI2TT.L1TI2TT_POSITION"].data<int>();
      thr_info.thrMult = row["TI2TT.L1TI2TT_MULTIPLICITY"].data<int>();
      thr_info.thr = menu.thresholdConfig().findTriggerThreshold(thr_info.thrId);
      if(thr_info.thr==0) {
         // load threshold (internal thresholds)
         thr_info.thr = new TriggerThreshold();
         thr_info.thr->setId(thr_info.thrId);
         if ( ! ttldr.load( *thr_info.thr ) ) {
            msg() << "MenuLoader:                       Error loading TriggerThreshold " << thr_info.thrId << endl;
            throw runtime_error( "MenuLoader::loadItems: error loading TriggerThreshold " );
         }
         menu.thresholdConfig().addTriggerThreshold(thr_info.thr);
      }
      item_thrInfo[ctpid].push_back(thr_info);
   }

   // build the item node tree
   for(int ctpid : ctpIDs) {
      TriggerItem* titem = menu.findTriggerItem( ctpid );
      if(verbose()>2) {
         msg() << "MenuLoader:                       Number of thresholds for item " << titem->name()
               << ": " << item_thrInfo[ctpid].size() << " - definition: " << titem->definition() << endl;
      }
      // construct the tree of TriggerItemNodes according to
      // definition and set the thresholds and multiplicities in each
      // leaf node
      titem->setTopNode( constructTree(titem->definition(), item_thrInfo[ctpid]) );
   }
}


TrigConf::TriggerItemNode*
TrigConf::MenuLoader::constructTree(const std::string& definition, const std::vector<ThrInfo>& thr_infos) {
   LogicExpression logic(msg());
   unsigned int n = logic.parse(definition);
   if ( n <= 0 ) {
      msg() << "Error parsing item definition : " << definition << std::endl;
      return 0;
   }
   unsigned int size = thr_infos.size();
   if (size != (n = logic.totalNumberOfElements()) ) {
      msg() << "Total number of elements are different: "
            << " input=" << size << ", parsed=" << n << endl;
   }

   logic.normalize();

   return constructTree(logic, thr_infos);
}


TrigConf::TriggerItemNode*
TrigConf::MenuLoader::constructTree(const LogicExpression& def, const std::vector<ThrInfo>& thr_infos) {
   TriggerItemNode* top_node=0;
   const std::vector<std::shared_ptr<LogicExpression>> & sub_logics = def.subLogics();

   switch (def.state()) {
   case LogicExpression::kELEMENT: {
      top_node = new TriggerItemNode(TriggerItemNode::OBJ);
      unsigned int pos = boost::lexical_cast<unsigned int,std::string>(def.element());
      // find all related information
      for(ThrInfo ti : thr_infos) {
         if(ti.thrPos==pos) {
            top_node->setPosition( pos );
            top_node->setMultiplicity( ti.thrMult );
            top_node->setTriggerThreshold( ti.thr );
            break;
         }
      }
      break;
   }
   case LogicExpression::kAND: 
   case LogicExpression::kOPEN:
      top_node = new TriggerItemNode(TriggerItemNode::AND);
      for(auto sl : sub_logics)
         top_node->addChild( constructTree(*sl, thr_infos) );
      break;
   case LogicExpression::kOR:
      if(sub_logics.size()>0) {
         top_node = new TriggerItemNode(TriggerItemNode::OR);
         for(auto sl : sub_logics)
            top_node->addChild( constructTree(*sl, thr_infos) );
      }
      break;
   case LogicExpression::kNOT:
      top_node = new TriggerItemNode(TriggerItemNode::NOT);
      for(auto sl : sub_logics)
         top_node->addChild( constructTree(*sl, thr_infos) );
      break;
   }

   return top_node;
}


/*********************************************
 * get monitoring counter
 *********************************************/
void
TrigConf::MenuLoader::loadMonitoring(TrigConf::Menu& menu) {
   if( ! (m_env == MenuLoader::ALL    ||
          m_env == MenuLoader::CTP    ||
          m_env == MenuLoader::CTPOnl ||
          m_env == MenuLoader::COOLL1) ) return;

   TRG_MSG_DEBUG("Load monitoring counter mapping ");

   unique_ptr< coral::IQuery > q(m_session.nominalSchema().newQuery());
   q->addToTableList ( "L1_TM_TO_TT_MON",      "TM2TTM" );
   q->addToTableList ( "L1_TRIGGER_THRESHOLD", "TT"     );
   q->addToTableList ( "L1_TM_TO_TT",          "TM2TT"  );

   //Bind list
   coral::AttributeList bindings;
   bindings.extend<int>("menuId");
   bindings[0].data<int>() = menu.id();

   string cond = "";
   cond += " TM2TTM.L1TM2TTM_TRIGGER_MENU_ID = :menuId";
   cond += " AND TM2TT.L1TM2TT_TRIGGER_MENU_ID = :menuId";
   cond += " AND TM2TTM.L1TM2TTM_TRIGGER_THRESHOLD_ID = TM2TT.L1TM2TT_TRIGGER_THRESHOLD_ID";
   cond += " AND TM2TT.L1TM2TT_TRIGGER_THRESHOLD_ID = TT.L1TT_ID";

   q->setCondition( cond, bindings );

   // Output data and types
   coral::AttributeList output;
   output.extend<std::string>( "TM2TTM.L1TM2TTM_NAME"                        );
   output.extend<int>        ( "TM2TTM.L1TM2TTM_TRIGGER_THRESHOLD_ID"        );
   output.extend<int>        ( "TM2TTM.L1TM2TTM_INTERNAL_COUNTER"            );
   output.extend<int>        ( "TM2TTM.L1TM2TTM_MULTIPLICITY"                );
   output.extend<long>       ( "TM2TTM.L1TM2TTM_BUNCH_GROUP_ID"              );
   output.extend<std::string>( "TM2TTM.L1TM2TTM_COUNTER_TYPE"                );
   output.extend<std::string>( "TT.L1TT_NAME"                                );
   output.extend<int>        ( "TT.L1TT_ACTIVE"                              );
   output.extend<std::string>( "TM2TT.L1TM2TT_CABLE_CTPIN"                   ); 
   output.extend<std::string>( "TM2TT.L1TM2TT_CABLE_CONNECTOR"               );
   output.extend<int>        ( "TM2TT.L1TM2TT_CABLE_START"                   ); 
   output.extend<int>        ( "TM2TT.L1TM2TT_CABLE_END"                     );
   fillQuery(q.get(), output);

   q->setRowCacheSize(500);

   coral::ICursor& cursor = q->execute();
   ThresholdMonitor* tm = 0;
 
   while (cursor.next()) {
      const coral::AttributeList& row = cursor.currentRow();
      tm = new ThresholdMonitor();

      tm->setName           ( row["TM2TTM.L1TM2TTM_NAME"].data<string>());
      tm->setThresholdId    ( row["TM2TTM.L1TM2TTM_TRIGGER_THRESHOLD_ID"].data<int>());
      tm->setInternalCounter( row["TM2TTM.L1TM2TTM_INTERNAL_COUNTER"].data<int>());
      tm->setMultiplicity   ( row["TM2TTM.L1TM2TTM_MULTIPLICITY"].data<int>());
      tm->setBunchGroupId   ( row["TM2TTM.L1TM2TTM_BUNCH_GROUP_ID"].data<long>()); 
      tm->setCounterType    ( row["TM2TTM.L1TM2TTM_COUNTER_TYPE"].data<string>());
      tm->setThresholdName  ( row["TT.L1TT_NAME"].data<string>());
      tm->setThresholdActive( row["TT.L1TT_ACTIVE"].data<int>());

      string slotString = row["TM2TT.L1TM2TT_CABLE_CTPIN"].data<std::string>();
      uint16_t slot = slotString[4]-'0'; // "SLOT7" -> 7
      if(slot<7 || slot>9) {
         TRG_MSG_ERROR("Unknown CTPIN string '" << slotString << "'");
         throw runtime_error( "MenuLoader (ThresholdMonitor): Error loading Monitoring counters " );
      }
      tm->setCtpinSlot(slot);

      string conString = row["TM2TT.L1TM2TT_CABLE_CONNECTOR"].data<std::string>();
      uint16_t con = conString[3]-'0'; // "CON2" -> 2
      if(con>3) {
         TRG_MSG_ERROR("Unknown CTPIN connector string '" << conString << "'");
         throw runtime_error( "MenuLoader (ThresholdMonitor): Error loading Monitoring counters " );
      }
      tm->setCtpinConnector(con);
      tm->setThresholdStartBit(row["TM2TT.L1TM2TT_CABLE_START"].data<int>());
      tm->setThresholdEndBit  (row["TM2TT.L1TM2TT_CABLE_END"].data<int>());
      menu.addThresholdMonitor(tm);
   } 

}








/***************************************
 * get Thresholds
 ***************************************/
void
TrigConf::MenuLoader::loadThresholds(TrigConf::Menu& menu) {
   TRG_MSG_INFO("Loading L1 trigger thresholds");

   ThresholdConfigLoader* thrldr = new ThresholdConfigLoader(m_storageMgr, m_session);
   thrldr->setLevel(outputLevel());
   if ( !thrldr->load( menu.thresholdConfig() ) ) {
      TRG_MSG_ERROR("Error loading ThresholdConfig " << menu.thresholdConfig().id());
      throw runtime_error( "MenuLoader: Error loading ThresholdConfig " );
   }

   loadPIT(menu);

   if( menu.pitVector().size() == menu.tipVector().size() ) {
      // this is the case when we 
      // a) have either no direct inputs in the menu (unlikely, but then the next call doesn't matter), or
      // b) the direct inputs were not filled in the TIP map: this is not the case  since TriggerTool-04-01-06 
      createTipFromDirectThresholds(menu);
   }

}



void
TrigConf::MenuLoader::loadPIT(TrigConf::Menu& menu) {
   if( ! (m_env == MenuLoader::ALL    ||
          m_env == MenuLoader::CTPOnl ||
          m_env == MenuLoader::COOLL1) ) return;

   set<int> tipNumbersUsed;


   unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   q->addToTableList ( "L1_TM_TO_TT",          "TM2TT" );
   q->addToTableList ( "L1_PITS",              "PITS"  );

   //Bind list
   coral::AttributeList bindings;
   bindings.extend<int>("menuId");
   bindings[0].data<int>() = menu.id();

   std::string cond("TM2TT.L1TM2TT_TRIGGER_MENU_ID = :menuId");
   cond += " AND PITS.L1PIT_TM_TO_TT_ID = TM2TT.L1TM2TT_ID";
   q->setCondition( cond, bindings );

   // should not be necessary, however currently the TriggerTool uploads identical copies in some cases
   q->setDistinct();

   // Output data and types
   coral::AttributeList attList;
   attList.extend<int>( "TM2TT.L1TM2TT_TRIGGER_THRESHOLD_ID" );
   attList.extend<int>( "TM2TT.L1TM2TT_ID"                   );
   attList.extend<int>( "PITS.L1PIT_PIT_NUMBER"              );
   attList.extend<int>( "PITS.L1PIT_THRESHOLD_BIT"           );
   fillQuery(q.get(), attList);

   uint npits(0), ntips(0);

   coral::ICursor& cursor = q->execute();
   while (cursor.next()) {
      const coral::AttributeList& row = cursor.currentRow();

      int ttid   = row["TM2TT.L1TM2TT_TRIGGER_THRESHOLD_ID"].data<int>();
      int tmtott = row["TM2TT.L1TM2TT_ID"].data<int>();
      int tipnum = row["PITS.L1PIT_PIT_NUMBER"].data<int>();
      int bitnum = row["PITS.L1PIT_THRESHOLD_BIT"].data<int>();

      TriggerThreshold* tt = menu.thresholdConfig().findTriggerThreshold(ttid);


      string slotString = tt->cableCtpin();
      uint16_t slot = 0;
      if( boost::iequals( slotString, "CTPCORE" ) ) {
         slot = 10;
      } else {
         slot = slotString[4]-'0'; // "SLOT7" -> (uint)7
      }
      if(slot<7 || slot>10) {
         TRG_MSG_ERROR("Unknown CTPIN SLOT '" << slotString << "'");
         throw runtime_error( "MenuLoader: Error loading PITs " );
      }

      string conString = tt->cableConnector();
      uint16_t con = conString[3]-'0'; // "CON2" -> (uint)2
      uint16_t conMax = slot==10 ? 2 : 3;
      if( con > conMax ) {
         TRG_MSG_ERROR("Unknown CTPIN CONNECTOR '" << conString << "'");
         throw runtime_error( "MenuLoader: Error loading PITs " );
      }

      if(slot!=10) {
         PIT* pit = new PIT();
         pit->setThresholdName(tt->name());
         pit->setCtpinSlot(slot);
         pit->setCtpinConnector( con );
         pit->setPitNumber(tipnum);
         pit->setThresholdBit(bitnum);
         pit->setCableBit( tt->cableStart() + bitnum );
         pit->setTmToTtId(tmtott);
         pit->setTriggerThresholdId(ttid);
         pit->setThresholdActive(tt->active());
         pit->setThresholdMapping(tt->mapping());
         menu.addPit(pit);
         npits++;
      }

      // this is for early menus
      if(tipNumbersUsed.count(tipnum) > 0) {
         tipnum = tipnum + 160;
      } else {
         tipNumbersUsed.insert(tipnum);
      }

      TIP* tip = new TIP();
      tip->setThresholdName(tt->name());
      tip->setSlot(slot);
      tip->setConnector( con );
      tip->setTipNumber(tipnum);
      tip->setClock( tipnum % 2 );
      tip->setThresholdBit(bitnum);
      tip->setCableBit( tt->cableStart() + bitnum );
      tip->setTmToTtId(tmtott);
      tip->setTriggerThresholdId(ttid);
      tip->setThresholdActive(tt->active());
      tip->setThresholdMapping(tt->mapping());
      if(slot==10) tip->setIsDirect(true);
      else tip->setIsDirect(false);
      menu.addTip(tip);
      ntips++;

      TRG_MSG_DEBUG("TIP " << tip->tipNumber() << " -->  " << tt->name());
   }

   TRG_MSG_INFO("Loaded " << npits << " PITs and " << ntips << " TIPs");
   TRG_MSG_INFO("Menu has " << menu.pitVector().size() << " PITs and " << menu.tipVector().size() << " TIPs");


}


void
TrigConf::MenuLoader::createTipFromDirectThresholds(TrigConf::Menu& menu) {

   // this is only needed as long as the TIPs from the direct input are not in the database

   unsigned int ntips(0);

   for(TriggerThreshold * thr : menu.thresholdConfig().getThresholdVector() ) {
      if(thr->ttype()==L1DataDef::TOPO || thr->ttype()==L1DataDef::ALFA) {

         const string & conn = thr->cableConnector(); // "CON0", "CON1", "CON2"

         unsigned int connector = conn[3]-'0';
         unsigned int cableOffset = 320 + connector * 64;
         unsigned int cableBit = (unsigned int) thr->cableStart();
         unsigned int clock = thr->clock();
         unsigned int tipNumber = 2*cableBit + clock + cableOffset;

         TIP* tip = new TIP();
         tip->setThresholdName(thr->name());
         tip->setSlot(10);
         tip->setConnector( connector );
         tip->setTipNumber( tipNumber );
         tip->setThresholdBit( thr->clock() );
         tip->setCableBit( thr->cableStart() );
         tip->setTriggerThresholdId(thr->id() );
         tip->setThresholdActive(thr->active());
         tip->setThresholdMapping(thr->mapping());
         tip->setIsDirect(true);
         menu.addTip(tip);
         
         TRG_MSG_DEBUG("TIP from direct input thresholds " << tip->tipNumber() << "  -->  " << thr->name());

         ntips++;
         
      }
   }

   TRG_MSG_INFO( "Number of TIPs from direct input thresholds " << ntips );

}



void
TrigConf::MenuLoader::loadCaloInfo(TrigConf::Menu& menu) {
   // load the CaloInfo
   TrigConf::CaloInfo ci;
   ci.setSMK(menu.smk());
   m_storageMgr.caloInfoLoader().setLevel(outputLevel());
   m_storageMgr.caloInfoLoader().load(ci);
   menu.setCaloInfo(ci);
}

/***********************************
 * retrieve menu name and version
 ***********************************/
void
TrigConf::MenuLoader::loadMenuAttributes(TrigConf::Menu& menu) {

   TRG_MSG_DEBUG("Loading menu data for SMK " << menu.smk());

   unique_ptr< coral::IQuery > q( m_session.nominalSchema().newQuery() );
   q->addToTableList ( "SUPER_MASTER_TABLE", "SM"    );
   q->addToTableList ( "L1_MASTER_TABLE",    "MT"    );
   q->addToTableList ( "L1_TRIGGER_MENU",    "TM"    );

   //Bind list
   coral::AttributeList bindings;
   bindings.extend<int>("smk");
   bindings[0].data<int>() = menu.smk();

   string condition("");
   condition += "SM.SMT_ID = :smk";
   condition += " AND SM.SMT_L1_MASTER_TABLE_ID = MT.L1MT_ID";
   condition += " AND MT.L1MT_TRIGGER_MENU_ID = TM.L1TM_ID";
   q->setCondition( condition , bindings );

   //Define the data types
   coral::AttributeList attList;
   attList.extend<int>("TM.L1TM_ID");
   attList.extend<string>("TM.L1TM_NAME");
   attList.extend<int>("TM.L1TM_VERSION");
   attList.extend<int>("MT.L1MT_ID" );
   fillQuery(q.get(), attList);
      
   coral::ICursor& cursor = q->execute();
   if ( ! cursor.next() ) {
      TRG_MSG_ERROR("No trigger menu in SMK " << menu.smk());
      throw runtime_error( "MenuLoader: ERROR trigger menu not available" );
   }
     
   // fill the object with data
   const coral::AttributeList& row = cursor.currentRow();
   menu.setId     ( row["TM.L1TM_ID"].data<int>() );
   menu.setName   ( row["TM.L1TM_NAME"].data<string>() );
   menu.setVersion( row["TM.L1TM_VERSION"].data<int>() );
   menu.thresholdConfig().setLvl1MasterTableId( row["MT.L1MT_ID"].data<int>() );
}
