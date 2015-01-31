/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCONDTPCNV_LAROFCBINSUBSETCNV_P1_H
#define LARCONDTPCNV_LAROFCBINSUBSETCNV_P1_H

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArCondTPCnv/LArSingleIntSubset_p1.h"
#include "LArRawConditions/LArOFCBinP.h"

class MsgStream;

typedef LArConditionsSubset<LArOFCBinP> OFCBinTransType;
typedef LArSingleIntSubset_p1           OFCBinPersType;

class LArOFCBinSubsetCnv_p1
    : public T_AthenaPoolTPCnvBase<OFCBinTransType,OFCBinPersType>
{
public:

    LArOFCBinSubsetCnv_p1() {}
    virtual void   persToTrans(const OFCBinPersType* persObj, OFCBinTransType* transObj, MsgStream &log) ;
    virtual void   transToPers(const OFCBinTransType* transObj, OFCBinPersType* persObj, MsgStream &log) ;
};
    
  

#endif
