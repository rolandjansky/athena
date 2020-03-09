/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETTRUTHCNV_P1_H
#define RECTPCNV_MISSINGETTRUTHCNV_P1_H

/********************************************************************

NAME:     MissingEtTruthCnv_p1.h
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingEtTruth class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingEtTruth_p1.h"
#include "MissingETEvent/MissingEtTruth.h"

class MsgStream;

class MissingEtTruthCnv_p1 : public T_AthenaPoolTPCnvConstBase<MissingEtTruth, MissingEtTruth_p1>
{
public:
        using base_class::transToPers;
        using base_class::persToTrans;


        MissingEtTruthCnv_p1() {};
        virtual void persToTrans( const MissingEtTruth_p1    *persObj,
                                  MissingEtTruth             *transObj,
                                  MsgStream                  &msg ) const override;

        virtual void transToPers( const MissingEtTruth       *transObj,
                                  MissingEtTruth_p1          *persObj,
                                  MsgStream                  &msg ) const override;
};


#endif // RECTPCNV_MISSINGETTRUTHCNV_P1_H
