/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjet_p1
 *
 * @brief persistent partner for TrigEFBjet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBjet_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBJET_P1_H
#define TRIGPARTICLETPCNV_TRIGEFBJET_P1_H

#include <stdint.h>
#include "TrigParticle/TrigEFBjet.h"
#include "AthenaPoolUtilities/TPObjRef.h"

class TrigEFBjet_p1 
{
  friend class TrigEFBjetCnv_p1;

 public:
  
  TrigEFBjet_p1() {}
  virtual ~TrigEFBjet_p1(){}
 
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
