/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCONTAINERCNV_P5_H
#define EFLOWOBJECTCONTAINERCNV_P5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "eflowEvent/eflowObjectContainer.h"
#include "eflowEventTPCnv/eflowObjectContainer_p5.h"
#include "eflowEventTPCnv/eflowObjectCnv_p5.h"

class MsgStream;
class eflowObject;
class eflowObject_p5;
class eflowObjectCnv_p5;

class eflowObjectContainerCnv_p5 : public T_AthenaPoolTPCnvBase< eflowObjectContainer, eflowObjectContainer_p5 > 
{

 public:

  eflowObjectContainerCnv_p5 () {};

  virtual void persToTrans( const eflowObjectContainer_p5 *persObj, eflowObjectContainer *transObj, MsgStream &eflowStream );
  virtual void transToPers( const eflowObjectContainer *transObj,eflowObjectContainer_p5 *persObj, MsgStream &eflowStream );

  friend class eflowObjectContainer;
  friend class eflowObjectContainer_p5;

 private:
  T_AthenaPoolTPCnvVector< DataVector<eflowObject>, std::vector<eflowObject_p5>, eflowObjectCnv_p5> m_vectorConv;

};

template<> class T_TPCnv<eflowObjectContainer, eflowObjectContainer_p5>  : public eflowObjectContainerCnv_p5
{
 public:
};

#endif


