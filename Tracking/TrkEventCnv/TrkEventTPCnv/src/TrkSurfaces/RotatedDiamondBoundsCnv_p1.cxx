/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RotatedDiamondBoundsCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#include "TrkSurfaces/RotatedDiamondBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/RotatedDiamondBoundsCnv_p1.h"

void RotatedDiamondBoundsCnv_p1 :: persToTrans( const Trk :: RotatedDiamondBounds_p1 *persObj,
                                               Trk :: RotatedDiamondBounds    *transObj,
                                               MsgStream               & )
{
  using namespace Trk;
  transObj->m_boundValues.resize(5); 
  assert(static_cast<int>(RotatedDiamondBounds::bv_length)==5); 
  transObj->m_boundValues[RotatedDiamondBounds::bv_minHalfX]          = persObj->m_minHalfX;
  transObj->m_boundValues[RotatedDiamondBounds::bv_medHalfX]          = persObj->m_medHalfX;
  transObj->m_boundValues[RotatedDiamondBounds::bv_maxHalfX]          = persObj->m_maxHalfX;
  transObj->m_boundValues[RotatedDiamondBounds::bv_halfY1]            = persObj->m_halfY1;  
  transObj->m_boundValues[RotatedDiamondBounds::bv_halfY2]            = persObj->m_halfY2;  
  transObj->m_alpha1                            = persObj->m_alpha1;  
  transObj->m_alpha2                            = persObj->m_alpha2;  
}

void RotatedDiamondBoundsCnv_p1 :: transToPers( const Trk :: RotatedDiamondBounds  *transObj,
                                               Trk :: RotatedDiamondBounds_p1      *persObj,
                                               MsgStream               & )
{
 persObj->m_minHalfX  = transObj->minHalflengthX();
 persObj->m_medHalfX  = transObj->medHalflengthX();
 persObj->m_maxHalfX  = transObj->maxHalflengthX();
 persObj->m_halfY1    = transObj->halflengthY1();
 persObj->m_halfY2    = transObj->halflengthY2();
 persObj->m_alpha1    = transObj->alpha1();
 persObj->m_alpha2    = transObj->alpha2();
}
