/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACNV_P2_H
#define INDETSIMDATACNV_P2_H

/*
  Transient/Persistent converter for InDetSimData class
  Author: Davide Costanzo
*/

#include "InDetSimData/InDetSimData.h"
#include "InDetEventAthenaPool/InDetSimData_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class IProxyDict;


class InDetSimDataCnv_p2  : public T_AthenaPoolTPCnvBase<InDetSimData, InDetSimData_p2>
{
public:

  InDetSimDataCnv_p2();
  virtual void          persToTrans(const InDetSimData_p2* persObj, InDetSimData* transObj, MsgStream &log);
  virtual void          transToPers(const InDetSimData* transObj, InDetSimData_p2* persObj, MsgStream &log);
  void setCurrentStore (IProxyDict* store);

private:
  IProxyDict* m_sg;
};


#endif


