/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINERCNV_P1_H
#define EFLOWOBJECTCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "eflowEvent/eflowObjectContainer.h"
#include "eflowEventTPCnv/eflowObjectContainer_p1.h"
#include "eflowEventTPCnv/eflowObjectCnv_p1.h"

class MsgStream;
class eflowObject;
class eflowObject_p1;
class eflowObjectCnv_p1;

class eflowObjectContainerCnv_p1 : public T_AthenaPoolTPCnvBase< eflowObjectContainer, eflowObjectContainer_p1 > 
{

 public:

  eflowObjectContainerCnv_p1 () {};

  virtual void persToTrans( const eflowObjectContainer_p1 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream );
  virtual void transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p1 *persObj, MsgStream &eflowStream );

  friend class eflowObjectContainer;
  friend class eflowObjectContainer_p1;

 private:
  T_AthenaPoolTPCnvVector< DataVector<eflowObject>, std::vector<eflowObject_p1>, eflowObjectCnv_p1> m_vectorConv;

};

template<> class T_TPCnv<eflowObjectContainer, eflowObjectContainer_p1>  : public eflowObjectContainerCnv_p1
{
 public:
};

#endif


