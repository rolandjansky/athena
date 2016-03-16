/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RotatedTrapezoidBoundsCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/RotatedTrapezoidBoundsCnv_p1.h"

void RotatedTrapezoidBoundsCnv_p1 :: persToTrans( const Trk :: RotatedTrapezoidBounds_p1 *persObj,
                                                 Trk :: RotatedTrapezoidBounds    *transObj,
                                                 MsgStream                 & )
{
  using namespace Trk;
  transObj->m_boundValues.resize(3); 
  assert(static_cast<int>(RotatedTrapezoidBounds::bv_length)==3); 
  transObj->m_boundValues[RotatedTrapezoidBounds::bv_minHalfY] = persObj->m_minHalfY;
  transObj->m_boundValues[RotatedTrapezoidBounds::bv_maxHalfY] = persObj->m_maxHalfY;
  transObj->m_boundValues[RotatedTrapezoidBounds::bv_halfX]    = persObj->m_halfX;
  transObj->m_kappa                    = persObj->m_kappa;
  transObj->m_delta                    = persObj->m_delta;
}

void RotatedTrapezoidBoundsCnv_p1 :: transToPers( const Trk :: RotatedTrapezoidBounds    * /**transObj*/,
                                                 Trk :: RotatedTrapezoidBounds_p1 * /**persObj*/,
                                                 MsgStream                 & ) 
{
    throw std::runtime_error("RotatedTrapezoidBoundsCnv_p1::transToPers is deprecated!");   
}
