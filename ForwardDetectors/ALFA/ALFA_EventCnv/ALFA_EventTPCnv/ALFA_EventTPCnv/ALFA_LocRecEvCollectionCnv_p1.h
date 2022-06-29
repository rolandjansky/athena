/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ALFA_LocRecEv/ALFA_LocRecEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEventCnv_p1.h"
#include "ALFA_LocRecEvCollection_p1.h"


class MsgStream;
 
 class ALFA_LocRecEvCollectionCnv_p1: public T_AthenaPoolTPCnvConstBase<ALFA_LocRecEvCollection, ALFA_LocRecEvCollection_p1> {
   
  public:
   using base_class::persToTrans;
   using base_class::transToPers;

   ALFA_LocRecEvCollectionCnv_p1() {}
   
   virtual void persToTrans(const ALFA_LocRecEvCollection_p1* persObj, ALFA_LocRecEvCollection*   transObj, MsgStream& log) const override;
   virtual void transToPers(const ALFA_LocRecEvCollection*   transObj, ALFA_LocRecEvCollection_p1* persObj, MsgStream& log) const override;
 };
 
 template<> class T_TPCnv<ALFA_LocRecEvCollection, ALFA_LocRecEvCollection_p1>: public ALFA_LocRecEvCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
