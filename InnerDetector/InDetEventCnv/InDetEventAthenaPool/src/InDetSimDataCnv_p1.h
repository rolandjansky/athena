/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACNV_P1_H
#define INDETSIMDATACNV_P1_H

/*
Transient/Persistent converter for InDetSimData class
Author: Davide Costanzo
*/

#include "InDetSimData/InDetSimData.h"
#include "InDetEventAthenaPool/InDetSimData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class IProxyDict;


class InDetSimDataCnv_p1  : public T_AthenaPoolTPCnvBase<InDetSimData, InDetSimData_p1>
{
public:

  InDetSimDataCnv_p1();
  virtual void          persToTrans(const InDetSimData_p1* persObj, InDetSimData* transObj, MsgStream &log);
  virtual void          transToPers(const InDetSimData* transObj, InDetSimData_p1* persObj, MsgStream &log);
  void setCurrentStore (IProxyDict* store);

private:
  IProxyDict* m_sg;
};


#endif


