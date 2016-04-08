/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowEventTPCnv/eflowObjectCnv_p3.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p3.h"
#include "AthLinks/ElementLinkVector.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

void eflowObjectContainerCnv_p3::persToTrans( const eflowObjectContainer_p3 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream ){

  transObj->setMissET (-999);
  transObj->setSumET  (-999);
  transObj->setpTx    (-999);
  transObj->setpTy    (-999);
  transObj->setNumEf  (-9);

  transObj->setNumPhot (-9);
  transObj->setNumEle (-9);
  transObj->setNumMuo (-9);

  transObj->setCircularity (-999);
  transObj->setThrust (-999);
  transObj->setOblateness (-999);

  transObj->set_valid (1);

  m_vectorConv.persToTrans(&persObj->m_myPersVector,transObj,eflowStream);

}

void eflowObjectContainerCnv_p3::transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p3 *persObj, MsgStream &eflowStream ){

  m_vectorConv.transToPers(transObj,&persObj->m_myPersVector,eflowStream);

}
