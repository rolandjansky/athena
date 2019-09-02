/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ObjectAccessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/ObjectAccessor.h"

#include "GaudiKernel/MsgStream.h"
#include <iostream>

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::ObjectAccessor& oac)
{
  for (const ObjectAccessor::value_type& elm : oac) {
    sl << (oac.end() - oac.begin()) << "-ObjectAccessor: | " << elm << " | ";
  }
  return sl;
}

std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::ObjectAccessor& oac)
{
  for (const ObjectAccessor::value_type& elm : oac) {
    sl << (oac.end() - oac.begin()) << "-ObjectAccessor: | " << elm << " | ";
  }
  return sl;
}
