/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/TrkSurfaces/TriangleBoundsCnv_p1.h"
#include <algorithm>

void TriangleBoundsCnv_p1 :: persToTrans( const Trk :: TriangleBounds_p1 *persObj,
                                                 Trk :: TriangleBounds    *transObj,
                                                 MsgStream                 & )
{
  *transObj = Trk::TriangleBounds (persObj->m_vertices);
}

void TriangleBoundsCnv_p1 :: transToPers( const Trk :: TriangleBounds    * /**transObj*/,
                                                 Trk :: TriangleBounds_p1 * /**persObj*/,
                                                 MsgStream                 & ) 
{
    throw std::runtime_error("TriangleBoundsCnv_p1::transToPers is deprecated!");
}
