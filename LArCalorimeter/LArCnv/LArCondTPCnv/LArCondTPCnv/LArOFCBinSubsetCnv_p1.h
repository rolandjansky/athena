/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDTPCNV_LAROFCBINSUBSETCNV_P1_H
#define LARCONDTPCNV_LAROFCBINSUBSETCNV_P1_H

#include "LArRawConditions/LArConditionsSubset.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArCondTPCnv/LArSingleIntSubset_p1.h"
#include "LArRawConditions/LArOFCBinP.h"

class MsgStream;

typedef LArConditionsSubset<LArOFCBinP> OFCBinTransType;
typedef LArSingleIntSubset_p1           OFCBinPersType;

class LArOFCBinSubsetCnv_p1
    : public T_AthenaPoolTPCnvConstBase<OFCBinTransType,OFCBinPersType>
{
public:
    using base_class::transToPers;
    using base_class::persToTrans;

    LArOFCBinSubsetCnv_p1() {}
    virtual void   persToTrans(const OFCBinPersType* persObj, OFCBinTransType* transObj, MsgStream &log) const override;
    virtual void   transToPers(const OFCBinTransType* transObj, OFCBinPersType* persObj, MsgStream &log) const override;
};
    
  

#endif
