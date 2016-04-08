/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcCoinMatrix.h"
#include "GaudiKernel/MsgStream.h"

MsgStream& operator << ( MsgStream& sl, const RpcCoinMatrix& coll) 
{
    sl << "RpcCoinMatrix: "<<", ";
    sl << "identify() = "<< coll.identify().getString()<<", ";
    sl << "onlineId() = "<< coll.onlineId()<<", ";
    sl << "crc() = "<< coll.crc()<<", ";
    sl << "fel1Id() = "<< coll.fel1Id()<<", ";
    sl << "febcId() = "<< coll.febcId()<<", ";
    for (const RpcFiredChannel* p : coll)
      sl << *p <<", ";
    sl<<"]"<<std::endl;
    return sl;
}


std::ostream& operator << ( std::ostream& sl, const RpcCoinMatrix& coll) 
{
    sl << "RpcCoinMatrix: "<<", ";
    sl << "identify() = "<< coll.identify().getString()<<", ";
    sl << "onlineId() = "<< coll.onlineId()<<", ";
    sl << "crc() = "<< coll.crc()<<", ";
    sl << "fel1Id() = "<< coll.fel1Id()<<", ";
    sl << "febcId() = "<< coll.febcId()<<", ";
    for (const RpcFiredChannel* p : coll)
      sl << *p <<", ";
    sl<<"]"<<std::endl;
    return sl;
}
