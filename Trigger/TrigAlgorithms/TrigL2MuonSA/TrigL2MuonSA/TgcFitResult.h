/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCFITRESULT_H
#define  TRIGL2MUONSA_TGCFITRESULT_H

namespace TrigL2MuonSA {
  
  class TgcFitResult
  {
   public:
    TgcFitResult() { Clear(); };
    ~TgcFitResult() {};
      
  public:
    void Clear() 
    {
      isSuccess     = false;
      tgcMidRhoChi2 = 0;
      tgcMidRhoNin  = 0;
      tgcMidRhoN    = 0;
      tgcMidPhiChi2 = 0;
      tgcMidPhiNin  = 0;
      tgcMidPhiN    = 0;
      tgcInnRhoStd  = 0;
      tgcInnRhoNin  = 0;
      tgcInnRhoN    = 0;
      tgcInnPhiStd  = 0;
      tgcInnPhiNin  = 0;
      tgcInnPhiN    = 0;
      tgcPT         = 0;
      slope         = 0;
      intercept     = 0;
      phiDir        = 0;
      isPhiDir      = false;
      phi           = 0;
      dPhidZ        = 0;
      for(int i=0; i<4; i++) { tgcMid1[i]=0; tgcMid2[i]=0; tgcInn[i]=0; }
    };
    
  public:
    bool  isSuccess;

    // Middle station
    float tgcMid1[4];       // Eta, Phi, R, Z at lowest Z
    float tgcMid2[4];       // Eta, Phi, R, Z at highest Z
    float tgcMidRhoChi2;    // Rho fit Chi2
    int   tgcMidRhoNin;       // Rho fit N-points in input
    int   tgcMidRhoN;         // Rho fit N-points
    float tgcMidPhiChi2;    // Phi fit Chi2
    int   tgcMidPhiNin;       // Phi fit N-points in input
    int   tgcMidPhiN;         // Phi fit N-points

    // Inner station
    float tgcInn[4];        // Eta, Phi, R, Z
    float tgcInnRhoStd;     // Rho fit standard dev
    int   tgcInnRhoNin;       // Rho fit N-points in input
    int   tgcInnRhoN;         // Rho fit N-points
    float tgcInnPhiStd;     // Phi fit standard dev
    int   tgcInnPhiNin;       // Phi fit N-points in input
    int   tgcInnPhiN;         // Phi fit N-points
    
    float tgcPT;
    // 
    float slope;
    float intercept;

    double phiDir;
    bool   isPhiDir;
    double phi;
    double dPhidZ;
  };
  
}

#endif
