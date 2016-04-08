/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINERCNV_P3_H
#define EFLOWOBJECTCONTAINERCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "eflowEvent/eflowObjectContainer.h"
#include "eflowEventTPCnv/eflowObjectContainer_p3.h"
#include "eflowEventTPCnv/eflowObjectCnv_p3.h"

class MsgStream;
class eflowObject;
class eflowObject_p3;
class eflowObjectCnv_p3;

class eflowObjectContainerCnv_p3 : public T_AthenaPoolTPCnvBase< eflowObjectContainer, eflowObjectContainer_p3 > 
{

 public:

  eflowObjectContainerCnv_p3 () {};

  virtual void persToTrans( const eflowObjectContainer_p3 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream );
  virtual void transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p3 *persObj, MsgStream &eflowStream );

  friend class eflowObjectContainer;
  friend class eflowObjectContainer_p3;

 private:
  T_AthenaPoolTPCnvVector< DataVector<eflowObject>, std::vector<eflowObject_p3>, eflowObjectCnv_p3> m_vectorConv;

};

template<> class T_TPCnv<eflowObjectContainer, eflowObjectContainer_p3>  : public eflowObjectContainerCnv_p3
{
 public:
};

#endif


