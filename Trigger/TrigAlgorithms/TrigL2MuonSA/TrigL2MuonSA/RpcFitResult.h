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
      SectorId   = 0;
      type       = 0;
      threshold  = 0;
      triggerNum = 0;
      slope      = 0;
      intercept  = 0;
      beta       = 0;
      phiDir     = 0;
      isPhiDir   = false;
      phi        = 0;
      dPhidZ     = 0;
      for(int i=0; i<4; i++) { rpc1[i]=0; rpc2[i]=0; rpc3[i]=0; }
    };
    
  public:
    bool  isSuccess;
    int   SectorId;
    int   type;
    int   threshold;
    int   triggerNum;
    float slope;
    float intercept;
    float rpc1[4];
    float rpc2[4];
    float rpc3[4];
    float beta;
    
    double phiDir;
    bool   isPhiDir;
    double phi;
    double dPhidZ;
  };
}

#endif
