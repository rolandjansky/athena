/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/ClusterId.h"


namespace TrkDriftCircleMath {

std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::ClusterId& id )
{
  if( id.isTgc() ) os << "TGC";
  else os << "RPC";
  if( id.measuresPhi() ) os << " phi";
  else os << " eta";
  return os;
}

} // namespace TrkDriftCircleMath
