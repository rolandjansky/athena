/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_EventTPCnv/ALFA_DigitCnv_p1.h"
#include "ALFA_DigitCollection_p1.h"
 
//typedef T_AthenaHitsVectorCnv< ALFA_DigitCollection, ALFA_DigitCollection_p1, ALFA_DigitCnv_p1 > ALFA_DigitCollectionCnv_p1;

//typedef T_AthenaPoolTPPtrVectorCnv< ALFA_DigitCollection, ALFA_DigitCollection_p1, ALFA_DigitCnv_p1 > ALFA_DigitCollectionCnv_p1;

class MsgStream;
 
 class ALFA_DigitCollectionCnv_p1: public T_AthenaPoolTPCnvConstBase<ALFA_DigitCollection, ALFA_DigitCollection_p1> {
   
  public:
   using base_class::persToTrans;
   using base_class::transToPers;

   ALFA_DigitCollectionCnv_p1() {}
   
   virtual void persToTrans(const ALFA_DigitCollection_p1* persObj, ALFA_DigitCollection*   transObj, MsgStream& log) const override;
   virtual void transToPers(const ALFA_DigitCollection*   transObj, ALFA_DigitCollection_p1* persObj, MsgStream& log) const override;
 };
 
 template<> class T_TPCnv<ALFA_DigitCollection, ALFA_DigitCollection_p1>: public ALFA_DigitCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
