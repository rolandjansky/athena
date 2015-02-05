/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETTRUTHCNV_P2_H
#define RECTPCNV_MISSINGETTRUTHCNV_P2_H

/********************************************************************

NAME:     MissingEtTruthCnv_p2.h
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  I. Vukotic
CREATED:  May 2009

PURPOSE:  Transient/Persisten converter for MissingEtTruth class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingEtTruth_p2.h"
#include "MissingETEvent/MissingEtTruth.h"

class MsgStream;

class MissingEtTruthCnv_p2 : public T_AthenaPoolTPCnvBase<MissingEtTruth, MissingEtTruth_p2>{
    public:
        MissingEtTruthCnv_p2() {};
        virtual void persToTrans( const MissingEtTruth_p2    *persObj,
                                  MissingEtTruth             *transObj,
                                  MsgStream                  &msg );

        virtual void transToPers( const MissingEtTruth       *transObj,
                                  MissingEtTruth_p2          *persObj,
                                  MsgStream                  &msg );
};


#endif // RECTPCNV_MISSINGETTRUTHCNV_P1_H
