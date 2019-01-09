/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/StripData.h"

std::ostream& operator<<(std::ostream& stream, NSWL1::StripData& data) {
  stream << "Strip hit at side " << ( (data.sideId())? "A":"C")
	 << ", trigger sector " << data.sectorId()
	 << ", layer " << data.layer()
	 << ", wedge " << data.wedge();
  return stream;
}

