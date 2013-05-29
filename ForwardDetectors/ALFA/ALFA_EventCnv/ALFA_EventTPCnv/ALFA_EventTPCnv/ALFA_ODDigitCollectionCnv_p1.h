/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "ALFA_RawEv/ALFA_ODDigitCollection.h"
#include "ALFA_EventTPCnv/ALFA_ODDigitCnv_p1.h"
#include "ALFA_ODDigitCollection_p1.h"
 
//typedef T_AthenaHitsVectorCnv< ALFA_DigitCollection, ALFA_DigitCollection_p1, ALFA_DigitCnv_p1 > ALFA_DigitCollectionCnv_p1;

//typedef T_AthenaPoolTPPtrVectorCnv< ALFA_DigitCollection, ALFA_DigitCollection_p1, ALFA_DigitCnv_p1 > ALFA_DigitCollectionCnv_p1;

class MsgStream;
 
 class ALFA_ODDigitCollectionCnv_p1: public T_AthenaPoolTPCnvBase<ALFA_ODDigitCollection, ALFA_ODDigitCollection_p1> {
   
  public:
   
   ALFA_ODDigitCollectionCnv_p1() {}
   
   virtual void persToTrans(const ALFA_ODDigitCollection_p1* persObj, ALFA_ODDigitCollection*   transObj, MsgStream& log);
   virtual void transToPers(const ALFA_ODDigitCollection*   transObj, ALFA_ODDigitCollection_p1* persObj, MsgStream& log);
 };
 
 template<> class T_TPCnv<ALFA_ODDigitCollection, ALFA_ODDigitCollection_p1>: public ALFA_ODDigitCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
