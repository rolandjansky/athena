/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/Strip_RDO_Collection.h"
#include "GaudiKernel/MsgStream.h"

MsgStream& operator << ( MsgStream& sl, const Strip_RDO_Collection& coll)
{
  sl << "Strip_RDO_Collection: "
     << "identify()="<< coll.identify()
    //       << ", identifyHash()="<< coll.identifyHash()
     << ", RDO=[";
  Strip_RDO_Collection::const_iterator it = coll.begin();
  Strip_RDO_Collection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}

std::ostream& operator << ( std::ostream& sl, const Strip_RDO_Collection& coll)
{
  sl << "Strip_RDO_Collection: "
     << "identify()="<< coll.identify()
    //    << ", identifyHash()="<< coll.identifyHash()
     << ", RDO=[";
  Strip_RDO_Collection::const_iterator it = coll.begin();
  Strip_RDO_Collection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}
