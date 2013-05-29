/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEventCnv_p1.h"
#include "ALFA_LocRecCorrODEvCollection_p1.h"


class MsgStream;
 
 class ALFA_LocRecCorrODEvCollectionCnv_p1: public T_AthenaPoolTPCnvBase<ALFA_LocRecCorrODEvCollection, ALFA_LocRecCorrODEvCollection_p1> {
   
  public:
   
   ALFA_LocRecCorrODEvCollectionCnv_p1() {}
   
   virtual void persToTrans(const ALFA_LocRecCorrODEvCollection_p1* persObj, ALFA_LocRecCorrODEvCollection*   transObj, MsgStream& log);
   virtual void transToPers(const ALFA_LocRecCorrODEvCollection*   transObj, ALFA_LocRecCorrODEvCollection_p1* persObj, MsgStream& log);
 };
 
 template<> class T_TPCnv<ALFA_LocRecCorrODEvCollection, ALFA_LocRecCorrODEvCollection_p1>: public ALFA_LocRecCorrODEvCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
