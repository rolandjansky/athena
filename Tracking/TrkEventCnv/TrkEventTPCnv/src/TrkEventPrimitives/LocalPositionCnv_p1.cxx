/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalPositionCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------


#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventPrimitives/ParamDefs.h"

void LocalPositionCnv_p1 :: persToTrans( const Trk :: LocalPosition_p1 *persObj,
                                               Amg::Vector2D           *transObj,
                                               MsgStream&) 
{
  (*transObj)[Trk::locX] = persObj->m_x; 
  (*transObj)[Trk::locY] = persObj->m_y; 
}

void LocalPositionCnv_p1 :: transToPers( const Amg::Vector2D           *transObj,
                                               Trk :: LocalPosition_p1 *persObj,
                                               MsgStream&)
{
    persObj->m_x = (*transObj)[Trk::locX];
    persObj->m_y = (*transObj)[Trk::locY];
}
