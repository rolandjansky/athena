/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTestDataCnv_p0_h
#define FTKTestDataCnv_p0_h

#include "TrigFTKPool/FTKTestData_p0.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class FTKTestData;

class FTKTestDataCnv_p0 : public T_AthenaPoolTPCnvBase<FTKTestData, FTKTestData_p0> {
public:
  FTKTestDataCnv_p0() {;}

  virtual void persToTrans(const FTKTestData_p0 *persObj, FTKTestData *transObj, MsgStream&);
  virtual void transToPers(const FTKTestData *transObj, FTKTestData_p0 *persObj, MsgStream&);
};
#endif // FTKTestDataCnv_p0_h
