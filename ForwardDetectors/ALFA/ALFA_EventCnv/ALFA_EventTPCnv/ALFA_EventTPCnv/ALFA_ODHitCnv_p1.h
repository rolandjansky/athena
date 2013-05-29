/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODHitCnv_p1_h
#define ALFA_ODHitCnv_p1_h


#include "ALFA_SimEv/ALFA_ODHit.h"
#include "ALFA_EventTPCnv/ALFA_ODHit_p1.h"


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_ODHitCnv_p1
    : public T_AthenaPoolTPCnvBase< ALFA_ODHit, ALFA_ODHit_p1> 
{
public:
  ALFA_ODHitCnv_p1() {}
  
  virtual void persToTrans( const ALFA_ODHit_p1* persObj, ALFA_ODHit* transObj, MsgStream &log);
  virtual void transToPers( const ALFA_ODHit* transObj, ALFA_ODHit_p1* persObj, MsgStream &log);
 
};
 
 
#endif
