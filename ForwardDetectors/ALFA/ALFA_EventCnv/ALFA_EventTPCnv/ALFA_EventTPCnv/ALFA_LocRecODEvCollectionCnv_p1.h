/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ALFA_LocRecEv/ALFA_LocRecODEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEventCnv_p1.h"
#include "ALFA_LocRecODEvCollection_p1.h"


class MsgStream;
 
 class ALFA_LocRecODEvCollectionCnv_p1: public T_AthenaPoolTPCnvConstBase<ALFA_LocRecODEvCollection, ALFA_LocRecODEvCollection_p1> {
   
  public:
   using base_class::persToTrans;
   using base_class::transToPers;

   ALFA_LocRecODEvCollectionCnv_p1() {}
   
   virtual void persToTrans(const ALFA_LocRecODEvCollection_p1* persObj, ALFA_LocRecODEvCollection*   transObj, MsgStream& log) const override;
   virtual void transToPers(const ALFA_LocRecODEvCollection*   transObj, ALFA_LocRecODEvCollection_p1* persObj, MsgStream& log) const override;
 };
 
 template<> class T_TPCnv<ALFA_LocRecODEvCollection, ALFA_LocRecODEvCollection_p1>: public ALFA_LocRecODEvCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
