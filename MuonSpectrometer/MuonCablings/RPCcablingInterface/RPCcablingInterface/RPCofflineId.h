/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCOFFLINEID_H
#define RPCOFFLINEID_H

struct RPCofflineId {
   std::string stationName;
           int stationEta;
           int stationPhi;
   	   int doubletR;
   	   int doubletZ;
           int doubletPhi;
   	   int gasGap;
   	   int measuresPhi;
   	   int strip;
   void init() {
     stationEta = -99;
     stationPhi = -99;
     doubletR = -99;
     doubletZ = -99;
     doubletPhi = -99;
     gasGap = -99;
     measuresPhi = -99;
     strip = -99;
   }
};

#endif
