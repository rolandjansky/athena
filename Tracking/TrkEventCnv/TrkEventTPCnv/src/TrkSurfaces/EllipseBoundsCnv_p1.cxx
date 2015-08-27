/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrkSurfaces/EllipseBounds.h"
#undef private

#include "TrkEventTPCnv/TrkSurfaces/EllipseBoundsCnv_p1.h"

void EllipseBoundsCnv_p1 :: persToTrans( const Trk :: EllipseBounds_p1 *persObj,
                                            Trk :: EllipseBounds    *transObj,
                                            MsgStream            & )
{
  using namespace Trk;
  transObj->m_boundValues.resize(6); 
  assert(static_cast<int>(EllipseBounds::bv_length)==6); 
  transObj->m_boundValues[EllipseBounds::bv_rMinX]                 = persObj->m_rMinX; 
  transObj->m_boundValues[EllipseBounds::bv_rMinY]                 = persObj->m_rMinY; 
  transObj->m_boundValues[EllipseBounds::bv_rMaxX]                 = persObj->m_rMaxX; 
  transObj->m_boundValues[EllipseBounds::bv_rMaxY]                 = persObj->m_rMaxY; 
  transObj->m_boundValues[EllipseBounds::bv_averagePhi]            = persObj->m_avePhi; 
  transObj->m_boundValues[EllipseBounds::bv_halfPhiSector]         = persObj->m_hPhiSec;
}

void EllipseBoundsCnv_p1 :: transToPers( const Trk :: EllipseBounds    * /**transObj*/,
                                            Trk :: EllipseBounds_p1 * /**persObj*/,
                                            MsgStream            & )
{
  throw std::runtime_error("EllipseBoundsCnv_p1::transToPers is deprecated!");   
}
