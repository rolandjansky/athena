/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLDeadTimeLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfL1Data/DeadTime.h"

bool
TrigConf::XMLDeadTimeLoader::load(DeadTime& deadtime) {

   if( key() == "" ) // no Deadtime element in XML file (removed in Run2)
      return true;

   if ( !is_a("Deadtime") ) { 
      return false;
   }

   int ival(0);
   std::string strval("");

   if( readAttribute(pt(), "id", ival) )              deadtime.setId(ival);
   if( readAttribute(pt(), "name", strval) )          deadtime.setName(strval);
   if( readAttribute(pt(), "version", ival) )         deadtime.setVersion(ival);
   if( readAttribute(pt(), "simple", ival) )          deadtime.setSimple(ival);
   if( readAttribute(pt(), "complex1_level", ival) )  deadtime.setComplex1Level(ival);
   if( readAttribute(pt(), "complex1_rate",  ival) )  deadtime.setComplex1Rate(ival);
   if( readAttribute(pt(), "complex2_level", ival) )  deadtime.setComplex2Level(ival);
   if( readAttribute(pt(), "complex2_rate",  ival) )  deadtime.setComplex2Rate(ival);

   return true;
}


