/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BINUTILITY_CNV_P1_H
#define BINUTILITY_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtility_p1.h"

class MsgStream;

class BinUtilityCnv_p1 : public T_AthenaPoolTPCnvBase<Trk::BinUtility, Trk::BinUtility_p1>
{
    public:
        BinUtilityCnv_p1() {}
        
        virtual void persToTrans( const Trk::BinUtility_p1 *persObj,
                                        Trk::BinUtility    *transObj,
                                        MsgStream &log );
        virtual void transToPers( const Trk::BinUtility    *transObj,
                                        Trk::BinUtility_p1 *persObj,
                                        MsgStream &log );
};

#endif // BINUTILITY_CNV_P1_H