/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCDATA_H
#define  TRIGL2MUONSA_RPCDATA_H

namespace TrigL2MuonSA {

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
  
  class RpcHitData
  {
  public:
    RpcHitData() :
      x(0),
      y(0),
      z(0),
      time(0),
      distToEtaReadout(0),
      distToPhiReadout(0),
      gasGap(0),
      doubletR(0),
      doubletPhi(0),
      doubletZ(0),
      layer(0),
      eta(0),
      phi(0),
      l(0),
      measuresPhi(false),
      stationName("")
	{};
      ~RpcHitData() {};
      
  public:
      double x;
      double y;
      double z;
      double time;
      double distToEtaReadout;
      double distToPhiReadout;
      int gasGap;
      int doubletR;
      int doubletPhi;
      int doubletZ;
      int layer;
      double eta;
      double phi;
      double l;      
      bool measuresPhi;
      std::string stationName;
  };
  
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
  
  typedef std::vector<RpcHitData>  RpcHits;

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
  
}

#endif  // TRIGL2MUONSA_RPCDATA_H
