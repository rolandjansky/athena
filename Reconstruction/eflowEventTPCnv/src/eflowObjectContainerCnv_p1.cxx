/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowEventTPCnv/eflowObjectCnv_p1.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p1.h"
#include "AthLinks/ElementLinkVector.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

void eflowObjectContainerCnv_p1::persToTrans( const eflowObjectContainer_p1 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream ){

  transObj->setMissET (persObj->m_misset);
  transObj->setSumET  (persObj->m_sumet);
  transObj->setpTx    (persObj->m_ptx);
  transObj->setpTy    (persObj->m_pty);
  transObj->setNumEf  (persObj->m_numef);
  
  transObj->setNumPhot (persObj->m_numphot);
  transObj->setNumEle (persObj->m_numele);
  transObj->setNumMuo (persObj->m_nummuo);
  
  transObj->setCircularity (persObj->m_circularity);
  transObj->setThrust (persObj->m_thrust);
  transObj->setOblateness (persObj->m_oblateness);
  
  transObj->set_valid (persObj->m_valid);

  m_vectorConv.persToTrans(&persObj->m_myPersVector,transObj,eflowStream);

}

void eflowObjectContainerCnv_p1::transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p1 *persObj, MsgStream &eflowStream ){

  persObj->m_misset = transObj->missET();

  persObj->m_sumet  = transObj->sumET();
  persObj->m_ptx    = transObj->pTx();
  persObj->m_pty    = transObj->pTy();
  persObj->m_numef  = transObj->numEf();
  
  persObj->m_numphot = transObj->numPhot();
  persObj->m_numele = transObj->numEle();
  persObj->m_nummuo = transObj->numMuo();
  
  persObj->m_circularity = transObj->circularity();
  persObj->m_thrust = transObj->thrust();
  persObj->m_oblateness = transObj->oblateness();
  
  persObj->m_valid = transObj->isValid();

  m_vectorConv.transToPers(transObj,&persObj->m_myPersVector,eflowStream);

}
