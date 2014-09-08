/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalPositionCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef LOCAL_POSITION_CNV_P1_H
#define LOCAL_POSITION_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"

class MsgStream;

// namespace Trk {
//   struct LocalPosition{
//     float m_locX;
//     float m_locY;
//   }; // Original EM was removed from release as part of 19.0.0 migration from CLHEP to Eigen.
// }

class LocalPositionCnv_p1: public T_AthenaPoolTPCnvBase<Amg::Vector2D,
                                                        Trk :: LocalPosition_p1>
{
    public:
        LocalPositionCnv_p1() {}
        virtual void persToTrans( const Trk :: LocalPosition_p1 *persObj,
                                        Amg::Vector2D           *transObj,
                                        MsgStream               &log );
        virtual void transToPers( const Amg::Vector2D           *transObj,
                                        Trk :: LocalPosition_p1 *persObj,
                                        MsgStream               &log );
};

#endif // LOCAL_POSITION_CNV_P1_H
