/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#define private public
#define protected public
#include "eflowEventTPCnv/eflowObjectContainerCnv_p3.h"
#include "DataModel/ElementLinkVector.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#undef private
#undef protected

void eflowObjectContainerCnv_p3::persToTrans( const eflowObjectContainer_p3 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream ){

  transObj->m_misset = -999;
  transObj->m_sumet  = -999;
  transObj->m_ptx    = -999;
  transObj->m_pty    = -999;
  transObj->m_numef  = -9;
  
  transObj->m_numphot = -9;
  transObj->m_numele = -9;
  transObj->m_nummuo = -9;
  
  transObj->m_circularity = -999;
  transObj->m_thrust = -999;
  transObj->m_oblateness = -999;
  
  transObj->m_valid = 1;

  m_vectorConv.persToTrans(&persObj->m_myPersVector,transObj,eflowStream);

}

void eflowObjectContainerCnv_p3::transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p3 *persObj, MsgStream &eflowStream ){

  m_vectorConv.transToPers(transObj,&persObj->m_myPersVector,eflowStream);

}
