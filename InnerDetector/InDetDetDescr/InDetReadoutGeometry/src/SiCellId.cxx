/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/SiCellId.h"

namespace InDetDD{
	std::ostream & operator << (std::ostream & os, const SiCellId & cellId)
	{
		if (cellId.isValid()){
			return os << "[" << cellId.phiIndex() << "." << cellId.etaIndex() << "]";
		} else {
			return os << "[INVALID]";
		}
	}
}// namespace InDetDD

