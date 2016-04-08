/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2Bjet_p3
 *
 * @brief persistent partner for TrigL2Bjet
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGL2BJET_P3_H
#define TRIGPARTICLETPCNV_TRIGL2BJET_P3_H

//#include "AthenaPoolUtilities/TPObjRef.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

class TrigL2Bjet_p3 {

  friend class TrigL2BjetCnv_p3;

 public:
  
  TrigL2Bjet_p3() :
    m_valid(0), m_roiID(0), m_P4PtEtaPhiM(0)
    {}
  virtual ~TrigL2Bjet_p3(){}
  
  //private:

  bool m_valid;
  
  int   m_roiID;
  float m_allTheFloats[10];

  //float m_prmVtx;
  //float m_xcomb;
  //float m_xIP1d;
  //float m_xIP2d;
  //float m_xIP3d;
  //float m_xChi2;
  //float m_xSv;
  //float m_xmvtx;
  //float m_xevtx;
  //float m_xnvtx;

  //TPObjRef m_p4PtEtaPhiM;
  P4PtEtaPhiM_p1 m_P4PtEtaPhiM;
  
};

#endif
