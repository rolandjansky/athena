/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLPrescaleSetLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfL1Data/L1PSNumber.h"
#include "TrigConfL1Data/PrescaleSet.h"

#include <sstream>

using namespace std;
using namespace TrigConf;

bool
TrigConf::XMLPrescaleSetLoader::load(PrescaleSet& ps) {

   if ( ! is_a("PrescaleSet") ) return false;

   int ctpid(0),maxctpid(0);
   for(value_type v: pt()) {
      if ( readAttribute(v.second, "ctpid", ctpid) )
         maxctpid = max(maxctpid, ctpid);
   }
   ps.resize(maxctpid+1);

   int ival = 0;
   string sval("");
   readAttribute(pt(), "id",      ival); ps.setId(ival);
   readAttribute(pt(), "name",    sval); ps.setName(sval);
   readAttribute(pt(), "version", ival); ps.setVersion(ival);
   if( readAttribute(pt(), "menuPartition", ival) ) ps.setPartition(ival);
   if( readAttribute(pt(), "type", sval) )          ps.setType(sval);

   bool newStyle(false); // means cuts are given

   // list of prescales
   for(value_type v: pt()) {

      int ctpid(-1);
      if( ! readAttribute(v.second, "ctpid", ctpid) )
         continue;

      int nn(-1), mm(0), dd(0);
      string scut("");
      float prescale = -1;
      if ( readAttribute(v.second, "n", nn) && readAttribute(v.second, "m", mm) && readAttribute(v.second, "d", dd) ) { // fractional prescale - run 1
         int64_t prescale64bit = L1PSNumber::encodeNMD(nn, mm, dd);
         prescale = L1PSNumber(prescale64bit).getFloatPrescale();
         ps.setPrescale(ctpid, prescale);
      } else if( readAttribute(v.second, "cut", scut) ) { // new style contains 'cut' and 'value' as attributes
         int cut;
         stringstream ss;
         ss << hex << scut;
         ss >> cut;
         ps.setCut(ctpid, cut);
         newStyle = true;
      } else if( readAttribute(v.second, "ps", prescale) ) { // new style contains 'cut' and 'value' as attributes
         ps.setPrescale(ctpid, prescale);
      } else if( getTextContent(v.second, prescale) ) {
         ps.setPrescale(ctpid, prescale);
      }
   }

   ps.setNewPrescaleStyle(newStyle);

   return true;
}
