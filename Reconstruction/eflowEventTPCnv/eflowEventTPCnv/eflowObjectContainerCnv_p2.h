/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINERCNV_P2_H
#define EFLOWOBJECTCONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "eflowEvent/eflowObjectContainer.h"
#include "eflowEventTPCnv/eflowObjectContainer_p2.h"
#include "eflowEventTPCnv/eflowObjectCnv_p2.h"

class MsgStream;
class eflowObject;
class eflowObject_p2;
class eflowObjectCnv_p2;

class eflowObjectContainerCnv_p2 : public T_AthenaPoolTPCnvBase< eflowObjectContainer, eflowObjectContainer_p2 > 
{

 public:

  eflowObjectContainerCnv_p2 () {};

  virtual void persToTrans( const eflowObjectContainer_p2 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream );
  virtual void transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p2 *persObj, MsgStream &eflowStream );

  friend class eflowObjectContainer;
  friend class eflowObjectContainer_p2;

 private:
  T_AthenaPoolTPCnvVector< DataVector<eflowObject>, std::vector<eflowObject_p2>, eflowObjectCnv_p2> m_vectorConv;

};

template<> class T_TPCnv<eflowObjectContainer, eflowObjectContainer_p2>  : public eflowObjectContainerCnv_p2
{
 public:
};

#endif


