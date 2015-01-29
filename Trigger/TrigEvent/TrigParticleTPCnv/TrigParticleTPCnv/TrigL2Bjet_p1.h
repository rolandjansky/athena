/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2Bjet_p1
 *
 * @brief persistent partner for TrigL2Bjet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2Bjet_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BJET_P1_H
#define TRIGPARTICLETPCNV_TRIGL2BJET_P1_H

#include <stdint.h>
#include "TrigParticle/TrigL2Bjet.h"
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigL2Bjet_p1 
{
  friend class TrigL2BjetCnv_p1;

 public:
  
  TrigL2Bjet_p1() {}
  virtual ~TrigL2Bjet_p1(){}
 
 private:

  bool m_valid;
  
  int   m_roiID;
  float m_eta;
  float m_phi;
  float m_prmVtx;
  			 
  float m_xcomb;
  float m_x2d;
  float m_xd0;
  float m_xz0; 
  float m_x3d;
  float m_xmvtx;
  float m_xevtx;
  float m_xnvtx;
  
};

#endif
