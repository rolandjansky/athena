/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CylinderBounds_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#define private public
#include "TrkSurfaces/CylinderBounds.h"
#undef private


#include "TrkEventTPCnv/TrkSurfaces/CylinderBoundsCnv_p1.h"

void CylinderBoundsCnv_p1 :: persToTrans( const Trk :: CylinderBounds_p1 *persObj,
                                                Trk :: CylinderBounds    *transObj,
                                                MsgStream                & )
{
  using namespace Trk;
  transObj->m_boundValues.resize(4); 
  assert(static_cast<int>(CylinderBounds::bv_length)==4); 
  transObj->m_boundValues[CylinderBounds::bv_radius]           = persObj->m_radius;
  transObj->m_boundValues[CylinderBounds::bv_averagePhi]       = persObj->m_averagePhi;
  transObj->m_boundValues[CylinderBounds::bv_halfPhiSector]    = persObj->m_halfPhiSector;
  transObj->m_boundValues[CylinderBounds::bv_halfZ]            = persObj->m_halfZ;
}

void CylinderBoundsCnv_p1 :: transToPers( const Trk :: CylinderBounds    * /**transObj*/,
                                                Trk :: CylinderBounds_p1 * /**persObj*/,
                                                MsgStream                & )
{
  throw std::runtime_error("CylinderBoundsCnv_p1::transToPers is deprecated!");   
}

