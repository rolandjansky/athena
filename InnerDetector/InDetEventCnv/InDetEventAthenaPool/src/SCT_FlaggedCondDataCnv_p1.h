/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_FlaggedCondDataCnv_p1_h
#define SCT_FlaggedCondDataCnv_p1_h

/*
Transient/Persistent converter for SCT_FlaggedCondData class
*/

#include "SCT_ConditionsData/SCT_FlaggedCondData.h"
#include "InDetEventAthenaPool/SCT_FlaggedCondData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class SCT_FlaggedCondDataCnv_p1 : public T_AthenaPoolTPCnvBase<SCT_FlaggedCondData, SCT_FlaggedCondData_p1>
{
public:
  SCT_FlaggedCondDataCnv_p1() {}
  virtual void persToTrans(const SCT_FlaggedCondData_p1* persObj, SCT_FlaggedCondData* transObj, MsgStream& log);
  virtual void transToPers(const SCT_FlaggedCondData* transObj, SCT_FlaggedCondData_p1* persObj, MsgStream& log);
};

#endif // SCT_FlaggedCondDataCnv_p1_h
