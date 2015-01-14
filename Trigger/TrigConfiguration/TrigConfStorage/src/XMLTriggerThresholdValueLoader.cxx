/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLTriggerThresholdValueLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/HelperFunctions.h"

#include <iostream>

using namespace std;

bool
TrigConf::XMLTriggerThresholdValueLoader::load(TriggerThresholdValue& thrv) {

   if( ! is_a("TriggerThresholdValue") ) return false;

   string sval(""), type("");
   int    ival(0);
   float  fval(0);
   
   if( readAttribute(pt(), "id", ival) )            thrv.setId(ival);                        
   if( readAttribute(pt(), "name", sval) )          thrv.setName(sval);      
   if( readAttribute(pt(), "version", ival) )       thrv.setVersion(ival);
   if( readAttribute(pt(), "thresholdval", fval) )  thrv.setPtcut(fval);      
   if( readAttribute(pt(), "type", type) )          thrv.setType(type);      
   if( readAttribute(pt(), "priority", fval) )      thrv.setPriority(fval);
   if( readAttribute(pt(), "window", ival) )        thrv.setWindow(ival);
   if( readAttribute(pt(), "etamin", ival) )        thrv.setEtaMin(ival);
   if( readAttribute(pt(), "etamax", ival) )        thrv.setEtaMax(ival);
   if( readAttribute(pt(), "phimin", ival) )        thrv.setPhiMin(ival);
   if( readAttribute(pt(), "phimax", ival) )        thrv.setPhiMax(ival);                
   
   if (type == L1DataDef::emType() || type == L1DataDef::tauType() || type == "EM_TAU") {
      ClusterThresholdValue& cluthrval = dynamic_cast<ClusterThresholdValue&>(thrv);
      if( readAttribute(pt(), "em_isolation", fval) )  cluthrval.setEmIsolation(fval);   
      if( readAttribute(pt(), "had_isolation", fval) ) cluthrval.setHadIsolation(fval); 
      if( readAttribute(pt(), "had_veto", fval) )      cluthrval.setHadVeto(fval);           

      if( readAttribute(pt(), "isobits", sval ) ) {
         cluthrval.setIsolationMask( TrigConf::bin2uint(sval) );
         cluthrval.setUseIsolationMask();
      }
   }

   return true;
}
