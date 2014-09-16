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
      eta(0),
      phi(0),
      r(0),
      z(0),
      width(0),
      sta(0),
      isStrip(false),
      bcTag(0),
      inRoad(false)
	{};
      ~RpcHitData() {};
      
  public:
      double eta;
      double phi;
      double r;
      double z;
      double width;
      int    sta;
      bool   isStrip;
      int    bcTag;
      bool   inRoad;
      
  };
  
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
  
  typedef std::vector<RpcHitData>  RpcHits;

  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
  
}

#endif  // TRIGL2MUONSA_RPCDATA_H
