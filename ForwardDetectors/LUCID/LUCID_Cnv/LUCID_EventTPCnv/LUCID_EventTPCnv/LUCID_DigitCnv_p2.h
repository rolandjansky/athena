/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITCNV_P2_H
#define LUCID_DIGITCNV_P2_H

#include "LUCID_Digit_p2.h"
#include "LUCID_RawEvent/LUCID_Digit.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_DigitCnv_p2: public T_AthenaPoolTPCnvBase<LUCID_Digit, LUCID_Digit_p2> {

 public:
  
  LUCID_DigitCnv_p2() {}
  
  virtual void persToTrans(const LUCID_Digit_p2* persObj, LUCID_Digit*    transObj, MsgStream& log);
  virtual void transToPers(const LUCID_Digit*   transObj, LUCID_Digit_p2* persObj , MsgStream& log);
};


#endif
