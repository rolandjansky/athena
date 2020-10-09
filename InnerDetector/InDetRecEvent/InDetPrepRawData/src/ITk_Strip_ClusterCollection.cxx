/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/ITk_Strip_ClusterCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace InDet
{

  MsgStream& operator << ( MsgStream& sl, const ITk_Strip_ClusterCollection& coll)
  {
    sl << "ITk_Strip_ClusterCollection: "
       << "identify()="<< coll.identify()
      //       << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    ITk_Strip_ClusterCollection::const_iterator it = coll.begin();
    ITk_Strip_ClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

  std::ostream& operator << ( std::ostream& sl, const ITk_Strip_ClusterCollection& coll)
  {
    sl << "ITk_Strip_ClusterCollection: "
       << "identify()="<< coll.identify()
      //    << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    ITk_Strip_ClusterCollection::const_iterator it = coll.begin();
    ITk_Strip_ClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

}//end of ns
