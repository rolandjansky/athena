/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      ZoverR_middle = 0;
      ZoverR_outer  = 0;
      phi_middle    = 0;
      phi_outer     = 0;
    };
    
  public:
    bool  isSuccess;
    double phiDir;
    bool   isPhiDir;
    double phi;
    double dPhidZ;
    
    double ZoverR_middle;
    double ZoverR_outer;
    double phi_middle;
    double phi_outer;
  };
}

#endif
