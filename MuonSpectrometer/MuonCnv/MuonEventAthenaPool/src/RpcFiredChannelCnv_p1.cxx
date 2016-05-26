/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcFiredChannel.h"
#include "RpcFiredChannelCnv_p1.h"


void
RpcFiredChannelCnv_p1::transToPers(const RpcFiredChannel* trans, RpcFiredChannel_p1* pers, MsgStream &) 
{
    pers->m_bcid	    = trans->bcid();
    pers->m_time      	= trans->time();
    pers->m_ijk   	    = trans->ijk(); 
    pers->m_channel    	= trans->channel();
    pers->m_ovl         = trans->ovl();
    pers->m_thr      	= trans->thr();
}


void
RpcFiredChannelCnv_p1::persToTrans(const RpcFiredChannel_p1* pers, RpcFiredChannel* trans, MsgStream &) 
{
    trans->m_bcid       = pers->bcid();
    trans->m_time    	= pers->time();
    trans->m_ijk        = pers->ijk(); 
    trans->m_channel    = pers->channel();
    trans->m_ovl        = pers->ovl();
    trans->m_thr        = pers->thr();
}



