/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_RAWTRACKTPCNV_FTK_RAWTRACKCNV_p1_H
#define TRIGFTK_RAWTRACKTPCNV_FTK_RAWTRACKCNV_p1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrack_p1.h"

class MsgStream;

class FTK_RawTrackCnv_p1: public T_AthenaPoolTPCnvBase<FTK_RawTrack, FTK_RawTrack_p1>
{
public:

  FTK_RawTrackCnv_p1() {}

  virtual void persToTrans( const FTK_RawTrack_p1 *, FTK_RawTrack *, MsgStream& );
  virtual void transToPers( const FTK_RawTrack *, FTK_RawTrack_p1 *, MsgStream& );

  enum {th1=0, th2=1, th3=2, th4=3, th5=4,th6=5};
  
};

#endif 
