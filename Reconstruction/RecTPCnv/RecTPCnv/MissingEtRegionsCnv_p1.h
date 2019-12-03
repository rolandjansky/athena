/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETREGIONSCNV_P1_H
#define RECTPCNV_MISSINGETREGIONSCNV_P1_H

/********************************************************************

NAME:     MissingEtRegionsCnv_p1.h
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for MissingEtRegions class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingEtRegions_p1.h"
#include "MissingETEvent/MissingEtRegions.h"

class MsgStream;

class MissingEtRegionsCnv_p1 : public T_AthenaPoolTPCnvConstBase<MissingEtRegions, MissingEtRegions_p1>
{
    public:
        using base_class::transToPers;
        using base_class::persToTrans;


        MissingEtRegionsCnv_p1() {};
        virtual void persToTrans( const MissingEtRegions_p1    *persObj,
                                  MissingEtRegions             *transObj,
                                  MsgStream                 &msg ) const override;

        virtual void transToPers( const MissingEtRegions       *transObj,
                                  MissingEtRegions_p1          *persObj,
                                  MsgStream                 &msg ) const override;
};

#endif // RECTPCNV_MissingEtRegionsCNV_P1_H
