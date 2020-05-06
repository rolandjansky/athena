/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACNV_P3_H
#define INDETSIMDATACNV_P3_H

/*
  Transient/Persistent converter for InDetSimData class
  Author: Davide Costanzo
*/

#include "InDetSimData/InDetSimData.h"
#include "InDetEventAthenaPool/InDetSimData_p3.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class IProxyDict;


class InDetSimDataCnv_p3  : public T_AthenaPoolTPCnvBase<InDetSimData, InDetSimData_p3>
{
public:

  InDetSimDataCnv_p3();
  virtual void          persToTrans(const InDetSimData_p3* persObj, InDetSimData* transObj, MsgStream &log);
  virtual void          transToPers(const InDetSimData* transObj, InDetSimData_p3* persObj, MsgStream &log);
  void setCurrentStore (IProxyDict* store);

private:
  IProxyDict* m_sg;
};


#endif


