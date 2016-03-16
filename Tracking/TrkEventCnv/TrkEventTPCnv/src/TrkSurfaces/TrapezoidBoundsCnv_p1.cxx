/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrapezoidBoundsCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/TrapezoidBoundsCnv_p1.h"

void TrapezoidBoundsCnv_p1 :: persToTrans( const Trk :: TrapezoidBounds_p1 *persObj,
                                                 Trk :: TrapezoidBounds    *transObj,
                                                 MsgStream                 & )
{
  using namespace Trk;
  transObj->m_boundValues.resize(3); 
  assert(static_cast<int>(TrapezoidBounds::bv_length)==3); 
  transObj->m_boundValues[TrapezoidBounds::bv_minHalfX]           = persObj->m_minHalfX;
  transObj->m_boundValues[TrapezoidBounds::bv_maxHalfX]           = persObj->m_maxHalfX;
  transObj->m_boundValues[TrapezoidBounds::bv_halfY]              = persObj->m_halfY;  
  transObj->m_alpha                                               = persObj->m_alpha;  
  transObj->m_beta                                                = persObj->m_beta;
}

void TrapezoidBoundsCnv_p1 :: transToPers( const Trk :: TrapezoidBounds    * /**transObj*/,
                                                 Trk :: TrapezoidBounds_p1 * /**persObj*/,
                                                 MsgStream                 & ) 
{
  throw std::runtime_error("TrapezoidBoundsCnv_p1::transToPers is deprecated!");   
}
