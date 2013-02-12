/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTPCNV_DETCONDCFLOATCNV_P1_H
#define DETDESCRCONDTPCNV_DETCONDCFLOATCNV_P1_H

#include "DetDescrConditions/DetCondCFloat.h"
#include "DetDescrCondTPCnv/DetCondCFloat_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class DetCondCFloatCnv_p1  : public T_AthenaPoolTPCnvBase<DetCondCFloat, DetCondCFloat_p1>  {
public:
  DetCondCFloatCnv_p1() {}
  virtual void   persToTrans(const DetCondCFloat_p1* persObj, DetCondCFloat* transObj, MsgStream &log) ;
  virtual void   transToPers(const DetCondCFloat* transObj, DetCondCFloat_p1* persObj, MsgStream &log) ;

};

#endif // DETDESCRCONDTPCNV_DETCONDCFLOATCNV_P1_H

