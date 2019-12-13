/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

class MissingEtCaloCnv_p1: public T_AthenaPoolTPCnvConstBase<MissingEtCalo, MissingEtCalo_p1>
{
    public:
        using base_class::transToPers;
        using base_class::persToTrans;
            
	MissingEtCaloCnv_p1() {};
        
 	virtual void persToTrans( const MissingEtCalo_p1    *persObj,
                                  MissingEtCalo             *transObj,
                                  MsgStream                 &msg ) const override;

        virtual void transToPers( const MissingEtCalo       *transObj,
                                  MissingEtCalo_p1          *persObj,
                                  MsgStream                 &msg ) const override;
};


#endif // RECTPCNV_MISSINGETCALOCNV_P1_H
