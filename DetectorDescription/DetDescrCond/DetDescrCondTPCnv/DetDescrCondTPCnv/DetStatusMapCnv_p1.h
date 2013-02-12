/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTPCNV_DETSTATUSMAPCNV_P1_H
#define DETDESCRCONDTPCNV_DETSTATUSMAPCNV_P1_H

#include "DetDescrConditions/DetStatusMap.h"
#include "DetDescrCondTPCnv/DetStatusMap_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class DetStatusMapCnv_p1  : public T_AthenaPoolTPCnvBase<DetStatusMap, DetStatusMap_p1>  {
public:
  DetStatusMapCnv_p1() {}
  virtual void   persToTrans(const DetStatusMap_p1* persObj, DetStatusMap* transObj, MsgStream &log) ;
  virtual void   transToPers(const DetStatusMap* transObj, DetStatusMap_p1* persObj, MsgStream &log) ;

};

#endif // DETDESCRCONDTPCNV_DETSTATUSMAPCNV_P1_H

