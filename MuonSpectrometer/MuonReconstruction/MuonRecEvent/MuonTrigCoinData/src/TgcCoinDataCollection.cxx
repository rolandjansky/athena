/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrigCoinData/TgcCoinDataCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

MsgStream& operator << ( MsgStream& sl, const TgcCoinDataCollection& coll) 
{
        sl << "TgcTrigCoinDataCollection: "
        << "identify()="<< coll.identify()
        << ", identifyHash()="<< coll.identifyHash()
        << ", TgcCoinData=[";
        TgcCoinDataCollection::const_iterator it = coll.begin();
        TgcCoinDataCollection::const_iterator itEnd = coll.end();
        for (;it!=itEnd;++it) sl<< (**it)<<", ";
        sl <<" ]"<<std::endl;
        return sl;
}
  
std::ostream& operator << ( std::ostream& sl, const TgcCoinDataCollection& coll) 
{
    sl << "TgcCoinDataCollection: "
    << "identify()="<< coll.identify()
    << ", identifyHash()="<< (unsigned int)(coll.identifyHash())
    << ", TgcCoinData=[";
    TgcCoinDataCollection::const_iterator it = coll.begin();
    TgcCoinDataCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
    }
}//end of ns
