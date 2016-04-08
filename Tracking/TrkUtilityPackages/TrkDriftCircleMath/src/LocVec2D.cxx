/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/LocVec2D.h"


namespace TrkDriftCircleMath {


std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::LocVec2D& lv )
{
  os << " x " << lv.x() << " y " << lv.y();
  return os;
}


} // namespace TrkDriftCircleMath
