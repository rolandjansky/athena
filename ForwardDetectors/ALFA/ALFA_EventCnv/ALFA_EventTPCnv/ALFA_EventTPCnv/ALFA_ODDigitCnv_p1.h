/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODDigitCnv_p1_h
#define ALFA_ODDigitCnv_p1_h

#include "ALFA_EventTPCnv/ALFA_ODDigit_p1.h"
#include "ALFA_RawEv/ALFA_ODDigit.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_ODDigitCnv_p1
    : public T_AthenaPoolTPCnvBase< ALFA_ODDigit, ALFA_ODDigit_p1> 
{
public:
  ALFA_ODDigitCnv_p1() {}
  
  virtual void persToTrans( const ALFA_ODDigit_p1* persObj, ALFA_ODDigit* transObj, MsgStream &log);
  virtual void transToPers( const ALFA_ODDigit* transObj, ALFA_ODDigit_p1* persObj, MsgStream &log);
 
};
 
 
#endif
