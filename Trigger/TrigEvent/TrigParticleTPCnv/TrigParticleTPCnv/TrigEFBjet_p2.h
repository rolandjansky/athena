/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjet_p2
 *
 * @brief persistent partner for TrigEFBjet
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGEFBJET_P2_H
#define TRIGPARTICLETPCNV_TRIGEFBJET_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"

class TrigEFBjet_p2 {

  friend class TrigEFBjetCnv_p2;

 public:
  
  TrigEFBjet_p2() :
    m_valid(false), m_roiID(0), m_prmVtx(0.0),
    m_xcomb(0.0), m_xIP1d(0.0),
    m_xIP2d(0.0), m_xIP3d(0.0),
    m_xChi2(0.0), m_xSv(0.0),
    m_xmvtx(0.0), m_xevtx(0.0), m_xnvtx(0.0)
    {}
  virtual ~TrigEFBjet_p2(){}
 
  //private:

  bool m_valid;
  
  int   m_roiID;
  float m_prmVtx;
  			 
  float m_xcomb;
  float m_xIP1d;
  float m_xIP2d;
  float m_xIP3d;
  float m_xChi2;
  float m_xSv;
  float m_xmvtx;
  float m_xevtx;
  float m_xnvtx;

  TPObjRef m_p4PtEtaPhiM;
  
};

#endif
