/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FatrasEvent/PlanarClusterCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace iFatras {
  
  MsgStream& operator << ( MsgStream& sl, const PlanarClusterCollection& coll)
  {
    sl << "PlanarClusterCollection: "
       << "identify()="<< coll.identify()
       << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    PlanarClusterCollection::const_iterator it = coll.begin();
    PlanarClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
  }
  
  std::ostream& operator << ( std::ostream& sl, const PlanarClusterCollection& coll)
  {
    sl << "PlanarClusterCollection: "
       << "identify()="<< coll.identify()
       << ", identifyHash()="<< coll.identifyHash()
       << ", PRD=[";
    PlanarClusterCollection::const_iterator it = coll.begin();
    PlanarClusterCollection::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
   }

}
