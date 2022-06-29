/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITCNV_P2_H
#define LUCID_DIGITCNV_P2_H

#include "LUCID_Digit_p2.h"
#include "LUCID_RawEvent/LUCID_Digit.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_DigitCnv_p2: public T_AthenaPoolTPCnvConstBase<LUCID_Digit, LUCID_Digit_p2> {

 public:
  using base_class::persToTrans;
  using base_class::transToPers;

  LUCID_DigitCnv_p2() {}
  
  virtual void persToTrans(const LUCID_Digit_p2* persObj, LUCID_Digit*    transObj, MsgStream& log) const override;
  virtual void transToPers(const LUCID_Digit*   transObj, LUCID_Digit_p2* persObj , MsgStream& log) const override;
};


#endif
