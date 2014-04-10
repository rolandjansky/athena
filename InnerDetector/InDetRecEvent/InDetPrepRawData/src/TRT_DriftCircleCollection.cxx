/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace InDet
{

  MsgStream& operator << ( MsgStream& sl, const TRT_DriftCircleCollection& coll)
  {
    sl << "TRT_DriftCircleCollection: "
       << "identify()="<< coll.identify()
      //       << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    TRT_DriftCircleCollection::const_iterator it = coll.begin();
    TRT_DriftCircleCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

  std::ostream& operator << ( std::ostream& sl, const TRT_DriftCircleCollection& coll)
  {
    sl << "TRT_DriftCircleCollection: "
       << "identify()="<< coll.identify()
      //    << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    TRT_DriftCircleCollection::const_iterator it = coll.begin();
    TRT_DriftCircleCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }

}//end of ns
