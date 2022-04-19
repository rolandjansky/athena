/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   ConeBoundsCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#include "TrkSurfaces/ConeBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/ConeBoundsCnv_p1.h"

void ConeBoundsCnv_p1 :: persToTrans( const Trk :: ConeBounds_p1 *persObj,
                                            Trk :: ConeBounds    *transObj,
                                            MsgStream            & )
{
  *transObj = Trk::ConeBounds (persObj->m_alpha,
                               persObj->m_minZ,
                               persObj->m_maxZ,
                               persObj->m_avePhi,
                               persObj->m_halfPhiSector);
}

void ConeBoundsCnv_p1 :: transToPers( const Trk :: ConeBounds    *transObj,
                                            Trk :: ConeBounds_p1 *persObj,
                                            MsgStream            & )
{
  persObj->m_alpha         = transObj->alpha();
  persObj->m_minZ          = transObj->minZ();
  persObj->m_maxZ          = transObj->maxZ();
  persObj->m_avePhi        = transObj->averagePhi();
  persObj->m_halfPhiSector = transObj->halfPhiSector();
}
