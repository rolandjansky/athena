/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

MsgStream& operator << ( MsgStream& sl, const CscStripPrepDataCollection& coll) 
{
        sl << "CscStripPrepDataCollection: "
        << "identify()="<< coll.identify()
 //       << ", identifyHash()="<< coll.identifyHash()
        << ", CscStripPrepData=[";
        CscStripPrepDataCollection::const_iterator it = coll.begin();
        CscStripPrepDataCollection::const_iterator itEnd = coll.end();
        for (;it!=itEnd;++it) sl<< (**it)<<", ";
        sl <<" ]"<<std::endl;
        return sl;
}

std::ostream& operator << ( std::ostream& sl, const CscStripPrepDataCollection& coll) 
{
    sl << "CscStripPrepDataCollection: "
    << "identify()="<< coll.identify()
//    << ", identifyHash()="<< coll.identifyHash()
    << ", CscStripPrepData=[";
    CscStripPrepDataCollection::const_iterator it = coll.begin();
    CscStripPrepDataCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
}

}//end of ns
