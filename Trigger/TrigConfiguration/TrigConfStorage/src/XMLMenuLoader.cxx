/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLMenuLoader.h"
#include "./XMLThresholdConfigLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/HelperFunctions.h"

#include <iostream>
#include <string>

#include "boost/algorithm/string.hpp"

using namespace std;

bool
TrigConf::XMLMenuLoader::load(Menu& menu) {

   if ( !is_a("TriggerMenu") ) return false;

   int ival(0);
   std::string sval("");

   if( readAttribute(pt(), "id", ival) )      menu.setId(ival);
   if( readAttribute(pt(), "name", sval) )    menu.setName(sval);
   if( readAttribute(pt(), "version", ival) ) menu.setVersion(ival);

   // need to load thresholds first because they are used for item building
   IThresholdConfigLoader& thrcfgloader = m_storageMgr.thresholdConfigLoader();
   thrcfgloader.setLevel(outputLevel());
   thrcfgloader.load( menu.thresholdConfig() );
   std::cerr << std::flush;
   std::cout << std::flush;
   TRG_MSG_INFO("Number of thresholds in the menu: " << menu.thresholdConfig().size());

   for(value_type v: pt()) {
      if(v.first != "TriggerItem") continue;
      TriggerItem * item = new TriggerItem();
      if( readAttribute(v.second, "id", ival) )               item->setId(ival);
      if( readAttribute(v.second, "name", sval) )             item->setName(sval);
      if( readAttribute(v.second, "version", ival) )          item->setVersion(ival);
      if( readAttribute(v.second, "complex_deadtime", ival) ) item->setComplexDeadtime(ival);
      if( readAttribute(v.second, "partition", ival) )        item->setPartition(ival);
      if( readAttribute(v.second, "definition", sval) )       item->setDefinition(sval);
      if( readAttribute(v.second, "ctpid", ival) )            item->setCtpId(ival);
      if( readAttribute(v.second, "trigger_type", sval) )     item->setTriggerType( TrigConf::bin2uint(sval) );
      if( readAttribute(v.second, "monitor", sval) )          {
         const short TBP = 0x1;
         const short TAP = 0x2;
         const short TAV = 0x4;
         

         unsigned short monMask = 0;
         vector<string> monLfHf;
         boost::split(monLfHf, sval, boost::is_any_of(":|"));
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
            if(sval.find("TBP") != string::npos) monMask |= TBP;
            if(sval.find("TAP") != string::npos) monMask |= TAP;
            if(sval.find("TAV") != string::npos) monMask |= TAV;
         }
         //cout << "JOERG " << sval << " ==> " << monMask << endl;
         item->setMonitor( monMask );
      }

      for(value_type top: v.second) {
         if(top.first=="AND" || top.first=="InternalTrigger" || top.first=="TriggerCondition") {
            // most times it is and AND of multiple conditions, except L1_BGRPx items
            TriggerItemNode* trig_node = readNode(top, menu.thresholdConfig().thresholdVector());
            item->setTopNode(trig_node);
            break;
         }
      }

      menu.addTriggerItem(item);
   }
   TRG_MSG_INFO("Number of items in the menu: " << menu.itemVector().size());

   return true;
}


TrigConf::TriggerItemNode*
TrigConf::XMLMenuLoader::readNode(const ptree::value_type& pt,
                                  const std::vector<TriggerThreshold*>& thr_vec) {

   string node_name = pt.first;
   TriggerItemNode::NodeType node_type = TriggerItemNode::typeFromString(node_name);
   TriggerItemNode* trig_node = new TriggerItemNode(node_type);

   if(node_type != TriggerItemNode::OBJ) {
      // AND, OR, NOT -> add all children
      for(value_type v: pt.second) {
         if(v.first=="<xmlattr>") continue;
         trig_node->addChild( readNode(v, thr_vec) );
      }
   } else {
      // object (triggerthreshold or internal trigger)
      if (node_name == "TriggerCondition") {
         std::string thr_name("");
         int mult(0);
         readAttribute(pt.second,"triggerthreshold", thr_name);
         readAttribute(pt.second,"multi", mult);
         if (thr_name != "") {
            TriggerThreshold* trig_thr = findThreshold(thr_name, thr_vec);
            if (trig_thr) {
               trig_node->setTriggerThreshold(trig_thr);
               trig_node->setMultiplicity(mult);
            } else {
               TRG_MSG_ERROR("cannot find threshold " << thr_name);
            }
         } else {
            TRG_MSG_ERROR("TriggerCondition without threshold");
         }
      } else if (node_name == "InternalTrigger") {
         std::string thr_name("");
         readAttribute(pt.second, "name", thr_name);
         trig_node->setInternalTrigger(thr_name);
      }
   }
   return trig_node;
}

TrigConf::TriggerThreshold* 
TrigConf::XMLMenuLoader::findThreshold(const std::string& name, 
                                       const std::vector<TriggerThreshold*>& thr_vec) {
   for(TriggerThreshold *p: thr_vec)
      if ( p->name() == name) return p;
   return 0;
}

