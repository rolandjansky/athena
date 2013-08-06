/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcPad.h"
#include "GaudiKernel/MsgStream.h"

MsgStream& operator << ( MsgStream& sl, const RpcPad& coll) 
{
    sl << "RpcPad: ";
    sl << "identify() = "<< coll.identify().getString()<<", ";
    sl << "status() = "<< coll.status()<<", ";
    sl << "errorCode() = "<< coll.errorCode()<<", ";
    sl << "onlineId() = "<< coll.onlineId()<<", ";
    sl << "sector() = "<< coll.sector()<<", CoinMatrices = [";
    std::vector<RpcCoinMatrix*>::const_iterator it = coll.begin();
    std::vector<RpcCoinMatrix*>::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it) <<", ";
    sl <<"]"<<std::endl; 
    return sl;
}


std::ostream& operator << ( std::ostream& sl, const RpcPad& coll) 
{
    sl << "RpcPad: ";
    sl << "identify() = "<< coll.identify().getString()<<", ";
    sl << "status() = "<< coll.status()<<", ";
    sl << "errorCode() = "<< coll.errorCode()<<", ";
    sl << "onlineId() = "<< coll.onlineId()<<", ";
    sl << "sector() = "<< coll.sector()<<", CoinMatrices = [";
    std::vector<RpcCoinMatrix*>::const_iterator it = coll.begin();
    std::vector<RpcCoinMatrix*>::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it);
    sl <<"]"<<std::endl; 
    return sl;
}



 
