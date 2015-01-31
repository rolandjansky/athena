/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARSHAPESUBSETCNV_P1_H
#define LARCONDTPCNV_LARSHAPESUBSETCNV_P1_H

#include "LArCondTPCnv/LArShapeSubset_p1.h"
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArShapeP1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef LArConditionsSubset<LArShapeP1> LArShapeTransType1;
typedef LArShapeSubset_p1               LArShapePersType1;

class LArShapeSubsetCnv_p1  
    : public T_AthenaPoolTPCnvBase<LArShapeTransType1, LArShapePersType1 >
{
public:

    LArShapeSubsetCnv_p1() {}
    virtual void   persToTrans(const LArShapePersType1* persObj, LArShapeTransType1* transObj, MsgStream &log) ;
    virtual void   transToPers(const LArShapeTransType1* transObj, LArShapePersType1* persObj, MsgStream &log) ;

};

#endif
