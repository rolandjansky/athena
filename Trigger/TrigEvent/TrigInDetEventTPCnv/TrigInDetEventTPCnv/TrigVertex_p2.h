/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertex_p2
 *
 * @brief persistent partner for TrigVertex
 *
 * @author Dmitry Emeliyanov
 *
 *
 **********************************************************************************/

#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEX_P2_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEX_P2_H

#include <stdint.h>
#include <vector>


#include "AthenaPoolUtilities/TPObjRef.h"

class TrigVertex_p2 
{
  friend class TrigVertexCnv_p2;

 public:
  
  TrigVertex_p2() {}
  virtual ~TrigVertex_p2(){}
  
 private:

  float m_allFloats[13];//m_x,m_y,m_z,m_mass,m_massVar,m_cov[6],m_energyFraction, m_chiSquared
  int m_allInts[3];//m_nTwoTracksSecVtx,m_nDOF,m_algId 
  
  std::vector<TPObjRef>  m_tracks           ; 
};

#endif
