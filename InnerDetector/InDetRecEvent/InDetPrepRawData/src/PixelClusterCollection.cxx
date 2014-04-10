/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/PixelClusterCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace InDet
{

  MsgStream& operator << ( MsgStream& sl, const PixelClusterCollection& coll)
  {
    sl << "PixelClusterCollection: "
       << "identify()="<< coll.identify()
      //       << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    PixelClusterCollection::const_iterator it = coll.begin();
    PixelClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

  std::ostream& operator << ( std::ostream& sl, const PixelClusterCollection& coll)
  {
    sl << "PixelClusterCollection: "
       << "identify()="<< coll.identify()
      //    << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    PixelClusterCollection::const_iterator it = coll.begin();
    PixelClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

}//end of ns
