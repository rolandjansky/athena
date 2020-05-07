/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "AFP_LocRecoEv/AFP_SIDLocRecoEvCollection.h"
#include "AFP_EventTPCnv/AFP_SIDLocRecoEventCnv_p1.h"
#include "AFP_SIDLocRecoEvCollection_p1.h"


class MsgStream;
 
 class AFP_SIDLocRecoEvCollectionCnv_p1: public T_AthenaPoolTPCnvConstBase<AFP_SIDLocRecoEvCollection, AFP_SIDLocRecoEvCollection_p1> {
   
  public:
   
   
      using base_class::persToTrans;
      using base_class::transToPers;

   AFP_SIDLocRecoEvCollectionCnv_p1() {}
   
    virtual void persToTrans(const AFP_SIDLocRecoEvCollection_p1* persObj, AFP_SIDLocRecoEvCollection*   transObj, MsgStream& log) const override;
    virtual void transToPers(const AFP_SIDLocRecoEvCollection*   transObj, AFP_SIDLocRecoEvCollection_p1* persObj, MsgStream& log)  const override;
 };
 
 template<> class T_TPCnv<AFP_SIDLocRecoEvCollection, AFP_SIDLocRecoEvCollection_p1>: public AFP_SIDLocRecoEvCollectionCnv_p1 {
   
  public:
 };
 
//#endif 
