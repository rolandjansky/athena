/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLTriggerThresholdLoader.h"
#include "./XMLTriggerThresholdValueLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfStorage/XMLStorageMgr.h"

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"

#include <string>
#include <iostream>

using namespace std;

bool
TrigConf::XMLTriggerThresholdLoader::load(TriggerThreshold& thr) {

   if (!is_a("TriggerThreshold")) return false;

   bool bval(false);
   std::string sval(""), type(""), name("");
   int ival(0);

   if( readAttribute( pt(), "id",         ival) )  thr.setId(ival);
   if( readAttribute( pt(), "version",    ival) )  thr.setVersion(ival);
   if( readAttribute( pt(), "name",       name) )  thr.setName(name);
   if( readAttribute( pt(), "version",    ival) )  thr.setVersion(ival);
   if( readAttribute( pt(), "type",       type) )  thr.setType(type);
   if( readAttribute( pt(), "seed",       sval) )  thr.setZBSeedingThresholdName(sval);
   if( readAttribute( pt(), "seed_multi", ival) )  thr.setZBSeedingThresholdMulti(ival);
   if( readAttribute( pt(), "bcdelay",    ival) )  thr.setBCDelay(ival);
   if( readAttribute( pt(), "mapping",    ival) )  thr.setMapping(ival);
   if( readAttribute( pt(), "active",     bval) )  thr.setActive(bval);
   if( readAttribute( pt(), "input",      sval) )  thr.setInput(sval);
   if( readAttribute( pt(), "bitnum",     ival) )  thr.setBitnum(ival);
   
   // <TriggerThresholdValue>'s
   XMLTriggerThresholdValueLoader& thrvldr = dynamic_cast<XMLTriggerThresholdValueLoader&>(m_storageMgr.triggerThresholdValueLoader());
   BOOST_FOREACH(value_type v, pt()) {
      if(v.first != "TriggerThresholdValue" ) continue;
      thrvldr.setPtree(&v);
      TriggerThresholdValue* thrv = TriggerThreshold::createThresholdValue(type);
      thrvldr.load(*thrv);
      thr.addThresholdValue(thrv);
   }

   // <Cable>
   ptree::const_assoc_iterator cable = pt().find("Cable");
   if (cable != pt().not_found()) {
      if(readAttribute(cable->second, "name",      sval)) thr.setCableName(sval);
      if(readAttribute(cable->second, "ctpin",     sval)) thr.setCableCtpin(sval);
      if(readAttribute(cable->second, "input",     sval)) thr.setCableCtpin(sval);
      if(readAttribute(cable->second, "connector", sval)) thr.setCableConnector(sval);
      // <Signal>
      ptree::const_assoc_iterator signal = cable->second.find("Signal");
      if( signal != cable->second.not_found()) {
         if(readAttribute(signal->second, "range_begin", ival)) thr.setCableStart(ival);
         if(readAttribute(signal->second, "range_end",   ival)) thr.setCableEnd(ival);
         if(readAttribute(signal->second, "clock",       ival)) thr.setClock(ival);
      }
   }
   return true;
}
