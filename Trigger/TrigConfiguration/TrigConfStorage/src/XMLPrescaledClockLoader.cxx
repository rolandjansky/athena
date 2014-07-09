/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLPrescaledClockLoader.h"
#include "./XMLBoostHelper.h"
#include "TrigConfL1Data/PrescaledClock.h"

using namespace std;

bool TrigConf::XMLPrescaledClockLoader::load(PrescaledClock& psc) {
   if( ! is_a("PrescaledClock") ) return false;

   int ival(0);
   string strval("");

   if( readAttribute(pt(), "id", ival) )      psc.setId(ival);
   if( readAttribute(pt(), "name", strval) )  psc.setName(strval);
   if( readAttribute(pt(), "version", ival) ) psc.setVersion(ival);
   if( readAttribute(pt(), "clock1", ival) )  psc.setClock1(ival);
   if( readAttribute(pt(), "clock2", ival) )  psc.setClock2(ival);

   return true;
}
