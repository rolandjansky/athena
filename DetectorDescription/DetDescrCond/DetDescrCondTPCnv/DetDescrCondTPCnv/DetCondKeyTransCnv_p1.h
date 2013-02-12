/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTPCNV_DETCONDKEYTRANSCNV_P1_H
#define DETDESCRCONDTPCNV_DETCONDKEYTRANSCNV_P1_H

#include "DetDescrConditions/DetCondKeyTrans.h"
#include "DetDescrCondTPCnv/DetCondKeyTrans_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class DetCondKeyTransCnv_p1  : public T_AthenaPoolTPCnvBase<DetCondKeyTrans, DetCondKeyTrans_p1>  {
public:
  DetCondKeyTransCnv_p1() {}
  virtual void   persToTrans(const DetCondKeyTrans_p1* persObj, DetCondKeyTrans* transObj, MsgStream &log) ;
  virtual void   transToPers(const DetCondKeyTrans* transObj, DetCondKeyTrans_p1* persObj, MsgStream &log) ;

};

#endif // DETDESCRCONDTPCNV_DETCONDKEYTRANSCNV_P1_H

