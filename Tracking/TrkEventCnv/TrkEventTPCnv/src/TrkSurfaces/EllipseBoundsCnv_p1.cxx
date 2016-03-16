/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkSurfaces/EllipseBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/EllipseBoundsCnv_p1.h"

void EllipseBoundsCnv_p1 :: persToTrans( const Trk :: EllipseBounds_p1 *persObj,
                                            Trk :: EllipseBounds    *transObj,
                                            MsgStream            & )
{
  *transObj = Trk::EllipseBounds (persObj->m_rMinX,
                                  persObj->m_rMinY,
                                  persObj->m_rMaxX,
                                  persObj->m_rMaxY,
                                  persObj->m_avePhi,
                                  persObj->m_hPhiSec);
}

void EllipseBoundsCnv_p1 :: transToPers( const Trk :: EllipseBounds    * /**transObj*/,
                                            Trk :: EllipseBounds_p1 * /**persObj*/,
                                            MsgStream            & )
{
  throw std::runtime_error("EllipseBoundsCnv_p1::transToPers is deprecated!");   
}
