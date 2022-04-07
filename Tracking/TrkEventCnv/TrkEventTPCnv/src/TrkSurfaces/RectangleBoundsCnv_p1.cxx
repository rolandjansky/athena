/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RectangleBoundsCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#include "TrkSurfaces/RectangleBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/RectangleBoundsCnv_p1.h"

void RectangleBoundsCnv_p1 :: persToTrans( const Trk :: RectangleBounds_p1 *persObj,
                                                 Trk :: RectangleBounds    *transObj,
                                                 MsgStream                 & )
{
  *transObj = Trk::RectangleBounds (persObj->m_halfX,
                                    persObj->m_halfY);
}

void RectangleBoundsCnv_p1 :: transToPers( const Trk :: RectangleBounds    *transObj,
                                                 Trk :: RectangleBounds_p1 *persObj,
                                                 MsgStream                 & )
{
  persObj->m_halfX    = transObj->halflengthX();
  persObj->m_halfY    = transObj->halflengthY();
}
