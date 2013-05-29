/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ALFA_GloRecEv/ALFA_GloRecEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_GloRecEventCnv_p1.h"
#include "ALFA_GloRecEvCollection_p1.h"


class MsgStream;
 
 class ALFA_GloRecEvCollectionCnv_p1: public T_AthenaPoolTPCnvBase<ALFA_GloRecEvCollection, ALFA_GloRecEvCollection_p1> {
   
  public:
   
   ALFA_GloRecEvCollectionCnv_p1() {}
   
   virtual void persToTrans(const ALFA_GloRecEvCollection_p1* persObj, ALFA_GloRecEvCollection*   transObj, MsgStream& log);
   virtual void transToPers(const ALFA_GloRecEvCollection*   transObj, ALFA_GloRecEvCollection_p1* persObj, MsgStream& log);
 };
 
 template<> class T_TPCnv<ALFA_GloRecEvCollection, ALFA_GloRecEvCollection_p1>: public ALFA_GloRecEvCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
