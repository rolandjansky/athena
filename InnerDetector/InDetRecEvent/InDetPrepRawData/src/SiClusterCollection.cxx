/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/SiClusterCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace InDet
{

  MsgStream& operator << ( MsgStream& sl, const SiClusterCollection& coll)
  {
    sl << "SiClusterCollection: "
       << "identify()="<< coll.identify()
      //       << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    SiClusterCollection::const_iterator it = coll.begin();
    SiClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

  std::ostream& operator << ( std::ostream& sl, const SiClusterCollection& coll)
  {
    sl << "SiClusterCollection: "
       << "identify()="<< coll.identify()
      //    << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    SiClusterCollection::const_iterator it = coll.begin();
    SiClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

}//end of ns
