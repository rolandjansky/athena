/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace Muon
{

MsgStream& operator << ( MsgStream& sl, const MdtPrepDataCollection& coll) 
{
        sl << "MdtPrepRawDataCollection: "
        << "identify()="<< coll.identify()
 //       << ", identifyHash()="<< coll.identifyHash()
        << ", MdtPrepData=[";
        MdtPrepDataCollection::const_iterator it = coll.begin();
        MdtPrepDataCollection::const_iterator itEnd = coll.end();
        for (;it!=itEnd;++it) sl<< (**it)<<", ";
        sl <<" ]"<<std::endl;
        return sl;
}

std::ostream& operator << ( std::ostream& sl, const MdtPrepDataCollection& coll) 
{
    sl << "MdtPrepRawDataCollection: "
    << "identify()="<< coll.identify()
//    << ", identifyHash()="<< coll.identifyHash()
    << ", MdtPrepData=[";
    MdtPrepDataCollection::const_iterator it = coll.begin();
    MdtPrepDataCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
}

}//end of ns
