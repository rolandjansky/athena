/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   DiscBoundsCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#include "TrkSurfaces/DiscBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/DiscBoundsCnv_p1.h"

void DiscBoundsCnv_p1 :: persToTrans( const Trk :: DiscBounds_p1 *persObj,
                                            Trk :: DiscBounds    *transObj,
                                            MsgStream            & )
{
  *transObj = Trk::DiscBounds (persObj->m_rMin,
                               persObj->m_rMax,
                               persObj->m_avePhi,
                               persObj->m_hPhiSec);
}

void DiscBoundsCnv_p1 :: transToPers( const Trk :: DiscBounds    * /**transObj*/,
                                            Trk :: DiscBounds_p1 * /**persObj*/,
                                            MsgStream            & )
{
  throw std::runtime_error("DiscBoundsCnv_p1::transToPers is deprecated!");
}
