/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_RPCFITRESULT_H
#define  TRIGL2MUONSA_RPCFITRESULT_H

namespace TrigL2MuonSA {
  
  // --------------------------------------------------------------------------------
  // --------------------------------------------------------------------------------
  
  class RpcFitResult
  {
  public:
    RpcFitResult() { Clear(); };
    ~RpcFitResult() {};
    
  public:
    void Clear()
    {
      isSuccess  = false;
      phiDir     = 0;
      isPhiDir   = false;
      phi        = 0;
      dPhidZ     = 0;

      phi_inner     = 0;
      slope_inner   = 0;
      offset_inner   = 0;

      phi_middle    = 0;
      slope_middle  = 0;
      offset_middle  = 0;

      phi_outer     = 0;
      slope_outer   = 0;
      offset_outer  = 0;
    };
    
  public:
    bool  isSuccess;
    double phiDir;
    bool   isPhiDir;
    double phi;
    double dPhidZ;
    
    double phi_inner;
    double slope_inner;
    double offset_inner;

    double phi_middle;
    double slope_middle;
    double offset_middle;

    double phi_outer;
    double slope_outer;
    double offset_outer;
  };
}

#endif
