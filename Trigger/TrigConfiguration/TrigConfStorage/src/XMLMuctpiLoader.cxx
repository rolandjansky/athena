/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./XMLMuctpiLoader.h"
#include "./XMLBoostHelper.h"

#include "TrigConfL1Data/Run3MuonTriggerThreshold.h"
using namespace std;

bool
TrigConf::XMLMuctpiLoader::load( Muctpi& muctpi ) {

   if( ! is_a("MuctpiInfo") ) return false;

   msg() << "XMLMuctpiLoader started loading data." << std::endl;

   int ival(0);
   std::string sval("");

   if( readAttribute(pt(), "name", sval) )     muctpi.setName(sval);
   if( readAttribute(pt(), "id", ival) )       muctpi.setId(ival);
   if( readAttribute(pt(), "version", ival) )  muctpi.setVersion(ival);
   if( readAttribute(pt(), "low_pt", ival) )   muctpi.setLowptThreshold( ival );
   if( readAttribute(pt(), "high_pt", ival) )  muctpi.setHighptThreshold( ival );
   if( readAttribute(pt(), "max_cand", ival) ) muctpi.setMaxCand( ival );
   for(value_type v : pt()) {
      if( v.first == "low_pt"   && getTextContent(v.second, ival) ) muctpi.setLowptThreshold( ival );
      if( v.first == "high_pt"  && getTextContent(v.second, ival) ) muctpi.setHighptThreshold( ival );
      if( v.first == "max_cand" && getTextContent(v.second, ival) ) muctpi.setMaxCand( ival );
      if( v.first == "TriggerThreshold" ) {
         Run3MuonTriggerThreshold * thr = new Run3MuonTriggerThreshold();
         if( readAttribute(v.second, "bitnum", ival) )                thr->setBitnum(ival);
         if( readAttribute(v.second, "id", ival) )                    thr->setId(ival);
         if( readAttribute(v.second, "mapping", ival) )               thr->setMapping(ival);
         if( readAttribute(v.second, "isMultiplicityThr", ival) )     thr->setIsMultiplicityThr(ival!=0);
         if( readAttribute(v.second, "name", sval) )                  thr->setName(sval);
         if( readAttribute(v.second, "type", sval) )                  thr->setType(sval);
         if( readAttribute(v.second, "input", sval) )                 thr->setInput(sval);
         for(value_type v2 : v.second) {
            if ( v2.first == "Region" ) {
               if( readAttribute(v2.second, "etamin", ival) )    thr->region().setEtamin( ival );
               if( readAttribute(v2.second, "etamax", ival) )    thr->region().setEtamax( ival );
               if( readAttribute(v2.second, "phimin", ival) )    thr->region().setPhimin( ival );
               if( readAttribute(v2.second, "phimax", ival) )    thr->region().setPhimax( ival );
               if( readAttribute(v2.second, "detector", sval) )  thr->region().setDetector( sval );             
            } else if ( v2.first == "RPC" ) {
               if( readAttribute(v2.second, "rpcThr", ival) )         thr->setRPCThr(ival);
               if( readAttribute(v2.second, "rpcExclROIList", sval) ) thr->setRPCExcl(sval);
            } else if ( v2.first == "TGC" ) {
               if( readAttribute(v2.second, "tgcThr", ival) )         thr->setTGCThr(ival);
               if( readAttribute(v2.second, "tgcFlags", sval) )       thr->setTGCFlags(sval);
               if( readAttribute(v2.second, "tgcExclROIList", sval) ) thr->setTGCExcl(sval);
            } else if ( v2.first == "Cable" ) {
               if( readAttribute(v2.second, "connector", sval) )  thr->cable().setConnector(sval);
               if( readAttribute(v2.second, "input", sval) )      thr->cable().setInput(sval);
               if( readAttribute(v2.second, "name", sval) )       thr->cable().setName(sval);
               if( readAttribute(v2.second, "type", sval) )       thr->cable().setType(sval);
               for(value_type v3 : v2.second) {
                  if ( v3.first == "Signal" ) {
                     if( readAttribute(v3.second, "range_begin", ival) )  thr->cable().setRangeBegin(ival);
                     if( readAttribute(v3.second, "range_end", ival) )  thr->cable().setRangeEnd(ival);
                  }
               }
            }

         }
         muctpi.addThreshold( thr );
      }
      if( v.first == "ExclusionLists" ) {
         for(value_type v2 : v.second) {
            if ( v2.first == "RpcExclROIList" || v2.first == "TgcExclROIList") {
               auto detector = (v2.first == "RpcExclROIList" ? Muctpi::RPC : Muctpi::TGC);
               std::string listname("");
               if( readAttribute(v2.second, "name", listname) ) {
                  muctpi.createRoiExclusionList(listname, detector);
                  for(value_type v3 : v2.second) {
                     if ( v3.first == "RoiIDlist" ) {
                        std::string sectorname(""),roiids("");
                        readAttribute(v3.second, "sectorname", sectorname);
                        readAttribute(v3.second, "roiids", roiids);
                        muctpi.addSectorRoiList(listname, detector, sectorname, roiids);
                     }
                  }
               } else {
                  cout << "WARNING found ROI exclusion list without a name" << endl;
               }
            }
         }
      }
   }
   return true;
}

