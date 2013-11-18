/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrigCoinData/RpcCoinDataCollection.h"
#include "GaudiKernel/MsgStream.h"

MsgStream& operator << ( MsgStream& sl,  const Muon::RpcCoinDataCollection & coll) 
{
	using namespace Muon;
        sl << "RpcCoinDataCollection: ";
        sl << "identify()="<< coll.identify()
 //       << ", identifyHash()="<< coll.identifyHash()
        << ", RpcCoinData=[";
        RpcCoinDataCollection::const_iterator it = coll.begin();
        RpcCoinDataCollection::const_iterator itEnd = coll.end();
        for (;it!=itEnd;++it) sl<< (**it)<<", ";
        sl <<" ]"<<std::endl;
        return sl;
}

std::ostream& operator << ( std::ostream& sl, const Muon::RpcCoinDataCollection& coll) 
{
	using namespace Muon;
    sl << "RpcCoinDataCollection: "
    << "identify()="<< coll.identify()
//    << ", identifyHash()="<< coll.identifyHash()
    << ", RpcCoinData=[";
    RpcCoinDataCollection::const_iterator it = coll.begin();
    RpcCoinDataCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
}

