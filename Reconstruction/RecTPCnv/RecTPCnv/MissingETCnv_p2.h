/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCNV_P2_H
#define RECTPCNV_MISSINGETCNV_P2_H

/********************************************************************

NAME:     MissingEtCnv_p2.h
PACKAGE:  offline/Reconstruction/RecTPCnv


PURPOSE:  Transient/Persisten converter for MissingET class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingET_p2.h"
#include "MissingETEvent/MissingET.h"

class MsgStream;

class MissingETCnv_p2 : public T_AthenaPoolTPCnvBase<MissingET, MissingET_p2>
{
    public:
        MissingETCnv_p2() {};
        virtual void persToTrans( const MissingET_p2    *persObj,
                                  MissingET             *transObj,
                                  MsgStream             &msg );

        virtual void transToPers( const MissingET       *transObj,
                                  MissingET_p2          *persObj,
                                  MsgStream             &msg );

		virtual void persToTrans( MissingET* trans, std::vector<float>::const_iterator i);
		virtual void transToPers( const MissingET* trans,  std::vector<float> &all);
};


#endif // RECTPCNV_MISSINGETCNV_P2_H
