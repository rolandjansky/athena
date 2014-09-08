/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RectangleBoundsCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#define private public
#include "TrkSurfaces/RectangleBounds.h"
#undef private

#include "TrkEventTPCnv/TrkSurfaces/RectangleBoundsCnv_p1.h"

void RectangleBoundsCnv_p1 :: persToTrans( const Trk :: RectangleBounds_p1 *persObj,
                                                 Trk :: RectangleBounds    *transObj,
                                                 MsgStream                 & )
{
    using namespace Trk;
    transObj->m_boundValues.resize(2); 
    assert(static_cast<int>(RectangleBounds::bv_length)==2); 
    transObj->m_boundValues[RectangleBounds::bv_halfX]          = persObj->m_halfX;
    transObj->m_boundValues[RectangleBounds::bv_halfY]          = persObj->m_halfY;
}

void RectangleBoundsCnv_p1 :: transToPers( const Trk :: RectangleBounds    * /**transObj*/,
                                                 Trk :: RectangleBounds_p1 * /**persObj*/,
                                                 MsgStream                 & )
{
    throw std::runtime_error("RectangleBoundsCnv_p1::transToPers is deprecated!");   
}
