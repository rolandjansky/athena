/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLRandomLoader.h"
#include "./XMLBoostHelper.h"
#include "TrigConfL1Data/Random.h"

using namespace std;

bool TrigConf::XMLRandomLoader::load(Random& random) {

   if( ! is_a("Random") ) return false;

   int ival(0);
   string sval("");
   
   if( readAttribute(pt(), "id", ival) )      random.setId(ival);
   if( readAttribute(pt(), "name", sval) )    random.setName(sval);
   if( readAttribute(pt(), "version", ival) ) random.setVersion(ival);
   if( readAttribute(pt(), "rate1", ival) )   random.setRate1(ival);
   if( readAttribute(pt(), "rate2", ival) )   random.setRate2(ival);

   // new style
   if( readAttribute(pt(), "name0", sval) )   random.setRndmName(0, sval);
   if( readAttribute(pt(), "name1", sval) )   random.setRndmName(1, sval);
   if( readAttribute(pt(), "name2", sval) )   random.setRndmName(2, sval);
   if( readAttribute(pt(), "name3", sval) )   random.setRndmName(3, sval);

   if( readAttribute(pt(), "cut0", ival) )   random.setCut(0, ival);
   if( readAttribute(pt(), "cut1", ival) )   random.setCut(1, ival);
   if( readAttribute(pt(), "cut2", ival) )   random.setCut(2, ival);
   if( readAttribute(pt(), "cut3", ival) )   random.setCut(3, ival);

   return true;
}
