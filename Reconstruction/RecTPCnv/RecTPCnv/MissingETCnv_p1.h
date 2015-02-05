/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCNV_P1_H
#define RECTPCNV_MISSINGETCNV_P1_H

/********************************************************************

NAME:     MissingEtCnv_p1.h
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingET class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingET_p1.h"
#include "MissingETEvent/MissingET.h"

class MsgStream;

class MissingETCnv_p1 : public T_AthenaPoolTPCnvBase<MissingET, MissingET_p1>
{
    public:
        MissingETCnv_p1() {};
        virtual void persToTrans( const MissingET_p1    *persObj,
                                  MissingET             *transObj,
                                  MsgStream             &msg );

        virtual void transToPers( const MissingET       *transObj,
                                  MissingET_p1          *persObj,
                                  MsgStream             &msg );
};

#endif // RECTPCNV_MISSINGETCNV_P1_H
