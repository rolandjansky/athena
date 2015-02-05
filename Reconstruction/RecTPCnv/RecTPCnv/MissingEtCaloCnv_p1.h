/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCALOCNV_P1_H
#define RECTPCNV_MISSINGETCALOCNV_P1_H

/********************************************************************

NAME:     MissingEtCaloCnv_p1.h
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingEtCalo class
          inherits from MissingETCnv_p1
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingEtCalo_p1.h"
#include "MissingETEvent/MissingEtCalo.h"

class MsgStream;

class MissingEtCaloCnv_p1: public T_AthenaPoolTPCnvBase<MissingEtCalo, MissingEtCalo_p1>
{
    public:
            
	MissingEtCaloCnv_p1() {};
        
 	virtual void persToTrans( const MissingEtCalo_p1    *persObj,
                                  MissingEtCalo             *transObj,
                                  MsgStream                 &msg );

        virtual void transToPers( const MissingEtCalo       *transObj,
                                  MissingEtCalo_p1          *persObj,
                                  MsgStream                 &msg );


};


#endif // RECTPCNV_MISSINGETCALOCNV_P1_H
