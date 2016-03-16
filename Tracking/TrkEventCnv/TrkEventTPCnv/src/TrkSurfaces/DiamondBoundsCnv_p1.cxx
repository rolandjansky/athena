/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   DiamondBoundsCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#include "TrkSurfaces/DiamondBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/DiamondBoundsCnv_p1.h"

void DiamondBoundsCnv_p1 :: persToTrans( const Trk :: DiamondBounds_p1 *persObj,
                                               Trk :: DiamondBounds    *transObj,
                                               MsgStream               & )
{
  using namespace Trk;
  transObj->m_boundValues.resize(5); 
  assert(static_cast<int>(DiamondBounds::bv_length)==5); 
  transObj->m_boundValues[DiamondBounds::bv_minHalfX]          = persObj->m_minHalfX;
  transObj->m_boundValues[DiamondBounds::bv_medHalfX]          = persObj->m_medHalfX;
  transObj->m_boundValues[DiamondBounds::bv_maxHalfX]          = persObj->m_maxHalfX;
  transObj->m_boundValues[DiamondBounds::bv_halfY1]            = persObj->m_halfY1;  
  transObj->m_boundValues[DiamondBounds::bv_halfY2]            = persObj->m_halfY2;  
  transObj->m_alpha1                            = persObj->m_alpha1;  
  transObj->m_alpha2                            = persObj->m_alpha2;  
}

void DiamondBoundsCnv_p1 :: transToPers( const Trk :: DiamondBounds    * /**transObj*/,
                                               Trk :: DiamondBounds_p1 * /**persObj*/,
                                               MsgStream               & )
{
  throw std::runtime_error("DiamondBoundsCnv_p1::transToPers is deprecated!");   
}
