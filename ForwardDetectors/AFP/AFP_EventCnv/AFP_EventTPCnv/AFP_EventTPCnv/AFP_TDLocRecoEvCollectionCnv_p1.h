/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "AFP_LocRecoEv/AFP_TDLocRecoEvCollection.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEventCnv_p1.h"
#include "AFP_TDLocRecoEvCollection_p1.h"


class MsgStream;
 
 class AFP_TDLocRecoEvCollectionCnv_p1: public T_AthenaPoolTPCnvConstBase<AFP_TDLocRecoEvCollection, AFP_TDLocRecoEvCollection_p1> {
   
  public:
   
   using base_class::transToPers;
   using base_class::persToTrans;
   


   AFP_TDLocRecoEvCollectionCnv_p1() {}
   
   virtual void persToTrans(const AFP_TDLocRecoEvCollection_p1* persObj, AFP_TDLocRecoEvCollection*   transObj, MsgStream& log) const override;
   virtual void transToPers(const AFP_TDLocRecoEvCollection*   transObj, AFP_TDLocRecoEvCollection_p1* persObj, MsgStream& log) const override;
 };
 
 template<> class T_TPCnv<AFP_TDLocRecoEvCollection, AFP_TDLocRecoEvCollection_p1>: public AFP_TDLocRecoEvCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
