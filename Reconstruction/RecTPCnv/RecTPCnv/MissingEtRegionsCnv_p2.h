/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETREGIONSCNV_P2_H
#define RECTPCNV_MISSINGETREGIONSCNV_P2_H

/********************************************************************

NAME:     MissingEtRegionsCnv_p2.h
PACKAGE:  offline/Reconstruction/RecTPCnv

PURPOSE:  Transient/Persisten converter for MissingEtRegions class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RecTPCnv/MissingEtRegions_p2.h"
#include "MissingETEvent/MissingEtRegions.h"

class MsgStream;

class MissingEtRegionsCnv_p2 : public T_AthenaPoolTPCnvConstBase<MissingEtRegions, MissingEtRegions_p2>
{
    public:
        using base_class::transToPers;
        using base_class::persToTrans;


        MissingEtRegionsCnv_p2() {};
        virtual void persToTrans( const MissingEtRegions_p2    *persObj,
                                  MissingEtRegions             *transObj,
                                  MsgStream                 &msg ) const override;

        virtual void transToPers( const MissingEtRegions       *transObj,
                                  MissingEtRegions_p2          *persObj,
                                  MsgStream                 &msg ) const override;

        void transToPers( const MissingEtRegions  *transObj, std::vector<float> &all ) const;
        void persToTrans(  MissingEtRegions* transObj, std::vector<float>::const_iterator i) const;
};

#endif // RECTPCNV_MissingEtRegionsCNV_P2_H
