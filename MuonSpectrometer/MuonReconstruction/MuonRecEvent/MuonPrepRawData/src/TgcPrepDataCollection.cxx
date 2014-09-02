/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

MsgStream& operator << ( MsgStream& sl, const TgcPrepDataCollection& coll) 
{
        sl << "TgcPrepRawDataCollection: "
        << "identify()="<< coll.identify()
 //       << ", identifyHash()="<< coll.identifyHash()
        << ", TgcPrepData=[";
        TgcPrepDataCollection::const_iterator it = coll.begin();
        TgcPrepDataCollection::const_iterator itEnd = coll.end();
        for (;it!=itEnd;++it) sl<< (**it)<<", ";
        sl <<" ]"<<std::endl;
        return sl;
}

std::ostream& operator << ( std::ostream& sl, const TgcPrepDataCollection& coll) 
{
    sl << "TgcPrepRawDataCollection: "
    << "identify()="<< coll.identify()
//    << ", identifyHash()="<< coll.identifyHash()
    << ", TgcPrepData=[";
    TgcPrepDataCollection::const_iterator it = coll.begin();
    TgcPrepDataCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
}

}//end of ns
