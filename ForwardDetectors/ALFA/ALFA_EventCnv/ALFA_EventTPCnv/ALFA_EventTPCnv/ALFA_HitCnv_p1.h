/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_HitCnv_p1_h
#define ALFA_HitCnv_p1_h


#include "ALFA_SimEv/ALFA_Hit.h"
#include "ALFA_EventTPCnv/ALFA_Hit_p1.h"


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_HitCnv_p1
    : public T_AthenaPoolTPCnvBase< ALFA_Hit, ALFA_Hit_p1> 
{
public:
  ALFA_HitCnv_p1() {}
  
  virtual void persToTrans( const ALFA_Hit_p1* persObj, ALFA_Hit* transObj, MsgStream &log);
  virtual void transToPers( const ALFA_Hit* transObj, ALFA_Hit_p1* persObj, MsgStream &log);
 
};
 
 
#endif
