/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   DiscBoundsCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#define private public
#include "TrkSurfaces/DiscBounds.h"
#undef private

#include "TrkEventTPCnv/TrkSurfaces/DiscBoundsCnv_p1.h"

void DiscBoundsCnv_p1 :: persToTrans( const Trk :: DiscBounds_p1 *persObj,
                                            Trk :: DiscBounds    *transObj,
                                            MsgStream            & )
{
  using namespace Trk;
  transObj->m_boundValues.resize(4); 
  assert(static_cast<int>(DiscBounds::bv_length)==4); 
  transObj->m_boundValues[DiscBounds::bv_rMin]          = persObj->m_rMin;
  transObj->m_boundValues[DiscBounds::bv_rMax]          = persObj->m_rMax;
  transObj->m_boundValues[DiscBounds::bv_averagePhi]    = persObj->m_avePhi;
  transObj->m_boundValues[DiscBounds::bv_halfPhiSector] = persObj->m_hPhiSec;
}

void DiscBoundsCnv_p1 :: transToPers( const Trk :: DiscBounds    * /**transObj*/,
                                            Trk :: DiscBounds_p1 * /**persObj*/,
                                            MsgStream            & )
{
  throw std::runtime_error("DiscBoundsCnv_p1::transToPers is deprecated!");
}
