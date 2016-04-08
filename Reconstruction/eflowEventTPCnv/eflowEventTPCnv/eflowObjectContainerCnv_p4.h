/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINERCNV_P4_H
#define EFLOWOBJECTCONTAINERCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "eflowEvent/eflowObjectContainer.h"
#include "eflowEventTPCnv/eflowObjectContainer_p4.h"
#include "eflowEventTPCnv/eflowObjectCnv_p4.h"

class MsgStream;
class eflowObject;
class eflowObject_p4;
class eflowObjectCnv_p4;

class eflowObjectContainerCnv_p4 : public T_AthenaPoolTPCnvBase< eflowObjectContainer, eflowObjectContainer_p4 > 
{

 public:

  eflowObjectContainerCnv_p4 () {};

  virtual void persToTrans( const eflowObjectContainer_p4 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream );
  virtual void transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p4 *persObj, MsgStream &eflowStream );

  friend class eflowObjectContainer;
  friend class eflowObjectContainer_p4;

 private:
  T_AthenaPoolTPCnvVector< DataVector<eflowObject>, std::vector<eflowObject_p4>, eflowObjectCnv_p4> m_vectorConv;

};

template<> class T_TPCnv<eflowObjectContainer, eflowObjectContainer_p4>  : public eflowObjectContainerCnv_p4
{
 public:
};

#endif


