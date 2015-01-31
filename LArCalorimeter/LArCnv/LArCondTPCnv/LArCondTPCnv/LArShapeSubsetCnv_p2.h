/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDTPCNV_LARSHAPESUBSETCNV_P2_H
#define LARCONDTPCNV_LARSHAPESUBSETCNV_P2_H

#include "LArCondTPCnv/LArShapeSubset_p2.h"
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArShapeP2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef LArConditionsSubset<LArShapeP2> LArShapeTransType2;
typedef LArShapeSubset_p2               LArShapePersType2;

class LArShapeSubsetCnv_p2  
    : public T_AthenaPoolTPCnvBase<LArShapeTransType2, LArShapePersType2 >
{
public:

    LArShapeSubsetCnv_p2() {}
    virtual void   persToTrans(const LArShapePersType2* persObj, LArShapeTransType2* transObj, MsgStream &log) ;
    virtual void   transToPers(const LArShapeTransType2* transObj, LArShapePersType2* persObj, MsgStream &log) ;

};

#endif
