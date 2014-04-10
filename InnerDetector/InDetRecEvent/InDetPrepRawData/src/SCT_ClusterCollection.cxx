/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace InDet
{

  MsgStream& operator << ( MsgStream& sl, const SCT_ClusterCollection& coll)
  {
    sl << "SCT_ClusterCollection: "
       << "identify()="<< coll.identify()
      //       << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    SCT_ClusterCollection::const_iterator it = coll.begin();
    SCT_ClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

  std::ostream& operator << ( std::ostream& sl, const SCT_ClusterCollection& coll)
  {
    sl << "SCT_ClusterCollection: "
       << "identify()="<< coll.identify()
      //    << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    SCT_ClusterCollection::const_iterator it = coll.begin();
    SCT_ClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

}//end of ns
