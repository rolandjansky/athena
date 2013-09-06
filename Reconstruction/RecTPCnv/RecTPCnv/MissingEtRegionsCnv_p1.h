/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class MissingEtRegionsCnv_p1 : public T_AthenaPoolTPCnvBase<MissingEtRegions, MissingEtRegions_p1>
{
    public:
        MissingEtRegionsCnv_p1() {};
        virtual void persToTrans( const MissingEtRegions_p1    *persObj,
                                  MissingEtRegions             *transObj,
                                  MsgStream                 &msg );

        virtual void transToPers( const MissingEtRegions       *transObj,
                                  MissingEtRegions_p1          *persObj,
                                  MsgStream                 &msg );
};

#endif // RECTPCNV_MissingEtRegionsCNV_P1_H
