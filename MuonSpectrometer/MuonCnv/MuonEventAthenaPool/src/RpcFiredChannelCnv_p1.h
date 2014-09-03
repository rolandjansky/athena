/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_RPCFIREDCHANNELCNV_P1_H
#define MUONEVENTATHENAPOOL_RPCFIREDCHANNELCNV_P1_H

/*
Transient/Persistent converter for RpcFiredChannel class
@author Edward.Moyse@cern.ch
 */

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonEventAthenaPool/RpcFiredChannel_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"  

class MsgStream;

class RpcFiredChannelCnv_p1  : public T_AthenaPoolTPCnvBase<RpcFiredChannel, RpcFiredChannel_p1>
{
public:
  RpcFiredChannelCnv_p1() {}
  
  virtual void		persToTrans(const RpcFiredChannel_p1* persObj, RpcFiredChannel* transObj, MsgStream &log) ;
  virtual void		transToPers(const RpcFiredChannel* transObj,   RpcFiredChannel_p1* persObj, MsgStream &log);
};


#endif

