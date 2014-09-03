/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RPCPADCNV_P1_H
#define MUONEVENTATHENAPOOL_RPCPADCNV_P1_H

/**
 Transient/Persistent converter for the Collection of RpcPad_p1,
@author Edward.Moyse@cern.ch
 */

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "RpcCoinMatrixCnv_p1.h"
#include "MuonEventAthenaPool/RpcPad_p1.h"
#include "MuonRDO/RpcPad.h"

class MsgStream;


typedef  T_AthenaPoolTPCnvVector< RpcPad, RpcPad_p1, RpcCoinMatrixCnv_p1 >	RpcPadCnv_p1_basetype;

class RpcPadCnv_p1
   : public RpcPadCnv_p1_basetype
{
public:
  RpcPadCnv_p1() {}

  virtual void	persToTrans(const RpcPad_p1* persColl, RpcPad* transColl,   MsgStream &log) ;
  virtual void	transToPers(const RpcPad* transColl,   RpcPad_p1* persColl, MsgStream &log) ;
};


#endif




