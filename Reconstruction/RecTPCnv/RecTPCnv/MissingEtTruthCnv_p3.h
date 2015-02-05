/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETTRUTHCNV_P3_H
#define RECTPCNV_MISSINGETTRUTHCNV_P3_H

/********************************************************************
NAME:     MissingEtTruthCnv_p3.h
PACKAGE:  offline/Reconstruction/RecTPCnv
PURPOSE:  Transient/Persisten converter for MissingEtTruth class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingEtTruth_p3.h"
#include "MissingETEvent/MissingEtTruth.h"

class MsgStream;

class MissingEtTruthCnv_p3 : public T_AthenaPoolTPCnvBase<MissingEtTruth, MissingEtTruth_p3>{
    public:
        MissingEtTruthCnv_p3() {};
        virtual void persToTrans( const MissingEtTruth_p3    *persObj,
                                  MissingEtTruth             *transObj,
                                  MsgStream                  &msg );

        virtual void transToPers( const MissingEtTruth       *transObj,
                                  MissingEtTruth_p3          *persObj,
                                  MsgStream                  &msg );
};


#endif
