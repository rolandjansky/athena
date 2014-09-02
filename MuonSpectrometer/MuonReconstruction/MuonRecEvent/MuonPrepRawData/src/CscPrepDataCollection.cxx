/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

MsgStream& operator << ( MsgStream& sl, const CscPrepDataCollection& coll) 
{
        sl << "CscPrepDataCollection: "
        << "identify()="<< coll.identify()
 //       << ", identifyHash()="<< coll.identifyHash()
        << ", CscPrepData=[";
        CscPrepDataCollection::const_iterator it = coll.begin();
        CscPrepDataCollection::const_iterator itEnd = coll.end();
        for (;it!=itEnd;++it) sl<< (**it)<<", ";
        sl <<" ]"<<std::endl;
        return sl;
}

std::ostream& operator << ( std::ostream& sl, const CscPrepDataCollection& coll) 
{
    sl << "CscPrepDataCollection: "
    << "identify()="<< coll.identify()
//    << ", identifyHash()="<< coll.identifyHash()
    << ", CscPrepData=[";
    CscPrepDataCollection::const_iterator it = coll.begin();
    CscPrepDataCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
}

}//end of ns
