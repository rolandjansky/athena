/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEventCnv_p1.h"
#include "ALFA_LocRecCorrEvCollection_p1.h"


class MsgStream;
 
 class ALFA_LocRecCorrEvCollectionCnv_p1: public T_AthenaPoolTPCnvConstBase<ALFA_LocRecCorrEvCollection, ALFA_LocRecCorrEvCollection_p1> {
   
  public:
   using base_class::persToTrans;
   using base_class::transToPers;

   ALFA_LocRecCorrEvCollectionCnv_p1() {}
   
   virtual void persToTrans(const ALFA_LocRecCorrEvCollection_p1* persObj, ALFA_LocRecCorrEvCollection*   transObj, MsgStream& log) const override;
   virtual void transToPers(const ALFA_LocRecCorrEvCollection*   transObj, ALFA_LocRecCorrEvCollection_p1* persObj, MsgStream& log) const override;
 };
 
 template<> class T_TPCnv<ALFA_LocRecCorrEvCollection, ALFA_LocRecCorrEvCollection_p1>: public ALFA_LocRecCorrEvCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
