/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/SiCellId.h"

std::ostream & operator << (std::ostream & os, const InDetDD::SiCellId & cellId)
{
  if (cellId.isValid()){
    return os << "[" << cellId.phiIndex() << "." << cellId.etaIndex() << "]";
  } else {
    return os << "[INVALID]";
  }
}

