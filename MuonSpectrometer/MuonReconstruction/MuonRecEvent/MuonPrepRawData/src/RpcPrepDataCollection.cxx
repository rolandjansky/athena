/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

MsgStream& operator << ( MsgStream& sl, const RpcPrepDataCollection& coll) 
{
        sl << "RpcPrepRawDataCollection: "
        << "identify()="<< coll.identify()
 //       << ", identifyHash()="<< coll.identifyHash()
        << ", RpcPrepData=[";
        RpcPrepDataCollection::const_iterator it = coll.begin();
        RpcPrepDataCollection::const_iterator itEnd = coll.end();
        for (;it!=itEnd;++it) sl<< (**it)<<", ";
        sl <<" ]"<<std::endl;
        return sl;
}

std::ostream& operator << ( std::ostream& sl, const RpcPrepDataCollection& coll) 
{
    sl << "RpcPrepRawDataCollection: "
    << "identify()="<< coll.identify()
//    << ", identifyHash()="<< coll.identifyHash()
    << ", RpcPrepData=[";
    RpcPrepDataCollection::const_iterator it = coll.begin();
    RpcPrepDataCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
}

}//end of ns
