/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RPCCOINMATRIXCNV_P1_H
#define MUONEVENTATHENAPOOL_RPCCOINMATRIXCNV_P1_H

/**
Transient/Persistent converter for the Collection of TgcRDO,
@author Edward.Moyse@cern.ch
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "RpcFiredChannelCnv_p1.h"
#include "MuonEventAthenaPool/RpcCoinMatrix_p1.h"
#include "MuonRDO/RpcCoinMatrix.h"

class MsgStream;


typedef  T_AthenaPoolTPCnvVector< RpcCoinMatrix, RpcCoinMatrix_p1, RpcFiredChannelCnv_p1 >	RpcCoinMatrixCnv_p1_basetype;

class RpcCoinMatrixCnv_p1
   : public RpcCoinMatrixCnv_p1_basetype
{
public:
  RpcCoinMatrixCnv_p1() {}

  virtual void	persToTrans(const RpcCoinMatrix_p1* persColl, RpcCoinMatrix* transColl,   MsgStream &log);
  virtual void	transToPers(const RpcCoinMatrix* transColl,   RpcCoinMatrix_p1* persColl, MsgStream &log);
};


#endif




