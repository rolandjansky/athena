/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CylinderBounds_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/CylinderBoundsCnv_p1.h"

void CylinderBoundsCnv_p1 :: persToTrans( const Trk :: CylinderBounds_p1 *persObj,
                                                Trk :: CylinderBounds    *transObj,
                                                MsgStream                & )
{
  *transObj = Trk::CylinderBounds (persObj->m_radius,
                                   persObj->m_halfPhiSector,
                                   persObj->m_averagePhi,
                                   persObj->m_halfZ);
}

void CylinderBoundsCnv_p1 :: transToPers( const Trk :: CylinderBounds    * /**transObj*/,
                                                Trk :: CylinderBounds_p1 * /**persObj*/,
                                                MsgStream                & )
{
  throw std::runtime_error("CylinderBoundsCnv_p1::transToPers is deprecated!");   
}

