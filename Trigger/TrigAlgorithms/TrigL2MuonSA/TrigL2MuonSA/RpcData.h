/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      gasGap(0),
      doubletR(0),
      doubletPhi(0),
      doubletZ(0),
      layer(0),
      measuresPhi(false),
      stationName("")
	{};
      ~RpcHitData() {};
      
  public:
      double x;
      double y;
      double z;
      int gasGap;
      int doubletR;
      int doubletPhi;
      int doubletZ;
      int layer;
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
