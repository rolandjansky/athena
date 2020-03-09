/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETASSOCIATIONBASECNV_P1
#define JETEVENTTPCNV_JETASSOCIATIONBASECNV_P1

#include "JetEventTPCnv/JetAssociationBase_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "JetEvent/JetAssociationBase.h"

class MsgStream;

class JetAssociationBaseCnv_p1 :
public T_AthenaPoolTPAbstractPolyCnvConstBase<JetAssociationBase, JetAssociationBase, JetAssociationBase_p1>  {
  
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  virtual void   persToTrans(const JetAssociationBase_p1* persObj,
			     JetAssociationBase *transObj,
			     MsgStream &log) const override;
  
  virtual void   transToPers(const JetAssociationBase* transObj,
			     JetAssociationBase_p1* persObj,
			     MsgStream &log) const override;
};
#endif

