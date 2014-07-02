/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETASSOCIATIONBASECNV_P1
#define JETEVENTTPCNV_JETASSOCIATIONBASECNV_P1

#include "JetEventTPCnv/JetAssociationBase_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetEvent/JetAssociationBase.h"

class MsgStream;

class JetAssociationBaseCnv_p1 :
public T_AthenaPoolTPAbstractPolyCnvBase<JetAssociationBase, JetAssociationBase, JetAssociationBase_p1>  {
  
 public:
  virtual void   persToTrans(const JetAssociationBase_p1* persObj,
			     JetAssociationBase *transObj,
			     MsgStream &log);
  
  virtual void   transToPers(const JetAssociationBase* transObj,
			     JetAssociationBase_p1* persObj,
			     MsgStream &log);
};
#endif

