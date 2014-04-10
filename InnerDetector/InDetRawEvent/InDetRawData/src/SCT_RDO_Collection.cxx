/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/SCT_RDO_Collection.h"
#include "GaudiKernel/MsgStream.h"

MsgStream& operator << ( MsgStream& sl, const SCT_RDO_Collection& coll)
{
  sl << "SCT_RDO_Collection: "
     << "identify()="<< coll.identify()
    //       << ", identifyHash()="<< coll.identifyHash()
     << ", RDO=[";
  SCT_RDO_Collection::const_iterator it = coll.begin();
  SCT_RDO_Collection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}

std::ostream& operator << ( std::ostream& sl, const SCT_RDO_Collection& coll)
{
  sl << "SCT_RDO_Collection: "
     << "identify()="<< coll.identify()
    //    << ", identifyHash()="<< coll.identifyHash()
     << ", RDO=[";
  SCT_RDO_Collection::const_iterator it = coll.begin();
  SCT_RDO_Collection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}
