/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "eflowEventTPCnv/eflowObjectContainerCnv_p1.h"
#include "DataModel/ElementLinkVector.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#undef private
#undef protected

void eflowObjectContainerCnv_p1::persToTrans( const eflowObjectContainer_p1 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream ){

  transObj->m_misset = persObj->m_misset;
  transObj->m_sumet  = persObj->m_sumet;
  transObj->m_ptx    = persObj->m_ptx;
  transObj->m_pty    = persObj->m_pty;
  transObj->m_numef  = persObj->m_numef;
  
  transObj->m_numphot = persObj->m_numphot;
  transObj->m_numele = persObj->m_numele;
  transObj->m_nummuo = persObj->m_nummuo;
  
  transObj->m_circularity = persObj->m_circularity;
  transObj->m_thrust = persObj->m_thrust;
  transObj->m_oblateness = persObj->m_oblateness;
  
  transObj->m_valid = persObj->m_valid;

  m_vectorConv.persToTrans(&persObj->m_myPersVector,transObj,eflowStream);

}

void eflowObjectContainerCnv_p1::transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p1 *persObj, MsgStream &eflowStream ){

  persObj->m_misset = transObj->m_misset;

  persObj->m_sumet  = transObj->m_sumet;
  persObj->m_ptx    = transObj->m_ptx;
  persObj->m_pty    = transObj->m_pty;
  persObj->m_numef  = transObj->m_numef;
  
  persObj->m_numphot = transObj->m_numphot;
  persObj->m_numele = transObj->m_numele;
  persObj->m_nummuo = transObj->m_nummuo;
  
  persObj->m_circularity = transObj->m_circularity;
  persObj->m_thrust = transObj->m_thrust;
  persObj->m_oblateness = transObj->m_oblateness;
  
  persObj->m_valid = transObj->m_valid;

  m_vectorConv.transToPers(transObj,&persObj->m_myPersVector,eflowStream);

}
