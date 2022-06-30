/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DigitCnv_p1_h
#define ALFA_DigitCnv_p1_h

#include "ALFA_EventTPCnv/ALFA_Digit_p1.h"
#include "ALFA_RawEv/ALFA_Digit.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_DigitCnv_p1
    : public T_AthenaPoolTPCnvConstBase< ALFA_Digit, ALFA_Digit_p1>
{
public:
  using base_class::persToTrans;
  using base_class::transToPers;

  ALFA_DigitCnv_p1() {}
  
  virtual void persToTrans( const ALFA_Digit_p1* persObj, ALFA_Digit* transObj, MsgStream &log) const override;
  virtual void transToPers( const ALFA_Digit* transObj, ALFA_Digit_p1* persObj, MsgStream &log) const override;
 
};
 
 
#endif
