/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLPrioritySetLoader.h"
#include "./XMLBoostHelper.h"
#include "TrigConfL1Data/PrioritySet.h"

bool
TrigConf::XMLPrioritySetLoader::load( PrioritySet& ps) {

   if( key() == "" ) // no PrioritySet in XML file (moved to Lvl1Item in Run2)
      return false;

   if( ! is_a("PrioritySet") ) return false;

   int ival(0);
   std::string strval("");

   if( readAttribute(pt(), "name", strval) )  ps.setName(strval);
   if( readAttribute(pt(), "id", ival) )      ps.setId(ival);

   int ctpid(-1);
   const int max_priorities = 256;
   std::string priorities[max_priorities];
   for (int i=0; i<max_priorities; ++i)
      priorities[i] = "LOW";

   BOOST_FOREACH(value_type v, pt()) {
      if(v.first!="Priority") continue;
      std::string ti_priority = "LOW";
      if( readAttribute(v.second, "ctpid", ctpid) && ctpid >= 0 && ctpid<max_priorities) {
         if( readAttribute(v.second, "priority", ti_priority) || getTextContent(v.second, ti_priority) ) // either attribute or text
            priorities[ctpid] = ti_priority;
      }
   }

   ps.setPriorities(priorities, max_priorities);
   return true;
}
