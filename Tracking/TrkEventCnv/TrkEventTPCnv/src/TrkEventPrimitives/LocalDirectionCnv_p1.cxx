/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalDirectionCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirectionCnv_p1.h"

void LocalDirectionCnv_p1 :: persToTrans( const Trk :: LocalDirection_p1 *persObj,
                                            Trk :: LocalDirection    *transObj,
                                            MsgStream&) 
{
    *transObj = Trk::LocalDirection (persObj->m_angleXZ,
                                     persObj->m_angleYZ);
}

void LocalDirectionCnv_p1 :: transToPers( const Trk :: LocalDirection    *transObj,
                                            Trk :: LocalDirection_p1 *persObj,
                                            MsgStream& )
{
    persObj->m_angleXZ = transObj->angleXZ();
    persObj->m_angleYZ = transObj->angleYZ();
}
