/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCALOCNV_P2_H
#define RECTPCNV_MISSINGETCALOCNV_P2_H

/********************************************************************
NAME:     MissingEtCaloCnv_p2.h
PURPOSE:  Transient/Persisten converter for MissingEtCalo class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingEtCalo_p2.h"
#include "MissingETEvent/MissingEtCalo.h"

class MsgStream;

class MissingEtCaloCnv_p2: public T_AthenaPoolTPCnvBase<MissingEtCalo, MissingEtCalo_p2>
{
    public:
            
	MissingEtCaloCnv_p2() {};
        
 	virtual void persToTrans( const MissingEtCalo_p2    *persObj,
                                  MissingEtCalo             *transObj,
                                  MsgStream                 &msg );

        virtual void transToPers( const MissingEtCalo       *transObj,
                                  MissingEtCalo_p2          *persObj,
                                  MsgStream                 &msg );


};


#endif // RECTPCNV_MISSINGETCALOCNV_P2_H
